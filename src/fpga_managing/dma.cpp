#include "dma.hpp"

#include <cmath>
#include <iostream>

// Input Controller
void DMA::SetInputControllerParams(int stream_id, int ddr_burst_size,
                                   int records_per_ddr_burst, int buffer_start,
                                   int buffer_end) {
  AccelerationModule::WriteToModule(
      (1 << 6) + stream_id * 4,
      ((ddr_burst_size - 1) << 24) +
          (static_cast<int>(log2(records_per_ddr_burst)) << 16) +
          (buffer_start << 8) + (buffer_end));
}
auto DMA::GetInputControllerParams(int stream_id) -> volatile uint32_t {
  return AccelerationModule::ReadFromModule((1 << 6) + stream_id * 4);
}
void DMA::SetInputControllerStreamAddress(int stream_id, uintptr_t address) {
  AccelerationModule::WriteToModule(((2 << 6) + (stream_id * 4)), address >> 4);
}
auto DMA::GetInputControllerStreamAddress(int stream_id) -> volatile uintptr_t {
  return AccelerationModule::ReadFromModule((2 << 6) + (stream_id * 4)) << 4;
}
void DMA::SetInputControllerStreamSize(
    int stream_id, int size) {  // starting size of stream in amount of records
  AccelerationModule::WriteToModule(((3 << 6) + (stream_id * 4)), size);
}
auto DMA::GetInputControllerStreamSize(int stream_id) -> volatile int {
  return AccelerationModule::ReadFromModule(((3 << 6) + (stream_id * 4)));
}
void DMA::StartInputController(
    bool stream_active[16]) {  // indicate which streams can be read from DDR
                               // and start processing
  int active_streams = 0;
  for (int i = 15; i >= 0; i--) {
    active_streams = active_streams << 1;
    if (stream_active[i]) {
      active_streams = active_streams + 1;
    }
  }
  AccelerationModule::WriteToModule(0, active_streams);
}
auto DMA::IsInputControllerFinished()
    -> bool {  // true if all input streams were read from DDR
  int active_streams = AccelerationModule::ReadFromModule(3 << 6);
  return (active_streams == 0);
}
// Input Controller in Crossbar

// How many chunks is a record on a particular stream_id
void DMA::SetRecordSize(int stream_id, int record_size) {
  AccelerationModule::WriteToModule(((1 << 17) + (1 << 8) + (stream_id * 4)),
                                    record_size - 1);
}
// set ChunkID at clock cycle of interfaceCycle for records on a particular
// stream_id
void DMA::SetRecordChunkIDs(int stream_id, int interface_cycle, int chunk_id) {
  AccelerationModule::WriteToModule(
      ((1 << 17) + (1 << 13) + (stream_id << 8) + (interface_cycle << 2)),
      chunk_id);
}

// Output Controller
void DMA::SetOutputControllerParams(int stream_id, int ddr_burst_size,
                                    int records_per_ddr_burst, int buffer_start,
                                    int buffer_end) {
  AccelerationModule::WriteToModule(
      ((1 << 16) + (stream_id * 4)),
      ((ddr_burst_size - 1) << 24) +
          (static_cast<int>(log2(records_per_ddr_burst)) << 16) +
          (buffer_start << 8) + (buffer_end));
}
auto DMA::GetOutputControllerParams(int stream_id) -> volatile uint32_t {
  return AccelerationModule::ReadFromModule(((1 << 16) + (stream_id * 4)));
}
void DMA::SetOutputControllerStreamAddress(int stream_id, uintptr_t address) {
  AccelerationModule::WriteToModule(((1 << 16) + (1 << 6) + (stream_id * 4)),
                                    address >> 4);
}
auto DMA::GetOutputControllerStreamAddress(int stream_id)
    -> volatile uintptr_t {
  return AccelerationModule::ReadFromModule(
      ((1 << 16) + (1 << 6) + stream_id * 4));
}
void DMA::SetOutputControllerStreamSize(
    int stream_id, int size) {  // starting size of stream in amount of records
  AccelerationModule::WriteToModule(((1 << 16) + (2 << 6) + (stream_id * 4)),
                                    size);
}
auto DMA::GetOutputControllerStreamSize(int stream_id)
    -> volatile int {  // starting size of stream in amount of records
  return AccelerationModule::ReadFromModule(
      ((1 << 16) + (2 << 6) + (stream_id * 4)));
}
void DMA::StartOutputController(
    bool stream_active[16]) {  // indicate which streams can be written to DDR
                               // and start processing
  int active_streams = 0;
  for (int i = 15; i >= 0; i--) {
    active_streams = active_streams << 1;
    if (stream_active[i]) {
      active_streams = active_streams + 1;
    }
  }
  AccelerationModule::WriteToModule(((1 << 16) + (3 << 6)), active_streams);
}
auto DMA::IsOutputControllerFinished()
    -> bool {  // true if all streams saw EOS from PR modules
  int active_streams =
      AccelerationModule::ReadFromModule(((1 << 16) + (3 << 6)));
  return (active_streams == 0);
}

