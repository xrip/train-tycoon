// Player economy: money, loans, and the calendar (see game/economy_tick.cpp).
#ifndef ECONOMY_H
#define ECONOMY_H
#include <array>
#include <cstdint>

// Economy owns the player's money, loan state, tax/interest parameters, and the
// game calendar. Values and save byte order are unchanged; fields are inherited
// as-is from the prior GameCanvas layout. Revenue history, station revenue, and
// report aggregates move into Economy in a later ECON-01 leaf.
struct Economy {
    int32_t money = 0;
    int32_t loanBalance = 0;
    int32_t loanCeiling = 0;
    int8_t interestRate = 0;
    int32_t dayOfMonth = 0;
    int32_t monthOfYear = 0;
    int32_t year = 0;
    int32_t enterpriseTaxPct = 0;

    // Spending and revenue aggregates (lifetime and per-period totals).
    int32_t totalSpent = 0;
    int32_t aiMoneySpent = 0;
    int32_t prevAiMoneySpent = 0;
    int32_t totalRevenue = 0;
    int32_t prevTotalRevenue = 0;
    int32_t revenueTotal = 0;
    int32_t lifetimeTrainRevenue = 0;
    int32_t trainRevenueScore = 0;

    // Per-station revenue history and the yearly/decade report series used by
    // the graphs. moneyHistory and decadeRevenueWithCurrent are runtime-only
    // (not saved). Values and save byte order are unchanged.
    std::array<int16_t, 48> stationRevenue{};
    std::array<int16_t, 48> prevStationRevenue{};
    int32_t decadeRevenueWithCurrent = 0;
    int32_t previousYearProfit = 0;
    int32_t prevDecadeRevenue = 0;
    int32_t decadeRevenueTotal = 0;
    int32_t decadeProfitGraphTotal = 0;
    std::array<int32_t, 12> yearProfitHistory{};
    std::array<int32_t, 12> yearRevenueHistory{};
    std::array<int32_t, 12> yearCompanyValueHistory{};
    std::array<int32_t, 12> decadeProfitHistory{};
    std::array<int32_t, 12> altDecadeRevenueTotal{};
    std::array<int32_t, 3> moneyHistory{};
};

#endif  // ECONOMY_H
