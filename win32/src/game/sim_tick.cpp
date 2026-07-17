// Simulation tick functions.
#include "game_internal.h"
#include <cstdlib>

void World::decay_construction_timers_quarterly(int32_t month) {
    if (month % 4 == 0) {
        for (int i = 0; i < 64; ++i) {
            if (constructionTimers[i] > 0) --constructionTimers[i];
        }
    }
}

void World::advance_construction_timers(int32_t n, int32_t n2, int32_t n3) {
    bool seen[64] = {false};
    for (int i = n - 5; i <= n + 5; ++i) {
        for (int j = n2 - 5; j <= n2 + 5; ++j) {
            if (!in_bounds(i, j) || (trackMap[i][j] & 0xC0) != 128)
                continue;
            int n4 = trackMap[i][j] & 0x3F;
            if (seen[n4]) continue;
            seen[n4] = true;
            int n5;
            if (n3 < 0) {
                n5 = constructionTimers[n4] * n3 * 3 / 200 - 1;
                if (n5 < n3) n3 = n5;
            } else {
                n5 = (100 - constructionTimers[n4]) / 30 * n3 / 3 + 1;
                if (n5 > n3) n5 = n3;
            }
            constructionTimers[n4] = (int8_t)(constructionTimers[n4] + n5);
            if (constructionTimers[n4] < 0) constructionTimers[n4] = 0;
            if (constructionTimers[n4] > 100) constructionTimers[n4] = 100;
        }
    }
}

void World::recount_zone_population() {
    for (int n = 0; n < 64; ++n) zonePopulation[n] = 0;
    for (int n = 0; n < terrain.mapWidth; ++n) {
        for (int i = 0; i < terrain.mapHeight; ++i) {
            if (terrain.tileMap[n][i] < tile::BUILT_FIRST
                || terrain.tileMap[n][i] > tile::CITY_ZONE_LAST)
                continue;
            int n2 = trackMap[n][i] & 0x3F;
            ++zonePopulation[n2];
        }
    }
}

void World::update_city_population(JavaRandom& random) {
    for (int i = 0; i < 48; ++i) {
        cities[i].population += (int8_t)(random.next_int() % 8);
        if (cities[i].population < 10) cities[i].population = 10;
        if (cities[i].population > 90) cities[i].population = 90;
    }
}

void Fleet::age_one_year() {
    for (TrainCar& car : trainCars) ++car.ageYears;
}

void game_chargeTrainRunningCosts(Game *game) {
    if (game->scenario.routeResourceFlags[0] != 0) return;
    for (int i = 0; i < 500; ++i) {
        if (!train_flag::isUsed(game->fleet.trainCars[i].flags) || !train_flag::isHead(game->fleet.trainCars[i].flags))
            continue;
        int n2 = unsignedByte(game->fleet.trainCars[i].x);
        int n = unsignedByte(game->fleet.trainCars[i].y);
        if (game->world.trackMap[n2][n] != 65) continue;
        int n3 = game->fleet.count_cars(i) * 2;
        if (train_flag::type(game->fleet.trainCars[i].flags) == 0) n3 += 3;
        else if (train_flag::type(game->fleet.trainCars[i].flags) == 1) n3 += 7;
        game->fleet.trainCars[i].revenue -= n3;
        game->economy.money -= n3;
        game->economy.fuelCostAccrued += n3;
    }
}

int32_t Fleet::count_cars(int32_t car) const {
    return car != -1 ? count_cars(trainCars[car].nextCar) + 1 : 0;
}

void game_removeTrainFromMap(Game *game, int32_t n, bool bl) {
    if (n == -1) return;
    game_removeTrainFromMap(game, game->fleet.trainCars[n].nextCar, bl);
    game->fleet.trainCars[n].flags = (int8_t)(game->fleet.trainCars[n].flags | 0xE0);
    game->fleet.trainCars[n].nextCar = -1;
    game->fleet.trainCars[n].energyCargoOrRemovalTicks = 7;
    if (bl)
        game_queueTileChange(game, unsignedByte(game->fleet.trainCars[n].x),
                                    unsignedByte(game->fleet.trainCars[n].y), false, true);
    game->renderer.lastViewportCenterX = -1;
    game->ready = false;
    game->ui.transitionCode = 16;
    game_setState(game, GameState::MenuOverlay);
}

