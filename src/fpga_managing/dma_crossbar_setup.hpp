#pragma once
#include <queue>
#include <vector>

#include "dma_setup_data.hpp"
class DMACrossbarSetup {
 public:
  static void CalculateCrossbarSetupData(
      DMASetupData& stream_setup_data, const int stream_size,
      const std::vector<int>& selected_columns);

 private:
  static auto GetReverseIndex(int index, int row_size) -> int;

  static void SetUpEmptyCrossbarSetupData(DMASetupData& stream_setup_data,
                                          const int required_chunk_count);

  static void FillSetupDataWithNegativePositions(
      DMASetupData& stream_setup_data);

  static void SetNextInputConfiguration(DMASetupData& stream_setup_data,
                                        const int current_location,
                                        const int target_location);
  static void SetNextOutputConfiguration(DMASetupData& stream_setup_data,
                                         const int current_location,
                                         const int target_location);

  static auto CreateFreeChunksVector() -> std::vector<int>;
  static void MarkChunksAsUsed(const DMASetupData& stream_setup_data,
                               const int column_id,
                               std::vector<int>& free_chunks);
  static void AllocateAvailableChunks(DMASetupData& stream_setup_data,
                                      const int column_id,
                                      const std::vector<int>& free_chunks);
  static void InsertMissingEmptySetupChunks(
      DMASetupData& stream_setup_data,
      const int missing_chunk_count_per_record);

  static void PrintCrossbarConfigData(
      const std::vector<int>& expanded_column_selection,
      const DMASetupData& stream_setup_data);

  static void ConfigureInputCrossbarSetupData(
      const std::vector<int>& selected_columns, DMASetupData& stream_setup_data,
      std::vector<int>& expanded_column_selection, const int& record_size);
  static void ConfigureOutputCrossbarSetupData(
      const std::vector<int>& selected_columns,
      std::vector<int>& expanded_column_selection,
      DMASetupData& stream_setup_data);
};
