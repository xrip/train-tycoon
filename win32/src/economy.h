// Player economy: money, loans, and the calendar (see game/economy_tick.cpp).
#ifndef ECONOMY_H
#define ECONOMY_H
#include "game_constants.h"
#include <cstdint>

// Economy owns the player's money, loan state, tax/interest parameters, and the
// game calendar. Values and save byte order are unchanged; fields are inherited
// as-is from the prior Game layout. Revenue history, station revenue, and
// report aggregates move into Economy in a later ECON-01 leaf.
struct Economy {
    bool spend(int32_t amount);
    void push_money_history(int32_t amount);
    void calculate_period_profit();
    void compute_year_end_metrics();
    int32_t train_cost_for_era(int32_t era) const;

    int32_t money = 200000;
    int32_t loanBalance = 0;
    int32_t loanCeiling = 600000;
    int8_t interestRate = 4;
    int32_t dayOfMonth = game_calendar::kFirstDay;
    int32_t monthOfYear = game_calendar::kFirstMonth;
    int32_t year = game_calendar::kBaseYear;
    int32_t enterpriseTaxPct = 2;

    // Spending and revenue aggregates (lifetime and per-period totals).
    int32_t totalSpent = 0;
    int32_t aiMoneySpent = 0;
    int32_t prevAiMoneySpent = 0;
    int32_t totalRevenue = 0;
    int32_t prevTotalRevenue = 0;
    int32_t revenueTotal = 0;
    int32_t lifetimeTrainRevenue = 0;
    int32_t trainRevenueScore = 0;

    // The yearly/decade report series used by the graphs. moneyHistory and
    // decadeRevenueWithCurrent are runtime-only (not saved). Per-station
    // revenue lives with each City. Values and save byte order are unchanged.
    int32_t decadeRevenueWithCurrent = 0;
    int32_t previousYearProfit = 0;
    int32_t prevDecadeRevenue = 0;
    int32_t decadeRevenueTotal = 0;
    int32_t decadeProfitGraphTotal = 0;
    int32_t yearProfitHistory[kHistoryCount]{};
    int32_t yearRevenueHistory[kHistoryCount]{};
    int32_t yearCompanyValueHistory[kHistoryCount]{};
    int32_t decadeProfitHistory[kHistoryCount]{};
    int32_t altDecadeRevenueTotal[kHistoryCount]{};
    int32_t moneyHistory[money_ticker::kSlotCount]{};

    // Era-scaled cost and train-performance model, loaded per scenario in map
    // generation and nudged by the yearly random economy event. The two
    // legacy money bases are kept as primary/secondary because their deeper
    // fare rules are not yet proved; the Early/Late fields are the two engine
    // performance tiers; the *Cost fields are build and wagon prices.
    // All saved as one consecutive block; save byte order is unchanged.
    int32_t moneyBasePrimary = 0;
    int32_t moneyBaseSecondary = 0;
    int32_t baseTrainSpeedEarly = 0;
    int32_t accelerationEarly = 0;
    int32_t brakingEarly = 0;
    int32_t baseTrainSpeedLate = 0;
    int32_t accelerationLate = 0;
    int32_t powerCost = 0;
    int32_t structureCost = 0;
    int32_t overheadCost = 0;
    int32_t maintenanceCost = 0;
    int32_t crewCost = 0;
    int32_t trainCostFirstEra = 0;
    int32_t trainCostSecondEra = 0;
    int32_t trainCostThirdEra = 0;
    int32_t trainCostFourthEra = 0;
    int32_t trainCostFifthEra = 0;
    int32_t trainCostSixthEra = 0;
    int32_t wagonOptionCostPassengerMail = 0;
    int32_t wagonOptionCostIndustryGroup1 = 0;
    int32_t wagonOptionCostIndustryGroup2 = 0;
    int32_t wagonOptionCostIndustryGroup3 = 0;

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
    int32_t altDecadeTotalCost[kHistoryCount]{};
};

#endif  // ECONOMY_H
