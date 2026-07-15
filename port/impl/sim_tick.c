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

// boolean isTrainHeadCarAt(int,int,int)
bool gc_isTrainHeadCarAt(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3) {
    if (n == -1) return false;
    if (gc_c(gc, gc->trainX[n]) == n2 && gc_c(gc, gc->trainY[n]) == n3)
        return true;
    return gc_isTrainHeadCarAt(gc, gc->nextCar[n], n2, n3);
}

// void removeTrainFromMapInlined(int,boolean)
void gc_removeTrainFromMapInlined(GameCanvas *gc, int32_t n, bool bl) {
    if (n == -1) return;
    gc_removeTrainFromMapInlined(gc, gc->nextCar[n], bl);
    gc->trainFlags[n] = (int8_t)(gc->trainFlags[n] | 0xE0);
    gc->nextCar[n] = -1;
    gc->trainEnergyOrCount[n] = 7;
    if (bl)
        gc_handleCursorClickInlined(gc, gc_c(gc, gc->trainX[n]),
                                    gc_c(gc, gc->trainY[n]), false, true);
    gc->lastViewportCenterX = -1;
    gc->ready = false;
    gc->transitionCode = 16;
    gc_setState(gc, 20);
}

// void mergeTrainAt(int)
void gc_mergeTrainAt(GameCanvas *gc, int32_t n) {
    for (int i = 0; i < 500; ++i) {
        if (n == i || (gc->trainFlags[i] & 0x80) != 128
            || (gc->trainFlags[i] & 0x40) != 64
            || !gc_isTrainHeadCarAt(gc, i, gc_c(gc, gc->trainX[n]),
                                    gc_c(gc, gc->trainY[n])))
            continue;
        gc_removeTrainFromMapInlined(gc, i, true);
        gc_removeTrainFromMapInlined(gc, n, true);
    }
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

// void archiveOrRecall()
void gc_archiveOrRecall(GameCanvas *gc) {
    gc->terrainTotal = gc->loanBalance * gc->interestRate / 1200;
    gc->decadeEnterpriseTax = gc->totalRevenue * gc->enterpriseTaxPct / 100;
    gc->xIndicator = gc->totalRevenue - gc->terrainTotal - gc->totalSpent
                     - gc->aiMoneySpent - gc->fuelCostAccrued - gc->subsidyCost
                     - gc->decadeEnterpriseTax;
}

// void computeYearEndMetrics()
void gc_computeYearEndMetrics(GameCanvas *gc) {
    gc_archiveOrRecall(gc);
    gc->yrN[0] = gc->xIndicator;
    gc->yrCx[0] = gc->totalRevenue;
    gc->altDecadeOtherXTotal[0] = gc->decadeOtherXTotal + gc->xIndicator;
    gc->altDecadeRevenueTotal[0] = gc->revenueTotal + gc->totalRevenue;
    gc->decadeSpentWithCurrent = gc->decadeCostTotal + gc->totalSpent;
    gc->decadeAiSpentWithCurrent = gc->decadeSpentTotal + gc->aiMoneySpent;
    gc->decadeFuelWithCurrent = gc->decadeFuelCost + gc->fuelCostAccrued;
    gc->decadeSubsidyWithCurrent = gc->decadeSubsidyTotal + gc->subsidyCost;
    gc->decadeRevenueWithCurrent = gc->decadeRevenueTotal + gc->totalRevenue;
    gc->decadeXWithCurrent = gc->decadeXTotal + gc->xIndicator;
    gc->decadeEnterpriseWithCurrent = gc->decadeEnterpriseTotal + gc->decadeEnterpriseTax;
}

// void handleCursorClickInlined(int,int,boolean,boolean)
void gc_handleCursorClickInlined(GameCanvas *gc, int32_t n, int32_t n2,
                                 bool bl, bool bl2) {
    bool bl3 = !bl;
    int n3;
    if (bl) {
        n3 = gc->tileMap[n][n2] >= 29 && gc->tileMap[n][n2] <= 33
             ? gc->baseTrainSpeed2 : -gc->accel2;
        if (gc->money >= n3 || gc->money < 0) {
            gc->backgroundCost += n3;
            bl3 = true;
            gc->ck = true;
        } else {
            gc->ck = false;
        }
    }
    for (n3 = 49; n3 >= 0; --n3) {
        if (gc->tileQueue[n3][0] != 0
            && gc_c(gc, gc->tileQueue[n3][1]) == n
            && gc_c(gc, gc->tileQueue[n3][2]) == n2)
            bl3 = false;
    }
    if (!bl3) return;
    if (gc_c(gc, gc->tileMap[n][n2]) < 128)
        gc->tileMap[n][n2] = (int8_t)(gc->tileMap[n][n2] + 128);
    n3 = 0;
    for (int i = 49; i >= 0; --i) {
        if (gc->tileQueue[i][0] == 0) {
            n3 = i;
            break;
        }
    }
    gc->tileQueue[n3][0] = 14;
    gc->go[n3] = bl2;
    gc->tileQueue[n3][1] = (int8_t)n;
    gc->tileQueue[n3][2] = (int8_t)n2;
}

// void placeCityTileInlined(int,int,int)
void gc_placeCityTileInlined(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3) {
    if (gc->tileMap[n][n2] == 0
        || (gc->tileMap[n][n2] >= 29 && gc->tileMap[n][n2] <= 33)) {
        gc->tileMap[n][n2] = gc_randomCityTile(gc);
        gc->trackMap[n][n2] = (int8_t)n3;
    }
}

// boolean pickValidForestTile()
bool gc_pickValidForestTile(GameCanvas *gc) {
    int n2 = abs(midp_random_next_int(&gc->rng) % (gc->mapWidth - 2)) + 1;
    int n = abs(midp_random_next_int(&gc->rng) % (gc->mapHeight - 2)) + 1;
    if (gc->tileMap[n2][n] != 0
        && (gc->tileMap[n2][n] < 29 || gc->tileMap[n2][n] > 33))
        return false;

    int8_t by = gc->tileMap[n2 - 1][n];
    int8_t by2 = gc->tileMap[n2 + 1][n];
    int8_t by3 = gc->tileMap[n2][n - 1];
    int8_t by4 = gc->tileMap[n2][n + 1];
    bool bl2 = by == 43 || by == 44 || by == 45 || by == 46;
    bool bl3 = by2 == 43 || by2 == 44 || by2 == 47 || by2 == 48;
    bool bl4 = by3 == 42 || by3 == 44 || by3 == 46 || by3 == 47;
    bool bl = by4 == 42 || by4 == 44 || by4 == 45 || by4 == 48;
    if (!bl2 && !bl3 && !bl4 && !bl) return false;

    int n3;
    if (bl2) n3 = gc->trackMap[n2 - 1][n];
    else if (bl3) n3 = gc->trackMap[n2 + 1][n];
    else if (bl4) n3 = gc->trackMap[n2][n - 1];
    else n3 = gc->trackMap[n2][n + 1];
    int n4 = n3 & 0x3F;
    if (n4 <= -1) return false;

    if ((bl2 && bl3 && bl4) || (bl2 && bl3 && bl)
        || (bl2 && bl4 && bl) || (bl3 && bl4 && bl)) {
        gc->tileMap[n2][n] = 44;
    } else if (bl2 && bl4) {
        gc->tileMap[n2][n] = 48;
    } else if (bl2 && bl) {
        gc->tileMap[n2][n] = 47;
    } else if (bl3 && bl4) {
        gc->tileMap[n2][n] = 45;
    } else if (bl3 && bl) {
        gc->tileMap[n2][n] = 46;
    } else {
        int n5 = abs(midp_random_next_int(&gc->rng) % 13);
        if (n5 == 12) {
            gc->tileMap[n2][n] = 44;
        } else if (n5 == 11) {
            if (bl2) gc->tileMap[n2][n] = 47;
            if (bl3) gc->tileMap[n2][n] = 46;
            if (bl4) gc->tileMap[n2][n] = 45;
            if (bl) gc->tileMap[n2][n] = 46;
        } else if (n5 == 10) {
            if (bl2) gc->tileMap[n2][n] = 48;
            if (bl3) gc->tileMap[n2][n] = 45;
            if (bl4) gc->tileMap[n2][n] = 48;
            if (bl) gc->tileMap[n2][n] = 47;
        } else {
            if (bl2 || bl3) gc->tileMap[n2][n] = 43;
            if (bl4 || bl) gc->tileMap[n2][n] = 42;
        }
    }

    switch (gc->tileMap[n2][n]) {
        case 42:
            gc_placeCityTileInlined(gc, n2 - 1, n, n3);
            gc_placeCityTileInlined(gc, n2 + 1, n, n3);
            break;
        case 43:
            gc_placeCityTileInlined(gc, n2, n - 1, n3);
            gc_placeCityTileInlined(gc, n2, n + 1, n3);
            break;
        case 45:
            gc_placeCityTileInlined(gc, n2 - 1, n, n3);
            gc_placeCityTileInlined(gc, n2, n + 1, n3);
            gc_placeCityTileInlined(gc, n2 - 1, n + 1, n3);
            break;
        case 46:
            gc_placeCityTileInlined(gc, n2 - 1, n, n3);
            gc_placeCityTileInlined(gc, n2, n - 1, n3);
            gc_placeCityTileInlined(gc, n2 - 1, n - 1, n3);
            break;
        case 47:
            gc_placeCityTileInlined(gc, n2 + 1, n, n3);
            gc_placeCityTileInlined(gc, n2, n - 1, n3);
            gc_placeCityTileInlined(gc, n2 + 1, n - 1, n3);
            break;
        case 48:
            gc_placeCityTileInlined(gc, n2 + 1, n, n3);
            gc_placeCityTileInlined(gc, n2, n + 1, n3);
            gc_placeCityTileInlined(gc, n2 + 1, n + 1, n3);
            break;
    }
    gc->trackMap[n2][n] = (int8_t)n3;
    return true;
}

// void persistSnapshotMode0()
void gc_persistSnapshotMode0(GameCanvas *gc) {
    gc_loadGame(gc, 5, 0);
    gc_writeSaveSnapshot(gc);
    gc_saveGame(gc);
}

// int computeMapTrainMetric(int)
int32_t gc_computeMapTrainMetric(GameCanvas *gc, int32_t n) {
    int n2 = gc->by;
    for (int i = 0; i < gc->mapWidth; ++i) {
        for (int j = 0; j < gc->mapHeight; ++j) {
            if (gc->trackMap[i][j] != 65) continue;
            n2 += 3;
            for (int k = 0; k < 500; ++k) {
                if ((gc->trainFlags[k] & 0x80) != 128
                    || gc_c(gc, gc->trainX[k]) != i || gc_c(gc, gc->trainY[k]) != j)
                    continue;
                n2 += (gc->trainFlags[k] & 0x40) == 64 ? 400 : 55;
            }
        }
    }
    n2 += gc->dayOfMonth + 30 * gc->monthOfYear + 365 * (gc->year - 1950);
    if (n < 0) {
        if (n2 > gc->transientH) gc->transientH = n2;
    } else {
        gc->gk[n] = n2;
    }
    gc_persistSnapshotMode0(gc);
    return n2;
}

// void everyTwoTickZ()
void gc_everyTwoTickZ(GameCanvas *gc) {
    bool bl = false;
    if (!gc->milestonesEnabled) {
        switch (gc->scenarioMilestoneIndex) {
            case 1: bl = gc->currentMilestoneScore <= gc->milestoneThreshold1; break;
            case 2: bl = gc->currentMilestoneScore <= gc->milestoneThreshold2; break;
            case 3: bl = gc->currentMilestoneScore <= gc->milestoneThreshold3; break;
            case 4: bl = gc->currentMilestoneScore <= gc->milestoneThreshold4; break;
            case 5: bl = gc->currentMilestoneScore <= gc->milestoneThreshold5; break;
            case 6:
                bl = gc->currentMilestoneScore <= gc->milestoneThreshold1
                     && gc->milestoneCoThreshold2 <= gc->milestoneThreshold2;
                break;
            case 7:
                bl = gc->currentMilestoneScore <= gc->milestoneThreshold1
                     && gc->milestoneCoThreshold2 <= gc->milestoneThreshold3;
                break;
            case 8:
                bl = gc->currentMilestoneScore <= gc->milestoneThreshold1
                     && gc->milestoneCoThreshold2 <= gc->milestoneThreshold4;
                break;
            case 9:
                bl = gc->currentMilestoneScore <= gc->milestoneThreshold2
                     && gc->milestoneCoThreshold2 <= gc->milestoneThreshold3;
                break;
            case 10:
                bl = gc->currentMilestoneScore <= gc->milestoneThreshold2
                     && gc->milestoneCoThreshold2 <= gc->milestoneThreshold4;
                break;
            case 11:
                bl = gc->currentMilestoneScore <= gc->milestoneThreshold4
                     && gc->milestoneCoThreshold2 <= gc->milestoneThreshold5;
                break;
            case 12: bl = gc->currentMilestoneScore <= gc->counterB; break;
            case 13: bl = gc->loanBalance == 0; break;
            case 14: bl = gc->year == gc->currentMilestoneScore; break;
            case 15: bl = gc->currentMilestoneScore <= gc->aiWagonsBought; break;
        }
    }
    if (!bl) return;
    gc->ready = false;
    gc->transitionCode = 6;
    gc->milestonesEnabled = true;
    if (gc->scenarioStep >= 2) {
        if (gc->scenarioStepCompare < gc->scenarioStep)
            gc->scenarioStepCompare = gc->scenarioStep;
        gc_persistSnapshotMode0(gc);
        gc->gJ = gc_computeMapTrainMetric(gc, gc->scenarioStep - 2);
    }
    gc_setState(gc, 20);
}

// int scanMapForMetric()
int32_t gc_scanMapForMetric(GameCanvas *gc) {
    int n4 = 0;
    for (int n3 = 0; n3 < gc->mapWidth; ++n3) {
        for (int n2 = 0; n2 < gc->mapHeight; ++n2) {
            if (gc->trackMap[n3][n2] != 65) continue;
            int n = gc->tileMap[n3][n2];
            if (n >= 70 && n <= 80) {
                if (n > 76) n4 += 94;
                n4 += 196;
            }
            if (n == 76) n4 += 168;
            if (n >= 81 && n <= 86) n4 += gc->accel1;
            if (n == 87 || n == 88) n4 += gc->costPower;
            if (n == 89 || n == 90) n4 += gc->costPower / 2;
        }
    }
    for (int n3 = 0; n3 < 500; ++n3) {
        if ((gc->trainFlags[n3] & 0x80) != 128) continue;
        int n2 = gc_c(gc, gc->trainX[n3]);
        int n = gc_c(gc, gc->trainY[n3]);
        if (gc->trackMap[n2][n] != 65) continue;
        if ((gc->trainFlags[n3] & 0x40) == 64) {
            switch (gc->trainEraOrStage[n3]) {
                case 0: n4 += gc->gWagonCost; break;
                case 1: n4 += gc->gWagonCost2; break;
                case 2: n4 += gc->gWagonCost3; break;
                case 3: n4 += gc->gWagonCost4; break;
                case 4: n4 += gc->gWagonCost5; break;
                case 5: n4 += gc->gWagonCost6; break;
            }
        } else {
            n4 += (gc->costGroupC1 + gc->costGroupC2 + gc->costGroupC3
                   + gc->costGroupC4) / 4;
        }
    }
    return n4 - gc->loanBalance;
}

// void newYearNotify()
void gc_newYearNotify(GameCanvas *gc) {
    for (int n = 0; n < 500; ++n) {
        gc->prevTrainRevenue[n] = gc->trainRevenue[n];
        gc->trainRevenue[n] = 0;
    }
    for (int n = 0; n < 48; ++n) {
        gc->prevStationRevenue[n] = gc->stationRevenue[n];
        gc->stationRevenue[n] = 0;
    }
    gc_archiveOrRecall(gc);
    gc->prevTotalSpent = gc->totalSpent;
    gc->prevAiMoneySpent = gc->aiMoneySpent;
    gc->prevFuelCost = gc->fuelCostAccrued;
    gc->prevSubsidyCost = gc->subsidyCost;
    gc->prevTotalRevenue = gc->totalRevenue;
    gc->prevTerrainTotal = gc->terrainTotal;
    gc->prevEnterpriseTotal = gc->decadeEnterpriseTax;
    gc->prevXIndicator = gc->xIndicator;
    for (int n = 10; n >= 0; --n) {
        gc->yrN[n + 1] = gc->yrN[n];
        gc->yrCx[n + 1] = gc->yrCx[n];
        gc->yrZ[n + 1] = gc->yrZ[n];
    }
    gc->yrN[0] = gc->yrCx[0] = gc->yrZ[0] = 0;
    gc->decadeCostTotal += gc->totalSpent;
    gc->decadeSpentTotal += gc->aiMoneySpent;
    gc->decadeFuelCost += gc->fuelCostAccrued;
    gc->decadeSubsidyTotal += gc->subsidyCost;
    gc->decadeRevenueTotal += gc->totalRevenue;
    gc->decadeXTotal += gc->xIndicator;
    gc->decadeEnterpriseTotal += gc->decadeEnterpriseTax;
    gc->decadeOtherXTotal += gc->xIndicator;
    gc->revenueTotal += gc->totalRevenue;
    if (gc->monthOfYear == 13) {
        gc->prevDecadeSpent = gc->decadeSpentWithCurrent;
        gc->prevDecadeAiSpent = gc->decadeAiSpentWithCurrent;
        gc->prevDecadeFuel = gc->decadeFuelWithCurrent;
        gc->prevDecadeSubsidy = gc->decadeSubsidyWithCurrent;
        gc->prevDecadeRevenue = gc->decadeRevenueWithCurrent;
        gc->prevDecadeGs = gc->gs;
        gc->prevDecadeDq = gc->decadeEnterpriseWithCurrent;
        gc->prevDecadeBm = gc->decadeXWithCurrent;
        gc->decadeCostTotal = gc->decadeSpentTotal = gc->decadeFuelCost = 0;
        gc->decadeSubsidyTotal = gc->decadeRevenueTotal = gc->decadeXTotal = 0;
        gc->decadeEnterpriseTotal = 0;
        for (int n = 10; n >= 0; --n) {
            gc->altDecadeOtherXTotal[n + 1] = gc->altDecadeOtherXTotal[n];
            gc->altDecadeRevenueTotal[n + 1] = gc->altDecadeRevenueTotal[n];
            gc->altDecadeTotalCost[n + 1] = gc->altDecadeTotalCost[n];
        }
        gc->decadeOtherXTotal = gc->revenueTotal = 0;
        gc->altDecadeOtherXTotal[0] = gc->altDecadeRevenueTotal[0] = 0;
        gc->altDecadeTotalCost[0] = 0;
    }
    gc->altDecadeTotalCost[0] = gc_scanMapForMetric(gc);
    gc->money -= gc->terrainTotal;
    if (gc->terrainTotal > 0) gc_pushMoneyHistory(gc, -gc->terrainTotal);
    gc->money -= gc->decadeEnterpriseTax;
    if (gc->decadeEnterpriseTax > 0)
        gc_pushMoneyHistory(gc, -gc->decadeEnterpriseTax);
    gc->totalSpent = gc->aiMoneySpent = gc->fuelCostAccrued = 0;
    gc->subsidyCost = gc->totalRevenue = gc->xIndicator = 0;
    gc->decadeEnterpriseTax = 0;
    gc->mapRedrawPending = true;
    gc->compareToPrevious = true;
    gc->monthRolloverFlag = true;
    if (gc->monthOfYear == 13) {
        if (gcs_q) {
            gc->ready = false;
            gc->transitionCode = 35;
            gc_setState(gc, 20);
        }
    } else if (gcs_cc) {
        gc->ready = false;
        gc->transitionCode = 32;
        gc_setState(gc, 20);
    }
}

// void tickInputOrAi(boolean)
void gc_tickInputOrAi(GameCanvas *gc, bool bl) {
    for (int i = 0; i < 250; ++i) {
        if ((gc->vehicleFlags[i] & 0x80) != 128) continue;
        int n3 = gc_c(gc, gc->wagonAJ[i]);
        int n4 = gc_c(gc, gc->wagonEQ[i]);
        int n5 = n3 - 3;
        int n6 = n4 - 3;
        int n7 = n3 + 3;
        int n8 = n4 + 3;
        if (n5 < 0) n5 = 0;
        if (n6 < 0) n6 = 0;
        if (n7 >= gc->mapWidth) n7 = gc->mapWidth - 1;
        if (n8 >= gc->mapHeight) n8 = gc->mapHeight - 1;
        int n9 = 0;
        bool seenZones[64] = {false};
        bool seenIndustry1[64] = {false};
        bool seenIndustry2[64] = {false};
        bool seenIndustry3[64] = {false};
        for (int n2 = n5; n2 <= n7; ++n2) {
            for (int n = n6; n <= n8; ++n) {
                int tile = gc->tileMap[n2][n];
                int n11 = gc->trackMap[n2][n] & 0x3F;
                if (tile >= 34 && tile <= 40) {
                    if ((gc->trackMap[n2][n] & 0xC0) == 128 && !bl
                        && !seenZones[n11]) {
                        seenZones[n11] = true;
                        if ((gc->trainFlagBits[i] & 1) == 1) {
                            int add = gc->zonePopulation[n11]
                                      * (gc->constructionTimers[n11] + 10
                                         + gc->zonePopulation[n11] / 10
                                         + midp_random_next_int(&gc->rng) % 40) / 170;
                            gc->trainOptionSlots[i][0] =
                                (int16_t)(gc->trainOptionSlots[i][0] + add);
                        }
                        if ((gc->trainFlagBits[i] & 0x10) == 0x10) {
                            int add = gc->zonePopulation[n11] / 2
                                      * (gc->constructionTimers[n11] + 10
                                         + gc->zonePopulation[n11] / 10
                                         + abs(midp_random_next_int(&gc->rng) % 40)) / 170;
                            gc->trainOptionSlots[i][4] =
                                (int16_t)(gc->trainOptionSlots[i][4] + add);
                        }
                        gc->zonePopulation[n11] =
                            (int16_t)(gc->zonePopulation[n11]
                                      + midp_random_next_int(&gc->rng) % 2);
                    }
                    ++n9;
                }
                if (tile >= 65 && tile <= 68 && !bl
                    && (gc->trackMap[n2][n] & 0xC0) == 192
                    && !seenIndustry1[n11]) {
                    seenIndustry1[n11] = true;
                    int n10 = gc->cityPopulation[n11] * 4 / 30 / gc->cityIndex[n11];
                    if (n10 == 0) n10 = 1;
                    gc->trainOptionSlots[i][1] =
                        (int16_t)(gc->trainOptionSlots[i][1] + n10);
                    gc->stationRevenue[n11] = (int16_t)(gc->stationRevenue[n11] + n10);
                    gc->wagonServiceCounter[i] =
                        (int8_t)(gc->wagonServiceCounter[i] | 4);
                }
                if (tile >= 53 && tile <= 55 && !bl
                    && (gc->trackMap[n2][n] & 0xC0) == 192
                    && !seenIndustry2[n11]) {
                    seenIndustry2[n11] = true;
                    int n10 = gc->cityPopulation[n11] * 4 / 30 / gc->cityIndex[n11];
                    if (n10 == 0) n10 = 1;
                    gc->trainOptionSlots[i][2] =
                        (int16_t)(gc->trainOptionSlots[i][2] + n10);
                    gc->stationRevenue[n11] = (int16_t)(gc->stationRevenue[n11] + n10);
                    gc->wagonServiceCounter[i] =
                        (int8_t)(gc->wagonServiceCounter[i] | 8);
                }
                if (tile >= 59 && tile <= 61 && !bl
                    && (gc->trackMap[n2][n] & 0xC0) == 192
                    && !seenIndustry3[n11]) {
                    seenIndustry3[n11] = true;
                    int n10 = gc->cityPopulation[n11] * 4 / 30 / gc->cityIndex[n11];
                    if (n10 == 0) n10 = 1;
                    gc->trainOptionSlots[i][3] =
                        (int16_t)(gc->trainOptionSlots[i][3] + n10);
                    gc->stationRevenue[n11] = (int16_t)(gc->stationRevenue[n11] + n10);
                    gc->wagonServiceCounter[i] =
                        (int8_t)(gc->wagonServiceCounter[i] | 2);
                }
                if (tile >= 50 && tile <= 52)
                    gc->trainFlagBits[i] = (int8_t)(gc->trainFlagBits[i] | 4);
                if (tile >= 56 && tile <= 58)
                    gc->trainFlagBits[i] = (int8_t)(gc->trainFlagBits[i] | 8);
                if (tile >= 62 && tile <= 64)
                    gc->trainFlagBits[i] = (int8_t)(gc->trainFlagBits[i] | 2);
            }
        }
        if (n9 / 3 > 0) {
            gc->trainFlagBits[i] = (int8_t)(gc->trainFlagBits[i] | 1);
            gc->wagonServiceCounter[i] = (int8_t)(gc->wagonServiceCounter[i] | 1);
        }
        if (n9 / 7 > 0) {
            gc->trainFlagBits[i] = (int8_t)(gc->trainFlagBits[i] | 0x10);
            gc->wagonServiceCounter[i] =
                (int8_t)(gc->wagonServiceCounter[i] | 0x10);
        }
        bool large = (gc->vehicleFlags[i] & 0x20) == 0x20;
        for (int n = 0; n < 5; ++n) {
            int limit;
            if (large) limit = n == 0 ? 250 : (n == 4 ? 325 : 450);
            else limit = n == 0 ? 100 : (n == 4 ? 150 : 200);
            if (gc->trainOptionSlots[i][n] > limit)
                gc->trainOptionSlots[i][n] = (int16_t)limit;
        }
    }
}
