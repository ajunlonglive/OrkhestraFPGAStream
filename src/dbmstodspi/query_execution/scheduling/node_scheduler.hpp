/*
Copyright 2021 University of Manchester

Licensed under the Apache License, Version 2.0(the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http:  // www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once
#include <memory>
#include <queue>
#include <set>
#include <utility>
#include <vector>

#include "operation_types.hpp"
#include "query_scheduling_data.hpp"

using orkhestrafs::core_interfaces::operation_types::QueryOperationType;
using orkhestrafs::core_interfaces::query_scheduling_data::
    ConfigurableModulesVector;
using orkhestrafs::core_interfaces::query_scheduling_data::QueryNode;

namespace orkhestrafs::dbmstodspi {

/**
 * @brief Class to schedule nodes to groups of different FPGA runs.
 */
class NodeScheduler {
 public:
  /**
   * @brief Find groups of accelerated query nodes which can be run in the same
   * FPGA run.
   * @param starting_nodes Input vector of leaf nodes from which the parsing can
   * begin.
   * @param supported_accelerator_bitstreams Map of hardware module combinations
   * which have a corresponding bitstream.
   * @param existing_modules_library Map of hardware modules and the available
   * variations with different computational capacity values.
   * @return Queue of groups of accelerated query
   * nodes to be accelerated next.
   */
  static auto FindAcceleratedQueryNodeSets(
      std::vector<std::shared_ptr<QueryNode>> starting_nodes,
      const std::map<ConfigurableModulesVector, std::string>
          &supported_accelerator_bitstreams,
      const std::map<QueryOperationType, std::vector<std::vector<int>>>
          &existing_modules_library,
      std::map<std::string,
               std::map<int, std::vector<std::pair<std::string, int>>>>
          &linked_nodes)
      -> std::queue<std::pair<ConfigurableModulesVector,
                              std::vector<std::shared_ptr<QueryNode>>>>;

 private:
  static void CheckExternalLinks(
      const std::vector<std::shared_ptr<QueryNode>> &current_query_nodes,
      std::map<std::string,
               std::map<int, std::vector<std::pair<std::string, int>>>>
          &linked_nodes);
  static auto IsModuleSetSupported(
      const ConfigurableModulesVector &module_set,
      const std::map<ConfigurableModulesVector, std::string>
          &supported_accelerator_bitstreams) -> bool;
  static auto IsNodeIncluded(const std::vector<QueryNode> &node_vector,
                             const QueryNode &searched_node) -> bool;
  static auto IsNodeAvailable(const std::vector<QueryNode> &scheduled_nodes,
                              const QueryNode &current_node) -> bool;
  /*static auto FindNextAvailableNode(
      const std::vector<query_scheduling_data::QueryNode>
          &already_scheduled_nodes,
      std::vector<query_scheduling_data::QueryNode> &starting_nodes)
      -> std::vector<query_scheduling_data::QueryNode>::iterator &;*/
  static auto FindMinPosition(
      const QueryNode *current_node,
      const std::vector<QueryNode> &current_query_nodes,
      const ConfigurableModulesVector &current_modules_vector) -> int;
  static void CheckNodeForModuleSet(
      int node_index, ConfigurableModulesVector &current_modules_vector,
      std::vector<std::shared_ptr<QueryNode>> &current_query_nodes,
      std::vector<std::shared_ptr<QueryNode>> &scheduled_queries,
      std::vector<std::shared_ptr<QueryNode>> &starting_nodes,
      const std::map<ConfigurableModulesVector, std::string>
          &supported_accelerator_bitstreams,
      const std::map<QueryOperationType, std::vector<std::vector<int>>>
          &existing_modules_library);
  static auto FindSuitableModuleCombination(
      QueryNode *current_node,
      const std::vector<QueryNode> &current_query_nodes,
      const ConfigurableModulesVector &current_modules_vector,
      const std::map<ConfigurableModulesVector, std::string>
          &supported_accelerator_bitstreams,
      const std::map<QueryOperationType, std::vector<std::vector<int>>>
          &existing_modules_library) -> ConfigurableModulesVector;
  static auto CreateNewModulesVector(
      QueryOperationType query_operation, int current_position,
      const ConfigurableModulesVector &current_modules_vector,
      const std::vector<int> &module_parameters) -> ConfigurableModulesVector;
  static auto CheckModuleParameterSupport(
      std::vector<int> module_parameters,
      const ConfigurableModulesVector &current_modules_vector,
      int module_position, int parameter_option_index,
      QueryOperationType query_operation,
      std::vector<std::vector<int>> current_module_possible_parameters,
      const std::map<ConfigurableModulesVector, std::string>
          &supported_accelerator_bitstreams) -> ConfigurableModulesVector;
  static auto FindNextNodeLocation(
      const std::vector<std::shared_ptr<QueryNode>> &next_nodes,
      const QueryNode *next_node) -> int;
  static auto IsProjectionOperationDefined(const QueryNode *current_node,
                                           const QueryNode *previous_node,
                                           int previous_node_index,
                                           int current_node_index) -> bool;
  static auto IsNodeMissingFromTheVector(
      const std::shared_ptr<QueryNode> &linked_node,
      const std::vector<std::shared_ptr<QueryNode>> &current_query_nodes)
      -> bool;
  static auto FindPreviousNodeLocation(
      const std::vector<std::weak_ptr<QueryNode>> &previous_nodes,
      const std::shared_ptr<QueryNode> &previous_node) -> int;
  static auto ReuseMemory(const QueryNode &source_node,
                          const QueryNode &target_node) -> bool;
};

}  // namespace orkhestrafs::dbmstodspi