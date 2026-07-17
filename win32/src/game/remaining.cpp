// Score, graph, text, and life-cycle functions.
#include "game_internal.h"

#include <stdio.h>
#include <string.h>

std::vector<int8_t> game_formatNumberWithSuffix(int32_t value, int32_t suffix) {
    char text[24];
    snprintf(text, sizeof(text), "%d", value);
    int length = (int)strlen(text);
    std::vector<int8_t> result(static_cast<size_t>(length + 1));
    for (int i = 0; i < length; ++i)
        result[i] = text[i] == '-' ? 22 : (int8_t)(text[i] - 47);
    result[length] = (int8_t)suffix;
    return result;
}

std::vector<int8_t> game_encodeDisplayText(const char *text) {
    int length = (int)strlen(text);
    std::vector<int8_t> result(static_cast<size_t>(length));
    for (int i = 0; i < length; ++i) {
        char ch = text[i];
        result[i] = ch == '-' ? 22 : ch == '.' ? 11 : ch == '/' ? 20
                    : ch == '%' ? 29 : (int8_t)(ch - 47);
    }
    return result;
}

void game_setYearGraphMode(Game *game, int32_t mode, bool alternate) {
    game->ui.yearGraphMode = mode;
    game->ui.yearGraphAlt = alternate;
    int minimum = 0;
    int maximum = 0;
    for (int index = 0; index < 12; ++index) {
        int value;
        if (!alternate) {
            game->ui.yearGraphOffsets[index] = (int8_t)(game->economy.year - 1940);
            game->ui.altYearArray[index] = (int8_t)(game->economy.monthOfYear - index);
            if (game->ui.altYearArray[index] < 1) {
                game->ui.altYearArray[index] = (int8_t)(12 + game->ui.altYearArray[index]);
                --game->ui.yearGraphOffsets[index];
            }
            value = mode == 0 ? game->economy.yearProfitHistory[index]
                    : mode == 1 ? game->economy.yearRevenueHistory[index]
                    : game->economy.yearCompanyValueHistory[index];
        } else {
            game->ui.altYearArray[index] = (int8_t)(game->economy.year - index - 1940);
            value = mode == 0 ? game->economy.decadeProfitHistory[index]
                    : mode == 1 ? game->economy.altDecadeRevenueTotal[index]
                    : game->economy.altDecadeTotalCost[index];
        }
        if (value > maximum) maximum = value;
        if (value < minimum) minimum = value;
    }
    int scale = maximum > -minimum ? maximum : -minimum;
    for (int index = 0; index < 12; ++index) {
        if (!alternate && mode == 0) {
            game->ui.yearGraphValues[index] =
                game->economy.yearProfitHistory[index] == 0 ? 0
                : (int8_t)(30 * game->economy.yearProfitHistory[index] / scale);
        } else if (!alternate && mode == 1) {
            game->ui.yearGraphValues[index] =
                game->economy.yearRevenueHistory[index] == 0 ? 0
                : (int8_t)(60 * game->economy.yearRevenueHistory[index] / scale);
        } else if (!alternate && mode == 2) {
            // The original tests profit for zero before scaling company value.
            game->ui.yearGraphValues[index] =
                game->economy.yearProfitHistory[index] == 0 ? 0
                : (int8_t)(30 * game->economy.yearCompanyValueHistory[index] / scale);
        } else if (alternate && mode == 0) {
            game->ui.yearGraphValues[index] =
                game->economy.decadeProfitHistory[index] == 0 ? 0
                : (int8_t)(30 * game->economy.decadeProfitHistory[index] / scale);
        } else if (alternate && mode == 1) {
            game->ui.yearGraphValues[index] =
                game->economy.altDecadeRevenueTotal[index] == 0 ? 0
                : (int8_t)(60 * game->economy.altDecadeRevenueTotal[index] / scale);
        } else if (alternate && mode == 2) {
            // The original tests the other-total array before scaling cost.
            game->ui.yearGraphValues[index] =
                game->economy.decadeProfitHistory[index] == 0 ? 0
                : (int8_t)(30 * game->economy.altDecadeTotalCost[index] / scale);
        }
    }
}

