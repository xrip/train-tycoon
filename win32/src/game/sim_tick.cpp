// Simulation tick functions.
#include "game_internal.h"
#include <cstdlib>

void game_decayConstructionTimersQuarterly(Game *game) {
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

void game_recountZonePopulation(Game *game) {
    for (int n = 0; n < 64; ++n) game->world.zonePopulation[n] = 0;
    for (int n = 0; n < game->world.mapWidth; ++n) {
        for (int i = 0; i < game->world.mapHeight; ++i) {
            if (game->world.tileMap[n][i] < tile::BUILT_FIRST || game->world.tileMap[n][i] > tile::CITY_ZONE_LAST) continue;
            int n2 = game->world.trackMap[n][i] & 0x3F;
            ++game->world.zonePopulation[n2];
        }
    }
}

void game_updateCityPopulation(Game *game) {
    for (int i = 0; i < 48; ++i) {
        game->world.cityPopulation[i] += (int8_t)(game->rng.next_int() % 8);
        if (game->world.cityPopulation[i] < 10) game->world.cityPopulation[i] = 10;
        if (game->world.cityPopulation[i] > 90) game->world.cityPopulation[i] = 90;
    }
}

void game_ageTrainsOneYear(Game *game) {
    for (int n = 0; n < 500; ++n) ++game->fleet.trainAgeYears[n];
}

void game_chargeTrainRunningCosts(Game *game) {
    if (game->scenario.routeResourceFlags[0] != 0) return;
    for (int i = 0; i < 500; ++i) {
        if (!train_flag::isUsed(game->fleet.trainFlags[i]) || !train_flag::isHead(game->fleet.trainFlags[i]))
            continue;
        int n2 = unsignedByte(game->fleet.trainX[i]);
        int n = unsignedByte(game->fleet.trainY[i]);
        if (game->world.trackMap[n2][n] != 65) continue;
        int n3 = game_countCarsInTrain(game, i) * 2;
        if (train_flag::type(game->fleet.trainFlags[i]) == 0) n3 += 3;
        else if (train_flag::type(game->fleet.trainFlags[i]) == 1) n3 += 7;
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
    if (unsignedByte(game->fleet.trainX[n]) == n2 && unsignedByte(game->fleet.trainY[n]) == n3)
        return true;
    return game_isTrainHeadCarAt(game, game->fleet.nextCar[n], n2, n3);
}

void game_removeTrainFromMap(Game *game, int32_t n, bool bl) {
    if (n == -1) return;
    game_removeTrainFromMap(game, game->fleet.nextCar[n], bl);
    game->fleet.trainFlags[n] = (int8_t)(game->fleet.trainFlags[n] | 0xE0);
    game->fleet.nextCar[n] = -1;
    game->fleet.energyCargoOrRemovalTicks[n] = 7;
    if (bl)
        game_queueTileChange(game, unsignedByte(game->fleet.trainX[n]),
                                    unsignedByte(game->fleet.trainY[n]), false, true);
    game->renderer.lastViewportCenterX = -1;
    game->ready = false;
    game->ui.transitionCode = 16;
    game_setState(game, GameState::MenuOverlay);
}

void game_mergeTrainAt(Game *game, int32_t n) {
    for (int i = 0; i < 500; ++i) {
        if (n == i || !train_flag::isUsed(game->fleet.trainFlags[i])
            || !train_flag::isHead(game->fleet.trainFlags[i])
            || !game_isTrainHeadCarAt(game, i, unsignedByte(game->fleet.trainX[n]),
                                    unsignedByte(game->fleet.trainY[n])))
            continue;
        game_removeTrainFromMap(game, i, true);
        game_removeTrainFromMap(game, n, true);
    }
}

void game_maybeAiTakeTurn(Game *game) {
    if (game->ui.buildMenuMode != 2 && game->ui.buildMenuMode != 4
        && game->rng.next_int() % game->scenario.aiFrequency > game->scenario.aiThreshold)
        game_setState(game, GameState::AiTurn);
}

void game_upgradeOneAiTrainForEra(Game *game) {
    for (int i = 0; i < 500; ++i) {
        if (!train_flag::isUsed(game->fleet.trainFlags[i]) || !train_flag::isHead(game->fleet.trainFlags[i]))
            continue;
        int n2 = unsignedByte(game->fleet.trainX[i]);
        int n = unsignedByte(game->fleet.trainY[i]);
        if ((game->world.trackMap[n2][n] != 66 && game->world.trackMap[n2][n] != 67
             && game->world.trackMap[n2][n] != 68)
            || game->fleet.trainEra[i] >= game->era)
            continue;
        game->fleet.trainEra[i] = (int8_t)game->era;
        game->fleet.trainFlags[i] = static_cast<int8_t>(
            train_flag::USED_SLOT | train_flag::HEAD | game->era / 3);
        game->fleet.secondaryCargoOrBaseEnergy[i] = (int8_t)(8 - game->era);
        break;
    }
}

void game_resetAiDepotTrainAges(Game *game) {
    for (int i = 0; i < 500; ++i) {
        if (!train_flag::isUsed(game->fleet.trainFlags[i])) continue;
        int n2 = unsignedByte(game->fleet.trainX[i]);
        int n = unsignedByte(game->fleet.trainY[i]);
        if (game->world.trackMap[n2][n] == 66 || game->world.trackMap[n2][n] == 67
            || game->world.trackMap[n2][n] == 68)
            game->fleet.trainAgeYears[i] = 0;
    }
}

void game_calculatePeriodProfit(Game *game) {
    game->economy.terrainTotal = game->economy.loanBalance * game->economy.interestRate / 1200;
    game->economy.decadeEnterpriseTax = game->economy.totalRevenue * game->economy.enterpriseTaxPct / 100;
    game->economy.netProfit = game->economy.totalRevenue - game->economy.terrainTotal - game->economy.totalSpent
                     - game->economy.aiMoneySpent - game->economy.fuelCostAccrued - game->economy.subsidyCost
                     - game->economy.decadeEnterpriseTax;
}

void game_computeYearEndMetrics(Game *game) {
    game_calculatePeriodProfit(game);
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

void game_queueTileChange(Game *game, int32_t n, int32_t n2,
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
        if (game->world.tileQueue[n3].countdown != 0
            && unsignedByte(game->world.tileQueue[n3].x) == n
            && unsignedByte(game->world.tileQueue[n3].y) == n2)
            bl3 = false;
    }
    if (!bl3) return;
    if (unsignedByte(game->world.tileMap[n][n2]) < 128)
        game->world.tileMap[n][n2] = (int8_t)(game->world.tileMap[n][n2] + 128);
    n3 = 0;
    for (int i = 49; i >= 0; --i) {
        if (game->world.tileQueue[i].countdown == 0) {
            n3 = i;
            break;
        }
    }
    game->world.tileQueue[n3].countdown = 14;
    game->world.tileQueueVisualOnly[n3] = bl2;
    game->world.tileQueue[n3].x = static_cast<int8_t>(n);
    game->world.tileQueue[n3].y = static_cast<int8_t>(n2);
}

void game_placeCityTile(Game *game, int32_t n, int32_t n2, int32_t n3) {
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
            game_placeCityTile(game, n2 - 1, n, n3);
            game_placeCityTile(game, n2 + 1, n, n3);
            break;
        case 43:
            game_placeCityTile(game, n2, n - 1, n3);
            game_placeCityTile(game, n2, n + 1, n3);
            break;
        case 45:
            game_placeCityTile(game, n2 - 1, n, n3);
            game_placeCityTile(game, n2, n + 1, n3);
            game_placeCityTile(game, n2 - 1, n + 1, n3);
            break;
        case 46:
            game_placeCityTile(game, n2 - 1, n, n3);
            game_placeCityTile(game, n2, n - 1, n3);
            game_placeCityTile(game, n2 - 1, n - 1, n3);
            break;
        case 47:
            game_placeCityTile(game, n2 + 1, n, n3);
            game_placeCityTile(game, n2, n - 1, n3);
            game_placeCityTile(game, n2 + 1, n - 1, n3);
            break;
        case 48:
            game_placeCityTile(game, n2 + 1, n, n3);
            game_placeCityTile(game, n2, n + 1, n3);
            game_placeCityTile(game, n2 + 1, n + 1, n3);
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
                if (!train_flag::isUsed(game->fleet.trainFlags[k])
                    || unsignedByte(game->fleet.trainX[k]) != i || unsignedByte(game->fleet.trainY[k]) != j)
                    continue;
                n2 += train_flag::isHead(game->fleet.trainFlags[k]) ? 400 : 55;
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

void game_checkScenarioMilestone(Game *game) {
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
    game_setState(game, GameState::MenuOverlay);
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
        if (!train_flag::isUsed(game->fleet.trainFlags[n3])) continue;
        int n2 = unsignedByte(game->fleet.trainX[n3]);
        int n = unsignedByte(game->fleet.trainY[n3]);
        if (game->world.trackMap[n2][n] != 65) continue;
        if (train_flag::isHead(game->fleet.trainFlags[n3])) {
            switch (game->fleet.trainEra[n3]) {
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
    game_calculatePeriodProfit(game);
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
        if (annualReportEnabled) {
            game->ready = false;
            game->ui.transitionCode = 35;
            game_setState(game, GameState::MenuOverlay);
        }
    } else if (monthlyReportEnabled) {
        game->ready = false;
        game->ui.transitionCode = 32;
        game_setState(game, GameState::MenuOverlay);
    }
}

void game_tickInputOrAi(Game *game, bool bl) {
    for (int i = 0; i < 250; ++i) {
        if ((game->fleet.vehicleFlags[i] & 0x80) != 128) continue;
        int n3 = unsignedByte(game->fleet.serviceVehicleX[i]);
        int n4 = unsignedByte(game->fleet.serviceVehicleY[i]);
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

namespace {

void advanceTileAnimationFrames(Game *game) {
    if (++game->tile50AnimationPhase == 2) {
        game->tile50AnimationPhase = 0;
        if (++game->tile50AnimationFrame == 3) game->tile50AnimationFrame = 0;
    }
    if (++game->tile64AnimationPhase == 2) {
        game->tile64AnimationPhase = 0;
        if (++game->tile64AnimationFrame == 3) game->tile64AnimationFrame = 0;
    }
    if (++game->tile55AnimationPhase == 3) {
        game->tile55AnimationPhase = 0;
        if (++game->tile55AnimationFrame == 3) game->tile55AnimationFrame = 0;
    }
    if (++game->world.woodAgeTimer == 150) {
        for (int mapX = 0; mapX < game->world.mapWidth; ++mapX) {
            for (int mapY = 0; mapY < game->world.mapHeight; ++mapY) {
                if (game->world.tileMap[mapX][mapY] < 59 || game->world.tileMap[mapX][mapY] > 61
                    || game->rng.next_int() % 3 != 0)
                    continue;
                ++game->world.tileMap[mapX][mapY];
                if (game->world.tileMap[mapX][mapY] == 62) game->world.tileMap[mapX][mapY] = 59;
            }
        }
        game->world.woodAgeTimer = 0;
    }
    if (++game->world.phaseTimer == 10) game->world.phaseTimer = 0;
}

void applyQueuedTileChanges(Game *game) {
    for (int queueIndex = 0; queueIndex < kTileQueueCount; ++queueIndex) {
        TileChange& change = game->world.tileQueue[queueIndex];
        if (change.countdown <= 0) continue;
        --change.countdown;
        int tileX = unsignedByte(change.x);
        int tileY = unsignedByte(change.y);
        if (change.countdown == 0 && tileX >= 0 && tileY >= 0
            && tileX < game->world.mapWidth && tileY < game->world.mapHeight)
            game->world.tileMap[tileX][tileY] = (int8_t)(unsignedByte(game->world.tileMap[tileX][tileY]) - 128);
        if (change.countdown != 7 || tileX < 0 || tileY < 0
            || tileX >= game->world.mapWidth || tileY >= game->world.mapHeight)
            continue;
        int tileCode = unsignedByte(game->world.tileMap[tileX][tileY]) - 128;
        if (game->world.tileQueueVisualOnly[queueIndex]) continue;
        if (tileCode >= 29 && tileCode <= 33) game->world.tileMap[tileX][tileY] = -128;
        if ((tileCode >= 70 && tileCode <= 75) || (tileCode >= 87 && tileCode <= 90)) {
            game->world.trackMap[tileX][tileY] = 0;
            game->world.tileMap[tileX][tileY] = -128;
            game->renderer.lastViewportCenterX = -1;
        }
        if (tileCode >= 77 && tileCode <= 80) {
            game->world.tileMap[tileX][tileY] = (int8_t)(128 + tileCode - 56);
            game->world.trackMap[tileX][tileY] = 0;
            game->renderer.lastViewportCenterX = -1;
        }
        if (tileCode == 81 || tileCode == 82) {
            game->world.tileMap[tileX][tileY] = -128;
            game->world.trackMap[tileX][tileY] = 0;
            game->renderer.lastViewportCenterX = -1;
        }
        if (tileCode >= 83 && tileCode <= 86) {
            game->world.tileMap[tileX][tileY] = (int8_t)(128 + tileCode - 76);
            game->world.trackMap[tileX][tileY] = 0;
            game->renderer.lastViewportCenterX = -1;
        }
        if (tileCode == 76) {
            int firstTrackOwner = game->world.trackMap[tileX][tileY] & 7;
            int secondTrackOwner = (game->world.trackMap[tileX][tileY] & 0x3F) >> 3;
            if (firstTrackOwner == 1 && secondTrackOwner == 1) {
                game->world.tileMap[tileX][tileY] = -128;
                game->world.trackMap[tileX][tileY] = 0;
            } else if (firstTrackOwner == 1) {
                game->world.tileMap[tileX][tileY] = -57;
                game->world.trackMap[tileX][tileY] = static_cast<int8_t>(
                    track::PRESENT | secondTrackOwner);
            } else if (secondTrackOwner == 1) {
                game->world.tileMap[tileX][tileY] = -58;
                game->world.trackMap[tileX][tileY] = static_cast<int8_t>(
                    track::PRESENT | firstTrackOwner);
            }
            game->renderer.lastViewportCenterX = -1;
        }
    }
}

void updateActiveTrains(Game *game) {
    for (int trainIndex = 0; trainIndex < kTrainSlotCount; ++trainIndex) {
        if (!train_flag::isUsed(game->fleet.trainFlags[trainIndex])
            || !train_flag::isHead(game->fleet.trainFlags[trainIndex]))
            continue;
        if (train_flag::isRemovalPending(game->fleet.trainFlags[trainIndex])) {
            if (game->fleet.energyCargoOrRemovalTicks[trainIndex] > 0) {
                --game->fleet.energyCargoOrRemovalTicks[trainIndex];
                continue;
            }
            game->fleet.trainPhase[trainIndex] = 0;
            game->fleet.trainFlags[trainIndex] = 0;
            continue;
        }
        game_updateTrain(game, trainIndex);
        game_updateTrain(game, trainIndex);
        if (game->fleet.trainPhase[trainIndex] == 0) game_mergeTrainAt(game, trainIndex);
    }
}

void advanceMonthIfDayEnded(Game *game) {
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
}

void applyNewMonthChanges(Game *game) {
    game->economy.dayOfMonth = 1;
    game_newYearNotify(game);
    game_decayConstructionTimersQuarterly(game);
    game_updateCityPopulation(game);
    game_upgradeOneAiTrainForEra(game);
    switch (std::abs(game->rng.next_int() % 7)) {
        case 0: game->economy.moneyBaseA += 9; game->economy.moneyBaseB += 9; break;
        case 1: ++game->economy.baseTrainSpeed; game->economy.accel1 += 3; game->economy.brake1 += 3; break;
        case 2: ++game->economy.baseTrainSpeed2; game->economy.accel2 += 2; break;
        case 3: game->economy.costPower += 47; game->economy.costStruct += 98; break;
        case 4:
            game->economy.costOverhead += 3;
            game->economy.costMaintenance += 59;
            game->economy.costCrew += 4;
            break;
        case 5:
            game->economy.gWagonCost += 378;
            game->economy.gWagonCost2 += 432;
            game->economy.gWagonCost3 += 518;
            game->economy.gWagonCost4 += 601;
            game->economy.gWagonCost5 += 714;
            game->economy.gWagonCost6 += 826;
            break;
        case 6:
            game->economy.costGroupC1 += 45;
            game->economy.costGroupC2 += 51;
            game->economy.costGroupC3 += 43;
            game->economy.costGroupC4 += 50;
            break;
    }
}

void advanceYearIfNeeded(Game *game) {
    if (game->economy.monthOfYear == 13) {
        game_ageTrainsOneYear(game);
        game_resetAiDepotTrainAges(game);
        game->economy.monthOfYear = 1;
        ++game->economy.year;
        if (game->economy.money < 0) {
            game->ready = false;
            game->ui.transitionCode = 7;
            game_setState(game, GameState::MenuOverlay);
        }
    }
}

void advanceEraIfNeeded(Game *game) {
    int previousEra = game->era;
    if (game->economy.dayOfMonth == 5 && game->economy.monthOfYear == 3 && game->economy.year == 1951 && game->era < 2)
        game->era = 2;
    if (game->economy.dayOfMonth == 17 && game->economy.monthOfYear == 7 && game->economy.year == 1953 && game->era < 3)
        game->era = 3;
    if (game->economy.dayOfMonth == 4 && game->economy.monthOfYear == 12 && game->economy.year == 1957 && game->era < 4)
        game->era = 4;
    if (game->economy.dayOfMonth == 26 && game->economy.monthOfYear == 2 && game->economy.year == 1962 && game->era < 5)
        game->era = 5;
    if (previousEra != game->era) {
        game->ready = false;
        game->ui.transitionCode = 13;
        game_setState(game, GameState::MenuOverlay);
    }
}

void advanceGameDay(Game *game) {
    game_computeYearEndMetrics(game);
    if (game->frameIndex > 0) --game->frameIndex;
    game->dayFrameCounter = 0;
    ++game->economy.dayOfMonth;
    game_everyMonthlyTick(game);
    if (game->economy.dayOfMonth % 4 == 0) game_maybeAiTakeTurn(game);
    if (game->economy.dayOfMonth % 2 == 0) {
        if (game->economy.dayOfMonth % 10 == 0) game_everyTenTick(game);
        game_chargeTrainRunningCosts(game);
        game_checkScenarioMilestone(game);
        if (game->scenario.aiExpansionEnabled) {
            int forestFound = 0;
            for (int attempt = 0; attempt < 30; ++attempt)
                forestFound = forestFound || game_pickValidForestTile(game);
            if (forestFound) game_recountZonePopulation(game);
        }
        game_tickInputOrAi(game, false);
    }

    int previousMonth = game->economy.monthOfYear;
    advanceMonthIfDayEnded(game);
    if (previousMonth != game->economy.monthOfYear) applyNewMonthChanges(game);
    advanceYearIfNeeded(game);
    advanceEraIfNeeded(game);
}

} // namespace

// Advance frame animation, queued map work, trains, and the calendar in that order.
void game_simulationTick(Game *game) {
    advanceTileAnimationFrames(game);
    applyQueuedTileChanges(game);
    updateActiveTrains(game);
    if (game->dayFrameCounter >= game->dayTickThreshold) {
        advanceGameDay(game);
    } else {
        ++game->dayFrameCounter;
    }
}
