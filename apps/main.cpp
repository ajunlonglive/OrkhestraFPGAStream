#include <chrono>
#include <cmath>
#include <iostream>
#include <optional>
#include <utility>
#include <vector>

#include "config.hpp"
#include "config_creator.hpp"
#include "cxxopts.hpp"
#include "graph_creator.hpp"
#include "input_config_reader.hpp"
#include "operation_types.hpp"
#include "query_manager.hpp"
#include "query_scheduling_data.hpp"
#include "rapidjson_reader.hpp"

#include "logger.hpp"

using dbmstodspi::fpga_managing::operation_types::QueryOperationType;
using dbmstodspi::query_managing::QueryManager;
using dbmstodspi::query_managing::query_scheduling_data::QueryNode;

using dbmstodspi::input_managing::Config;
using dbmstodspi::input_managing::ConfigCreator;
using dbmstodspi::input_managing::GraphCreator;
using dbmstodspi::input_managing::InputConfigReader;
using dbmstodspi::input_managing::RapidJSONReader;

using dbmstodspi::logger::LogLevel;
using dbmstodspi::logger::SetLoggingLevel;
using dbmstodspi::logger::Log;

/**
 * @brief Helper method to run the given query nodes and their subsequent nodes
 * while measuring and printing the overall time it took to process the queries.
 *
 * This includes data writing and reading from and to the DDR.
 * @param leaf_nodes Vector of nodes from which the parsing starts.
 */
void MeasureOverallTime(std::vector<std::shared_ptr<QueryNode>> leaf_nodes,
                        const Config& config) {
  std::chrono::steady_clock::time_point begin =
      std::chrono::steady_clock::now();
  QueryManager::RunQueries(std::move(leaf_nodes), config);
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

  std::cout
      << "Overall time = "
      << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count()
      << "[s]" << std::endl;
}

/**
 * @brief Main method of the program.
 *
 * Creates query nodes to be processed and runs the
 * query nodes according to the given config.
 */
auto main(int argc, char* argv[]) -> int {
  cxxopts::Options options(
      "DBMStoDSPI",
      "Accelerate the given query operations with an available FPGA!");

  options.add_options()("i,input", "Input definition",
                        cxxopts::value<std::string>())(
      "c,config", "Config file for used hardware",
      cxxopts::value<std::string>())("h,help", "Print usage");

  auto result = options.parse(argc, argv);

  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    exit(0);
  }

  auto config_creator = ConfigCreator(std::make_unique<RapidJSONReader>(),
                                      std::make_unique<InputConfigReader>());
  auto graph_maker = GraphCreator(std::make_unique<RapidJSONReader>());
  MeasureOverallTime(
      std::move(graph_maker.MakeGraph(result["input"].as<std::string>())),
      config_creator.GetConfig(result["config"].as<std::string>()));

  SetLoggingLevel(LogLevel::kDebug);  // Set global log level to debug
  Log(LogLevel::kTrace, "We done! 42");
  Log(LogLevel::kDebug, "We done! 42");
  Log(LogLevel::kInfo, "We done! 42");

  // Tests
  // MeasureOverallTime(std::move(graph_maker.MakeGraph("filter_testing.json")),
  //                   config_creator.GetConfig("config.ini"));
  // MeasureOverallTime(std::move(graph_maker.MakeGraph("filter_join_testing.json")),
  //                   config_creator.GetConfig("config.ini"));
  // MeasureOverallTime(std::move(graph_maker.MakeGraph("concurrency_testing.json")),
  //                   config_creator.GetConfig("config.ini"));
  // MeasureOverallTime(std::move(graph_maker.MakeGraph("single_run_testing.json")),
  //                   config_creator.GetConfig("config.ini"));
  // MeasureOverallTime(std::move(graph_maker.MakeGraph("double_run_testing.json")),
  //                   config_creator.GetConfig("config.ini"));
  // MeasureOverallTime(std::move(graph_maker.MakeGraph("TPCH_Q19_SF001.json")),
  //                   config_creator.GetConfig("config.ini"));
  // MeasureOverallTime(std::move(graph_maker.MakeGraph("TPCH_Q19_SF01.json")),
  //                   config_creator.GetConfig("config.ini"));
  // Need to unpack SF=0.3 data
  /*MeasureOverallTime(std::move(graph_maker.MakeGraph("TPCH_Q19_SF03.json")),
                     config_creator.GetConfig("extended_config.ini"));*/

  return 0;
}

// Temp not supported
// QueryNode
// merge_sort_query_8k_once_double = {
//    {"CAR_DATA_HALF_SORTED_8K_64WAY.csv"},
//    {"CAR_DATA_SORTED_8K.csv"},
//    QueryOperationType::kMergeSort,
//    {nullptr},
//    {nullptr}};