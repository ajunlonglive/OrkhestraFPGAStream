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
#include <cstdint>

#include "memory_manager_interface.hpp"

namespace orkhestrafs::dbmstodspi {

/**
 * @brief Base abstract class which all acceleration modules have to extend to
 * be able to access memory mapped registers
 */
class AccelerationModule {
 private:
  /// Memory manager instance to be able to access the register memory space.
  MemoryManagerInterface* memory_manager_;
  /// Location of the module on the FPGA.
  const int module_position_;

  /**
   * @brief Calculate where the virtual memory address is memory mapped to.
   *
   * The result depends on the module position.
   * @param module_internal_address Internal virtual address which is desired to
   * be accessed.
   * @return Global memory mapped address which can be read or written to.
   */
  auto CalculateMemoryMappedAddress(int module_internal_address)
      -> volatile uint32_t*;

 protected:
  /**
   * @brief Write data to a module configuration register.
   * @param module_internal_address Internal address of the register.
   * @param write_data Data to be written to the register.
   */
  void WriteToModule(int module_internal_address, uint32_t write_data);
  /**
   * @brief Read data from a module configuration register.
   * @param module_internal_address Internal address of the register
   * @return Data read from the register.
   */
  auto ReadFromModule(int module_internal_address) -> volatile uint32_t;
  /**
   * @brief Constructor to pass the memory manager instance and the module
   * position information.
   * @param memory_manager Memory manager instance to access memory mapped
   * registers.
   * @param module_position Integer showing the position of the module on the
   * FPGA.
   */
  AccelerationModule(MemoryManagerInterface* memory_manager,
                     int module_position)
      : memory_manager_(memory_manager), module_position_(module_position){};

 public:
  virtual ~AccelerationModule() = 0;
};

}  // namespace orkhestrafs::dbmstodspi