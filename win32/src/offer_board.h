#ifndef OFFER_BOARD_H
#define OFFER_BOARD_H

#include "game_constants.h"
#include <cstdint>

struct Economy;
class JavaRandom;

struct Offer {
    int8_t objectiveType = 0;
    int8_t issuedDay = 0;
    int8_t dueDay = 0;
    int8_t issuedMonth = 0;
    int8_t dueMonth = 0;
    int8_t issuedYear = 0;
    int8_t dueYear = 0;
    int32_t target = 0;
    int32_t progress = 0;
    int8_t effectType = 0;
    int8_t effectValue = 0;
    int32_t effectAmount = 0;
};

class OfferBoard {
public:
    int8_t display_progress(int32_t row, const Economy& economy) const;
    void schedule_due_date(int32_t row, int32_t duration_units,
                           JavaRandom& random);
    void copy_to_summary_and_clear(int32_t row);

    Offer offers[kMenuRowCount]{};
};

#endif  // OFFER_BOARD_H
