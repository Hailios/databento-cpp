#pragma once

#include <cstddef>  // size_t
#include <cstdint>
#include <map>  // map, multimap
#include <string>
#include <vector>

#include "databento/batch.hpp"     // BatchJob
#include "databento/datetime.hpp"  // DateRange, DateTimeRange, UnixNanos
#include "databento/dbn_file_store.hpp"
#include "databento/detail/http_client.hpp"  // HttpClient
#include "databento/enums.hpp"  // BatchState, Delivery, DurationInterval, Packaging, Schema, SType
#include "databento/metadata.hpp"  // DatasetConditionDetail, DatasetRange, FieldsByDatasetEncodingAndSchema, PriceByFeedMode, PriceByFeedModeAndSchema, PriceBySchema
#include "databento/symbology.hpp"  // SymbologyResolution
#include "databento/timeseries.hpp"  // KeepGoing, MetadataCallback, RecordCallback

namespace databento {
// A client for interfacing with Databento's historical market data API.
class Historical {
 public:
  Historical(std::string key, HistoricalGateway gateway);
  // Primarily for unit tests
  Historical(std::string key, std::string gateway, std::uint16_t port);

  /*
   * Getters
   */

  const std::string& Key() const { return key_; };
  const std::string& Gateway() const { return gateway_; }

  /*
   * Batch API
   */

  BatchJob BatchSubmitJob(const std::string& dataset,
                          const std::vector<std::string>& symbols,
                          Schema schema,
                          const DateTimeRange<UnixNanos>& datetime_range);
  BatchJob BatchSubmitJob(const std::string& dataset,
                          const std::vector<std::string>& symbols,
                          Schema schema,
                          const DateTimeRange<std::string>& datetime_range);
  BatchJob BatchSubmitJob(const std::string& dataset,
                          const std::vector<std::string>& symbols,
                          Schema schema,
                          const DateTimeRange<UnixNanos>& datetime_range,
                          Compression compression, SplitDuration split_duration,
                          std::size_t split_size, Packaging packaging,
                          Delivery delivery, SType stype_in, SType stype_out,
                          std::size_t limit);
  BatchJob BatchSubmitJob(const std::string& dataset,
                          const std::vector<std::string>& symbols,
                          Schema schema,
                          const DateTimeRange<std::string>& datetime_range,
                          Compression compression, SplitDuration split_duration,
                          std::size_t split_size, Packaging packaging,
                          Delivery delivery, SType stype_in, SType stype_out,
                          std::size_t limit);
  std::vector<BatchJob> BatchListJobs();
  std::vector<BatchJob> BatchListJobs(const std::vector<JobState>& states,
                                      UnixNanos since);
  std::vector<BatchJob> BatchListJobs(const std::vector<JobState>& states,
                                      const std::string& since);
  std::vector<BatchFileDesc> BatchListFiles(const std::string& job_id);
  // Returns the paths of the downloaded files.
  std::vector<std::string> BatchDownload(const std::string& output_dir,
                                         const std::string& job_id);
  // Returns the path of the downloaded file.
  std::string BatchDownload(const std::string& output_dir,
                            const std::string& job_id,
                            const std::string& filename_to_download);

  /*
   * Metadata API
   */