int32_t game_computeMilestoneScore(Game *game) {
    int target = game->scenario.currentMilestoneScore;
    int current = 0;
    if (game->scenario.scenarioMilestoneIndex >= 6 && game->scenario.scenarioMilestoneIndex <= 11)
        target += game->scenario.milestoneCoThreshold2;
    switch (game->scenario.scenarioMilestoneIndex) {
        case 1: current = game->scenario.milestoneThreshold1; break;
        case 2: current = game->scenario.milestoneThreshold2; break;
        case 3: current = game->scenario.milestoneThreshold3; break;
        case 4: current = game->scenario.milestoneThreshold4; break;
        case 5: current = game->scenario.milestoneThreshold5; break;
        case 6:
            current = game->scenario.milestoneThreshold1 + game->scenario.milestoneThreshold2;
            break;
        case 7:
            current = game->scenario.milestoneThreshold1 + game->scenario.milestoneThreshold3;
            break;
        case 8:
            current = game->scenario.milestoneThreshold1 + game->scenario.milestoneThreshold4;
            break;
        case 9:
            current = game->scenario.milestoneThreshold2 + game->scenario.milestoneThreshold3;
            break;
        case 10:
            current = game->scenario.milestoneThreshold2 + game->scenario.milestoneThreshold4;
            break;
        case 11:
            current = game->scenario.milestoneThreshold4 + game->scenario.milestoneThreshold5;
            break;
        case 12: current = game->economy.lifetimeTrainRevenue; break;
        case 13:
            current = game->economy.loanCeiling - game->economy.loanBalance;
            target = game->economy.loanCeiling;
            break;
        case 14:
            current = game->economy.year - 1950;
            target = game->scenario.currentMilestoneScore - 1950;
            break;
        case 15: current = game->scenario.aiWagonsBought; break;
    }
    int score = 100 * current / target;
    if (score < 0) score = 0;
    if (score > 100) score = 100;
    return score;
}

int8_t OfferBoard::display_progress(int32_t row, const Economy& economy) const {
    int value;
    if (offers[row].objectiveType == 1) {
        value = 100 * offers[row].progress / offers[row].target;
    } else if (offers[row].objectiveType == 2) {
        if (economy.money == 0) {
            value = 0;
        } else {
            value = economy.money > 0
                    ? 100 * economy.money / offers[row].target
                    : 100 * offers[row].target / economy.money;
        }
    } else if (offers[row].objectiveType >= 4 && offers[row].objectiveType <= 14) {
        value = 100 * offers[row].progress / offers[row].target;
    } else {
        return 0;
    }
    if (value > 100) value = 100;
    if (value < 0) value = 0;
    return (int8_t)value;
}

void MusicController::load(const std::filesystem::path& path) {
    available = output && output->load(path);
    if (available) {
        output->set_volume(20 * volumeLevel);
    }
    playing = false;
}

void MusicController::play() {
    if (available && !playing && enabled && output->play()) {
        playing = true;
    }
}

void MusicController::set_volume(int32_t level) {
    if (available) {
        output->set_volume(level * 20);
    }
}

void MusicController::stop() {
    if (available && playing) {
        output->stop();
    }
    playing = false;
}

void MusicController::update() {
    if (output) output->update();
}

void Game::update_audio() {
    music.update();
}

void Game::load_music() {
    music.load(resources::path_for("music.mid"));
}

void Game::play_music() {
    music.play();
}

void Game::set_music_volume(int32_t level) {
    music.set_volume(level);
}

void Game::stop_music() {
    music.stop();
}

bool game_syncHighScores(Game *game, bool upload) {
    (void)game; (void)upload;
    return false;
}