void game_mergeTrainAt(Game *game, int32_t n) {
    for (int i = 0; i < 500; ++i) {
        if (n == i || !train_flag::isUsed(game->fleet.trainCars[i].flags)
            || !train_flag::isHead(game->fleet.trainCars[i].flags)
            || !game->fleet.has_car_at(i, unsignedByte(game->fleet.trainCars[n].x),
                                    unsignedByte(game->fleet.trainCars[n].y)))
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
        if (!train_flag::isUsed(game->fleet.trainCars[i].flags) || !train_flag::isHead(game->fleet.trainCars[i].flags))
            continue;
        int n2 = unsignedByte(game->fleet.trainCars[i].x);
        int n = unsignedByte(game->fleet.trainCars[i].y);
        if ((game->world.trackMap[n2][n] != 66 && game->world.trackMap[n2][n] != 67
             && game->world.trackMap[n2][n] != 68)
            || game->fleet.trainCars[i].era >= game->era)
            continue;
        game->fleet.trainCars[i].era = (int8_t)game->era;
        game->fleet.trainCars[i].flags = static_cast<int8_t>(
            train_flag::USED_SLOT | train_flag::HEAD | game->era / 3);
        game->fleet.trainCars[i].secondaryCargoOrBaseEnergy = (int8_t)(8 - game->era);
        break;
    }
}

void game_resetAiDepotTrainAges(Game *game) {
    for (int i = 0; i < 500; ++i) {
        if (!train_flag::isUsed(game->fleet.trainCars[i].flags)) continue;
        int n2 = unsignedByte(game->fleet.trainCars[i].x);
        int n = unsignedByte(game->fleet.trainCars[i].y);
        if (game->world.trackMap[n2][n] == 66 || game->world.trackMap[n2][n] == 67
            || game->world.trackMap[n2][n] == 68)
            game->fleet.trainCars[i].ageYears = 0;
    }
}

void Economy::calculate_period_profit() {
    terrainTotal = loanBalance * interestRate / 1200;
    decadeEnterpriseTax = totalRevenue * enterpriseTaxPct / 100;
    netProfit = totalRevenue - terrainTotal - totalSpent - aiMoneySpent
                - fuelCostAccrued - subsidyCost - decadeEnterpriseTax;
}

void Economy::compute_year_end_metrics() {
    calculate_period_profit();
    yearProfitHistory[0] = netProfit;
    yearRevenueHistory[0] = totalRevenue;
    decadeProfitHistory[0] = decadeProfitGraphTotal + netProfit;
    altDecadeRevenueTotal[0] = revenueTotal + totalRevenue;
    decadeSpentWithCurrent = decadeCostTotal + totalSpent;
    decadeAiSpentWithCurrent = decadeSpentTotal + aiMoneySpent;
    decadeFuelWithCurrent = decadeFuelCost + fuelCostAccrued;
    decadeSubsidyWithCurrent = decadeSubsidyTotal + subsidyCost;
    decadeRevenueWithCurrent = decadeRevenueTotal + totalRevenue;
    decadeProfitWithCurrent = decadeProfitTotal + netProfit;
    decadeEnterpriseWithCurrent = decadeEnterpriseTotal + decadeEnterpriseTax;
}

void game_queueTileChange(Game *game, int32_t n, int32_t n2,
                                 bool bl, bool bl2) {
    bool bl3 = !bl;
    int n3;
    if (bl) {
        n3 = game->world.terrain.tileMap[n][n2] >= tile::SCENERY_FIRST && game->world.terrain.tileMap[n][n2] <= tile::SCENERY_LAST
             ? game->economy.baseTrainSpeed2 : -game->economy.accel2;
        if (game->economy.money >= n3 || game->economy.money < 0) {
            game->build.backgroundCost += n3;
            bl3 = true;
            game->build.demolitionAffordable = true;
        } else {
            game->build.demolitionAffordable = false;
        }
    }
    if (!bl3) return;
    game->world.terrain.queue_change(n, n2, bl2);
}

