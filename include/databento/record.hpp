#pragma once

#include <array>
#include <chrono>  // nanoseconds
#include <cstddef>
#include <cstdint>
#include <cstring>  // strncmp
#include <string>
#include <tuple>  // tie

#include "databento/datetime.hpp"  // UnixNanos
#include "databento/enums.hpp"
#include "databento/flag_set.hpp"  // FlagSet

namespace databento {
// Common data for all Databento Records.
struct RecordHeader {
  static constexpr std::size_t kLengthMultiplier = 4;

  // The length of the message in 32-bit words.
  std::uint8_t length;
  // The record type.
  RType rtype;
  // The publisher ID assigned by Databento.
  std::uint16_t publisher_id;
  // The numeric ID assigned to the instrument.
  std::uint32_t instrument_id;
  // The exchange timestamp in UNIX epoch nanoseconds.
  UnixNanos ts_event;

  std::size_t Size() const;
};

class Record {
 public:
  explicit Record(RecordHeader* record) : record_{record} {}

  const RecordHeader& Header() const { return *record_; }

  template <typename T>
  bool Holds() const {
    return T::HasRType(record_->rtype);
  }

  template <typename T>
  const T& Get() const {
    return *reinterpret_cast<const T*>(record_);
  }
  template <typename T>
  T& Get() {
    return *reinterpret_cast<T*>(record_);
  }

  std::size_t Size() const;
  static std::size_t SizeOfSchema(Schema schema);
  static RType RTypeFromSchema(Schema schema);

 private:
  RecordHeader* record_;
};

// Market-by-order (MBO) message.
struct MboMsg {
  static bool HasRType(RType rtype) { return rtype == RType::Mbo; }

  RecordHeader hd;
  std::uint64_t order_id;
  std::int64_t price;
  std::uint32_t size;
  FlagSet flags;
  std::uint8_t channel_id;
  Action action;
  Side side;
  UnixNanos ts_recv;
  TimeDeltaNanos ts_in_delta;
  std::uint32_t sequence;
};

static_assert(sizeof(MboMsg) == 56, "MboMsg size must match C");

struct BidAskPair {
  std::int64_t bid_px;
  std::int64_t ask_px;
  std::uint32_t bid_sz;
  std::uint32_t ask_sz;
  std::uint32_t bid_ct;
  std::uint32_t ask_ct;
};

static_assert(sizeof(BidAskPair) == 32, "BidAskPair size must match C");

namespace detail {
template <std::size_t N>
struct MbpMsg {
  static_assert(N <= 15, "The maximum number of levels in an MbpMsg is 15");

  static bool HasRType(RType rtype) {
    return static_cast<std::uint8_t>(rtype) == N;
  }

  RecordHeader hd;
  std::int64_t price;
  std::uint32_t size;
  Action action;
  Side side;
  FlagSet flags;
  // Depth of the actual book change.
  std::uint8_t depth;
  UnixNanos ts_recv;
  TimeDeltaNanos ts_in_delta;
  std::uint32_t sequence;
  std::array<BidAskPair, N> levels;
};

}  // namespace detail

struct TradeMsg {
  static bool HasRType(RType rtype) { return rtype == RType::Mbp0; }

  RecordHeader hd;
  std::int64_t price;
  std::uint32_t size;
  Action action;
  Side side;
  FlagSet flags;
  // Depth of the actual book change.
  std::uint8_t depth;
  UnixNanos ts_recv;
  TimeDeltaNanos ts_in_delta;
  std::uint32_t sequence;
  // 0-sized types don't exist in C++ so levels is omitted
};

using Mbp1Msg = detail::MbpMsg<1>;
using TbboMsg = Mbp1Msg;
using Mbp10Msg = detail::MbpMsg<10>;

static_assert(sizeof(TradeMsg) == 48, "TradeMsg size must match C");
static_assert(sizeof(Mbp1Msg) == sizeof(TradeMsg) + sizeof(BidAskPair),
              "Mbp1Msg size must match C");

// Aggregate of open, high, low, and close prices with volume.
struct OhlcvMsg {
  static bool HasRType(RType rtype) {
    switch (rtype) {
      case RType::OhlcvDeprecated:  // fallthrough
      case RType::Ohlcv1S:          // fallthrough
      case RType::Ohlcv1M:          // fallthrough
      case RType::Ohlcv1H:          // fallthrough
      case RType::Ohlcv1D:
        return true;
      default:
        return false;
    }
  }

  RecordHeader hd;
  std::int64_t open;
  std::int64_t high;
  std::int64_t low;
  std::int64_t close;
  std::uint64_t volume;
};

static_assert(sizeof(OhlcvMsg) == 56, "OhlcvMsg size must match C");

// Instrument definition.
struct InstrumentDefMsg {
  static bool HasRType(RType rtype) { return rtype == RType::InstrumentDef; }

