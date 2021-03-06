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

#include "dma_crossbar_setup.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <string>

#include "dma_setup_data.hpp"
#include "query_acceleration_constants.hpp"
namespace {
using orkhestrafs::dbmstodspi::DMACrossbarSetup;
using orkhestrafs::dbmstodspi::DMASetupData;
const int kDatapathLength =
    orkhestrafs::dbmstodspi::query_acceleration_constants::kDatapathLength;
const int kDatapathWidth =
    orkhestrafs::dbmstodspi::query_acceleration_constants::kDatapathWidth;

void GetGoldenConfigFromFile(std::vector<std::vector<int>>& golden_config,
                             const std::string& file_name) {
  std::ifstream input_file(file_name);
  ASSERT_TRUE(input_file);

  std::string line;
  while (std::getline(input_file, line)) {
    std::istringstream string_stream(line);
    int config_value = 0;
    std::vector<int> current_cycle_golden_config;
    while (string_stream >> config_value) {
      current_cycle_golden_config.push_back(config_value);
    }
    golden_config.push_back(current_cycle_golden_config);
  }
}

auto CreateLinearSelectedColumnsVector(const int vector_size)
    -> std::vector<int> {
  std::vector<int> selected_column(vector_size);
  std::generate(selected_column.begin(), selected_column.end(),
                [n = 0]() mutable { return n++; });
  return selected_column;
}

void ExpectConfigurationDataIsUnconfigured(
    const DMASetupData& configuration_data) {
  for (int clock_cycle_index = 0; clock_cycle_index < kDatapathLength;
       clock_cycle_index++) {
    EXPECT_THAT(configuration_data.crossbar_setup_data.size(), testing::Eq(0));
  }
}

void ExpectConfigurationDataIsConfigured(
    DMASetupData configuration_data, const std::string& golden_chunk_data_file,
    const std::string& golden_position_data_file) {
  std::vector<std::vector<int>> golden_chunk_config;
  GetGoldenConfigFromFile(golden_chunk_config, golden_chunk_data_file);
  std::vector<std::vector<int>> golden_position_config;
  GetGoldenConfigFromFile(golden_position_config, golden_position_data_file);
  for (int clock_cycle_index = 0; clock_cycle_index < kDatapathLength;
       clock_cycle_index++) {
    EXPECT_THAT(
        configuration_data.crossbar_setup_data[clock_cycle_index]
            .chunk_selection,
        testing::ElementsAreArray(golden_chunk_config[clock_cycle_index]))
        << "Vectors differ at clock cycle: " << clock_cycle_index;
    EXPECT_THAT(
        configuration_data.crossbar_setup_data[clock_cycle_index]
            .position_selection,
        testing::ElementsAreArray(golden_position_config[clock_cycle_index]))
        << "Vectors differ at clock cycle: " << clock_cycle_index;
  }
}

TEST(DMACrossbarSetupTest, RecordSize18BufferToInterfaceSetupCheck) {
  DMASetupData test_stream_setup_data;
  test_stream_setup_data.is_input_stream = true;
  test_stream_setup_data.active_channel_count = -1;
  test_stream_setup_data.chunks_per_record = 2;
  test_stream_setup_data.records_per_ddr_burst = 16;
  ExpectConfigurationDataIsUnconfigured(test_stream_setup_data);

  DMACrossbarSetup::CalculateCrossbarSetupData(
      test_stream_setup_data, 18, CreateLinearSelectedColumnsVector(18));

  ExpectConfigurationDataIsConfigured(
      test_stream_setup_data,
      "DMACrossbarSetupTest/RecordSize18BufferToInterfaceChunkSetup.txt",
      "DMACrossbarSetupTest/RecordSize18BufferToInterfacePositionSetup.txt");
}

TEST(DMACrossbarSetupTest, RecordSize18InterfaceToBufferSetupCheck) {
  DMASetupData test_stream_setup_data;
  test_stream_setup_data.is_input_stream = false;
  test_stream_setup_data.chunks_per_record = 2;
  test_stream_setup_data.records_per_ddr_burst = 16;
  ExpectConfigurationDataIsUnconfigured(test_stream_setup_data);

  DMACrossbarSetup::CalculateCrossbarSetupData(
      test_stream_setup_data, 18, CreateLinearSelectedColumnsVector(18));

  ExpectConfigurationDataIsConfigured(
      test_stream_setup_data,
      "DMACrossbarSetupTest/RecordSize18InterfaceToBufferChunkSetup.txt",
      "DMACrossbarSetupTest/RecordSize18InterfaceToBufferPositionSetup.txt");
}

TEST(DMACrossbarSetupTest, RecordSize4BufferToInterfaceSetupCheck) {
  DMASetupData test_stream_setup_data;
  test_stream_setup_data.is_input_stream = true;
  test_stream_setup_data.active_channel_count = -1;
  test_stream_setup_data.chunks_per_record = 1;
  test_stream_setup_data.records_per_ddr_burst = 32;
  ExpectConfigurationDataIsUnconfigured(test_stream_setup_data);

  DMACrossbarSetup::CalculateCrossbarSetupData(
      test_stream_setup_data, 4, CreateLinearSelectedColumnsVector(4));

  ExpectConfigurationDataIsConfigured(
      test_stream_setup_data,
      "DMACrossbarSetupTest/RecordSize4BufferToInterfaceChunkSetup.txt",
      "DMACrossbarSetupTest/RecordSize4BufferToInterfacePositionSetup.txt");
}

TEST(DMACrossbarSetupTest, RecordSize4InterfaceToBufferSetupCheck) {
  DMASetupData test_stream_setup_data;
  test_stream_setup_data.is_input_stream = false;
  test_stream_setup_data.chunks_per_record = 1;
  test_stream_setup_data.records_per_ddr_burst = 32;
  ExpectConfigurationDataIsUnconfigured(test_stream_setup_data);

  DMACrossbarSetup::CalculateCrossbarSetupData(
      test_stream_setup_data, 4, CreateLinearSelectedColumnsVector(4));

  ExpectConfigurationDataIsConfigured(
      test_stream_setup_data,
      "DMACrossbarSetupTest/RecordSize4InterfaceToBufferChunkSetup.txt",
      "DMACrossbarSetupTest/RecordSize4InterfaceToBufferPositionSetup.txt");
}

TEST(DMACrossbarSetupTest, RecordSize46BufferToInterfaceSetupCheck) {
  DMASetupData test_stream_setup_data;
  test_stream_setup_data.is_input_stream = true;
  test_stream_setup_data.active_channel_count = -1;
  test_stream_setup_data.chunks_per_record = 3;
  test_stream_setup_data.records_per_ddr_burst = 8;
  ExpectConfigurationDataIsUnconfigured(test_stream_setup_data);

  DMACrossbarSetup::CalculateCrossbarSetupData(
      test_stream_setup_data, 46, CreateLinearSelectedColumnsVector(46));

  ExpectConfigurationDataIsConfigured(
      test_stream_setup_data,
      "DMACrossbarSetupTest/RecordSize46BufferToInterfaceChunkSetup.txt",
      "DMACrossbarSetupTest/RecordSize46BufferToInterfacePositionSetup.txt");
}

TEST(DMACrossbarSetupTest, RecordSize46InterfaceToBufferSetupCheck) {
  DMASetupData test_stream_setup_data;
  test_stream_setup_data.is_input_stream = false;
  test_stream_setup_data.chunks_per_record = 3;
  test_stream_setup_data.records_per_ddr_burst = 8;
  ExpectConfigurationDataIsUnconfigured(test_stream_setup_data);

  DMACrossbarSetup::CalculateCrossbarSetupData(
      test_stream_setup_data, 46, CreateLinearSelectedColumnsVector(46));

  ExpectConfigurationDataIsConfigured(
      test_stream_setup_data,
      "DMACrossbarSetupTest/RecordSize46InterfaceToBufferChunkSetup.txt",
      "DMACrossbarSetupTest/RecordSize46InterfaceToBufferPositionSetup.txt");
}

TEST(DMACrossbarSetupTest, RecordSize57BufferToInterfaceSetupCheck) {
  DMASetupData test_stream_setup_data;
  test_stream_setup_data.is_input_stream = true;
  test_stream_setup_data.active_channel_count = -1;
  test_stream_setup_data.chunks_per_record = 4;
  test_stream_setup_data.records_per_ddr_burst = 8;
  ExpectConfigurationDataIsUnconfigured(test_stream_setup_data);

  DMACrossbarSetup::CalculateCrossbarSetupData(
      test_stream_setup_data, 57, CreateLinearSelectedColumnsVector(57));

  ExpectConfigurationDataIsConfigured(
      test_stream_setup_data,
      "DMACrossbarSetupTest/RecordSize57BufferToInterfaceChunkSetup.txt",
      "DMACrossbarSetupTest/RecordSize57BufferToInterfacePositionSetup.txt");
}

TEST(DMACrossbarSetupTest, RecordSize57InterfaceToBufferSetupCheck) {
  DMASetupData test_stream_setup_data;
  test_stream_setup_data.is_input_stream = false;
  test_stream_setup_data.chunks_per_record = 4;
  test_stream_setup_data.records_per_ddr_burst = 8;
  ExpectConfigurationDataIsUnconfigured(test_stream_setup_data);

  DMACrossbarSetup::CalculateCrossbarSetupData(
      test_stream_setup_data, 57, CreateLinearSelectedColumnsVector(57));

  ExpectConfigurationDataIsConfigured(
      test_stream_setup_data,
      "DMACrossbarSetupTest/RecordSize57InterfaceToBufferChunkSetup.txt",
      "DMACrossbarSetupTest/RecordSize57InterfaceToBufferPositionSetup.txt");
}

TEST(DMACrossbarSetupTest, RecordSize478BufferToInterfaceSetupCheck) {
  DMASetupData test_stream_setup_data;
  test_stream_setup_data.is_input_stream = true;
  test_stream_setup_data.active_channel_count = -1;
  test_stream_setup_data.chunks_per_record = 30;
  test_stream_setup_data.records_per_ddr_burst = 1;
  ExpectConfigurationDataIsUnconfigured(test_stream_setup_data);

  DMACrossbarSetup::CalculateCrossbarSetupData(
      test_stream_setup_data, 478, CreateLinearSelectedColumnsVector(478));

  ExpectConfigurationDataIsConfigured(
      test_stream_setup_data,
      "DMACrossbarSetupTest/RecordSize478BufferToInterfaceChunkSetup.txt",
      "DMACrossbarSetupTest/RecordSize478BufferToInterfacePositionSetup.txt");
}

TEST(DMACrossbarSetupTest, RecordSize478InterfaceToBufferSetupCheck) {
  DMASetupData test_stream_setup_data;
  test_stream_setup_data.is_input_stream = false;
  test_stream_setup_data.chunks_per_record = 30;
  test_stream_setup_data.records_per_ddr_burst = 1;
  ExpectConfigurationDataIsUnconfigured(test_stream_setup_data);

  DMACrossbarSetup::CalculateCrossbarSetupData(
      test_stream_setup_data, 478, CreateLinearSelectedColumnsVector(478));

  ExpectConfigurationDataIsConfigured(
      test_stream_setup_data,
      "DMACrossbarSetupTest/RecordSize478InterfaceToBufferChunkSetup.txt",
      "DMACrossbarSetupTest/RecordSize478InterfaceToBufferPositionSetup.txt");
}

TEST(DMACrossbarSetupTest, RecordSize80BufferToInterfaceSetupCheck) {
  DMASetupData test_stream_setup_data;
  test_stream_setup_data.is_input_stream = true;
  test_stream_setup_data.active_channel_count = -1;
  test_stream_setup_data.chunks_per_record = 5;
  test_stream_setup_data.records_per_ddr_burst = 4;
  ExpectConfigurationDataIsUnconfigured(test_stream_setup_data);

  DMACrossbarSetup::CalculateCrossbarSetupData(
      test_stream_setup_data, 80, CreateLinearSelectedColumnsVector(80));

  ExpectConfigurationDataIsConfigured(
      test_stream_setup_data,
      "DMACrossbarSetupTest/RecordSize80BufferToInterfaceChunkSetup.txt",
      "DMACrossbarSetupTest/RecordSize80BufferToInterfacePositionSetup.txt");
}

TEST(DMACrossbarSetupTest, RecordSize80InterfaceToBufferSetupCheck) {
  DMASetupData test_stream_setup_data;
  test_stream_setup_data.is_input_stream = false;
  test_stream_setup_data.chunks_per_record = 5;
  test_stream_setup_data.records_per_ddr_burst = 4;
  ExpectConfigurationDataIsUnconfigured(test_stream_setup_data);

  DMACrossbarSetup::CalculateCrossbarSetupData(
      test_stream_setup_data, 80, CreateLinearSelectedColumnsVector(80));

  ExpectConfigurationDataIsConfigured(
      test_stream_setup_data,
      "DMACrossbarSetupTest/RecordSize80InterfaceToBufferChunkSetup.txt",
      "DMACrossbarSetupTest/RecordSize80InterfaceToBufferPositionSetup.txt");
}

TEST(DMACrossbarSetupTest, InterfaceToBufferWithOverwritesSetup) {
  DMASetupData test_stream_setup_data;
  test_stream_setup_data.is_input_stream = false;
  test_stream_setup_data.chunks_per_record = 2;
  test_stream_setup_data.records_per_ddr_burst = 16;
  ExpectConfigurationDataIsUnconfigured(test_stream_setup_data);

  const int any_record_size = -1;

  DMACrossbarSetup::CalculateCrossbarSetupData(
      test_stream_setup_data, any_record_size,
      {0,  1,  -1, 3,  4,  5,  22, 7,  -1, 25, 10, 11, 12, 13, 14, 15,
       16, 17, 18, 19, 20, 21, 6,  23, 24, 9,  26, 27, 28, -1, -1, 25});

  ExpectConfigurationDataIsConfigured(
      test_stream_setup_data,
      "DMACrossbarSetupTest/InterfaceToBufferWithOverwritesChunkSetup.txt",
      "DMACrossbarSetupTest/InterfaceToBufferWithOverwritesPositionSetup.txt");
}

}  // namespace
