#pragma once

#include <cstdint>
#include <limits>

namespace databento {
static constexpr auto kApiVersion = 0;
static constexpr auto kApiVersionStr = "0";
static constexpr auto kApiKeyLength = 32;
// The decimal scaler of fixed prices.
static constexpr std::int64_t kFixedPriceScale = 1000000000;
// The sentinel value for a null or undefined price.
static constexpr auto kUndefPrice = std::numeric_limits<std::int64_t>::max();
// The sentinel value for a null or undefined order size.
static constexpr auto kUndefOrderSize =
    std::numeric_limits<std::uint32_t>::max();

// This is not necessarily a comprehensive list of available datasets. Please
// use `Historical.MetadataListDatasets` to retrieve an up-to-date list.
namespace dataset {
// The dataset code for Databento Equities Basic.
static constexpr auto kDbeqBasic = "DBEQ.BASIC";
// The dataset code for CME Globex MDP 3.0.
static constexpr auto kGlbxMdp3 = "GLBX.MDP3";
// The dataset code for OPRA.PILLAR.
static constexpr auto kOpraPillar = "OPRA.PILLAR";
// The dataset code for Nasdaq TotalView ITCH.
static constexpr auto kXnasItch = "XNAS.ITCH";
}  // namespace dataset
}  // namespace databento