bool Terrain::queue_change(int32_t x, int32_t y, bool visual_only) {
    for (int index = kTileQueueCount - 1; index >= 0; --index) {
        if (tileQueue[index].countdown != 0
            && unsignedByte(tileQueue[index].x) == x
            && unsignedByte(tileQueue[index].y) == y)
            return false;
    }
    if (unsignedByte(tileMap[x][y]) < 128)
        tileMap[x][y] = (int8_t)(tileMap[x][y] + 128);
    int slot = 0;
    for (int index = kTileQueueCount - 1; index >= 0; --index) {
        if (tileQueue[index].countdown == 0) {
            slot = index;
            break;
        }
    }
    tileQueue[slot].countdown = 14;
    tileQueueVisualOnly[slot] = visual_only;
    tileQueue[slot].x = static_cast<int8_t>(x);
    tileQueue[slot].y = static_cast<int8_t>(y);
    return true;
}

void game_placeCityTile(Game *game, int32_t n, int32_t n2, int32_t n3) {
    if (game->world.terrain.tileMap[n][n2] == 0
        || (game->world.terrain.tileMap[n][n2] >= tile::SCENERY_FIRST && game->world.terrain.tileMap[n][n2] <= tile::SCENERY_LAST)) {
        game->world.terrain.tileMap[n][n2] = game_randomCityTile(game);
        game->world.trackMap[n][n2] = (int8_t)n3;
    }
}

bool game_pickValidForestTile(Game *game) {
    int n2 = std::abs(game->rng.next_int() % (game->world.terrain.mapWidth - 2)) + 1;
    int n = std::abs(game->rng.next_int() % (game->world.terrain.mapHeight - 2)) + 1;
    if (game->world.terrain.tileMap[n2][n] != 0
        && (game->world.terrain.tileMap[n2][n] < tile::SCENERY_FIRST || game->world.terrain.tileMap[n2][n] > tile::SCENERY_LAST))
        return false;

    int8_t by = game->world.terrain.tileMap[n2 - 1][n];
    int8_t by2 = game->world.terrain.tileMap[n2 + 1][n];
    int8_t by3 = game->world.terrain.tileMap[n2][n - 1];
    int8_t by4 = game->world.terrain.tileMap[n2][n + 1];
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
        game->world.terrain.tileMap[n2][n] = 44;
    } else if (bl2 && bl4) {
        game->world.terrain.tileMap[n2][n] = 48;
    } else if (bl2 && bl) {
        game->world.terrain.tileMap[n2][n] = 47;
    } else if (bl3 && bl4) {
        game->world.terrain.tileMap[n2][n] = 45;
    } else if (bl3 && bl) {
        game->world.terrain.tileMap[n2][n] = 46;
    } else {
        int n5 = std::abs(game->rng.next_int() % 13);
        if (n5 == 12) {
            game->world.terrain.tileMap[n2][n] = 44;
        } else if (n5 == 11) {
            if (bl2) game->world.terrain.tileMap[n2][n] = 47;
            if (bl3) game->world.terrain.tileMap[n2][n] = 46;
            if (bl4) game->world.terrain.tileMap[n2][n] = 45;
            if (bl) game->world.terrain.tileMap[n2][n] = 46;
        } else if (n5 == 10) {
            if (bl2) game->world.terrain.tileMap[n2][n] = 48;
            if (bl3) game->world.terrain.tileMap[n2][n] = 45;
            if (bl4) game->world.terrain.tileMap[n2][n] = 48;
            if (bl) game->world.terrain.tileMap[n2][n] = 47;
        } else {
            if (bl2 || bl3) game->world.terrain.tileMap[n2][n] = 43;
            if (bl4 || bl) game->world.terrain.tileMap[n2][n] = 42;
        }
    }

    switch (game->world.terrain.tileMap[n2][n]) {
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
    game->saveCodec.load_slot(5, save::MODE_WRITE);
    game->saveCodec.walk_settings(*game);
    game->saveCodec.persist();
}

