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

#include "multiplication.hpp"

using orkhestrafs::dbmstodspi::Multiplication;

void Multiplication::DefineActiveStreams(std::bitset<16> active_streams) {
  AccelerationModule::WriteToModule(0, active_streams.to_ulong());
}

void Multiplication::ChooseMultiplicationResults(
    int chunk_id, std::bitset<8> active_positions) {
  AccelerationModule::WriteToModule(128 + chunk_id * 4,
                                    active_positions.to_ulong());
}
