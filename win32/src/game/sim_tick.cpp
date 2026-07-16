// Simulation tick functions.
#include "game_methods.h"
#include <cstdlib>

void game_dayResetP(Game *game) {
    if (game->economy.monthOfYear % 4 == 0) {
        for (int i = 0; i < 64; ++i) {
            if (game->world.constructionTimers[i] > 0)
                --game->world.constructionTimers[i];
        }
    }
}

void game_advanceConstructionTimers(Game *game, int32_t n, int32_t n2, int32_t n3) {
    bool seen[64] = {false};
    for (int i = n - 5; i <= n + 5; ++i) {
        for (int j = n2 - 5; j <= n2 + 5; ++j) {
            if (!game->world.in_bounds(i, j)
                || (game->world.trackMap[i][j] & 0xC0) != 128)
                continue;
            int n4 = game->world.trackMap[i][j] & 0x3F;
            if (seen[n4]) continue;
            seen[n4] = true;
            int n5;
            if (n3 < 0) {
                n5 = game->world.constructionTimers[n4] * n3 * 3 / 200 - 1;
                if (n5 < n3) n3 = n5;
            } else {
                n5 = (100 - game->world.constructionTimers[n4]) / 30 * n3 / 3 + 1;
                if (n5 > n3) n5 = n3;
            }
            game->world.constructionTimers[n4] = (int8_t)(game->world.constructionTimers[n4] + n5);
            if (game->world.constructionTimers[n4] < 0) game->world.constructionTimers[n4] = 0;
            if (game->world.constructionTimers[n4] > 100) game->world.constructionTimers[n4] = 100;
        }
    }
}

void game_maybeAITick(Game *game) {
    for (int n = 0; n < 64; ++n) game->world.zonePopulation[n] = 0;
    for (int n = 0; n < game->world.mapWidth; ++n) {
        for (int i = 0; i < game->world.mapHeight; ++i) {
            if (game->world.tileMap[n][i] < tile::BUILT_FIRST || game->world.tileMap[n][i] > tile::CITY_ZONE_LAST) continue;
            int n2 = game->world.trackMap[n][i] & 0x3F;
            ++game->world.zonePopulation[n2];
        }
    }
}

void game_dayResetAg(Game *game) {
    for (int i = 0; i < 48; ++i) {
        game->world.cityPopulation[i] += (int8_t)(game->rng.next_int() % 8);
        if (game->world.cityPopulation[i] < 10) game->world.cityPopulation[i] = 10;
        if (game->world.cityPopulation[i] > 90) game->world.cityPopulation[i] = 90;
    }
}

void game_revenueOrYear(Game *game) {
    for (int n = 0; n < 500; ++n) ++game->fleet.trainAgeOrTimer[n];
}

void game_everyTwoTick(Game *game) {
    if (game->scenario.routeResourceFlags[0] != 0) return;
    for (int i = 0; i < 500; ++i) {
        if ((game->fleet.trainFlags[i] & 0x80) != 128 || (game->fleet.trainFlags[i] & 0x40) != 64)
            continue;
        int n2 = game_byteToUnsignedInt(game, game->fleet.trainX[i]);
        int n = game_byteToUnsignedInt(game, game->fleet.trainY[i]);
        if (game->world.trackMap[n2][n] != 65) continue;
        int n3 = game_countCarsInTrain(game, i) * 2;
        if ((game->fleet.trainFlags[i] & 0xF) == 0) n3 += 3;
        else if ((game->fleet.trainFlags[i] & 0xF) == 1) n3 += 7;
        game->fleet.trainRevenue[i] -= n3;
        game->economy.money -= n3;
        game->economy.fuelCostAccrued += n3;
    }
}

int32_t game_countCarsInTrain(Game *game, int32_t n) {
    return n != -1 ? game_countCarsInTrain(game, game->fleet.nextCar[n]) + 1 : 0;
}

bool game_isTrainHeadCarAt(Game *game, int32_t n, int32_t n2, int32_t n3) {
    if (n == -1) return false;
    if (game_byteToUnsignedInt(game, game->fleet.trainX[n]) == n2 && game_byteToUnsignedInt(game, game->fleet.trainY[n]) == n3)
        return true;
    return game_isTrainHeadCarAt(game, game->fleet.nextCar[n], n2, n3);
}

void game_removeTrainFromMapInlined(Game *game, int32_t n, bool bl) {
    if (n == -1) return;
    game_removeTrainFromMapInlined(game, game->fleet.nextCar[n], bl);
    game->fleet.trainFlags[n] = (int8_t)(game->fleet.trainFlags[n] | 0xE0);
    game->fleet.nextCar[n] = -1;
    game->fleet.trainEnergyOrCount[n] = 7;
    if (bl)
        game_handleCursorClickInlined(game, game_byteToUnsignedInt(game, game->fleet.trainX[n]),
                                    game_byteToUnsignedInt(game, game->fleet.trainY[n]), false, true);
    game->renderer.lastViewportCenterX = -1;
    game->ready = false;
    game->ui.transitionCode = 16;
    game_setState(game, +GameState::MenuOverlay);
}

void game_mergeTrainAt(Game *game, int32_t n) {
    for (int i = 0; i < 500; ++i) {
        if (n == i || (game->fleet.trainFlags[i] & 0x80) != 128
            || (game->fleet.trainFlags[i] & 0x40) != 64
            || !game_isTrainHeadCarAt(game, i, game_byteToUnsignedInt(game, game->fleet.trainX[n]),
                                    game_byteToUnsignedInt(game, game->fleet.trainY[n])))
            continue;
        game_removeTrainFromMapInlined(game, i, true);
        game_removeTrainFromMapInlined(game, n, true);
    }
}

