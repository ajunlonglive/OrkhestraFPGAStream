#include "virtual_memory_block.hpp"

using namespace dbmstodspi::fpga_managing;

VirtualMemoryBlock::~VirtualMemoryBlock() = default;

auto VirtualMemoryBlock::GetVirtualAddress() -> volatile uint32_t* {
  return &memory_area_[0];
}

auto VirtualMemoryBlock::GetPhysicalAddress() -> volatile uint32_t* {
  return &memory_area_[0];
}

auto VirtualMemoryBlock::GetSize() -> const uint32_t {
  return memory_area_.size() * 4;
}
