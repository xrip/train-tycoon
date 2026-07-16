// Player economy: money, loans, and the calendar (see game/economy_tick.cpp).
#ifndef ECONOMY_H
#define ECONOMY_H
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
};

#endif  // ECONOMY_H
