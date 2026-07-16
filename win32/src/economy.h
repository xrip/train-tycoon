// Player economy: money, loans, and the calendar (see game/economy_tick.cpp).
#ifndef ECONOMY_H
#define ECONOMY_H
#include <array>
#include <cstdint>

// Economy owns the player's money, loan state, tax/interest parameters, and the
// game calendar. Values and save byte order are unchanged; fields are inherited
// as-is from the prior Game layout. Revenue history, station revenue, and
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

    // Era-scaled cost and train-performance model, loaded per scenario in map
    // generation and nudged by the yearly random economy event. moneyBase* are
    // the fare base; baseTrainSpeed/accel/brake are the two engine performance
    // tiers; cost* and gWagonCost*/costGroupC* are the build and wagon prices.
    // All saved as one consecutive block; save byte order is unchanged.
    int32_t moneyBaseA = 0;
    int32_t moneyBaseB = 0;
    int32_t baseTrainSpeed = 0;
    int32_t accel1 = 0;
    int32_t brake1 = 0;
    int32_t baseTrainSpeed2 = 0;
    int32_t accel2 = 0;
    int32_t costPower = 0;
    int32_t costStruct = 0;
    int32_t costOverhead = 0;
    int32_t costMaintenance = 0;
    int32_t costCrew = 0;
    int32_t gWagonCost = 0;
    int32_t gWagonCost2 = 0;
    int32_t gWagonCost3 = 0;
    int32_t gWagonCost4 = 0;
    int32_t gWagonCost5 = 0;
    int32_t gWagonCost6 = 0;
    int32_t costGroupC1 = 0;
    int32_t costGroupC2 = 0;
    int32_t costGroupC3 = 0;
    int32_t costGroupC4 = 0;

    // Period/decade financial report aggregates. The "WithCurrent" running
    // accumulators (and legacyDecadeMetric) are runtime-only and are not saved;
    // all other fields here are saved. Values and save byte order are unchanged.
    int32_t fuelCostAccrued = 0;
    int32_t subsidyCost = 0;
    int32_t terrainTotal = 0;
    int32_t decadeEnterpriseTax = 0;
    int32_t netProfit = 0;
    int32_t decadeSpentWithCurrent = 0;
    int32_t decadeAiSpentWithCurrent = 0;
    int32_t decadeFuelWithCurrent = 0;
    int32_t decadeSubsidyWithCurrent = 0;
    int32_t legacyDecadeMetric = 0;
    int32_t decadeEnterpriseWithCurrent = 0;
    int32_t decadeProfitWithCurrent = 0;
    int32_t prevTotalSpent = 0;
    int32_t prevFuelCost = 0;
    int32_t prevSubsidyCost = 0;
    int32_t prevTerrainTotal = 0;
    int32_t prevEnterpriseTotal = 0;
    int32_t prevDecadeSpent = 0;
    int32_t prevDecadeAiSpent = 0;
    int32_t prevDecadeFuel = 0;
    int32_t prevDecadeSubsidy = 0;
    int32_t previousLegacyDecadeMetric = 0;
    int32_t previousDecadeEnterpriseTax = 0;
    int32_t previousDecadeProfit = 0;
    int32_t decadeCostTotal = 0;
    int32_t decadeSpentTotal = 0;
    int32_t decadeFuelCost = 0;
    int32_t decadeSubsidyTotal = 0;
    int32_t savedLegacyMetricEb = 0;
    int32_t decadeEnterpriseTotal = 0;
    int32_t decadeProfitTotal = 0;
    std::array<int32_t, 12> altDecadeTotalCost{};
};

#endif  // ECONOMY_H
