#pragma once

#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace databento {
// Sentinel value for requesting all symbols
static const std::vector<std::string> kAllSymbols{"ALL_SYMBOLS"};

struct StrMappingInterval {
  // YYYY-MM-DD
  std::string start_date;
  // YYYY-MM-DD
  std::string end_date;
  std::string symbol;
};

struct SymbologyResolution {
  std::unordered_map<std::string, std::vector<StrMappingInterval>> mappings;
  std::vector<std::string> partial;
  std::vector<std::string> not_found;
};

// Converts a vector of symbols to a comma-delineated string for sending to
// Databento's historical and live APIs.
//
// Throws InvalidArgumentError if symbols is empty or the iterator range is
// empty.
std::string JoinSymbolStrings(
    const std::string& method_name,
    std::vector<std::string>::const_iterator symbols_begin,
    std::vector<std::string>::const_iterator symbols_end);
std::string JoinSymbolStrings(const std::string& method_name,
                              const std::vector<std::string>& symbols);
std::string ToString(const StrMappingInterval& mapping_interval);
std::string ToString(const SymbologyResolution& sym_res);
std::ostream& operator<<(std::ostream& stream,
                         const StrMappingInterval& mapping_interval);
std::ostream& operator<<(std::ostream& stream,
                         const SymbologyResolution& sym_res);
}  // namespace databento