void game_maybeAiTakeTurn(Game *game) {
    if (game->ui.buildMenuMode != 2 && game->ui.buildMenuMode != 4
        && game->rng.next_int() % game->scenario.aiFrequency > game->scenario.aiThreshold)
        game_setState(game, +GameState::AiTurn);
}

void game_dayResetAd(Game *game) {
    for (int i = 0; i < 500; ++i) {
        if ((game->fleet.trainFlags[i] & 0x80) != 128 || (game->fleet.trainFlags[i] & 0x40) != 64)
            continue;
        int n2 = game_byteToUnsignedInt(game, game->fleet.trainX[i]);
        int n = game_byteToUnsignedInt(game, game->fleet.trainY[i]);
        if ((game->world.trackMap[n2][n] != 66 && game->world.trackMap[n2][n] != 67
             && game->world.trackMap[n2][n] != 68)
            || game->fleet.trainEraOrStage[i] >= game->era)
            continue;
        game->fleet.trainEraOrStage[i] = (int8_t)game->era;
        game->fleet.trainFlags[i] = (int8_t)(0xC0 | game->era / 3);
        game->fleet.wagonEnergyRemaining[i] = (int8_t)(8 - game->era);
        break;
    }
}

void game_postRevenueReset(Game *game) {
    for (int i = 0; i < 500; ++i) {
        if ((game->fleet.trainFlags[i] & 0x80) != 128) continue;
        int n2 = game_byteToUnsignedInt(game, game->fleet.trainX[i]);
        int n = game_byteToUnsignedInt(game, game->fleet.trainY[i]);
        if (game->world.trackMap[n2][n] == 66 || game->world.trackMap[n2][n] == 67
            || game->world.trackMap[n2][n] == 68)
            game->fleet.trainAgeOrTimer[i] = 0;
    }
}

void game_archiveOrRecall(Game *game) {
    game->economy.terrainTotal = game->economy.loanBalance * game->economy.interestRate / 1200;
    game->economy.decadeEnterpriseTax = game->economy.totalRevenue * game->economy.enterpriseTaxPct / 100;
    game->economy.netProfit = game->economy.totalRevenue - game->economy.terrainTotal - game->economy.totalSpent
                     - game->economy.aiMoneySpent - game->economy.fuelCostAccrued - game->economy.subsidyCost
                     - game->economy.decadeEnterpriseTax;
}

void game_computeYearEndMetrics(Game *game) {
    game_archiveOrRecall(game);
    game->economy.yearProfitHistory[0] = game->economy.netProfit;
    game->economy.yearRevenueHistory[0] = game->economy.totalRevenue;
    game->economy.decadeProfitHistory[0] = game->economy.decadeProfitGraphTotal + game->economy.netProfit;
    game->economy.altDecadeRevenueTotal[0] = game->economy.revenueTotal + game->economy.totalRevenue;
    game->economy.decadeSpentWithCurrent = game->economy.decadeCostTotal + game->economy.totalSpent;
    game->economy.decadeAiSpentWithCurrent = game->economy.decadeSpentTotal + game->economy.aiMoneySpent;
    game->economy.decadeFuelWithCurrent = game->economy.decadeFuelCost + game->economy.fuelCostAccrued;
    game->economy.decadeSubsidyWithCurrent = game->economy.decadeSubsidyTotal + game->economy.subsidyCost;
    game->economy.decadeRevenueWithCurrent = game->economy.decadeRevenueTotal + game->economy.totalRevenue;
    game->economy.decadeProfitWithCurrent = game->economy.decadeProfitTotal + game->economy.netProfit;
    game->economy.decadeEnterpriseWithCurrent = game->economy.decadeEnterpriseTotal + game->economy.decadeEnterpriseTax;
}

void game_handleCursorClickInlined(Game *game, int32_t n, int32_t n2,
                                 bool bl, bool bl2) {
    bool bl3 = !bl;
    int n3;
    if (bl) {
        n3 = game->world.tileMap[n][n2] >= tile::SCENERY_FIRST && game->world.tileMap[n][n2] <= tile::SCENERY_LAST
             ? game->economy.baseTrainSpeed2 : -game->economy.accel2;
        if (game->economy.money >= n3 || game->economy.money < 0) {
            game->build.backgroundCost += n3;
            bl3 = true;
            game->build.demolitionAffordable = true;
        } else {
            game->build.demolitionAffordable = false;
        }
    }
    for (n3 = 49; n3 >= 0; --n3) {
        if (game->world.tileQueue[n3][0] != 0
            && game_byteToUnsignedInt(game, game->world.tileQueue[n3][1]) == n
            && game_byteToUnsignedInt(game, game->world.tileQueue[n3][2]) == n2)
            bl3 = false;
    }
    if (!bl3) return;
    if (game_byteToUnsignedInt(game, game->world.tileMap[n][n2]) < 128)
        game->world.tileMap[n][n2] = (int8_t)(game->world.tileMap[n][n2] + 128);
    n3 = 0;
    for (int i = 49; i >= 0; --i) {
        if (game->world.tileQueue[i][0] == 0) {
            n3 = i;
            break;
        }
    }
    game->world.tileQueue[n3][0] = 14;
    game->world.tileQueueVisualOnly[n3] = bl2;
    game->world.tileQueue[n3][1] = (int8_t)n;
    game->world.tileQueue[n3][2] = (int8_t)n2;
}

