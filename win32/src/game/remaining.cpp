// Score, graph, text, and life-cycle functions.
#include "game_methods.h"

#include <stdio.h>
#include <string.h>

int8_t *gc_formatNumberWithSuffix(GameCanvas *gc, int32_t value,
                                  int32_t suffix) {
    (void)gc;
    char text[24];
    snprintf(text, sizeof(text), "%d", value);
    int length = (int)strlen(text);
    int8_t *result = GC_NEW(int8_t, length + 1);
    for (int i = 0; i < length; ++i)
        result[i] = text[i] == '-' ? 22 : (int8_t)(text[i] - 47);
    result[length] = (int8_t)suffix;
    return result;
}

int8_t *gc_encodeDisplayText(GameCanvas *gc, char *text) {
    (void)gc;
    int length = (int)strlen(text);
    int8_t *result = GC_NEW(int8_t, length);
    for (int i = 0; i < length; ++i) {
        char ch = text[i];
        result[i] = ch == '-' ? 22 : ch == '.' ? 11 : ch == '/' ? 20
                    : ch == '%' ? 29 : (int8_t)(ch - 47);
    }
    return result;
}

void gc_setYearGraphMode(GameCanvas *gc, int32_t mode, bool alternate) {
    gc->yearGraphMode = mode;
    gc->yearGraphAlt = alternate;
    int minimum = 0;
    int maximum = 0;
    for (int index = 0; index < 12; ++index) {
        int value;
        if (!alternate) {
            gc->yearGraphOffsets[index] = (int8_t)(gc->year - 1940);
            gc->altYearArray[index] = (int8_t)(gc->monthOfYear - index);
            if (gc->altYearArray[index] < 1) {
                gc->altYearArray[index] = (int8_t)(12 + gc->altYearArray[index]);
                --gc->yearGraphOffsets[index];
            }
            value = mode == 0 ? gc->yearProfitHistory[index]
                    : mode == 1 ? gc->yearRevenueHistory[index]
                    : gc->yearCompanyValueHistory[index];
        } else {
            gc->altYearArray[index] = (int8_t)(gc->year - index - 1940);
            value = mode == 0 ? gc->decadeProfitHistory[index]
                    : mode == 1 ? gc->altDecadeRevenueTotal[index]
                    : gc->altDecadeTotalCost[index];
        }
        if (value > maximum) maximum = value;
        if (value < minimum) minimum = value;
    }
    int scale = maximum > -minimum ? maximum : -minimum;
    for (int index = 0; index < 12; ++index) {
        if (!alternate && mode == 0) {
            gc->yearGraphValues[index] =
                gc->yearProfitHistory[index] == 0 ? 0
                : (int8_t)(30 * gc->yearProfitHistory[index] / scale);
        } else if (!alternate && mode == 1) {
            gc->yearGraphValues[index] =
                gc->yearRevenueHistory[index] == 0 ? 0
                : (int8_t)(60 * gc->yearRevenueHistory[index] / scale);
        } else if (!alternate && mode == 2) {
            // The original tests profit for zero before scaling company value.
            gc->yearGraphValues[index] =
                gc->yearProfitHistory[index] == 0 ? 0
                : (int8_t)(30 * gc->yearCompanyValueHistory[index] / scale);
        } else if (alternate && mode == 0) {
            gc->yearGraphValues[index] =
                gc->decadeProfitHistory[index] == 0 ? 0
                : (int8_t)(30 * gc->decadeProfitHistory[index] / scale);
        } else if (alternate && mode == 1) {
            gc->yearGraphValues[index] =
                gc->altDecadeRevenueTotal[index] == 0 ? 0
                : (int8_t)(60 * gc->altDecadeRevenueTotal[index] / scale);
        } else if (alternate && mode == 2) {
            // The original tests the other-total array before scaling cost.
            gc->yearGraphValues[index] =
                gc->decadeProfitHistory[index] == 0 ? 0
                : (int8_t)(30 * gc->altDecadeTotalCost[index] / scale);
        }
    }
}

