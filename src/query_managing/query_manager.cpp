#include "query_manager.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <stdexcept>

#include "accelerated_query_node.hpp"
#include "data_manager.hpp"
#include "elastic_module_checker.hpp"
#include "fpga_manager.hpp"
#include "id_manager.hpp"
#include "logger.hpp"
#include "memory_block_interface.hpp"
#include "node_scheduler.hpp"
#include "operation_types.hpp"
#include "query_acceleration_constants.hpp"
#include "query_scheduling_data.hpp"
#include "stream_parameter_calculator.hpp"
#include "table_manager.hpp"
#include "util.hpp"

using namespace dbmstodspi::query_managing;
using dbmstodspi::data_managing::table_data::TableData;
using dbmstodspi::logger::Log;
using dbmstodspi::logger::LogLevel;
using dbmstodspi::logger::ShouldLog;
using dbmstodspi::util::CreateReferenceVector;

void QueryManager::CheckTableData(const TableData& expected_table,
                                  const TableData& resulting_table) {
  if (expected_table == resulting_table) {
    Log(LogLevel::kDebug, "Query results are correct!");
  } else {
    Log(LogLevel::kError,
        "Incorrect query results: " +
            std::to_string(
                expected_table.table_data_vector.size() /
                TableManager::GetRecordSizeFromTable(expected_table)) +
            " vs " +
            std::to_string(
                resulting_table.table_data_vector.size() /
                TableManager::GetRecordSizeFromTable(resulting_table)) +
            " rows!");
    data_managing::DataManager::PrintTableData(resulting_table);
  }
}

