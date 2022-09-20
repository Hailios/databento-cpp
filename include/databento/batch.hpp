#pragma once

#include <chrono>
#include <cstddef>
#include <string>
#include <vector>

#include "databento/enums.hpp"

namespace databento {
struct BatchJob {
  std::string id;
  std::string user_id;
  std::string bill_id;
  std::string dataset;
  std::vector<std::string> symbols;
  SType stype_in;
  SType stype_out;
  Schema schema;
  std::chrono::system_clock::duration start;
  std::chrono::system_clock::duration end;
  std::size_t limit;
  Compression compression;
  DurationInterval split_duration;
  std::size_t split_size;
  bool split_symbols;
  Packaging packaging;
  Delivery delivery;
  bool is_full_book;
  bool is_example;
  std::size_t record_count;
  std::size_t billed_size;
  std::size_t actual_size;
  std::size_t package_size;
  BatchState state;
  std::chrono::system_clock ts_received;
  std::chrono::system_clock ts_queued;
  std::chrono::system_clock ts_process_start;
  std::chrono::system_clock ts_process_done;
  std::chrono::system_clock ts_expiration;
};
}  // namespace databento