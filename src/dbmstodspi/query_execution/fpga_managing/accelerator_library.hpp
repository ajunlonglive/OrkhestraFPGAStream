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
#include <map>

#include "acceleration_module_setup_interface.hpp"
#include "accelerator_library_interface.hpp"

namespace orkhestrafs::dbmstodspi {
/**
 * @brief Library to hold modules and their drivers.
 */
class AcceleratorLibrary : public AcceleratorLibraryInterface {
 public:
  ~AcceleratorLibrary() override = default;
  AcceleratorLibrary(
      MemoryManagerInterface* memory_manager_,
      std::unique_ptr<DMASetupInterface> dma_setup,
      std::map<QueryOperationType, std::unique_ptr<AccelerationModuleSetupInterface>>
          module_driver_library)
      : memory_manager_{memory_manager_},
        dma_setup_{std::move(dma_setup)},
        module_driver_library_{std::move(module_driver_library)} {};
  /**
   * @brief Setup a given operation with the given parameters.
   * @param node_parameters Parameters struct containing stream metadata and
   * resource elasticity information.
   */
  void SetupOperation(const AcceleratedQueryNode& node_parameters) override;
  /**
   * @brief Method to generate a DMA module for the FPGAManager to setup
   * execution.
   * @return DMA module class to access initialisation registers.
   */
  auto GetDMAModule() -> std::unique_ptr<DMAInterface> override;
  /**
   * @brief Check if the operation has readback modules and return them.
   * @return ReadBackModule to read if results have to be read from a register.
   */
  auto ExportLastModulesIfReadback()
      -> std::vector<std::unique_ptr<ReadBackModule>> override;
  /**
   * @brief Get the DMA module setup implementation
   * @return DMA driver setup class
   */
  auto GetDMAModuleSetup() -> DMASetupInterface& override;

  /**
   * @brief Get a pointer to an ILA module for debugging
   * @return Can return a nullpointer
   */
  auto GetILAModule() -> std::unique_ptr<ILA> override;

 private:
  std::unique_ptr<DMASetupInterface> dma_setup_;
  std::map<QueryOperationType,
           std::unique_ptr<AccelerationModuleSetupInterface>>
      module_driver_library_;
  MemoryManagerInterface* memory_manager_;
  std::vector<std::unique_ptr<AccelerationModule>> recent_setup_modules_;
};

}  // namespace orkhestrafs::dbmstodspi