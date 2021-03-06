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
#include "linear_sort_interface.hpp"

namespace orkhestrafs::dbmstodspi {

/**
 * @brief Linear sort setup class which will calculate the configuration data to
 * setup the module
 */
class LinearSortSetup {
 private:
 public:
  /**
   * @brief Setup linear sort module by giving the stream data to be sorted.
   * @param linear_sort_module Module instance to access the configuration
   * registers.
   * @param stream_id ID of the stream to be sorted.
   * @param record_size How many integers worth of data does a record have.
   */
  static void SetupLinearSortModule(LinearSortInterface& linear_sort_module,
                                    int stream_id, int record_size);
};

}  // namespace orkhestrafs::dbmstodspi