  RecordHeader hd;
  UnixNanos ts_recv;
  std::int64_t min_price_increment;
  std::int64_t display_factor;
  UnixNanos expiration;
  UnixNanos activation;
  std::int64_t high_limit_price;
  std::int64_t low_limit_price;
  std::int64_t max_price_variation;
  std::int64_t trading_reference_price;
  std::int64_t unit_of_measure_qty;
  std::int64_t min_price_increment_amount;
  std::int64_t price_ratio;
  std::int32_t inst_attrib_value;
  std::uint32_t underlying_id;
  std::array<char, 4> _reserved1;
  std::int32_t market_depth_implied;
  std::int32_t market_depth;
  std::uint32_t market_segment_id;
  std::uint32_t max_trade_vol;
  std::int32_t min_lot_size;
  std::int32_t min_lot_size_block;
  std::int32_t min_lot_size_round_lot;
  std::uint32_t min_trade_vol;
  std::array<char, 4> _reserved2;
  std::int32_t contract_multiplier;
  std::int32_t decay_quantity;
  std::int32_t original_contract_size;
  std::array<char, 4> _reserved3;
  std::uint16_t trading_reference_date;
  std::int16_t appl_id;
  std::uint16_t maturity_year;
  std::uint16_t decay_start_date;
  std::uint16_t channel_id;
  std::array<char, 4> currency;
  std::array<char, 4> settl_currency;
  std::array<char, 6> secsubtype;
  std::array<char, 22> raw_symbol;
  std::array<char, 21> group;
  std::array<char, 5> exchange;
  std::array<char, 7> asset;
  std::array<char, 7> cfi;
  std::array<char, 7> security_type;
  std::array<char, 31> unit_of_measure;
  std::array<char, 21> underlying;
  std::array<char, 4> strike_price_currency;
  InstrumentClass instrument_class;
  std::array<char, 2> _reserved4;
  std::int64_t strike_price;
  std::array<char, 6> _reserved5;
  MatchAlgorithm match_algorithm;
  std::uint8_t md_security_trading_status;
  std::uint8_t main_fraction;
  std::uint8_t price_display_format;
  std::uint8_t settl_price_type;
  std::uint8_t sub_fraction;
  std::uint8_t underlying_product;
  SecurityUpdateAction security_update_action;
  std::uint8_t maturity_month;
  std::uint8_t maturity_day;
  std::uint8_t maturity_week;
  UserDefinedInstrument user_defined_instrument;
  std::int8_t contract_multiplier_unit;
  std::int8_t flow_schedule_type;
  std::uint8_t tick_rule;
  // padding for alignment
  std::array<char, 3> dummy;
};

static_assert(sizeof(InstrumentDefMsg) == 360,
              "InstrumentDefMsg size must match C");

// An order imbalance message.
struct ImbalanceMsg {
  static bool HasRType(RType rtype) { return rtype == RType::Imbalance; }

  RecordHeader hd;
  UnixNanos ts_recv;
  std::int64_t ref_price;
  UnixNanos auction_time;
  std::int64_t cont_book_clr_price;
  std::int64_t auct_interest_clr_price;
  std::int64_t ssr_filling_price;
  std::int64_t ind_match_price;
  std::int64_t upper_collar;
  std::int64_t lower_collar;
  std::uint32_t paired_qty;
  std::uint32_t total_imbalance_qty;
  std::uint32_t market_imbalance_qty;
  std::uint32_t unpaired_qty;
  char auction_type;
  Side side;
  std::uint8_t auction_status;
  std::uint8_t freeze_status;
  std::uint8_t num_extensions;
  Side unpaired_side;
  char significant_imbalance;
  // padding for alignment
  std::array<char, 1> dummy;
};

static_assert(sizeof(ImbalanceMsg) == 112, "ImbalanceMsg size must match C");

/// A statistics message. A catchall for various data disseminated by
/// publishers. The `stat_type` indicates the statistic contained in the
/// message.
struct StatMsg {
  static bool HasRType(RType rtype) { return rtype == RType::Statistics; }

  RecordHeader hd;
  UnixNanos ts_recv;
  UnixNanos ts_ref;
  std::int64_t price;
  std::int32_t quantity;
  std::uint32_t sequence;
  TimeDeltaNanos ts_in_delta;
  StatType stat_type;
  std::uint16_t channel_id;
  StatUpdateAction update_action;
  std::uint8_t stat_flags;
  std::array<char, 6> dummy;
};

static_assert(sizeof(StatMsg) == 64, "StatMsg size must match C");

// An error message from the Live Subscription Gateway (LSG). This will never
// be present in historical data.
struct ErrorMsg {
  static bool HasRType(RType rtype) { return rtype == RType::Error; }