void game_placeCityTileInlined(Game *game, int32_t n, int32_t n2, int32_t n3) {
    if (game->world.tileMap[n][n2] == 0
        || (game->world.tileMap[n][n2] >= tile::SCENERY_FIRST && game->world.tileMap[n][n2] <= tile::SCENERY_LAST)) {
        game->world.tileMap[n][n2] = game_randomCityTile(game);
        game->world.trackMap[n][n2] = (int8_t)n3;
    }
}

bool game_pickValidForestTile(Game *game) {
    int n2 = std::abs(game->rng.next_int() % (game->world.mapWidth - 2)) + 1;
    int n = std::abs(game->rng.next_int() % (game->world.mapHeight - 2)) + 1;
    if (game->world.tileMap[n2][n] != 0
        && (game->world.tileMap[n2][n] < tile::SCENERY_FIRST || game->world.tileMap[n2][n] > tile::SCENERY_LAST))
        return false;

    int8_t by = game->world.tileMap[n2 - 1][n];
    int8_t by2 = game->world.tileMap[n2 + 1][n];
    int8_t by3 = game->world.tileMap[n2][n - 1];
    int8_t by4 = game->world.tileMap[n2][n + 1];
    bool bl2 = by == 43 || by == 44 || by == 45 || by == 46;
    bool bl3 = by2 == 43 || by2 == 44 || by2 == 47 || by2 == 48;
    bool bl4 = by3 == 42 || by3 == 44 || by3 == 46 || by3 == 47;
    bool bl = by4 == 42 || by4 == 44 || by4 == 45 || by4 == 48;
    if (!bl2 && !bl3 && !bl4 && !bl) return false;

    int n3;
    if (bl2) n3 = game->world.trackMap[n2 - 1][n];
    else if (bl3) n3 = game->world.trackMap[n2 + 1][n];
    else if (bl4) n3 = game->world.trackMap[n2][n - 1];
    else n3 = game->world.trackMap[n2][n + 1];
    int n4 = n3 & 0x3F;
    if (n4 <= -1) return false;

    if ((bl2 && bl3 && bl4) || (bl2 && bl3 && bl)
        || (bl2 && bl4 && bl) || (bl3 && bl4 && bl)) {
        game->world.tileMap[n2][n] = 44;
    } else if (bl2 && bl4) {
        game->world.tileMap[n2][n] = 48;
    } else if (bl2 && bl) {
        game->world.tileMap[n2][n] = 47;
    } else if (bl3 && bl4) {
        game->world.tileMap[n2][n] = 45;
    } else if (bl3 && bl) {
        game->world.tileMap[n2][n] = 46;
    } else {
        int n5 = std::abs(game->rng.next_int() % 13);
        if (n5 == 12) {
            game->world.tileMap[n2][n] = 44;
        } else if (n5 == 11) {
            if (bl2) game->world.tileMap[n2][n] = 47;
            if (bl3) game->world.tileMap[n2][n] = 46;
            if (bl4) game->world.tileMap[n2][n] = 45;
            if (bl) game->world.tileMap[n2][n] = 46;
        } else if (n5 == 10) {
            if (bl2) game->world.tileMap[n2][n] = 48;
            if (bl3) game->world.tileMap[n2][n] = 45;
            if (bl4) game->world.tileMap[n2][n] = 48;
            if (bl) game->world.tileMap[n2][n] = 47;
        } else {
            if (bl2 || bl3) game->world.tileMap[n2][n] = 43;
            if (bl4 || bl) game->world.tileMap[n2][n] = 42;
        }
    }

    switch (game->world.tileMap[n2][n]) {
        case 42:
            game_placeCityTileInlined(game, n2 - 1, n, n3);
            game_placeCityTileInlined(game, n2 + 1, n, n3);
            break;
        case 43:
            game_placeCityTileInlined(game, n2, n - 1, n3);
            game_placeCityTileInlined(game, n2, n + 1, n3);
            break;
        case 45:
            game_placeCityTileInlined(game, n2 - 1, n, n3);
            game_placeCityTileInlined(game, n2, n + 1, n3);
            game_placeCityTileInlined(game, n2 - 1, n + 1, n3);
            break;
        case 46:
            game_placeCityTileInlined(game, n2 - 1, n, n3);
            game_placeCityTileInlined(game, n2, n - 1, n3);
            game_placeCityTileInlined(game, n2 - 1, n - 1, n3);
            break;
        case 47:
            game_placeCityTileInlined(game, n2 + 1, n, n3);
            game_placeCityTileInlined(game, n2, n - 1, n3);
            game_placeCityTileInlined(game, n2 + 1, n - 1, n3);
            break;
        case 48:
            game_placeCityTileInlined(game, n2 + 1, n, n3);
            game_placeCityTileInlined(game, n2, n + 1, n3);
            game_placeCityTileInlined(game, n2 + 1, n + 1, n3);
            break;
    }
    game->world.trackMap[n2][n] = (int8_t)n3;
    return true;
}

void game_persistSnapshotMode0(Game *game) {
    game_loadGame(game, 5, save::MODE_WRITE);
    game_writeSaveSnapshot(game);
    game_saveGame(game);
}

