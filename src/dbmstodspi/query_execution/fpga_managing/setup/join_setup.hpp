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
#include "join_interface.hpp"

namespace orkhestrafs::dbmstodspi {

/**
 * @brief Class to calculate the join module configuration data and write the
 * data to the registers.
 */
class JoinSetup {
 private:
  static void SetupTimeMultiplexer(JoinInterface& join_module,
                                   int first_stream_size,
                                   int second_stream_size, int shift_size);

 public:
  /**
   * @brief Method to setup the join module.
   *
   * Elements from the first stream come first and the streams will be joined
   * with the first element of both records. The second stream will have to be
   * shifted to avoid overwriting data.
   * @param join_module Join instance to access memory mapped configuration
   * registers.
   * @param first_input_stream_id ID of first input stream.
   * @param first_input_record_size How many integers worth of data is there in
   * the first stream.
   * @param second_input_stream_id ID of the second input stream.
   * @param second_input_record_size How many integers worth of data is there in
   * the second stream.
   * @param output_stream_id Output stream ID.
   * @param output_chunks_per_record How many chunks will be used for the output
   * stream.
   * @param shift_size How much is the second stream shifted.
   */
  static void SetupJoinModule(JoinInterface& join_module,
                              int first_input_stream_id,
                              int first_input_record_size,
                              int second_input_stream_id,
                              int second_input_record_size,
                              int output_stream_id,
                              int output_chunks_per_record, int shift_size);
};

}  // namespace orkhestrafs::dbmstodspi