  const char* Err() const { return err.data(); }

  RecordHeader hd;
  std::array<char, 64> err;
};

/// A symbol mapping message.
struct SymbolMappingMsg {
  static bool HasRType(RType rtype) { return rtype == RType::SymbolMapping; }

  RecordHeader hd;
  std::array<char, 22> stype_in_symbol;
  std::array<char, 22> stype_out_symbol;
  // padding for alignment
  std::array<char, 4> dummy;
  UnixNanos start_ts;
  UnixNanos end_ts;
};

struct SystemMsg {
  static bool HasRType(RType rtype) { return rtype == RType::System; }

  const char* Msg() const { return msg.data(); }
  bool IsHeartbeat() const {
    return std::strncmp(msg.data(), "Heartbeat", 9) == 0;
  }

  RecordHeader hd;
  std::array<char, 64> msg;
};

inline bool operator==(const RecordHeader& lhs, const RecordHeader& rhs) {
  return lhs.length == rhs.length && lhs.rtype == rhs.rtype &&
         lhs.publisher_id == rhs.publisher_id &&
         lhs.instrument_id == rhs.instrument_id && lhs.ts_event == rhs.ts_event;
}
inline bool operator!=(const RecordHeader& lhs, const RecordHeader& rhs) {
  return !(lhs == rhs);
}

inline bool operator==(const MboMsg& lhs, const MboMsg& rhs) {
  return lhs.hd == rhs.hd && lhs.order_id == rhs.order_id &&
         lhs.price == rhs.price && lhs.size == rhs.size &&
         lhs.flags == rhs.flags && lhs.channel_id == rhs.channel_id &&
         lhs.action == rhs.action && lhs.side == rhs.side &&
         lhs.ts_recv == rhs.ts_recv && lhs.ts_in_delta == rhs.ts_in_delta &&
         lhs.sequence == rhs.sequence;
}
inline bool operator!=(const MboMsg& lhs, const MboMsg& rhs) {
  return !(lhs == rhs);
}

inline bool operator==(const BidAskPair& lhs, const BidAskPair& rhs) {
  return lhs.bid_px == rhs.bid_px && lhs.ask_px == rhs.ask_px &&
         lhs.bid_sz == rhs.bid_sz && lhs.ask_sz == rhs.ask_sz &&
         lhs.bid_ct == rhs.bid_ct && lhs.ask_ct == rhs.ask_ct;
}
inline bool operator!=(const BidAskPair& lhs, const BidAskPair& rhs) {
  return !(lhs == rhs);
}

namespace detail {
template <std::size_t N>
bool operator==(const MbpMsg<N>& lhs, const MbpMsg<N>& rhs) {
  return lhs.hd == rhs.hd && lhs.price == rhs.price && lhs.size == rhs.size &&
         lhs.action == rhs.action && lhs.side == rhs.side &&
         lhs.flags == rhs.flags && lhs.depth == rhs.depth &&
         lhs.ts_recv == rhs.ts_recv && lhs.ts_in_delta == rhs.ts_in_delta &&
         lhs.sequence == rhs.sequence && lhs.levels == rhs.levels;
}
template <std::size_t N>
bool operator!=(const MbpMsg<N>& lhs, const MbpMsg<N>& rhs) {
  return !(lhs == rhs);
}

template <std::size_t N>
std::string ToString(const MbpMsg<N>& mbp_msg);
template <>
std::string ToString(const Mbp1Msg& mbp_msg);
template <>
std::string ToString(const Mbp10Msg& mbp_msg);

template <std::size_t N>
std::ostream& operator<<(std::ostream& stream, const MbpMsg<N>& mbp_msg);
template <>
std::ostream& operator<<(std::ostream& stream, const Mbp1Msg& mbp_msg);
template <>
std::ostream& operator<<(std::ostream& stream, const Mbp10Msg& mbp_msg);
}  // namespace detail

inline bool operator==(const TradeMsg& lhs, const TradeMsg& rhs) {
  return lhs.hd == rhs.hd && lhs.price == rhs.price && lhs.size == rhs.size &&
         lhs.action == rhs.action && lhs.side == rhs.side &&
         lhs.flags == rhs.flags && lhs.depth == rhs.depth &&
         lhs.ts_recv == rhs.ts_recv && lhs.ts_in_delta == rhs.ts_in_delta &&
         lhs.sequence == rhs.sequence;
}
inline bool operator!=(const TradeMsg& lhs, const TradeMsg& rhs) {
  return !(lhs == rhs);
}

inline bool operator==(const OhlcvMsg& lhs, const OhlcvMsg& rhs) {
  return lhs.hd == rhs.hd && lhs.open == rhs.open && lhs.high == rhs.high &&
         lhs.low == rhs.low && lhs.close == rhs.close &&
         lhs.volume == rhs.volume;
}
inline bool operator!=(const OhlcvMsg& lhs, const OhlcvMsg& rhs) {
  return !(lhs == rhs);
}

bool operator==(const InstrumentDefMsg& lhs, const InstrumentDefMsg& rhs);
inline bool operator!=(const InstrumentDefMsg& lhs,
                       const InstrumentDefMsg& rhs) {
  return !(lhs == rhs);
}

bool operator==(const ImbalanceMsg& lhs, const ImbalanceMsg& rhs);
inline bool operator!=(const ImbalanceMsg& lhs, const ImbalanceMsg& rhs) {
  return !(lhs == rhs);
}

inline bool operator==(const StatMsg& lhs, const StatMsg& rhs) {
  return std::tie(lhs.hd, lhs.ts_recv, lhs.ts_ref, lhs.price, lhs.quantity,
                  lhs.sequence, lhs.ts_in_delta, lhs.stat_type, lhs.channel_id,
                  lhs.update_action, lhs.stat_flags) ==
         std::tie(rhs.hd, rhs.ts_recv, rhs.ts_ref, rhs.price, rhs.quantity,
                  rhs.sequence, rhs.ts_in_delta, rhs.stat_type, rhs.channel_id,
                  rhs.update_action, rhs.stat_flags);
}
inline bool operator!=(const StatMsg& lhs, const StatMsg& rhs) {
  return !(lhs == rhs);
}

inline bool operator==(const ErrorMsg& lhs, const ErrorMsg& rhs) {
  return lhs.hd == rhs.hd && lhs.err == rhs.err;
}
inline bool operator!=(const ErrorMsg& lhs, const ErrorMsg& rhs) {
  return !(lhs == rhs);
}

inline bool operator==(const SystemMsg& lhs, const SystemMsg& rhs) {
  return lhs.hd == rhs.hd && lhs.msg == rhs.msg;
}
inline bool operator!=(const SystemMsg& lhs, const SystemMsg& rhs) {
  return !(lhs == rhs);
}

inline bool operator==(const SymbolMappingMsg& lhs,
                       const SymbolMappingMsg& rhs) {
  return lhs.hd == rhs.hd && lhs.stype_in_symbol == rhs.stype_in_symbol &&
         lhs.stype_out_symbol == rhs.stype_out_symbol &&
         lhs.start_ts == rhs.start_ts && lhs.end_ts == rhs.end_ts;
}
inline bool operator!=(const SymbolMappingMsg& lhs,
                       const SymbolMappingMsg& rhs) {
  return !(lhs == rhs);
}

std::string ToString(const RecordHeader& header);
std::ostream& operator<<(std::ostream& stream, const RecordHeader& header);
std::string ToString(const MboMsg& mbo_msg);
std::ostream& operator<<(std::ostream& stream, const MboMsg& mbo_msg);
std::string ToString(const BidAskPair& ba_pair);
std::ostream& operator<<(std::ostream& stream, const BidAskPair& ba_pair);
std::string ToString(const TradeMsg& trade_msg);
std::ostream& operator<<(std::ostream& stream, const TradeMsg& trade_msg);
std::string ToString(const OhlcvMsg& ohlcv_msg);
std::ostream& operator<<(std::ostream& stream, const OhlcvMsg& ohlcv_msg);
std::string ToString(const InstrumentDefMsg& instr_def_msg);
std::ostream& operator<<(std::ostream& stream,
                         const InstrumentDefMsg& instr_def_msg);
std::string ToString(const ImbalanceMsg& imbalance_msg);
std::ostream& operator<<(std::ostream& stream,
                         const ImbalanceMsg& imbalance_msg);
std::string ToString(const StatMsg& stat_msg);
std::ostream& operator<<(std::ostream& stream, const StatMsg& stat_msg);
std::string ToString(const ErrorMsg& err_msg);
std::ostream& operator<<(std::ostream& stream, const ErrorMsg& err_msg);
std::string ToString(const SystemMsg& system_msg);
std::ostream& operator<<(std::ostream& stream, const SystemMsg& system_msg);
std::string ToString(const SymbolMappingMsg& symbol_mapping_msg);
std::ostream& operator<<(std::ostream& stream,
                         const SymbolMappingMsg& symbol_mapping_msg);
}  // namespace databento