int32_t game_computeMapTrainMetric(Game *game, int32_t n) {
    int n2 = game->economy.trainRevenueScore;
    for (int i = 0; i < game->world.mapWidth; ++i) {
        for (int j = 0; j < game->world.mapHeight; ++j) {
            if (game->world.trackMap[i][j] != 65) continue;
            n2 += 3;
            for (int k = 0; k < 500; ++k) {
                if ((game->fleet.trainFlags[k] & 0x80) != 128
                    || game_byteToUnsignedInt(game, game->fleet.trainX[k]) != i || game_byteToUnsignedInt(game, game->fleet.trainY[k]) != j)
                    continue;
                n2 += (game->fleet.trainFlags[k] & 0x40) == 64 ? 400 : 55;
            }
        }
    }
    n2 += game->economy.dayOfMonth + 30 * game->economy.monthOfYear + 365 * (game->economy.year - 1950);
    if (n < 0) {
        if (n2 > game->scenario.bestScenarioScore) game->scenario.bestScenarioScore = n2;
    } else {
        game->scenario.scenarioScores[n] = n2;
    }
    game_persistSnapshotMode0(game);
    return n2;
}

void game_everyTwoTickZ(Game *game) {
    bool bl = false;
    if (!game->scenario.milestonesEnabled) {
        switch (game->scenario.scenarioMilestoneIndex) {
            case 1: bl = game->scenario.currentMilestoneScore <= game->scenario.milestoneThreshold1; break;
            case 2: bl = game->scenario.currentMilestoneScore <= game->scenario.milestoneThreshold2; break;
            case 3: bl = game->scenario.currentMilestoneScore <= game->scenario.milestoneThreshold3; break;
            case 4: bl = game->scenario.currentMilestoneScore <= game->scenario.milestoneThreshold4; break;
            case 5: bl = game->scenario.currentMilestoneScore <= game->scenario.milestoneThreshold5; break;
            case 6:
                bl = game->scenario.currentMilestoneScore <= game->scenario.milestoneThreshold1
                     && game->scenario.milestoneCoThreshold2 <= game->scenario.milestoneThreshold2;
                break;
            case 7:
                bl = game->scenario.currentMilestoneScore <= game->scenario.milestoneThreshold1
                     && game->scenario.milestoneCoThreshold2 <= game->scenario.milestoneThreshold3;
                break;
            case 8:
                bl = game->scenario.currentMilestoneScore <= game->scenario.milestoneThreshold1
                     && game->scenario.milestoneCoThreshold2 <= game->scenario.milestoneThreshold4;
                break;
            case 9:
                bl = game->scenario.currentMilestoneScore <= game->scenario.milestoneThreshold2
                     && game->scenario.milestoneCoThreshold2 <= game->scenario.milestoneThreshold3;
                break;
            case 10:
                bl = game->scenario.currentMilestoneScore <= game->scenario.milestoneThreshold2
                     && game->scenario.milestoneCoThreshold2 <= game->scenario.milestoneThreshold4;
                break;
            case 11:
                bl = game->scenario.currentMilestoneScore <= game->scenario.milestoneThreshold4
                     && game->scenario.milestoneCoThreshold2 <= game->scenario.milestoneThreshold5;
                break;
            case 12: bl = game->scenario.currentMilestoneScore <= game->economy.lifetimeTrainRevenue; break;
            case 13: bl = game->economy.loanBalance == 0; break;
            case 14: bl = game->economy.year == game->scenario.currentMilestoneScore; break;
            case 15: bl = game->scenario.currentMilestoneScore <= game->scenario.aiWagonsBought; break;
        }
    }
    if (!bl) return;
    game->ready = false;
    game->ui.transitionCode = 6;
    game->scenario.milestonesEnabled = true;
    if (game->scenario.scenarioStep >= 2) {
        if (game->scenario.scenarioStepCompare < game->scenario.scenarioStep)
            game->scenario.scenarioStepCompare = game->scenario.scenarioStep;
        game_persistSnapshotMode0(game);
        game->scenario.currentScenarioScore = game_computeMapTrainMetric(game, game->scenario.scenarioStep - 2);
    }
    game_setState(game, +GameState::MenuOverlay);
}

int32_t game_scanMapForMetric(Game *game) {
    int n4 = 0;
    for (int n3 = 0; n3 < game->world.mapWidth; ++n3) {
        for (int n2 = 0; n2 < game->world.mapHeight; ++n2) {
            if (game->world.trackMap[n3][n2] != 65) continue;
            int n = game->world.tileMap[n3][n2];
            if (n >= 70 && n <= 80) {
                if (n > 76) n4 += 94;
                n4 += 196;
            }
            if (n == 76) n4 += 168;
            if (n >= 81 && n <= 86) n4 += game->economy.accel1;
            if (n == 87 || n == 88) n4 += game->economy.costPower;
            if (n == 89 || n == 90) n4 += game->economy.costPower / 2;
        }
    }
    for (int n3 = 0; n3 < 500; ++n3) {
        if ((game->fleet.trainFlags[n3] & 0x80) != 128) continue;
        int n2 = game_byteToUnsignedInt(game, game->fleet.trainX[n3]);
        int n = game_byteToUnsignedInt(game, game->fleet.trainY[n3]);
        if (game->world.trackMap[n2][n] != 65) continue;
        if ((game->fleet.trainFlags[n3] & 0x40) == 64) {
            switch (game->fleet.trainEraOrStage[n3]) {
                case 0: n4 += game->economy.gWagonCost; break;
                case 1: n4 += game->economy.gWagonCost2; break;
                case 2: n4 += game->economy.gWagonCost3; break;
                case 3: n4 += game->economy.gWagonCost4; break;
                case 4: n4 += game->economy.gWagonCost5; break;
                case 5: n4 += game->economy.gWagonCost6; break;
            }
        } else {
            n4 += (game->economy.costGroupC1 + game->economy.costGroupC2 + game->economy.costGroupC3
                   + game->economy.costGroupC4) / 4;
        }
    }
    return n4 - game->economy.loanBalance;
}