// Input Crossbar from Buffers to Interface
void DMA::SetBufferToInterfaceChunk(int stream_id, int clock_cycle, int offset,
                                    int source_chunk4, int source_chunk3,
                                    int source_chunk2, int source_chunk1) {
  /*When 32-bit data packets inside the {clockCycle} clock cycle of a record
  sent to PR Interface of stream with ID {stream_id} is read from the BRAM
  buffers in positions  {offset*4}-{offset*4+3}, they can read from any source
  chunk to enable data reordering and duplication. sourceChunk1 represents
  position {offset*4} etc.*/
  AccelerationModule::WriteToModule(
      ((2 << 17) + (1 << 16) + (stream_id << 12) + (clock_cycle << 5) +
       (offset << 2)),
      ((source_chunk4 << 24) + (source_chunk3 << 16) + (source_chunk2 << 8) +
       source_chunk1));
}
void DMA::SetBufferToInterfaceSourcePosition(int stream_id, int clock_cycle,
                                             int offset, int source_position4,
                                             int source_position3,
                                             int source_position2,
                                             int source_position1) {
  /*When 32-bit data packets inside the {clockCycle} clock cycle of a record
  sent to PR Interface of stream with ID {stream_id} is sent to PR at 32-bit
  data positions {offset*4}-{offset*4+3}, they can originate from from any
  source 32-bit BRAM to enable data reordering and duplication. sourceChunk1
  represents position {offset*4} etc..*/
  AccelerationModule::WriteToModule(
      ((2 << 17) + (stream_id << 12) + (clock_cycle << 5) + (offset << 2)),
      ((source_position4 << 24) + (source_position3 << 16) +
       (source_position2 << 8) + source_position1));
}

// Output Crossbar from Interface to Buffers
void DMA::SetInterfaceToBufferChunk(int stream_id, int clock_cycle, int offset,
                                    int target_chunk4, int target_chunk3,
                                    int target_chunk2, int target_chunk1) {
  /*When 32-bit data packets inside the {clockCycle} clock cycle of a record
  coming from the end of the PR Interface of stream with ID {stream_id} is
  written to the BRAM buffers in positions  {offset*4}-{offset*4+3}, they can be
  written to any target chunk to enable data reordering/removal of duplication
  before writing back to DDR. sourceChunk1 represents position {offset*4} etc.*/
  AccelerationModule::WriteToModule(
      ((3 << 17) + (1 << 16) + (stream_id << 12) + (clock_cycle << 5) +
       (offset << 2)),
      ((target_chunk4 << 24) + (target_chunk3 << 16) + (target_chunk2 << 8) +
       target_chunk1));
}
void DMA::SetInterfaceToBufferSourcePosition(int stream_id, int clock_cycle,
                                             int offset, int source_position4,
                                             int source_position3,
                                             int source_position2,
                                             int source_position1) {
  /*When 32-bit data packets inside the {clockCycle} clock cycle of a record
  coming from PR Interface of stream with ID {stream_id} is written to BRAM
  buffers at 32-bit data positions {offset*4}-{offset*4+3}, they can originate
  from from any source 32-bit word from the interface to enable data reordering
  and duplication. sourceChunk1 represents position {offset*4} etc..*/
  AccelerationModule::WriteToModule(
      ((3 << 17) + (stream_id << 12) + (clock_cycle << 5) + (offset << 2)),
      ((source_position4 << 24) + (source_position3 << 16) +
       (source_position2 << 8) + source_position1));
}

// MultiChannel
void DMA::SetNumberOfInputStreamsWithMultipleChannels(
    int number) {  // Number of special channeled streams (for example for merge
                   // sorting) These streams would be located at StreamIDs
                   // 0..(number-1)
  AccelerationModule::WriteToModule(4, number);
}

void DMA::SetRecordsPerBurstForMultiChannelStreams(
    int stream_id, int records_per_burst) {  // possible values 1-32
  AccelerationModule::WriteToModule(0x80000 + (stream_id * 4),
                                    records_per_burst);
}

void DMA::SetDDRBurstSizeForMultiChannelStreams(
    int stream_id, int ddr_burst_size) {
  AccelerationModule::WriteToModule(0x80000 + (1 << 6) + (stream_id * 4),
                                    ddr_burst_size - 1);
}

void DMA::SetNumberOfActiveChannelsForMultiChannelStreams(
    int stream_id,
    int active_channels) {  // possible values 1 to the synthesized channel
                            // capacity (1024 currently)
  AccelerationModule::WriteToModule(0x80000 + (2 << 6) + (stream_id * 4),
                                    active_channels);
}

void DMA::SetAddressForMultiChannelStreams(int stream_id, int channel_id,
                                           uintptr_t address) {
  AccelerationModule::WriteToModule(
      0x80000 + (1 << 16) + (stream_id << 14) + (channel_id << 2),
      address >> 4);
}
void DMA::SetSizeForMultiChannelStreams(int stream_id, int channel_id,
                                        int number_of_records) {
  AccelerationModule::WriteToModule(
      0x80000 + (2 << 16) + (stream_id << 14) + (channel_id << 2),
      number_of_records + 1);
}

auto DMA::GetRuntime() -> volatile uint64_t {
  auto high = AccelerationModule::ReadFromModule(0x8000);
  auto low = AccelerationModule::ReadFromModule(0x8004);
  return ((static_cast<uint64_t>(high)) << 32) | (static_cast<uint64_t>(low));
}
auto DMA::GetValidReadCyclesCount() -> volatile uint64_t {
  auto high = AccelerationModule::ReadFromModule(0x8008);
  auto low = AccelerationModule::ReadFromModule(0x800c);
  return ((static_cast<uint64_t>(high)) << 32) | (static_cast<uint64_t>(low));
}
auto DMA::GetValidWriteCyclesCount() -> volatile uint64_t {
  auto high = AccelerationModule::ReadFromModule(0x8010);
  auto low = AccelerationModule::ReadFromModule(0x8014);
  return ((static_cast<uint64_t>(high)) << 32) | (static_cast<uint64_t>(low));
}

const int kResetDuration = 8;

void DMA::GlobalReset() {
  AccelerationModule::WriteToModule(8, kResetDuration);
}