  // Retrievs a mapping of publisher name to publisher ID.
  std::map<std::string, std::int32_t> MetadataListPublishers();
  std::vector<std::string> MetadataListDatasets();
  std::vector<std::string> MetadataListDatasets(const DateRange& date_range);
  std::vector<Schema> MetadataListSchemas(const std::string& dataset);
  FieldsByDatasetEncodingAndSchema MetadataListFields();
  FieldsByDatasetEncodingAndSchema MetadataListFields(
      const std::string& dataset);
  FieldsByDatasetEncodingAndSchema MetadataListFields(
      const std::string& dataset, Encoding encoding, Schema schema);
  PriceByFeedModeAndSchema MetadataListUnitPrices(const std::string& dataset);
  PriceBySchema MetadataListUnitPrices(const std::string& dataset,
                                       FeedMode mode);
  PriceByFeedMode MetadataListUnitPrices(const std::string& dataset,
                                         Schema schema);
  double MetadataListUnitPrices(const std::string& dataset, FeedMode mode,
                                Schema schema);
  std::vector<DatasetConditionDetail> MetadataGetDatasetCondition(
      const std::string& dataset);
  std::vector<DatasetConditionDetail> MetadataGetDatasetCondition(
      const std::string& dataset, const DateRange& date_range);
  DatasetRange MetadataGetDatasetRange(const std::string& dataset);
  std::size_t MetadataGetRecordCount(
      const std::string& dataset,
      const DateTimeRange<UnixNanos>& datetime_range,
      const std::vector<std::string>& symbols, Schema schema);
  std::size_t MetadataGetRecordCount(
      const std::string& dataset,
      const DateTimeRange<std::string>& datetime_range,
      const std::vector<std::string>& symbols, Schema schema);
  std::size_t MetadataGetRecordCount(
      const std::string& dataset,
      const DateTimeRange<UnixNanos>& datetime_range,
      const std::vector<std::string>& symbols, Schema schema, SType stype_in,
      std::size_t limit);
  std::size_t MetadataGetRecordCount(
      const std::string& dataset,
      const DateTimeRange<std::string>& datetime_range,
      const std::vector<std::string>& symbols, Schema schema, SType stype_in,
      std::size_t limit);
  std::size_t MetadataGetBillableSize(
      const std::string& dataset,
      const DateTimeRange<UnixNanos>& datetime_range,
      const std::vector<std::string>& symbols, Schema schema);
  std::size_t MetadataGetBillableSize(
      const std::string& dataset,
      const DateTimeRange<std::string>& datetime_range,
      const std::vector<std::string>& symbols, Schema schema);
  std::size_t MetadataGetBillableSize(
      const std::string& dataset,
      const DateTimeRange<UnixNanos>& datetime_range,
      const std::vector<std::string>& symbols, Schema schema, SType stype_in,
      std::size_t limit);
  std::size_t MetadataGetBillableSize(
      const std::string& dataset,
      const DateTimeRange<std::string>& datetime_range,
      const std::vector<std::string>& symbols, Schema schema, SType stype_in,
      std::size_t limit);
  double MetadataGetCost(const std::string& dataset,
                         const DateTimeRange<UnixNanos>& datetime_range,
                         const std::vector<std::string>& symbols,
                         Schema schema);
  double MetadataGetCost(const std::string& dataset,
                         const DateTimeRange<std::string>& datetime_range,
                         const std::vector<std::string>& symbols,
                         Schema schema);
  double MetadataGetCost(const std::string& dataset,
                         const DateTimeRange<UnixNanos>& datetime_range,
                         const std::vector<std::string>& symbols, Schema schema,
                         FeedMode mode, SType stype_in, std::size_t limit);
  double MetadataGetCost(const std::string& dataset,
                         const DateTimeRange<std::string>& datetime_range,
                         const std::vector<std::string>& symbols, Schema schema,
                         FeedMode mode, SType stype_in, std::size_t limit);

  /*
   * Symbology API
   */

  SymbologyResolution SymbologyResolve(const std::string& dataset,
                                       const std::vector<std::string>& symbols,
                                       SType stype_in, SType stype_out,
                                       const DateRange& date_range);
  SymbologyResolution SymbologyResolve(const std::string& dataset,
                                       const std::vector<std::string>& symbols,
                                       SType stype_in, SType stype_out,
                                       const DateRange& date_range,
                                       const std::string& default_value);

  /*
   * Timeseries API
   */