void game_newYearNotify(Game *game) {
    for (int n = 0; n < 500; ++n) {
        game->fleet.prevTrainRevenue[n] = game->fleet.trainRevenue[n];
        game->fleet.trainRevenue[n] = 0;
    }
    for (int n = 0; n < 48; ++n) {
        game->economy.prevStationRevenue[n] = game->economy.stationRevenue[n];
        game->economy.stationRevenue[n] = 0;
    }
    game_archiveOrRecall(game);
    game->economy.prevTotalSpent = game->economy.totalSpent;
    game->economy.prevAiMoneySpent = game->economy.aiMoneySpent;
    game->economy.prevFuelCost = game->economy.fuelCostAccrued;
    game->economy.prevSubsidyCost = game->economy.subsidyCost;
    game->economy.prevTotalRevenue = game->economy.totalRevenue;
    game->economy.prevTerrainTotal = game->economy.terrainTotal;
    game->economy.prevEnterpriseTotal = game->economy.decadeEnterpriseTax;
    game->economy.previousYearProfit = game->economy.netProfit;
    for (int n = 10; n >= 0; --n) {
        game->economy.yearProfitHistory[n + 1] = game->economy.yearProfitHistory[n];
        game->economy.yearRevenueHistory[n + 1] = game->economy.yearRevenueHistory[n];
        game->economy.yearCompanyValueHistory[n + 1] = game->economy.yearCompanyValueHistory[n];
    }
    game->economy.yearProfitHistory[0] = game->economy.yearRevenueHistory[0] = game->economy.yearCompanyValueHistory[0] = 0;
    game->economy.decadeCostTotal += game->economy.totalSpent;
    game->economy.decadeSpentTotal += game->economy.aiMoneySpent;
    game->economy.decadeFuelCost += game->economy.fuelCostAccrued;
    game->economy.decadeSubsidyTotal += game->economy.subsidyCost;
    game->economy.decadeRevenueTotal += game->economy.totalRevenue;
    game->economy.decadeProfitTotal += game->economy.netProfit;
    game->economy.decadeEnterpriseTotal += game->economy.decadeEnterpriseTax;
    game->economy.decadeProfitGraphTotal += game->economy.netProfit;
    game->economy.revenueTotal += game->economy.totalRevenue;
    if (game->economy.monthOfYear == 13) {
        game->economy.prevDecadeSpent = game->economy.decadeSpentWithCurrent;
        game->economy.prevDecadeAiSpent = game->economy.decadeAiSpentWithCurrent;
        game->economy.prevDecadeFuel = game->economy.decadeFuelWithCurrent;
        game->economy.prevDecadeSubsidy = game->economy.decadeSubsidyWithCurrent;
        game->economy.prevDecadeRevenue = game->economy.decadeRevenueWithCurrent;
        game->economy.previousLegacyDecadeMetric = game->economy.legacyDecadeMetric;
        game->economy.previousDecadeEnterpriseTax = game->economy.decadeEnterpriseWithCurrent;
        game->economy.previousDecadeProfit = game->economy.decadeProfitWithCurrent;
        game->economy.decadeCostTotal = game->economy.decadeSpentTotal = game->economy.decadeFuelCost = 0;
        game->economy.decadeSubsidyTotal = game->economy.decadeRevenueTotal = game->economy.decadeProfitTotal = 0;
        game->economy.decadeEnterpriseTotal = 0;
        for (int n = 10; n >= 0; --n) {
            game->economy.decadeProfitHistory[n + 1] = game->economy.decadeProfitHistory[n];
            game->economy.altDecadeRevenueTotal[n + 1] = game->economy.altDecadeRevenueTotal[n];
            game->economy.altDecadeTotalCost[n + 1] = game->economy.altDecadeTotalCost[n];
        }
        game->economy.decadeProfitGraphTotal = game->economy.revenueTotal = 0;
        game->economy.decadeProfitHistory[0] = game->economy.altDecadeRevenueTotal[0] = 0;
        game->economy.altDecadeTotalCost[0] = 0;
    }
    game->economy.altDecadeTotalCost[0] = game_scanMapForMetric(game);
    game->economy.money -= game->economy.terrainTotal;
    if (game->economy.terrainTotal > 0) game_pushMoneyHistory(game, -game->economy.terrainTotal);
    game->economy.money -= game->economy.decadeEnterpriseTax;
    if (game->economy.decadeEnterpriseTax > 0)
        game_pushMoneyHistory(game, -game->economy.decadeEnterpriseTax);
    game->economy.totalSpent = game->economy.aiMoneySpent = game->economy.fuelCostAccrued = 0;
    game->economy.subsidyCost = game->economy.totalRevenue = game->economy.netProfit = 0;
    game->economy.decadeEnterpriseTax = 0;
    game->renderer.mapRedrawPending = true;
    game->compareToPrevious = true;
    game->monthRolloverFlag = true;
    if (game->economy.monthOfYear == 13) {
        if (gcs_annualReportEnabled) {
            game->ready = false;
            game->ui.transitionCode = 35;
            game_setState(game, +GameState::MenuOverlay);
        }
    } else if (gcs_monthlyReportEnabled) {
        game->ready = false;
        game->ui.transitionCode = 32;
        game_setState(game, +GameState::MenuOverlay);
    }
}

