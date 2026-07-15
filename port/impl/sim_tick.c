// port/impl/sim_tick.c - hand-ported simulation tick helpers.
#include "game_methods.h"

// void dayResetP()
void gc_dayResetP(GameCanvas *gc) {
    if (gc->monthOfYear % 4 == 0) {
        for (int i = 0; i < 64; ++i) {
            if (gc->constructionTimers[i] > 0)
                --gc->constructionTimers[i];
        }
    }
}

// void advanceConstructionTimers(int,int,int)
void gc_advanceConstructionTimers(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3) {
    bool seen[64] = {false};
    for (int i = n - 5; i <= n + 5; ++i) {
        for (int j = n2 - 5; j <= n2 + 5; ++j) {
            if (i < 0 || j < 0 || i >= gc->mapWidth || j >= gc->mapHeight
                || (gc->trackMap[i][j] & 0xC0) != 128)
                continue;
            int n4 = gc->trackMap[i][j] & 0x3F;
            if (seen[n4]) continue;
            seen[n4] = true;
            int n5;
            if (n3 < 0) {
                n5 = gc->constructionTimers[n4] * n3 * 3 / 200 - 1;
                if (n5 < n3) n3 = n5;
            } else {
                n5 = (100 - gc->constructionTimers[n4]) / 30 * n3 / 3 + 1;
                if (n5 > n3) n5 = n3;
            }
            gc->constructionTimers[n4] = (int8_t)(gc->constructionTimers[n4] + n5);
            if (gc->constructionTimers[n4] < 0) gc->constructionTimers[n4] = 0;
            if (gc->constructionTimers[n4] > 100) gc->constructionTimers[n4] = 100;
        }
    }
}

// void maybeAITick()
void gc_maybeAITick(GameCanvas *gc) {
    for (int n = 0; n < 64; ++n) gc->zonePopulation[n] = 0;
    for (int n = 0; n < gc->mapWidth; ++n) {
        for (int i = 0; i < gc->mapHeight; ++i) {
            if (gc->tileMap[n][i] < 34 || gc->tileMap[n][i] > 40) continue;
            int n2 = gc->trackMap[n][i] & 0x3F;
            ++gc->zonePopulation[n2];
        }
    }
}

// void dayResetAg()
void gc_dayResetAg(GameCanvas *gc) {
    for (int i = 0; i < 48; ++i) {
        gc->cityPopulation[i] += (int8_t)(midp_random_next_int(&gc->rng) % 8);
        if (gc->cityPopulation[i] < 10) gc->cityPopulation[i] = 10;
        if (gc->cityPopulation[i] > 90) gc->cityPopulation[i] = 90;
    }
}

// void revenueOrYear()
void gc_revenueOrYear(GameCanvas *gc) {
    for (int n = 0; n < 500; ++n) ++gc->trainAgeOrTimer[n];
}

// void everyTwoTick()
void gc_everyTwoTick(GameCanvas *gc) {
    if (gc->routeResourceFlags[0] != 0) return;
    for (int i = 0; i < 500; ++i) {
        if ((gc->trainFlags[i] & 0x80) != 128 || (gc->trainFlags[i] & 0x40) != 64)
            continue;
        int n2 = gc_c(gc, gc->trainX[i]);
        int n = gc_c(gc, gc->trainY[i]);
        if (gc->trackMap[n2][n] != 65) continue;
        int n3 = gc_countCarsInTrain(gc, i) * 2;
        if ((gc->trainFlags[i] & 0xF) == 0) n3 += 3;
        else if ((gc->trainFlags[i] & 0xF) == 1) n3 += 7;
        gc->trainRevenue[i] -= n3;
        gc->money -= n3;
        gc->fuelCostAccrued += n3;
    }
}

// int countCarsInTrain(int)
int32_t gc_countCarsInTrain(GameCanvas *gc, int32_t n) {
    return n != -1 ? gc_countCarsInTrain(gc, gc->nextCar[n]) + 1 : 0;
}

// void maybeAiTakeTurn()
void gc_maybeAiTakeTurn(GameCanvas *gc) {
    if (gc->buildMenuMode != 2 && gc->buildMenuMode != 4
        && midp_random_next_int(&gc->rng) % gc->aiFrequency > gc->aiThreshold)
        gc_setState(gc, 105);
}

// void dayResetAd()
void gc_dayResetAd(GameCanvas *gc) {
    for (int i = 0; i < 500; ++i) {
        if ((gc->trainFlags[i] & 0x80) != 128 || (gc->trainFlags[i] & 0x40) != 64)
            continue;
        int n2 = gc_c(gc, gc->trainX[i]);
        int n = gc_c(gc, gc->trainY[i]);
        if ((gc->trackMap[n2][n] != 66 && gc->trackMap[n2][n] != 67
             && gc->trackMap[n2][n] != 68)
            || gc->trainEraOrStage[i] >= gc->era)
            continue;
        gc->trainEraOrStage[i] = (int8_t)gc->era;
        gc->trainFlags[i] = (int8_t)(0xC0 | gc->era / 3);
        gc->wagonEnergyRemaining[i] = (int8_t)(8 - gc->era);
        break;
    }
}

// void postRevenueReset()
void gc_postRevenueReset(GameCanvas *gc) {
    for (int i = 0; i < 500; ++i) {
        if ((gc->trainFlags[i] & 0x80) != 128) continue;
        int n2 = gc_c(gc, gc->trainX[i]);
        int n = gc_c(gc, gc->trainY[i]);
        if (gc->trackMap[n2][n] == 66 || gc->trackMap[n2][n] == 67
            || gc->trackMap[n2][n] == 68)
            gc->trainAgeOrTimer[i] = 0;
    }
}