int32_t game_computeMapTrainMetric(Game *game, int32_t n) {
    int n2 = game->economy.trainRevenueScore;
    for (int i = 0; i < game->world.terrain.mapWidth; ++i) {
        for (int j = 0; j < game->world.terrain.mapHeight; ++j) {
            if (game->world.trackMap[i][j] != 65) continue;
            n2 += 3;
            for (int k = 0; k < 500; ++k) {
                if (!train_flag::isUsed(game->fleet.trainCars[k].flags)
                    || unsignedByte(game->fleet.trainCars[k].x) != i || unsignedByte(game->fleet.trainCars[k].y) != j)
                    continue;
                n2 += train_flag::isHead(game->fleet.trainCars[k].flags) ? 400 : 55;
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
    for (int n3 = 0; n3 < game->world.terrain.mapWidth; ++n3) {
        for (int n2 = 0; n2 < game->world.terrain.mapHeight; ++n2) {
            if (game->world.trackMap[n3][n2] != 65) continue;
            int n = game->world.terrain.tileMap[n3][n2];
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
        if (!train_flag::isUsed(game->fleet.trainCars[n3].flags)) continue;
        int n2 = unsignedByte(game->fleet.trainCars[n3].x);
        int n = unsignedByte(game->fleet.trainCars[n3].y);
        if (game->world.trackMap[n2][n] != 65) continue;
        if (train_flag::isHead(game->fleet.trainCars[n3].flags)) {
            switch (game->fleet.trainCars[n3].era) {
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
        game->fleet.trainCars[n].previousRevenue = game->fleet.trainCars[n].revenue;
        game->fleet.trainCars[n].revenue = 0;
    }
    for (int n = 0; n < 48; ++n) {
        game->world.cities[n].previousStationRevenue = game->world.cities[n].stationRevenue;
        game->world.cities[n].stationRevenue = 0;
    }
    game->economy.calculate_period_profit();
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
        if (game->ui.annualReportEnabled) {
            game->ready = false;
            game->ui.transitionCode = 35;
            game_setState(game, GameState::MenuOverlay);
        }
    } else if (game->ui.monthlyReportEnabled) {
        game->ready = false;
        game->ui.transitionCode = 32;
        game_setState(game, GameState::MenuOverlay);
    }
}

void game_tickInputOrAi(Game *game, bool bl) {
    for (int i = 0; i < 250; ++i) {
        if ((game->fleet.serviceSites[i].flags & 0x80) != 128) continue;
        int n3 = unsignedByte(game->fleet.serviceSites[i].x);
        int n4 = unsignedByte(game->fleet.serviceSites[i].y);
        int n5 = n3 - 3;
        int n6 = n4 - 3;
        int n7 = n3 + 3;
        int n8 = n4 + 3;
        if (n5 < 0) n5 = 0;
        if (n6 < 0) n6 = 0;
        if (n7 >= game->world.terrain.mapWidth) n7 = game->world.terrain.mapWidth - 1;
        if (n8 >= game->world.terrain.mapHeight) n8 = game->world.terrain.mapHeight - 1;
        int n9 = 0;
        bool seenZones[64] = {false};
        bool seenIndustry1[64] = {false};
        bool seenIndustry2[64] = {false};
        bool seenIndustry3[64] = {false};
        for (int n2 = n5; n2 <= n7; ++n2) {
            for (int n = n6; n <= n8; ++n) {
                int tile = game->world.terrain.tileMap[n2][n];
                int n11 = game->world.trackMap[n2][n] & 0x3F;
                if (tile >= 34 && tile <= 40) {
                    if ((game->world.trackMap[n2][n] & 0xC0) == 128 && !bl
                        && !seenZones[n11]) {
                        seenZones[n11] = true;
                        if ((game->fleet.serviceSites[i].unloadFlags & 1) == 1) {
                            int add = game->world.zonePopulation[n11]
                                      * (game->world.constructionTimers[n11] + 10
                                         + game->world.zonePopulation[n11] / 10
                                         + game->rng.next_int() % 40) / 170;
                            game->fleet.serviceSites[i].resourceStock[0] =
                                (int16_t)(game->fleet.serviceSites[i].resourceStock[0] + add);
                        }
                        if ((game->fleet.serviceSites[i].unloadFlags & 0x10) == 0x10) {
                            int add = game->world.zonePopulation[n11] / 2
                                      * (game->world.constructionTimers[n11] + 10
                                         + game->world.zonePopulation[n11] / 10
                                         + std::abs(game->rng.next_int() % 40)) / 170;
                            game->fleet.serviceSites[i].resourceStock[4] =
                                (int16_t)(game->fleet.serviceSites[i].resourceStock[4] + add);
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
                    int n10 = game->world.cities[n11].population * 4 / 30 / game->world.cities[n11].zoneTileCount;
                    if (n10 == 0) n10 = 1;
                    game->fleet.serviceSites[i].resourceStock[1] =
                        (int16_t)(game->fleet.serviceSites[i].resourceStock[1] + n10);
                    game->world.cities[n11].stationRevenue = (int16_t)(game->world.cities[n11].stationRevenue + n10);
                    game->fleet.serviceSites[i].loadFlags = (int8_t)(game->fleet.serviceSites[i].loadFlags | 4);
                }
                if (tile >= 53 && tile <= 55 && !bl
                    && (game->world.trackMap[n2][n] & 0xC0) == 192
                    && !seenIndustry2[n11]) {
                    seenIndustry2[n11] = true;
                    int n10 = game->world.cities[n11].population * 4 / 30 / game->world.cities[n11].zoneTileCount;
                    if (n10 == 0) n10 = 1;
                    game->fleet.serviceSites[i].resourceStock[2] =
                        (int16_t)(game->fleet.serviceSites[i].resourceStock[2] + n10);
                    game->world.cities[n11].stationRevenue = (int16_t)(game->world.cities[n11].stationRevenue + n10);
                    game->fleet.serviceSites[i].loadFlags = (int8_t)(game->fleet.serviceSites[i].loadFlags | 8);
                }
                if (tile >= 59 && tile <= 61 && !bl
                    && (game->world.trackMap[n2][n] & 0xC0) == 192
                    && !seenIndustry3[n11]) {
                    seenIndustry3[n11] = true;
                    int n10 = game->world.cities[n11].population * 4 / 30 / game->world.cities[n11].zoneTileCount;
                    if (n10 == 0) n10 = 1;
                    game->fleet.serviceSites[i].resourceStock[3] =
                        (int16_t)(game->fleet.serviceSites[i].resourceStock[3] + n10);
                    game->world.cities[n11].stationRevenue = (int16_t)(game->world.cities[n11].stationRevenue + n10);
                    game->fleet.serviceSites[i].loadFlags = (int8_t)(game->fleet.serviceSites[i].loadFlags | 2);
                }
                if (tile >= 50 && tile <= 52)
                    game->fleet.serviceSites[i].unloadFlags = (int8_t)(game->fleet.serviceSites[i].unloadFlags | 4);
                if (tile >= 56 && tile <= 58)
                    game->fleet.serviceSites[i].unloadFlags = (int8_t)(game->fleet.serviceSites[i].unloadFlags | 8);
                if (tile >= 62 && tile <= 64)
                    game->fleet.serviceSites[i].unloadFlags = (int8_t)(game->fleet.serviceSites[i].unloadFlags | 2);
            }
        }
        if (n9 / 3 > 0) {
            game->fleet.serviceSites[i].unloadFlags = (int8_t)(game->fleet.serviceSites[i].unloadFlags | 1);
            game->fleet.serviceSites[i].loadFlags = (int8_t)(game->fleet.serviceSites[i].loadFlags | 1);
        }
        if (n9 / 7 > 0) {
            game->fleet.serviceSites[i].unloadFlags = (int8_t)(game->fleet.serviceSites[i].unloadFlags | 0x10);
            game->fleet.serviceSites[i].loadFlags = (int8_t)(game->fleet.serviceSites[i].loadFlags | 0x10);
        }
        bool large = (game->fleet.serviceSites[i].flags & 0x20) == 0x20;
        for (int n = 0; n < 5; ++n) {
            int limit;
            if (large) limit = n == 0 ? 250 : (n == 4 ? 325 : 450);
            else limit = n == 0 ? 100 : (n == 4 ? 150 : 200);
            if (game->fleet.serviceSites[i].resourceStock[n] > limit)
                game->fleet.serviceSites[i].resourceStock[n] = (int16_t)limit;
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
    if (++game->world.terrain.woodAgeTimer == 150) {
        for (int mapX = 0; mapX < game->world.terrain.mapWidth; ++mapX) {
            for (int mapY = 0; mapY < game->world.terrain.mapHeight; ++mapY) {
                if (game->world.terrain.tileMap[mapX][mapY] < 59 || game->world.terrain.tileMap[mapX][mapY] > 61
                    || game->rng.next_int() % 3 != 0)
                    continue;
                ++game->world.terrain.tileMap[mapX][mapY];
                if (game->world.terrain.tileMap[mapX][mapY] == 62) game->world.terrain.tileMap[mapX][mapY] = 59;
            }
        }
        game->world.terrain.woodAgeTimer = 0;
    }
    if (++game->world.terrain.phaseTimer == 10) game->world.terrain.phaseTimer = 0;
}

void applyQueuedTileChanges(Game *game) {
    for (int queueIndex = 0; queueIndex < kTileQueueCount; ++queueIndex) {
        TileChange& change = game->world.terrain.tileQueue[queueIndex];
        if (change.countdown <= 0) continue;
        --change.countdown;
        int tileX = unsignedByte(change.x);
        int tileY = unsignedByte(change.y);
        if (change.countdown == 0 && tileX >= 0 && tileY >= 0
            && tileX < game->world.terrain.mapWidth && tileY < game->world.terrain.mapHeight)
            game->world.terrain.tileMap[tileX][tileY] = (int8_t)(unsignedByte(game->world.terrain.tileMap[tileX][tileY]) - 128);
        if (change.countdown != 7 || tileX < 0 || tileY < 0
            || tileX >= game->world.terrain.mapWidth || tileY >= game->world.terrain.mapHeight)
            continue;
        int tileCode = unsignedByte(game->world.terrain.tileMap[tileX][tileY]) - 128;
        if (game->world.terrain.tileQueueVisualOnly[queueIndex]) continue;
        if (tileCode >= 29 && tileCode <= 33) game->world.terrain.tileMap[tileX][tileY] = -128;
        if ((tileCode >= 70 && tileCode <= 75) || (tileCode >= 87 && tileCode <= 90)) {
            game->world.trackMap[tileX][tileY] = 0;
            game->world.terrain.tileMap[tileX][tileY] = -128;
            game->renderer.lastViewportCenterX = -1;
        }
        if (tileCode >= 77 && tileCode <= 80) {
            game->world.terrain.tileMap[tileX][tileY] = (int8_t)(128 + tileCode - 56);
            game->world.trackMap[tileX][tileY] = 0;
            game->renderer.lastViewportCenterX = -1;
        }
        if (tileCode == 81 || tileCode == 82) {
            game->world.terrain.tileMap[tileX][tileY] = -128;
            game->world.trackMap[tileX][tileY] = 0;
            game->renderer.lastViewportCenterX = -1;
        }
        if (tileCode >= 83 && tileCode <= 86) {
            game->world.terrain.tileMap[tileX][tileY] = (int8_t)(128 + tileCode - 76);
            game->world.trackMap[tileX][tileY] = 0;
            game->renderer.lastViewportCenterX = -1;
        }
        if (tileCode == 76) {
            int firstTrackOwner = game->world.trackMap[tileX][tileY] & 7;
            int secondTrackOwner = (game->world.trackMap[tileX][tileY] & 0x3F) >> 3;
            if (firstTrackOwner == 1 && secondTrackOwner == 1) {
                game->world.terrain.tileMap[tileX][tileY] = -128;
                game->world.trackMap[tileX][tileY] = 0;
            } else if (firstTrackOwner == 1) {
                game->world.terrain.tileMap[tileX][tileY] = -57;
                game->world.trackMap[tileX][tileY] = static_cast<int8_t>(
                    track::PRESENT | secondTrackOwner);
            } else if (secondTrackOwner == 1) {
                game->world.terrain.tileMap[tileX][tileY] = -58;
                game->world.trackMap[tileX][tileY] = static_cast<int8_t>(
                    track::PRESENT | firstTrackOwner);
            }
            game->renderer.lastViewportCenterX = -1;
        }
    }
}

void updateActiveTrains(Game *game) {
    for (int trainIndex = 0; trainIndex < kTrainSlotCount; ++trainIndex) {
        if (!train_flag::isUsed(game->fleet.trainCars[trainIndex].flags)
            || !train_flag::isHead(game->fleet.trainCars[trainIndex].flags))
            continue;
        if (train_flag::isRemovalPending(game->fleet.trainCars[trainIndex].flags)) {
            if (game->fleet.trainCars[trainIndex].energyCargoOrRemovalTicks > 0) {
                --game->fleet.trainCars[trainIndex].energyCargoOrRemovalTicks;
                continue;
            }
            game->fleet.trainCars[trainIndex].phase = 0;
            game->fleet.trainCars[trainIndex].flags = 0;
            continue;
        }
        game_updateTrain(game, trainIndex);
        game_updateTrain(game, trainIndex);
        if (game->fleet.trainCars[trainIndex].phase == 0) game_mergeTrainAt(game, trainIndex);
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
    game->world.decay_construction_timers_quarterly(game->economy.monthOfYear);
    game->world.update_city_population(game->rng);
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
        game->fleet.age_one_year();
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
    game->economy.compute_year_end_metrics();
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
            if (forestFound) game->world.recount_zone_population();
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