void game_tickInputOrAi(Game *game, bool bl) {
    for (int i = 0; i < 250; ++i) {
        if ((game->fleet.vehicleFlags[i] & 0x80) != 128) continue;
        int n3 = game_byteToUnsignedInt(game, game->fleet.serviceVehicleX[i]);
        int n4 = game_byteToUnsignedInt(game, game->fleet.serviceVehicleY[i]);
        int n5 = n3 - 3;
        int n6 = n4 - 3;
        int n7 = n3 + 3;
        int n8 = n4 + 3;
        if (n5 < 0) n5 = 0;
        if (n6 < 0) n6 = 0;
        if (n7 >= game->world.mapWidth) n7 = game->world.mapWidth - 1;
        if (n8 >= game->world.mapHeight) n8 = game->world.mapHeight - 1;
        int n9 = 0;
        bool seenZones[64] = {false};
        bool seenIndustry1[64] = {false};
        bool seenIndustry2[64] = {false};
        bool seenIndustry3[64] = {false};
        for (int n2 = n5; n2 <= n7; ++n2) {
            for (int n = n6; n <= n8; ++n) {
                int tile = game->world.tileMap[n2][n];
                int n11 = game->world.trackMap[n2][n] & 0x3F;
                if (tile >= 34 && tile <= 40) {
                    if ((game->world.trackMap[n2][n] & 0xC0) == 128 && !bl
                        && !seenZones[n11]) {
                        seenZones[n11] = true;
                        if ((game->fleet.serviceUnloadFlags[i] & 1) == 1) {
                            int add = game->world.zonePopulation[n11]
                                      * (game->world.constructionTimers[n11] + 10
                                         + game->world.zonePopulation[n11] / 10
                                         + game->rng.next_int() % 40) / 170;
                            game->fleet.serviceResourceStock[i][0] =
                                (int16_t)(game->fleet.serviceResourceStock[i][0] + add);
                        }
                        if ((game->fleet.serviceUnloadFlags[i] & 0x10) == 0x10) {
                            int add = game->world.zonePopulation[n11] / 2
                                      * (game->world.constructionTimers[n11] + 10
                                         + game->world.zonePopulation[n11] / 10
                                         + std::abs(game->rng.next_int() % 40)) / 170;
                            game->fleet.serviceResourceStock[i][4] =
                                (int16_t)(game->fleet.serviceResourceStock[i][4] + add);
                        }
                        game->world.zonePopulation[n11] =
                            (int16_t)(game->world.zonePopulation[n11]
                                      + game->rng.next_int() % 2);
                    }
                    ++n9;
                }
                if (tile >= 65 && tile <= 68 && !bl
                    && (game->world.trackMap[n2][n] & 0xC0) == 192
                    && !seenIndustry1[n11]) {
                    seenIndustry1[n11] = true;
                    int n10 = game->world.cityPopulation[n11] * 4 / 30 / game->world.cityIndex[n11];
                    if (n10 == 0) n10 = 1;
                    game->fleet.serviceResourceStock[i][1] =
                        (int16_t)(game->fleet.serviceResourceStock[i][1] + n10);
                    game->economy.stationRevenue[n11] = (int16_t)(game->economy.stationRevenue[n11] + n10);
                    game->fleet.serviceLoadFlags[i] = (int8_t)(game->fleet.serviceLoadFlags[i] | 4);
                }
                if (tile >= 53 && tile <= 55 && !bl
                    && (game->world.trackMap[n2][n] & 0xC0) == 192
                    && !seenIndustry2[n11]) {
                    seenIndustry2[n11] = true;
                    int n10 = game->world.cityPopulation[n11] * 4 / 30 / game->world.cityIndex[n11];
                    if (n10 == 0) n10 = 1;
                    game->fleet.serviceResourceStock[i][2] =
                        (int16_t)(game->fleet.serviceResourceStock[i][2] + n10);
                    game->economy.stationRevenue[n11] = (int16_t)(game->economy.stationRevenue[n11] + n10);
                    game->fleet.serviceLoadFlags[i] = (int8_t)(game->fleet.serviceLoadFlags[i] | 8);
                }
                if (tile >= 59 && tile <= 61 && !bl
                    && (game->world.trackMap[n2][n] & 0xC0) == 192
                    && !seenIndustry3[n11]) {
                    seenIndustry3[n11] = true;
                    int n10 = game->world.cityPopulation[n11] * 4 / 30 / game->world.cityIndex[n11];
                    if (n10 == 0) n10 = 1;
                    game->fleet.serviceResourceStock[i][3] =
                        (int16_t)(game->fleet.serviceResourceStock[i][3] + n10);
                    game->economy.stationRevenue[n11] = (int16_t)(game->economy.stationRevenue[n11] + n10);
                    game->fleet.serviceLoadFlags[i] = (int8_t)(game->fleet.serviceLoadFlags[i] | 2);
                }
                if (tile >= 50 && tile <= 52)
                    game->fleet.serviceUnloadFlags[i] = (int8_t)(game->fleet.serviceUnloadFlags[i] | 4);
                if (tile >= 56 && tile <= 58)
                    game->fleet.serviceUnloadFlags[i] = (int8_t)(game->fleet.serviceUnloadFlags[i] | 8);
                if (tile >= 62 && tile <= 64)
                    game->fleet.serviceUnloadFlags[i] = (int8_t)(game->fleet.serviceUnloadFlags[i] | 2);
            }
        }
        if (n9 / 3 > 0) {
            game->fleet.serviceUnloadFlags[i] = (int8_t)(game->fleet.serviceUnloadFlags[i] | 1);
            game->fleet.serviceLoadFlags[i] = (int8_t)(game->fleet.serviceLoadFlags[i] | 1);
        }
        if (n9 / 7 > 0) {
            game->fleet.serviceUnloadFlags[i] = (int8_t)(game->fleet.serviceUnloadFlags[i] | 0x10);
            game->fleet.serviceLoadFlags[i] = (int8_t)(game->fleet.serviceLoadFlags[i] | 0x10);
        }
        bool large = (game->fleet.vehicleFlags[i] & 0x20) == 0x20;
        for (int n = 0; n < 5; ++n) {
            int limit;
            if (large) limit = n == 0 ? 250 : (n == 4 ? 325 : 450);
            else limit = n == 0 ? 100 : (n == 4 ? 150 : 200);
            if (game->fleet.serviceResourceStock[i][n] > limit)
                game->fleet.serviceResourceStock[i][n] = (int16_t)limit;
        }
    }
}

