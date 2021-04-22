#pragma once

namespace dbmstodspi {
namespace fpga_managing {
namespace modules {

/**
 * @brief Interface class to be implemented in #LinearSort
 */
class LinearSortInterface {
 public:
  virtual ~LinearSortInterface() = default;

  virtual void SetStreamParams(int stream_id, int record_size) = 0;
  virtual void StartPrefetchingData() = 0;
};

}  // namespace modules
}  // namespace fpga_managing
}  // namespace dbmstodspi