void QueryManager::RunQueries(
    std::vector<std::shared_ptr<query_scheduling_data::QueryNode>>
        starting_query_nodes,
    const Config& config) {
  Log(LogLevel::kTrace, "Starting up!");
  data_managing::DataManager data_manager(config.data_sizes);
  fpga_managing::MemoryManager memory_manager;
  fpga_managing::FPGAManager fpga_manager(&memory_manager);

  auto query_node_runs_queue = NodeScheduler::FindAcceleratedQueryNodeSets(
      std::move(starting_query_nodes), config.accelerator_library,
      config.module_library);
  Log(LogLevel::kTrace, "Scheduling done!");

  while (!query_node_runs_queue.empty()) {
    const auto executable_query_nodes = query_node_runs_queue.front().second;

    const auto& bitstream_file_name =
        config.accelerator_library.at(query_node_runs_queue.front().first);
    query_node_runs_queue.pop();

    memory_manager.LoadBitstreamIfNew(
        bitstream_file_name,
        config.required_memory_space.at(bitstream_file_name));

    IDManager id_manager;
    std::vector<std::vector<int>> output_ids;
    std::vector<std::vector<int>> input_ids;
    std::vector<
        std::vector<std::unique_ptr<fpga_managing::MemoryBlockInterface>>>
        input_memory_blocks;
    std::vector<
        std::vector<std::unique_ptr<fpga_managing::MemoryBlockInterface>>>
        output_memory_blocks;
    std::vector<TableData> expected_output_tables(
        fpga_managing::query_acceleration_constants::kMaxIOStreamCount);
    std::vector<std::string> expected_output_files(
        fpga_managing::query_acceleration_constants::kMaxIOStreamCount);
    std::vector<fpga_managing::AcceleratedQueryNode> query_nodes;

    id_manager.AllocateStreamIDs(CreateReferenceVector(executable_query_nodes),
                                 input_ids, output_ids);

    for (int node_index = 0; node_index < executable_query_nodes.size();
         node_index++) {
      auto current_node = *executable_query_nodes.at(node_index);

      // Allocate memory blocks
      std::vector<std::unique_ptr<fpga_managing::MemoryBlockInterface>>
          allocated_input_memory_blocks;
      for (const auto& linked_node : current_node.previous_nodes) {
        auto observed_node = linked_node.lock();
        if (!observed_node) {
          allocated_input_memory_blocks.push_back(
              memory_manager.GetAvailableMemoryBlock());
        } else {
          allocated_input_memory_blocks.push_back(nullptr);
        }
      }

      std::vector<std::unique_ptr<fpga_managing::MemoryBlockInterface>>
          allocated_output_memory_blocks;
      for (const auto& linked_node : current_node.next_nodes) {
        if (!linked_node) {
          allocated_output_memory_blocks.push_back(
              memory_manager.GetAvailableMemoryBlock());
        } else {
          allocated_output_memory_blocks.push_back(nullptr);
        }
      }

      // Get parameters and write input to allocated blocks
      std::vector<fpga_managing::StreamDataParameters> input_stream_parameters;
      TableManager::ReadInputTables(
          input_stream_parameters, data_manager,
          current_node.input_data_definition_files, input_ids[node_index],
          allocated_input_memory_blocks,
          current_node.operation_parameters.input_stream_parameters);

      std::vector<fpga_managing::StreamDataParameters> output_stream_parameters;
      TableManager::ReadExpectedTables(
          output_stream_parameters, data_manager,
          current_node.output_data_definition_files, output_ids[node_index],
          allocated_output_memory_blocks, expected_output_tables,
          expected_output_files,
          current_node.operation_parameters.output_stream_parameters);

      // Check if the loaded modules are correct based on the input.
      ElasticModuleChecker::CheckElasticityNeeds(
          input_stream_parameters, current_node.operation_type,
          current_node.operation_parameters.operation_parameters);

      query_nodes.push_back(
          {std::move(input_stream_parameters),
           std::move(output_stream_parameters), current_node.operation_type,
           current_node.module_location,
           current_node.operation_parameters.operation_parameters});

      // Keep memory blocks during the query execution
      input_memory_blocks.push_back(std::move(allocated_input_memory_blocks));
      output_memory_blocks.push_back(std::move(allocated_output_memory_blocks));
    }

    // Run query
    Log(LogLevel::kTrace, "Setup query!");
    fpga_manager.SetupQueryAcceleration(query_nodes);
    Log(LogLevel::kTrace, "Running query!");
    auto result_sizes = fpga_manager.RunQueryAcceleration();
    Log(LogLevel::kTrace, "Query done!");

    // Check results & free memory
    std::vector<TableData> output_tables = expected_output_tables;
    for (int node_index = 0; node_index < query_nodes.size(); node_index++) {
      TableManager::ReadResultTables(query_nodes[node_index].output_streams,
                                     output_tables, result_sizes,
                                     output_memory_blocks[node_index]);
      for (int stream_index = 0; stream_index < output_ids[node_index].size();
           stream_index++) {
        if (output_memory_blocks[node_index][stream_index]) {
          Log(LogLevel::kDebug,
              "Result has " +
                  std::to_string(
                      result_sizes[output_ids[node_index][stream_index]]) +
                  " rows!");

          if (expected_output_tables[output_ids[node_index][stream_index]]
                  .table_data_vector.empty() &&
              result_sizes[output_ids[node_index][stream_index]] != 0) {
            TableManager::WriteResultTableFile(
                output_tables[output_ids[node_index][stream_index]],
                expected_output_files[output_ids[node_index][stream_index]]);
          } else {
            CheckTableData(
                expected_output_tables[output_ids[node_index][stream_index]],
                output_tables[output_ids[node_index][stream_index]]);
          }
        }
      }

      // Free all memory for now.
      for (auto& memory_pointer : input_memory_blocks[node_index]) {
        if (memory_pointer) {
          memory_manager.FreeMemoryBlock(std::move(memory_pointer));
        }
      }
      for (auto& memory_pointer : output_memory_blocks[node_index]) {
        if (memory_pointer) {
          memory_manager.FreeMemoryBlock(std::move(memory_pointer));
        }
      }
    }
  }
}