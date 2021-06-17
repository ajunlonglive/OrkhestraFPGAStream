#pragma once
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include <map>

namespace dbmstodspi::data_managing {

enum class ColumnDataType { kInteger, kVarchar, kNull, kDecimal, kDate };

const std::map<std::string, ColumnDataType> data_type_names = {
    {"integer", ColumnDataType::kInteger},
    {"varchar", ColumnDataType::kVarchar},
    {"null", ColumnDataType::kNull},
    {"decimal", ColumnDataType::kDecimal},
    {"date", ColumnDataType::kDate}};

/**
 * @brief Struct to hold the information about the table data.
 */
struct TableData {
  /// Vector to hold data about column data types and sizes.
  std::vector<std::pair<ColumnDataType, int>> table_column_label_vector;
  /// Vector to hold the table data in integer format.
  std::vector<uint32_t> table_data_vector;

  auto operator==(const TableData& comparable_table) const -> bool {
    const bool same_columns =
        table_column_label_vector == comparable_table.table_column_label_vector;
    const bool same_data =
        table_data_vector == comparable_table.table_data_vector;
    return same_columns && same_data;
  }
};

}  // namespace dbmstodspi::data_managing