  // Stream historical market data to `record_callback`. This method will
  // return only after all data has been returned or `record_callback` returns
  // `KeepGoing::Stop`.
  //
  // NOTE: This method spawns a thread, however, the callbacks will be called
  // from the current thread.
  void TimeseriesGetRange(const std::string& dataset,
                          const DateTimeRange<UnixNanos>& datetime_range,
                          const std::vector<std::string>& symbols,
                          Schema schema, const RecordCallback& record_callback);
  void TimeseriesGetRange(const std::string& dataset,
                          const DateTimeRange<std::string>& datetime_range,
                          const std::vector<std::string>& symbols,
                          Schema schema, const RecordCallback& record_callback);
  // Stream historical market data to `record_callback`. `metadata_callback`
  // will be called exactly once, before any calls to `record_callback`.
  // This method will return only after all data has been returned or
  // `record_callback` returns `KeepGoing::Stop`.
  //
  // NOTE: This method spawns a thread, however, the callbacks will be called
  // from the current thread.
  void TimeseriesGetRange(const std::string& dataset,
                          const DateTimeRange<UnixNanos>& datetime_range,
                          const std::vector<std::string>& symbols,
                          Schema schema, SType stype_in, SType stype_out,
                          std::size_t limit,
                          const MetadataCallback& metadata_callback,
                          const RecordCallback& record_callback);
  void TimeseriesGetRange(const std::string& dataset,
                          const DateTimeRange<std::string>& datetime_range,
                          const std::vector<std::string>& symbols,
                          Schema schema, SType stype_in, SType stype_out,
                          std::size_t limit,
                          const MetadataCallback& metadata_callback,
                          const RecordCallback& record_callback);
  // Stream historical market data to a file at `path`. Returns a `DbnFileStore`
  // object for replaying the data in `file_path`.
  //
  // If a file at `file_path` already exists, it will be overwritten.
  DbnFileStore TimeseriesGetRangeToFile(
      const std::string& dataset,
      const DateTimeRange<UnixNanos>& datetime_range,
      const std::vector<std::string>& symbols, Schema schema,
      const std::string& file_path);
  DbnFileStore TimeseriesGetRangeToFile(
      const std::string& dataset,
      const DateTimeRange<std::string>& datetime_range,
      const std::vector<std::string>& symbols, Schema schema,
      const std::string& file_path);
  DbnFileStore TimeseriesGetRangeToFile(
      const std::string& dataset,
      const DateTimeRange<UnixNanos>& datetime_range,
      const std::vector<std::string>& symbols, Schema schema, SType stype_in,
      SType stype_out, std::size_t limit, const std::string& file_path);
  DbnFileStore TimeseriesGetRangeToFile(
      const std::string& dataset,
      const DateTimeRange<std::string>& datetime_range,
      const std::vector<std::string>& symbols, Schema schema, SType stype_in,
      SType stype_out, std::size_t limit, const std::string& file_path);

 private:
  using HttplibParams = std::multimap<std::string, std::string>;

  BatchJob BatchSubmitJob(const HttplibParams& params);
  void DownloadFile(const std::string& url, const std::string& output_path);
  std::vector<BatchJob> BatchListJobs(const HttplibParams& params);
  std::vector<DatasetConditionDetail> MetadataGetDatasetCondition(
      const HttplibParams& params);
  std::size_t MetadataGetRecordCount(const HttplibParams& params);
  std::size_t MetadataGetBillableSize(const HttplibParams& params);
  double MetadataGetCost(const HttplibParams& params);
  FieldsByDatasetEncodingAndSchema MetadataListFields(
      const HttplibParams& params);
  void TimeseriesGetRange(const HttplibParams& params,
                          const MetadataCallback& metadata_callback,
                          const RecordCallback& record_callback);
  DbnFileStore TimeseriesGetRangeToFile(const HttplibParams& params,
                                        const std::string& file_path);

  const std::string key_;
  const std::string gateway_;
  detail::HttpClient client_;
};

// A helper class for constructing an instance of Historical.
class HistoricalBuilder {
 public:
  HistoricalBuilder() = default;

  // Sets `key_` based on the environment variable DATABENTO_API_KEY.
  //
  // NOTE: This is not thread-safe if `std::setenv` is used elsewhere in the
  // program.
  HistoricalBuilder& SetKeyFromEnv();
  HistoricalBuilder& SetKey(std::string key);
  HistoricalBuilder& SetGateway(HistoricalGateway gateway);
  // Attempts to construct an instance of Historical or throws an exception if
  // no key has been set.
  Historical Build();

 private:
  std::string key_;
  HistoricalGateway gateway_{HistoricalGateway::Bo1};
};
}  // namespace databento
