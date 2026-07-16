// Simulation tick functions.
#include "game_methods.h"
#include <cstdlib>

// void dayResetP()
void gc_dayResetP(GameCanvas *gc) {
    if (gc->economy.monthOfYear % 4 == 0) {
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
            if (!gc->world.in_bounds(i, j)
                || (gc->world.trackMap[i][j] & 0xC0) != 128)
                continue;
            int n4 = gc->world.trackMap[i][j] & 0x3F;
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
    for (int n = 0; n < gc->world.mapWidth; ++n) {
        for (int i = 0; i < gc->world.mapHeight; ++i) {
            if (gc->world.tileMap[n][i] < tile::BUILT_FIRST || gc->world.tileMap[n][i] > tile::CITY_ZONE_LAST) continue;
            int n2 = gc->world.trackMap[n][i] & 0x3F;
            ++gc->zonePopulation[n2];
        }
    }
}

// void dayResetAg()
void gc_dayResetAg(GameCanvas *gc) {
    for (int i = 0; i < 48; ++i) {
        gc->world.cityPopulation[i] += (int8_t)(gc->rng.next_int() % 8);
        if (gc->world.cityPopulation[i] < 10) gc->world.cityPopulation[i] = 10;
        if (gc->world.cityPopulation[i] > 90) gc->world.cityPopulation[i] = 90;
    }
}

// void revenueOrYear()
void gc_revenueOrYear(GameCanvas *gc) {
    for (int n = 0; n < 500; ++n) ++gc->fleet.trainAgeOrTimer[n];
}

// void everyTwoTick()
void gc_everyTwoTick(GameCanvas *gc) {
    if (gc->routeResourceFlags[0] != 0) return;
    for (int i = 0; i < 500; ++i) {
        if ((gc->fleet.trainFlags[i] & 0x80) != 128 || (gc->fleet.trainFlags[i] & 0x40) != 64)
            continue;
        int n2 = gc_byteToUnsignedInt(gc, gc->fleet.trainX[i]);
        int n = gc_byteToUnsignedInt(gc, gc->fleet.trainY[i]);
        if (gc->world.trackMap[n2][n] != 65) continue;
        int n3 = gc_countCarsInTrain(gc, i) * 2;
        if ((gc->fleet.trainFlags[i] & 0xF) == 0) n3 += 3;
        else if ((gc->fleet.trainFlags[i] & 0xF) == 1) n3 += 7;
        gc->fleet.trainRevenue[i] -= n3;
        gc->economy.money -= n3;
        gc->economy.fuelCostAccrued += n3;
    }
}

// int countCarsInTrain(int)
int32_t gc_countCarsInTrain(GameCanvas *gc, int32_t n) {
    return n != -1 ? gc_countCarsInTrain(gc, gc->fleet.nextCar[n]) + 1 : 0;
}

// boolean isTrainHeadCarAt(int,int,int)
bool gc_isTrainHeadCarAt(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3) {
    if (n == -1) return false;
    if (gc_byteToUnsignedInt(gc, gc->fleet.trainX[n]) == n2 && gc_byteToUnsignedInt(gc, gc->fleet.trainY[n]) == n3)
        return true;
    return gc_isTrainHeadCarAt(gc, gc->fleet.nextCar[n], n2, n3);
}

// void removeTrainFromMapInlined(int,boolean)
void gc_removeTrainFromMapInlined(GameCanvas *gc, int32_t n, bool bl) {
    if (n == -1) return;
    gc_removeTrainFromMapInlined(gc, gc->fleet.nextCar[n], bl);
    gc->fleet.trainFlags[n] = (int8_t)(gc->fleet.trainFlags[n] | 0xE0);
    gc->fleet.nextCar[n] = -1;
    gc->fleet.trainEnergyOrCount[n] = 7;
    if (bl)
        gc_handleCursorClickInlined(gc, gc_byteToUnsignedInt(gc, gc->fleet.trainX[n]),
                                    gc_byteToUnsignedInt(gc, gc->fleet.trainY[n]), false, true);
    gc->lastViewportCenterX = -1;
    gc->ready = false;
    gc->transitionCode = 16;
    gc_setState(gc, +GameState::MenuOverlay);
}

// void mergeTrainAt(int)
void gc_mergeTrainAt(GameCanvas *gc, int32_t n) {
    for (int i = 0; i < 500; ++i) {
        if (n == i || (gc->fleet.trainFlags[i] & 0x80) != 128
            || (gc->fleet.trainFlags[i] & 0x40) != 64
            || !gc_isTrainHeadCarAt(gc, i, gc_byteToUnsignedInt(gc, gc->fleet.trainX[n]),
                                    gc_byteToUnsignedInt(gc, gc->fleet.trainY[n])))
            continue;
        gc_removeTrainFromMapInlined(gc, i, true);
        gc_removeTrainFromMapInlined(gc, n, true);
    }
}

// void maybeAiTakeTurn()
void gc_maybeAiTakeTurn(GameCanvas *gc) {
    if (gc->buildMenuMode != 2 && gc->buildMenuMode != 4
        && gc->rng.next_int() % gc->aiFrequency > gc->aiThreshold)
        gc_setState(gc, +GameState::AiTurn);
}

// void dayResetAd()
void gc_dayResetAd(GameCanvas *gc) {
    for (int i = 0; i < 500; ++i) {
        if ((gc->fleet.trainFlags[i] & 0x80) != 128 || (gc->fleet.trainFlags[i] & 0x40) != 64)
            continue;
        int n2 = gc_byteToUnsignedInt(gc, gc->fleet.trainX[i]);
        int n = gc_byteToUnsignedInt(gc, gc->fleet.trainY[i]);
        if ((gc->world.trackMap[n2][n] != 66 && gc->world.trackMap[n2][n] != 67
             && gc->world.trackMap[n2][n] != 68)
            || gc->fleet.trainEraOrStage[i] >= gc->era)
            continue;
        gc->fleet.trainEraOrStage[i] = (int8_t)gc->era;
        gc->fleet.trainFlags[i] = (int8_t)(0xC0 | gc->era / 3);
        gc->fleet.wagonEnergyRemaining[i] = (int8_t)(8 - gc->era);
        break;
    }
}

// void postRevenueReset()
void gc_postRevenueReset(GameCanvas *gc) {
    for (int i = 0; i < 500; ++i) {
        if ((gc->fleet.trainFlags[i] & 0x80) != 128) continue;
        int n2 = gc_byteToUnsignedInt(gc, gc->fleet.trainX[i]);
        int n = gc_byteToUnsignedInt(gc, gc->fleet.trainY[i]);
        if (gc->world.trackMap[n2][n] == 66 || gc->world.trackMap[n2][n] == 67
            || gc->world.trackMap[n2][n] == 68)
            gc->fleet.trainAgeOrTimer[i] = 0;
    }
}

// void archiveOrRecall()
void gc_archiveOrRecall(GameCanvas *gc) {
    gc->economy.terrainTotal = gc->economy.loanBalance * gc->economy.interestRate / 1200;
    gc->economy.decadeEnterpriseTax = gc->economy.totalRevenue * gc->economy.enterpriseTaxPct / 100;
    gc->economy.netProfit = gc->economy.totalRevenue - gc->economy.terrainTotal - gc->economy.totalSpent
                     - gc->economy.aiMoneySpent - gc->economy.fuelCostAccrued - gc->economy.subsidyCost
                     - gc->economy.decadeEnterpriseTax;
}

// void computeYearEndMetrics()
void gc_computeYearEndMetrics(GameCanvas *gc) {
    gc_archiveOrRecall(gc);
    gc->economy.yearProfitHistory[0] = gc->economy.netProfit;
    gc->economy.yearRevenueHistory[0] = gc->economy.totalRevenue;
    gc->economy.decadeProfitHistory[0] = gc->economy.decadeProfitGraphTotal + gc->economy.netProfit;
    gc->economy.altDecadeRevenueTotal[0] = gc->economy.revenueTotal + gc->economy.totalRevenue;
    gc->economy.decadeSpentWithCurrent = gc->economy.decadeCostTotal + gc->economy.totalSpent;
    gc->economy.decadeAiSpentWithCurrent = gc->economy.decadeSpentTotal + gc->economy.aiMoneySpent;
    gc->economy.decadeFuelWithCurrent = gc->economy.decadeFuelCost + gc->economy.fuelCostAccrued;
    gc->economy.decadeSubsidyWithCurrent = gc->economy.decadeSubsidyTotal + gc->economy.subsidyCost;
    gc->economy.decadeRevenueWithCurrent = gc->economy.decadeRevenueTotal + gc->economy.totalRevenue;
    gc->economy.decadeProfitWithCurrent = gc->economy.decadeProfitTotal + gc->economy.netProfit;
    gc->economy.decadeEnterpriseWithCurrent = gc->economy.decadeEnterpriseTotal + gc->economy.decadeEnterpriseTax;
}

// void handleCursorClickInlined(int,int,boolean,boolean)
void gc_handleCursorClickInlined(GameCanvas *gc, int32_t n, int32_t n2,
                                 bool bl, bool bl2) {
    bool bl3 = !bl;
    int n3;
    if (bl) {
        n3 = gc->world.tileMap[n][n2] >= tile::SCENERY_FIRST && gc->world.tileMap[n][n2] <= tile::SCENERY_LAST
             ? gc->baseTrainSpeed2 : -gc->accel2;
        if (gc->economy.money >= n3 || gc->economy.money < 0) {
            gc->backgroundCost += n3;
            bl3 = true;
            gc->demolitionAffordable = true;
        } else {
            gc->demolitionAffordable = false;
        }
    }
    for (n3 = 49; n3 >= 0; --n3) {
        if (gc->tileQueue[n3][0] != 0
            && gc_byteToUnsignedInt(gc, gc->tileQueue[n3][1]) == n
            && gc_byteToUnsignedInt(gc, gc->tileQueue[n3][2]) == n2)
            bl3 = false;
    }
    if (!bl3) return;
    if (gc_byteToUnsignedInt(gc, gc->world.tileMap[n][n2]) < 128)
        gc->world.tileMap[n][n2] = (int8_t)(gc->world.tileMap[n][n2] + 128);
    n3 = 0;
    for (int i = 49; i >= 0; --i) {
        if (gc->tileQueue[i][0] == 0) {
            n3 = i;
            break;
        }
    }
    gc->tileQueue[n3][0] = 14;
    gc->tileQueueVisualOnly[n3] = bl2;
    gc->tileQueue[n3][1] = (int8_t)n;
    gc->tileQueue[n3][2] = (int8_t)n2;
}

// void placeCityTileInlined(int,int,int)
void gc_placeCityTileInlined(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3) {
    if (gc->world.tileMap[n][n2] == 0
        || (gc->world.tileMap[n][n2] >= tile::SCENERY_FIRST && gc->world.tileMap[n][n2] <= tile::SCENERY_LAST)) {
        gc->world.tileMap[n][n2] = gc_randomCityTile(gc);
        gc->world.trackMap[n][n2] = (int8_t)n3;
    }
}

// boolean pickValidForestTile()
bool gc_pickValidForestTile(GameCanvas *gc) {
    int n2 = std::abs(gc->rng.next_int() % (gc->world.mapWidth - 2)) + 1;
    int n = std::abs(gc->rng.next_int() % (gc->world.mapHeight - 2)) + 1;
    if (gc->world.tileMap[n2][n] != 0
        && (gc->world.tileMap[n2][n] < tile::SCENERY_FIRST || gc->world.tileMap[n2][n] > tile::SCENERY_LAST))
        return false;

    int8_t by = gc->world.tileMap[n2 - 1][n];
    int8_t by2 = gc->world.tileMap[n2 + 1][n];
    int8_t by3 = gc->world.tileMap[n2][n - 1];
    int8_t by4 = gc->world.tileMap[n2][n + 1];
    bool bl2 = by == 43 || by == 44 || by == 45 || by == 46;
    bool bl3 = by2 == 43 || by2 == 44 || by2 == 47 || by2 == 48;
    bool bl4 = by3 == 42 || by3 == 44 || by3 == 46 || by3 == 47;
    bool bl = by4 == 42 || by4 == 44 || by4 == 45 || by4 == 48;
    if (!bl2 && !bl3 && !bl4 && !bl) return false;

    int n3;
    if (bl2) n3 = gc->world.trackMap[n2 - 1][n];
    else if (bl3) n3 = gc->world.trackMap[n2 + 1][n];
    else if (bl4) n3 = gc->world.trackMap[n2][n - 1];
    else n3 = gc->world.trackMap[n2][n + 1];
    int n4 = n3 & 0x3F;
    if (n4 <= -1) return false;

    if ((bl2 && bl3 && bl4) || (bl2 && bl3 && bl)
        || (bl2 && bl4 && bl) || (bl3 && bl4 && bl)) {
        gc->world.tileMap[n2][n] = 44;
    } else if (bl2 && bl4) {
        gc->world.tileMap[n2][n] = 48;
    } else if (bl2 && bl) {
        gc->world.tileMap[n2][n] = 47;
    } else if (bl3 && bl4) {
        gc->world.tileMap[n2][n] = 45;
    } else if (bl3 && bl) {
        gc->world.tileMap[n2][n] = 46;
    } else {
        int n5 = std::abs(gc->rng.next_int() % 13);
        if (n5 == 12) {
            gc->world.tileMap[n2][n] = 44;
        } else if (n5 == 11) {
            if (bl2) gc->world.tileMap[n2][n] = 47;
            if (bl3) gc->world.tileMap[n2][n] = 46;
            if (bl4) gc->world.tileMap[n2][n] = 45;
            if (bl) gc->world.tileMap[n2][n] = 46;
        } else if (n5 == 10) {
            if (bl2) gc->world.tileMap[n2][n] = 48;
            if (bl3) gc->world.tileMap[n2][n] = 45;
            if (bl4) gc->world.tileMap[n2][n] = 48;
            if (bl) gc->world.tileMap[n2][n] = 47;
        } else {
            if (bl2 || bl3) gc->world.tileMap[n2][n] = 43;
            if (bl4 || bl) gc->world.tileMap[n2][n] = 42;
        }
    }

    switch (gc->world.tileMap[n2][n]) {
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
    gc->world.trackMap[n2][n] = (int8_t)n3;
    return true;
}

// void persistSnapshotMode0()
void gc_persistSnapshotMode0(GameCanvas *gc) {
    gc_loadGame(gc, 5, save::MODE_WRITE);
    gc_writeSaveSnapshot(gc);
    gc_saveGame(gc);
}

// int computeMapTrainMetric(int)
int32_t gc_computeMapTrainMetric(GameCanvas *gc, int32_t n) {
    int n2 = gc->economy.trainRevenueScore;
    for (int i = 0; i < gc->world.mapWidth; ++i) {
        for (int j = 0; j < gc->world.mapHeight; ++j) {
            if (gc->world.trackMap[i][j] != 65) continue;
            n2 += 3;
            for (int k = 0; k < 500; ++k) {
                if ((gc->fleet.trainFlags[k] & 0x80) != 128
                    || gc_byteToUnsignedInt(gc, gc->fleet.trainX[k]) != i || gc_byteToUnsignedInt(gc, gc->fleet.trainY[k]) != j)
                    continue;
                n2 += (gc->fleet.trainFlags[k] & 0x40) == 64 ? 400 : 55;
            }
        }
    }
    n2 += gc->economy.dayOfMonth + 30 * gc->economy.monthOfYear + 365 * (gc->economy.year - 1950);
    if (n < 0) {
        if (n2 > gc->bestScenarioScore) gc->bestScenarioScore = n2;
    } else {
        gc->scenarioScores[n] = n2;
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
            case 12: bl = gc->currentMilestoneScore <= gc->economy.lifetimeTrainRevenue; break;
            case 13: bl = gc->economy.loanBalance == 0; break;
            case 14: bl = gc->economy.year == gc->currentMilestoneScore; break;
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
        gc->currentScenarioScore = gc_computeMapTrainMetric(gc, gc->scenarioStep - 2);
    }
    gc_setState(gc, +GameState::MenuOverlay);
}

// int scanMapForMetric()
int32_t gc_scanMapForMetric(GameCanvas *gc) {
    int n4 = 0;
    for (int n3 = 0; n3 < gc->world.mapWidth; ++n3) {
        for (int n2 = 0; n2 < gc->world.mapHeight; ++n2) {
            if (gc->world.trackMap[n3][n2] != 65) continue;
            int n = gc->world.tileMap[n3][n2];
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
        if ((gc->fleet.trainFlags[n3] & 0x80) != 128) continue;
        int n2 = gc_byteToUnsignedInt(gc, gc->fleet.trainX[n3]);
        int n = gc_byteToUnsignedInt(gc, gc->fleet.trainY[n3]);
        if (gc->world.trackMap[n2][n] != 65) continue;
        if ((gc->fleet.trainFlags[n3] & 0x40) == 64) {
            switch (gc->fleet.trainEraOrStage[n3]) {
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
    return n4 - gc->economy.loanBalance;
}

// void newYearNotify()
void gc_newYearNotify(GameCanvas *gc) {
    for (int n = 0; n < 500; ++n) {
        gc->fleet.prevTrainRevenue[n] = gc->fleet.trainRevenue[n];
        gc->fleet.trainRevenue[n] = 0;
    }
    for (int n = 0; n < 48; ++n) {
        gc->economy.prevStationRevenue[n] = gc->economy.stationRevenue[n];
        gc->economy.stationRevenue[n] = 0;
    }
    gc_archiveOrRecall(gc);
    gc->economy.prevTotalSpent = gc->economy.totalSpent;
    gc->economy.prevAiMoneySpent = gc->economy.aiMoneySpent;
    gc->economy.prevFuelCost = gc->economy.fuelCostAccrued;
    gc->economy.prevSubsidyCost = gc->economy.subsidyCost;
    gc->economy.prevTotalRevenue = gc->economy.totalRevenue;
    gc->economy.prevTerrainTotal = gc->economy.terrainTotal;
    gc->economy.prevEnterpriseTotal = gc->economy.decadeEnterpriseTax;
    gc->economy.previousYearProfit = gc->economy.netProfit;
    for (int n = 10; n >= 0; --n) {
        gc->economy.yearProfitHistory[n + 1] = gc->economy.yearProfitHistory[n];
        gc->economy.yearRevenueHistory[n + 1] = gc->economy.yearRevenueHistory[n];
        gc->economy.yearCompanyValueHistory[n + 1] = gc->economy.yearCompanyValueHistory[n];
    }
    gc->economy.yearProfitHistory[0] = gc->economy.yearRevenueHistory[0] = gc->economy.yearCompanyValueHistory[0] = 0;
    gc->economy.decadeCostTotal += gc->economy.totalSpent;
    gc->economy.decadeSpentTotal += gc->economy.aiMoneySpent;
    gc->economy.decadeFuelCost += gc->economy.fuelCostAccrued;
    gc->economy.decadeSubsidyTotal += gc->economy.subsidyCost;
    gc->economy.decadeRevenueTotal += gc->economy.totalRevenue;
    gc->economy.decadeProfitTotal += gc->economy.netProfit;
    gc->economy.decadeEnterpriseTotal += gc->economy.decadeEnterpriseTax;
    gc->economy.decadeProfitGraphTotal += gc->economy.netProfit;
    gc->economy.revenueTotal += gc->economy.totalRevenue;
    if (gc->economy.monthOfYear == 13) {
        gc->economy.prevDecadeSpent = gc->economy.decadeSpentWithCurrent;
        gc->economy.prevDecadeAiSpent = gc->economy.decadeAiSpentWithCurrent;
        gc->economy.prevDecadeFuel = gc->economy.decadeFuelWithCurrent;
        gc->economy.prevDecadeSubsidy = gc->economy.decadeSubsidyWithCurrent;
        gc->economy.prevDecadeRevenue = gc->economy.decadeRevenueWithCurrent;
        gc->economy.previousLegacyDecadeMetric = gc->economy.legacyDecadeMetric;
        gc->economy.previousDecadeEnterpriseTax = gc->economy.decadeEnterpriseWithCurrent;
        gc->economy.previousDecadeProfit = gc->economy.decadeProfitWithCurrent;
        gc->economy.decadeCostTotal = gc->economy.decadeSpentTotal = gc->economy.decadeFuelCost = 0;
        gc->economy.decadeSubsidyTotal = gc->economy.decadeRevenueTotal = gc->economy.decadeProfitTotal = 0;
        gc->economy.decadeEnterpriseTotal = 0;
        for (int n = 10; n >= 0; --n) {
            gc->economy.decadeProfitHistory[n + 1] = gc->economy.decadeProfitHistory[n];
            gc->economy.altDecadeRevenueTotal[n + 1] = gc->economy.altDecadeRevenueTotal[n];
            gc->economy.altDecadeTotalCost[n + 1] = gc->economy.altDecadeTotalCost[n];
        }
        gc->economy.decadeProfitGraphTotal = gc->economy.revenueTotal = 0;
        gc->economy.decadeProfitHistory[0] = gc->economy.altDecadeRevenueTotal[0] = 0;
        gc->economy.altDecadeTotalCost[0] = 0;
    }
    gc->economy.altDecadeTotalCost[0] = gc_scanMapForMetric(gc);
    gc->economy.money -= gc->economy.terrainTotal;
    if (gc->economy.terrainTotal > 0) gc_pushMoneyHistory(gc, -gc->economy.terrainTotal);
    gc->economy.money -= gc->economy.decadeEnterpriseTax;
    if (gc->economy.decadeEnterpriseTax > 0)
        gc_pushMoneyHistory(gc, -gc->economy.decadeEnterpriseTax);
    gc->economy.totalSpent = gc->economy.aiMoneySpent = gc->economy.fuelCostAccrued = 0;
    gc->economy.subsidyCost = gc->economy.totalRevenue = gc->economy.netProfit = 0;
    gc->economy.decadeEnterpriseTax = 0;
    gc->mapRedrawPending = true;
    gc->compareToPrevious = true;
    gc->monthRolloverFlag = true;
    if (gc->economy.monthOfYear == 13) {
        if (gcs_annualReportEnabled) {
            gc->ready = false;
            gc->transitionCode = 35;
            gc_setState(gc, +GameState::MenuOverlay);
        }
    } else if (gcs_monthlyReportEnabled) {
        gc->ready = false;
        gc->transitionCode = 32;
        gc_setState(gc, +GameState::MenuOverlay);
    }
}

// void tickInputOrAi(boolean)
void gc_tickInputOrAi(GameCanvas *gc, bool bl) {
    for (int i = 0; i < 250; ++i) {
        if ((gc->fleet.vehicleFlags[i] & 0x80) != 128) continue;
        int n3 = gc_byteToUnsignedInt(gc, gc->fleet.serviceVehicleX[i]);
        int n4 = gc_byteToUnsignedInt(gc, gc->fleet.serviceVehicleY[i]);
        int n5 = n3 - 3;
        int n6 = n4 - 3;
        int n7 = n3 + 3;
        int n8 = n4 + 3;
        if (n5 < 0) n5 = 0;
        if (n6 < 0) n6 = 0;
        if (n7 >= gc->world.mapWidth) n7 = gc->world.mapWidth - 1;
        if (n8 >= gc->world.mapHeight) n8 = gc->world.mapHeight - 1;
        int n9 = 0;
        bool seenZones[64] = {false};
        bool seenIndustry1[64] = {false};
        bool seenIndustry2[64] = {false};
        bool seenIndustry3[64] = {false};
        for (int n2 = n5; n2 <= n7; ++n2) {
            for (int n = n6; n <= n8; ++n) {
                int tile = gc->world.tileMap[n2][n];
                int n11 = gc->world.trackMap[n2][n] & 0x3F;
                if (tile >= 34 && tile <= 40) {
                    if ((gc->world.trackMap[n2][n] & 0xC0) == 128 && !bl
                        && !seenZones[n11]) {
                        seenZones[n11] = true;
                        if ((gc->fleet.serviceUnloadFlags[i] & 1) == 1) {
                            int add = gc->zonePopulation[n11]
                                      * (gc->constructionTimers[n11] + 10
                                         + gc->zonePopulation[n11] / 10
                                         + gc->rng.next_int() % 40) / 170;
                            gc->fleet.serviceResourceStock[i][0] =
                                (int16_t)(gc->fleet.serviceResourceStock[i][0] + add);
                        }
                        if ((gc->fleet.serviceUnloadFlags[i] & 0x10) == 0x10) {
                            int add = gc->zonePopulation[n11] / 2
                                      * (gc->constructionTimers[n11] + 10
                                         + gc->zonePopulation[n11] / 10
                                         + std::abs(gc->rng.next_int() % 40)) / 170;
                            gc->fleet.serviceResourceStock[i][4] =
                                (int16_t)(gc->fleet.serviceResourceStock[i][4] + add);
                        }
                        gc->zonePopulation[n11] =
                            (int16_t)(gc->zonePopulation[n11]
                                      + gc->rng.next_int() % 2);
                    }
                    ++n9;
                }
                if (tile >= 65 && tile <= 68 && !bl
                    && (gc->world.trackMap[n2][n] & 0xC0) == 192
                    && !seenIndustry1[n11]) {
                    seenIndustry1[n11] = true;
                    int n10 = gc->world.cityPopulation[n11] * 4 / 30 / gc->world.cityIndex[n11];
                    if (n10 == 0) n10 = 1;
                    gc->fleet.serviceResourceStock[i][1] =
                        (int16_t)(gc->fleet.serviceResourceStock[i][1] + n10);
                    gc->economy.stationRevenue[n11] = (int16_t)(gc->economy.stationRevenue[n11] + n10);
                    gc->fleet.serviceLoadFlags[i] = (int8_t)(gc->fleet.serviceLoadFlags[i] | 4);
                }
                if (tile >= 53 && tile <= 55 && !bl
                    && (gc->world.trackMap[n2][n] & 0xC0) == 192
                    && !seenIndustry2[n11]) {
                    seenIndustry2[n11] = true;
                    int n10 = gc->world.cityPopulation[n11] * 4 / 30 / gc->world.cityIndex[n11];
                    if (n10 == 0) n10 = 1;
                    gc->fleet.serviceResourceStock[i][2] =
                        (int16_t)(gc->fleet.serviceResourceStock[i][2] + n10);
                    gc->economy.stationRevenue[n11] = (int16_t)(gc->economy.stationRevenue[n11] + n10);
                    gc->fleet.serviceLoadFlags[i] = (int8_t)(gc->fleet.serviceLoadFlags[i] | 8);
                }
                if (tile >= 59 && tile <= 61 && !bl
                    && (gc->world.trackMap[n2][n] & 0xC0) == 192
                    && !seenIndustry3[n11]) {
                    seenIndustry3[n11] = true;
                    int n10 = gc->world.cityPopulation[n11] * 4 / 30 / gc->world.cityIndex[n11];
                    if (n10 == 0) n10 = 1;
                    gc->fleet.serviceResourceStock[i][3] =
                        (int16_t)(gc->fleet.serviceResourceStock[i][3] + n10);
                    gc->economy.stationRevenue[n11] = (int16_t)(gc->economy.stationRevenue[n11] + n10);
                    gc->fleet.serviceLoadFlags[i] = (int8_t)(gc->fleet.serviceLoadFlags[i] | 2);
                }
                if (tile >= 50 && tile <= 52)
                    gc->fleet.serviceUnloadFlags[i] = (int8_t)(gc->fleet.serviceUnloadFlags[i] | 4);
                if (tile >= 56 && tile <= 58)
                    gc->fleet.serviceUnloadFlags[i] = (int8_t)(gc->fleet.serviceUnloadFlags[i] | 8);
                if (tile >= 62 && tile <= 64)
                    gc->fleet.serviceUnloadFlags[i] = (int8_t)(gc->fleet.serviceUnloadFlags[i] | 2);
            }
        }
        if (n9 / 3 > 0) {
            gc->fleet.serviceUnloadFlags[i] = (int8_t)(gc->fleet.serviceUnloadFlags[i] | 1);
            gc->fleet.serviceLoadFlags[i] = (int8_t)(gc->fleet.serviceLoadFlags[i] | 1);
        }
        if (n9 / 7 > 0) {
            gc->fleet.serviceUnloadFlags[i] = (int8_t)(gc->fleet.serviceUnloadFlags[i] | 0x10);
            gc->fleet.serviceLoadFlags[i] = (int8_t)(gc->fleet.serviceLoadFlags[i] | 0x10);
        }
        bool large = (gc->fleet.vehicleFlags[i] & 0x20) == 0x20;
        for (int n = 0; n < 5; ++n) {
            int limit;
            if (large) limit = n == 0 ? 250 : (n == 4 ? 325 : 450);
            else limit = n == 0 ? 100 : (n == 4 ? 150 : 200);
            if (gc->fleet.serviceResourceStock[i][n] > limit)
                gc->fleet.serviceResourceStock[i][n] = (int16_t)limit;
        }
    }
}
