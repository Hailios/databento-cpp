#pragma once

#include <cstdint>  // uint32_t

#include "databento/dbn.hpp"  // Metadata
#include "databento/iwritable.hpp"
#include "databento/record.hpp"
#include "databento/with_ts_out.hpp"

namespace databento {
class DbnEncoder {
 public:
  explicit DbnEncoder(const Metadata& metadata, IWritable* output);

  static void EncodeMetadata(const Metadata& metadata, IWritable* output);
  static void EncodeRecord(const Record& record, IWritable* output);

  template <typename R>
  void EncodeRecord(const R& record) {
    static_assert(
        has_header_v<R>,
        "must be a DBN record struct with an `hd` RecordHeader field");
    EncodeRecord(Record{&record.hd});
  }
  template <typename R>
  void EncodeRecord(const WithTsOut<R> record) {
    static_assert(
        has_header_v<R>,
        "must be a DBN record struct with an `hd` RecordHeader field");
    EncodeRecord(Record{&record.rec.hd});
  }
  void EncodeRecord(const Record& record);

 private:
  static std::uint32_t CalcLength(const Metadata& metadata);

  IWritable* output_;
};
}  // namespace databento