int32_t gc_computeMilestoneScore(GameCanvas *gc) {
    int target = gc->currentMilestoneScore;
    int current = 0;
    if (gc->scenarioMilestoneIndex >= 6 && gc->scenarioMilestoneIndex <= 11)
        target += gc->milestoneCoThreshold2;
    switch (gc->scenarioMilestoneIndex) {
        case 1: current = gc->milestoneThreshold1; break;
        case 2: current = gc->milestoneThreshold2; break;
        case 3: current = gc->milestoneThreshold3; break;
        case 4: current = gc->milestoneThreshold4; break;
        case 5: current = gc->milestoneThreshold5; break;
        case 6:
            current = gc->milestoneThreshold1 + gc->milestoneThreshold2;
            break;
        case 7:
            current = gc->milestoneThreshold1 + gc->milestoneThreshold3;
            break;
        case 8:
            current = gc->milestoneThreshold1 + gc->milestoneThreshold4;
            break;
        case 9:
            current = gc->milestoneThreshold2 + gc->milestoneThreshold3;
            break;
        case 10:
            current = gc->milestoneThreshold2 + gc->milestoneThreshold4;
            break;
        case 11:
            current = gc->milestoneThreshold4 + gc->milestoneThreshold5;
            break;
        case 12: current = gc->lifetimeTrainRevenue; break;
        case 13:
            current = gc->loanCeiling - gc->loanBalance;
            target = gc->loanCeiling;
            break;
        case 14:
            current = gc->year - 1950;
            target = gc->currentMilestoneScore - 1950;
            break;
        case 15: current = gc->aiWagonsBought; break;
    }
    int score = 100 * current / target;
    if (score < 0) score = 0;
    if (score > 100) score = 100;
    return score;
}

int8_t gc_computeMenuRowDisplayValue(GameCanvas *gc, int32_t row) {
    int value;
    if (gc->menuRowType[row] == 1) {
        value = 100 * gc->menuRowValue[row] / gc->menuRowCost[row];
    } else if (gc->menuRowType[row] == 2) {
        value = gc->money > 0
                ? 100 * gc->money / gc->menuRowCost[row]
                : 100 * gc->menuRowCost[row] / gc->money;
    } else if (gc->menuRowType[row] >= 4 && gc->menuRowType[row] <= 14) {
        value = 100 * gc->menuRowValue[row] / gc->menuRowCost[row];
    } else {
        return 0;
    }
    if (value > 100) value = 100;
    if (value < 0) value = 0;
    return (int8_t)value;
}

void gc_loadMusic(GameCanvas *gc) {
    gc->musicAvailable = midp_media_load_midi("music.mid");
    gc->musicPlayer = gc->musicAvailable ? gc : NULL;
    gc->volumeControl = gc->musicAvailable ? gc : NULL;
    if (gc->musicAvailable)
        midp_media_set_volume(20 * gc->gameStateCounter);
    gc->musicPlaying = false;
}

void gc_playMusic(GameCanvas *gc) {
    if (gc->musicAvailable && gc->musicPlayer && !gc->musicPlaying &&
        gc->autoMusic && midp_media_play_midi())
        gc->musicPlaying = true;
}

void gc_setMusicVolume(GameCanvas *gc, int32_t level) {
    if (gc->volumeControl) midp_media_set_volume(level * 20);
}

void gc_stopMusic(GameCanvas *gc) {
    if (gc->musicAvailable && gc->musicPlayer && gc->musicPlaying)
        midp_media_stop();
    gc->musicPlaying = false;
}

// These helpers are used only by the excluded high-score network request.
char *gc_decodeString(GameCanvas *gc, int8_t *bytes) {
    (void)gc; (void)bytes;
    return NULL;
}

char *gc_buildPlayerNameString(GameCanvas *gc) {
    (void)gc;
    return NULL;
}

bool gc_syncHighScores(GameCanvas *gc, bool upload) {
    (void)gc; (void)upload;
    return false;
}

void gc_hideNotify(GameCanvas *gc) {
    gc->paused = true;
    if (gc->gameState > 10 && gc->gameState != 120) gc_stopMusic(gc);
    if (gc->gameState == 30) {
        gc->ready = false;
        gc->transitionCode = 1;
        gc_setState(gc, 20);
    }
}

void gc_showNotify(GameCanvas *gc) {
    if (gc->gameState > 10 && gc->gameState != 120) gc_playMusic(gc);
    gc->paused = false;
}