// frame timers, tile-fade queue, train updates, then (when counterA reaches
// dH) the day/month/year calendar cadence with its monthly/yearly hooks.
void game_simulationTick(Game *game) {
            game->tickStart = midp_current_time_millis();
            if (++game->timerA1 == 2) {
                game->timerA1 = 0;
                if (++game->timerA2 == 3) game->timerA2 = 0;
            }
            if (++game->timerB1 == 2) {
                game->timerB1 = 0;
                if (++game->timerB2 == 3) game->timerB2 = 0;
            }
            if (++game->timerC1 == 3) {
                game->timerC1 = 0;
                if (++game->timerC2 == 3) game->timerC2 = 0;
            }
            if (++game->world.woodAgeTimer == 150) {
                for (int n27 = 0; n27 < game->world.mapWidth; ++n27) {
                    for (int n = 0; n < game->world.mapHeight; ++n) {
                        if (game->world.tileMap[n27][n] < 59 || game->world.tileMap[n27][n] > 61
                            || game->rng.next_int() % 3 != 0)
                            continue;
                        ++game->world.tileMap[n27][n];
                        if (game->world.tileMap[n27][n] == 62) game->world.tileMap[n27][n] = 59;
                    }
                }
                game->world.woodAgeTimer = 0;
            }
            if (++game->world.phaseTimer == 10) game->world.phaseTimer = 0;

            for (int n27 = 0; n27 < 50; ++n27) {
                if (game->world.tileQueue[n27][0] <= 0) continue;
                --game->world.tileQueue[n27][0];
                int n = game_byteToUnsignedInt(game, game->world.tileQueue[n27][1]);
                int n29 = game_byteToUnsignedInt(game, game->world.tileQueue[n27][2]);
                if (game->world.tileQueue[n27][0] == 0 && n >= 0 && n29 >= 0
                    && n < game->world.mapWidth && n29 < game->world.mapHeight)
                    game->world.tileMap[n][n29] = (int8_t)(game_byteToUnsignedInt(game, game->world.tileMap[n][n29]) - 128);
                if (game->world.tileQueue[n27][0] != 7 || n < 0 || n29 < 0
                    || n >= game->world.mapWidth || n29 >= game->world.mapHeight)
                    continue;
                int n30 = game_byteToUnsignedInt(game, game->world.tileMap[n][n29]) - 128;
                if (game->world.tileQueueVisualOnly[n27]) continue;
                if (n30 >= 29 && n30 <= 33) game->world.tileMap[n][n29] = -128;
                if ((n30 >= 70 && n30 <= 75) || (n30 >= 87 && n30 <= 90)) {
                    game->world.trackMap[n][n29] = 0;
                    game->world.tileMap[n][n29] = -128;
                    game->renderer.lastViewportCenterX = -1;
                }
                if (n30 >= 77 && n30 <= 80) {
                    game->world.tileMap[n][n29] = (int8_t)(128 + n30 - 56);
                    game->world.trackMap[n][n29] = 0;
                    game->renderer.lastViewportCenterX = -1;
                }
                if (n30 == 81 || n30 == 82) {
                    game->world.tileMap[n][n29] = -128;
                    game->world.trackMap[n][n29] = 0;
                    game->renderer.lastViewportCenterX = -1;
                }
                if (n30 >= 83 && n30 <= 86) {
                    game->world.tileMap[n][n29] = (int8_t)(128 + n30 - 76);
                    game->world.trackMap[n][n29] = 0;
                    game->renderer.lastViewportCenterX = -1;
                }
                if (n30 == 76) {
                    int n31 = game->world.trackMap[n][n29] & 7;
                    int n32 = (game->world.trackMap[n][n29] & 0x3F) >> 3;
                    if (n31 == 1 && n32 == 1) {
                        game->world.tileMap[n][n29] = -128;
                        game->world.trackMap[n][n29] = 0;
                    } else if (n31 == 1) {
                        game->world.tileMap[n][n29] = -57;
                        game->world.trackMap[n][n29] = (int8_t)(0x40 | n32);
                    } else if (n32 == 1) {
                        game->world.tileMap[n][n29] = -58;
                        game->world.trackMap[n][n29] = (int8_t)(0x40 | n31);
                    }
                    game->renderer.lastViewportCenterX = -1;
                }
            }

            for (int n27 = 0; n27 < 500; ++n27) {
                if ((game->fleet.trainFlags[n27] & 0x80) != 128
                    || (game->fleet.trainFlags[n27] & 0x40) != 64)
                    continue;
                if ((game->fleet.trainFlags[n27] & 0x20) == 32) {
                    if (game->fleet.trainEnergyOrCount[n27] > 0) {
                        --game->fleet.trainEnergyOrCount[n27];
                        continue;
                    }
                    game->fleet.trainPhase[n27] = 0;
                    game->fleet.trainFlags[n27] = 0;
                    continue;
                }
                game_updateTrain(game, n27);
                game_updateTrain(game, n27);
                if (game->fleet.trainPhase[n27] == 0) game_mergeTrainAt(game, n27);
            }

            if (game->counterA >= game->dayTickThreshold) {
                game_computeYearEndMetrics(game);
                if (game->frameIndex > 0) --game->frameIndex;
                game->counterA = 0;
                ++game->economy.dayOfMonth;
                game_everyMonthlyTick(game);
                if (game->economy.dayOfMonth % 4 == 0) game_maybeAiTakeTurn(game);
                if (game->economy.dayOfMonth % 2 == 0) {
                    if (game->economy.dayOfMonth % 10 == 0) game_everyTenTick(game);
                    game_everyTwoTick(game);
                    game_everyTwoTickZ(game);
                    if (game->scenario.aiExpansionEnabled) {
                        int found = 0;
                        for (int n = 0; n < 30; ++n)
                            found = found || game_pickValidForestTile(game);
                        if (found) game_maybeAITick(game);
                    }
                    game_tickInputOrAi(game, false);
                }
                int oldMonth = game->economy.monthOfYear;
                switch (game->economy.monthOfYear) {
                    case 1: case 3: case 5: case 7: case 8: case 10: case 12:
                        if (game->economy.dayOfMonth == 32) ++game->economy.monthOfYear;
                        break;
                    case 2:
                        if ((game->economy.year % 4 != 0 && game->economy.dayOfMonth == 29)
                            || (game->economy.year % 4 == 0 && game->economy.dayOfMonth == 30))
                            ++game->economy.monthOfYear;
                        break;
                    case 4: case 6: case 9: case 11:
                        if (game->economy.dayOfMonth == 31) ++game->economy.monthOfYear;
                        break;
                }
                if (oldMonth != game->economy.monthOfYear) {
                    game->economy.dayOfMonth = 1;
                    game_newYearNotify(game);
                    game_dayResetP(game);
                    game_dayResetAg(game);
                    game_dayResetAd(game);
                    switch (std::abs(game->rng.next_int() % 7)) {
                        case 0: game->economy.moneyBaseA += 9; game->economy.moneyBaseB += 9; break;
                        case 1: ++game->economy.baseTrainSpeed; game->economy.accel1 += 3; game->economy.brake1 += 3; break;
                        case 2: ++game->economy.baseTrainSpeed2; game->economy.accel2 += 2; break;
                        case 3: game->economy.costPower += 47; game->economy.costStruct += 98; break;
                        case 4:
                            game->economy.costOverhead += 3; game->economy.costMaintenance += 59; game->economy.costCrew += 4;
                            break;
                        case 5:
                            game->economy.gWagonCost += 378; game->economy.gWagonCost2 += 432;
                            game->economy.gWagonCost3 += 518; game->economy.gWagonCost4 += 601;
                            game->economy.gWagonCost5 += 714; game->economy.gWagonCost6 += 826;
                            break;
                        case 6:
                            game->economy.costGroupC1 += 45; game->economy.costGroupC2 += 51;
                            game->economy.costGroupC3 += 43; game->economy.costGroupC4 += 50;
                            break;
                    }
                }
                if (game->economy.monthOfYear == 13) {
                    game_revenueOrYear(game);
                    game_postRevenueReset(game);
                    game->economy.monthOfYear = 1;
                    ++game->economy.year;
                    if (game->economy.money < 0) {
                        game->ready = false;
                        game->ui.transitionCode = 7;
                        game_setState(game, +GameState::MenuOverlay);
                    }
                }
                int oldEra = game->era;
                if (game->economy.dayOfMonth == 5 && game->economy.monthOfYear == 3 && game->economy.year == 1951 && game->era < 2)
                    game->era = 2;
                if (game->economy.dayOfMonth == 17 && game->economy.monthOfYear == 7 && game->economy.year == 1953 && game->era < 3)
                    game->era = 3;
                if (game->economy.dayOfMonth == 4 && game->economy.monthOfYear == 12 && game->economy.year == 1957 && game->era < 4)
                    game->era = 4;
                if (game->economy.dayOfMonth == 26 && game->economy.monthOfYear == 2 && game->economy.year == 1962 && game->era < 5)
                    game->era = 5;
                if (oldEra != game->era) {
                    game->ready = false;
                    game->ui.transitionCode = 13;
                    game_setState(game, +GameState::MenuOverlay);
                }
            } else {
                ++game->counterA;
            }
            game->tickEnd = midp_current_time_millis();
}
