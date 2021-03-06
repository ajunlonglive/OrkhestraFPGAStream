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

#include <bitset>

namespace orkhestrafs::dbmstodspi {

/**
 * @brief Interface class which is implemented in the #Multiplication class.
 */
class MultiplicationInterface {
 public:
  virtual ~MultiplicationInterface() = default;

  virtual void DefineActiveStreams(std::bitset<16> active_streams) = 0;
  virtual void ChooseMultiplicationResults(int chunk_id,
                                           std::bitset<8> active_positions) = 0;
};

}  // namespace orkhestrafs::dbmstodspi