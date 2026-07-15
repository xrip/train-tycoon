// port/impl/paint.c - paint() (full) + update() (boot states; PARTIAL).
// paint is fully ported. update currently covers the loading/boot states
// (0,1,2,4,10); the gameplay states (25 map-gen, 30 sim, menus, year-end...)
// remain to be ported -- tracked as `partial` in the manifest.
#include "game_methods.h"

#define RGB3(r, g, b) (((r) << 16) | ((g) << 8) | (b))
#define WIDTH  MIDP_SCREEN_W   // this.getWidth()
#define HEIGHT MIDP_SCREEN_H   // this.getHeight()

// public void paint(Graphics graphics)
void gc_paint(GameCanvas *gc, MidpGraphics *g) {
    // setFullScreenMode(true) is a no-op on this backend.
    int n = WIDTH, n2 = HEIGHT;
    if (gc->ce) return;
    gc_setCanvasSize(gc, n, n2);
    if (gc->gameState != 40 && gc->gameState != 35 && gc->gameState != 45
        && gc->gameState != 55 && gc->gameState != 65 && gc->gameState != 105
        && gc->gameState != 120 && (gc->gameState != 20 || gc->ready)) {
        midp_set_color(g, 0);
        midp_fill_rect(g, 0, 0, n, n2);
    }
    midp_set_clip(g, 0, 0, gcs_screenWidth, gcs_hs);
    switch (gc->gameState) {
        case 0: case 1: case 2: case 3: {
            midp_set_color(g, 0xFFFFFF);
            if (gc->gameState == 0) midp_set_color(g, 0);
            midp_fill_rect(g, 0, 0, gcs_screenWidth, gcs_hs);
            int n3 = gcs_screenWidth / 2;
            int n4 = gcs_hs / 2;
            int n5 = midp_image_get_width(gc->cycleFlag) / 2;
            int n6 = midp_image_get_height(gc->cycleFlag);
            int n7 = n6 / 10 + 1;
            for (int i = 0; i < 10; ++i) {
                int n8 = (n3 + n5 + 30) * (gc->loadingTimer - 5 * (i / 2)) / 10;
                if (n8 < 0) n8 = 0;
                if (n8 > n3 + n5 + 30) n8 = n3 + n5 + 30;
                int n9 = n3 - (n3 + n5 + 30 - n8);
                if (i % 2 == 1) n9 = n3 + (n3 + n5 + 30 - n8);
                midp_set_clip(g, n9 - n5, n4 - n6 / 2 + i * n7, n5 * 2, n7);
                midp_draw_image(g, gc->cycleFlag, n9, n4, 3);
            }
            midp_set_clip(g, 0, 0, gcs_screenWidth, gcs_hs);
            break;
        }
        case 10: {
            midp_set_color(g, 0xFFFFFF);
            midp_fill_rect(g, 0, 0, gcs_screenWidth, gcs_hs);
            midp_draw_image(g, gc->cycleFlag, gcs_screenWidth / 2, gcs_hs / 2, 3);
            midp_set_color(g, RGB3(215, 222, 238));
            midp_fill_rect(g, 10, gcs_hs - 13, gcs_screenWidth - 21, 9);
            int n10 = gc->spriteLoadCounter * (gcs_screenWidth - 20) / (gc->scenarioResourceCount - 1);
            midp_set_color(g, RGB3(60, 105, 144));
            midp_fill_rect(g, 10, gcs_hs - 13, n10, 9);
            midp_set_color(g, RGB3(49, 64, 90));
            midp_draw_rect(g, 10, gcs_hs - 13, gcs_screenWidth - 21, 9);
            break;
        }
        case 20: {
            if (gc->ready) {
                midp_set_color(g, 0xFFFFFF);
                midp_fill_rect(g, 0, 0, gcs_screenWidth, gcs_hs);
                if (gc->cycleFlag != NULL)
                    midp_draw_image(g, gc->cycleFlag, gcs_screenWidth / 2, gcs_hs / 2, 3);
            }
            if (gc->transitionCode == 36) {
                gc->gameState = 40;
                gc_paint(gc, g);
                gc->gameState = 20;
            }
            gc_renderPackedSprite(gc, g, gc->transitionCode);
            break;
        }
        case 25: {
            midp_set_color(g, 0);
            midp_fill_rect(g, 0, 0, gcs_screenWidth, gcs_hs);
            gc_drawTextBox(gc, g, gcs_screenWidth / 2, gcs_hs / 2, 0, 3,
                           gc->spriteBitmapTable[gc->spriteIndexTable[0]]);
            midp_set_color(g, RGB3(215, 222, 238));
            midp_fill_rect(g, 10, gcs_hs - 23, gcs_screenWidth - 21, 9);
            int n11 = gc->totalTilesToGenerate < 2 ? 2 : gc->totalTilesToGenerate;
            int n12 = gc->mapGenTilesDone * (gcs_screenWidth - 20) / (n11 - 1);
            midp_set_color(g, RGB3(60, 105, 144));
            midp_fill_rect(g, 10, gcs_hs - 23, n12, 9);
            midp_set_color(g, 0xFFFFFF);
            midp_draw_rect(g, 10, gcs_hs - 23, gcs_screenWidth - 21, 9);
            break;
        }
        case 30: {
            gc->tickStart = midp_current_time_millis();
            gc_renderMap(gc, g);
            gc->tickEnd = midp_current_time_millis();
            if (gc->inputPending) {
                int n13;
                midp_set_color(g, 0);
                midp_set_clip(g, 0, 15, gcs_screenWidth, gcs_fX - 15);
                for (n13 = 0; n13 < gcs_screenWidth; ++n13)
                    midp_draw_line(g, n13 * 2, 0, gcs_hs + n13 * 2, gcs_hs);
                for (n13 = 0; n13 < gcs_hs; ++n13)
                    midp_draw_line(g, 0, n13 * 2, gcs_screenWidth, gcs_screenWidth + n13 * 2);
                midp_set_clip(g, 0, 0, gcs_screenWidth, gcs_hs);
                gc_drawSpriteQuad(gc, g, 234, 0, 15, 20);
            }
            gc_drawMoneyTicker(gc, g);
            if (gc->pauseFlag) gc_drawSpriteQuad(gc, g, 236, 0, 15, 0);
            if (gc->pendingTransitionCode > -1) {
                gc->transitionCode = gc->pendingTransitionCode;
                gc_o(gc);
                gc->pendingTransitionCode = -1;
                gc_renderPackedSprite(gc, g, gc->transitionCode);
                gc_setState(gc, 20);
            }
            if (gc->transitionDebounce <= 0) break;
            gc->transitionDebounce = (int8_t)(gc->transitionDebounce - 1);
            if (gc->transitionDebounce != 0) break;
            gc->transitionCode = 80 + gc->ff;
            gc_o(gc);
            gc->ready = false;
            gc_renderPackedSprite(gc, g, gc->transitionCode);
            gc_setState(gc, 20);
            break;
        }
        case 35:
            gc_drawTextBox(gc, g, gcs_screenWidth / 2, gcs_hs / 2, 0, 3,
                           gc->spriteBitmapTable[gc->spriteIndexTable[50]]);
            break;
        case 40: {
            int n14;
            midp_set_clip(g, 0, 15, gcs_screenWidth, gcs_eN);
            midp_set_color(g, 0);
            midp_fill_rect(g, 0, 15, gcs_screenWidth, gcs_eN);
            int n15 = gc->viewportCenterX / 16;
            int n16 = gc->viewportCenterY / 8;
            int n17 = gc->mapWidth + gc->mapHeight;
            int n18 = gcs_screenWidth / 2 - n15 - gcs_screenWidth / 32;
            if (gcs_screenWidth - n18 > n17) n18 = gcs_screenWidth - n17;
            if (n18 > 0) n18 = 0;
            if (gcs_eN - (n14 = gcs_eN / 2 - n16 - gcs_eN / 16) > n17) n14 = gcs_eN - n17;
            if (n14 > 0) n14 = 0;
            int n19 = n17 < gcs_screenWidth ? (gcs_screenWidth - n17) / 2 : 0;
            int n20 = n17 < gcs_eN ? (gcs_eN - n17) / 2 : 0;
            midp_draw_image(g, gc->backbuffer, n18 + n19, 15 + n14 + n20, 20);
            int n21 = gc->viewportCenterX - 2 * gc->viewportCenterY + 16 + 2 * gc->mapHalfWidth;
            int n22 = gc->viewportCenterX + 2 * gc->viewportCenterY - 16 - 2 * gc->mapHalfWidth;
            int n23 = gc->mapWidth - (n21 /= 32) + (n22 /= 32);
            int n24 = n21 + n22;
            midp_set_color(g, 0xFFFFFF);
            midp_draw_rect(g, n24 + n18 + n19, 15 + n23 + n14 + n20, gcs_screenWidth / 16, gcs_eN / 8);
            midp_set_clip(g, 0, 0, gcs_screenWidth, gcs_hs);
            gc_drawConfirmBar(gc, g);
            gc_drawConfirmDialog(gc, g);
            if (gc->ee) {
                gc->ready = false;
                gc->transitionCode = 36;
                gc_setState(gc, 20);
                gc->ee = false;
            }
            if (gc->pendingTransitionCode <= -1) break;
            gc->transitionCode = gc->pendingTransitionCode;
            gc_o(gc);
            gc->pendingTransitionCode = -1;
            gc_renderPackedSprite(gc, g, gc->transitionCode);
            gc_setState(gc, 20);
            break;
        }
        case 45:
            gc_drawTextBox(gc, g, gcs_screenWidth / 2, gcs_hs / 2, 0, 3,
                           gc->spriteBitmapTable[gc->spriteIndexTable[51]]);
            break;
        case 55:
            gc_drawTextBox(gc, g, gcs_screenWidth / 2, gcs_hs / 2, 0, 3,
                           gc->spriteBitmapTable[gc->spriteIndexTable[52]]);
            break;
        case 65:
            gc_drawTextBox(gc, g, gcs_screenWidth / 2, gcs_hs / 2, 0, 3,
                           gc->spriteBitmapTable[gc->spriteIndexTable[53]]);
            break;
        case 105:
            gc_drawTextBox(gc, g, gcs_screenWidth / 2, gcs_hs / 2, 0, 3,
                           gc->spriteBitmapTable[gc->spriteIndexTable[49]]);
            break;
        case 120:
            gc_drawTextBox(gc, g, gcs_screenWidth / 2, gcs_hs / 2, 0, 3,
                           gc->spriteBitmapTable[gc->spriteIndexTable[66]]);
            break;
    }
}

// public void update() -- PARTIAL: loading/boot states only.
void gc_update(GameCanvas *gc) {
    switch (gc->gameState) {
        case 0: case 1: case 2: {
            if (midp_current_time_millis() - gc->loadingStartTime > 5000L)
                gc->loadingTimeout = true;
            ++gc->loadingTimer;
            if (!gc->loadingTimeout) break;
            if (gc->gameState == 1) {
                for (int i = 0; i < 6; ++i) gc->ca[i] = gc_decodePngFromSpriteStream(gc);
                gc_setState(gc, 4);
                break;
            }
            gc_setState(gc, gc->gameState + 1);
            break;
        }
        case 4:
            gc->ready = true;
            gc->screenParam = 3;
            gc->transitionCode = 14;
            gc_o(gc);
            gc_setState(gc, 20);
            break;
        case 10:
            gc->tileSprites[gc->spriteLoadCounter] = gc_decodePngFromSpriteStream(gc);
            ++gc->spriteLoadCounter;
            if (gc->spriteLoadCounter == gc->scenarioResourceCount / 2) {
                gc_loadMusic(gc);
                if (gc->musicAutoStart) { gc_playMusic(gc); gc->autoMusic = true; }
                else gc->autoMusic = false;
            }
            if (gc->spriteLoadCounter != gc->scenarioResourceCount) break;
            gc->ready = true;
            gc->transitionCode = 1;
            gc_setState(gc, 20);
            break;
        case 25: {
            ++gc->loadStep;
            if (gc->loadStep == 2) {
                gc_initSaveOrLoad(gc, 2);
                gc->dayOfMonth = 1; gc->monthOfYear = 1; gc->year = 1950;
                gc->milestonesEnabled = false; gc->pauseFlag = false; gc->aiEnabled = false;
                gc->moneyBaseA = 974; gc->moneyBaseB = 974;
                gc->baseTrainSpeed = 173; gc->accel1 = 342; gc->brake1 = 311;
                gc->baseTrainSpeed2 = 173; gc->accel2 = 234;
                gc->costPower = 4759; gc->costStruct = 9845; gc->costOverhead = 347;
                gc->costMaintenance = 5982; gc->costCrew = 481;
                gc->gWagonCost = 37894; gc->gWagonCost2 = 43247; gc->gWagonCost3 = 51872;
                gc->gWagonCost4 = 60149; gc->gWagonCost5 = 71423; gc->gWagonCost6 = 82675;
                gc->costGroupC1 = 4578; gc->costGroupC2 = 5196; gc->costGroupC3 = 4314; gc->costGroupC4 = 5072;
                MidpResource *in = midp_resource_open("/5");
                if (in) {
                    int cnt = midp_resource_read_byte(in);
                    for (int i = 0; i < cnt * 36; ++i) {
                        int a2 = midp_resource_read_byte(in);
                        int a3 = midp_resource_read_byte(in);
                        int a4 = midp_resource_read_byte(in);
                        int a5 = midp_resource_read_byte(in);
                        int a6 = midp_resource_read_byte(in) + 256 * midp_resource_read_byte(in);
                        int a7 = midp_resource_read_byte(in);
                        int a8 = midp_resource_read_byte(in);
                        int a9 = midp_resource_read_byte(in);
                        int a10 = midp_resource_read_byte(in);
                        int a11 = midp_resource_read_byte(in);
                        int a12 = midp_resource_read_byte(in);
                        int a13 = midp_resource_read_byte(in);
                        int a14 = midp_resource_read_byte(in);
                        int a15 = midp_resource_read_byte(in);
                        int a16 = midp_resource_read_byte(in) + 256 * midp_resource_read_byte(in) + 65536 * midp_resource_read_byte(in);
                        int a17 = midp_resource_read_byte(in) + 256 * midp_resource_read_byte(in) + 65536 * midp_resource_read_byte(in);
                        int a18 = midp_resource_read_byte(in) + 256 * midp_resource_read_byte(in) + 65536 * midp_resource_read_byte(in);
                        int a19 = midp_resource_read_byte(in);
                        int a20 = midp_resource_read_byte(in);
                        int a21 = midp_resource_read_byte(in);
                        int a22 = midp_resource_read_byte(in);
                        int a23 = midp_resource_read_byte(in) + 256 * midp_resource_read_byte(in) + 65536 * midp_resource_read_byte(in);
                        int a24 = midp_resource_read_byte(in) + 256 * midp_resource_read_byte(in) + 65536 * midp_resource_read_byte(in);
                        int a25 = midp_resource_read_byte(in);
                        int a26 = midp_resource_read_byte(in);
                        if (a2 != gc->activeScenario) continue;
                        gc->scenarioStep = a2;
                        gc->mapWidth = gc->mapHeight = a3;
                        gc->mapHalfWidth = (gc->mapWidth - 1) * 8;
                        gc->mapScreenWidth = (gc->mapWidth + gc->mapHeight) * 16;
                        gc->mapScreenHeight = (gc->mapWidth + gc->mapHeight) * 8;
                        gc->viewportHalfWidth = gc->mapScreenWidth / 2;
                        gc->viewportHalfHeight = gc->mapScreenHeight / 2;
                        gc->viewportCenterX = gc->viewportHalfWidth;
                        gc->viewportCenterY = gc->viewportHalfHeight;
                        gc->viewportBaseX = gc->viewportCenterX;
                        gc->viewportBaseY = gc->viewportCenterY;
                        gc->mapScaleX = gc->viewportBaseX * 2;
                        gc->mapScaleY = gc->viewportBaseY * 2;
                        gc->dQ = a6; gc->hf = a4; gc->archiveOrRecallMethod = a5; gc->cK = a7;
                        gc->mapGenColumns = a8; gc->mapGenRows = a9;
                        gc->terrainFeatureStage7Count = a10; gc->terrainFeatureStage6Count = a11;
                        gc->weightAccel = a12; gc->weightAccel2 = a13; gc->weightAccel3 = a14; gc->weightAccel4 = a15;
                        gc->money = a16; gc->loanCeiling = a17; gc->loanBalance = a18;
                        gc->interestRate = (int8_t)a19; gc->era = a20 - 1;
                        gc->scenarioMilestoneIndex = (int8_t)a22;
                        gc->currentMilestoneScore = a23; gc->milestoneCoThreshold2 = a24;
                        gc_initBuildMenuForScenario(gc, a25);
                        gc->flagsA = (a26 & 2) == 2;
                        gc->flagsB = (a26 & 4) == 4;
                        if ((a26 & 1) != 1) { gc->aiFrequency = 25; gc->aiThreshold = 100; }
                        else { gc->aiFrequency = 32; gc->aiThreshold = 29; }
                        gc->scenarioFlagAo = a21;
                        if (a21 != 0) { gc->transitionDebounce = 5; gc->ff = (int8_t)a21; continue; }
                        gc->transitionDebounce = 0;
                    }
                    midp_resource_close(in);
                }
                if (gc->activeScenario <= 1) {
                    if (gc->difficultySlot == 0) { gc->aiFrequency = 32; gc->aiThreshold = 29; }
                    if (gc->difficultySlot == 1) { gc->aiFrequency = 25; gc->aiThreshold = 22; }
                    if (gc->difficultyA == 0) { gc->interestRate = 4; gc->enterpriseTaxPct = 2; }
                    if (gc->difficultyA == 1) { gc->interestRate = 8; gc->enterpriseTaxPct = 5; }
                    if (gc->difficultyB == 0) { gc->mapGenColumns /= 2; gc->mapGenRows /= 2; }
                    if (gc->difficultyC == 0) {
                        gc->terrainFeatureStage7Count /= 2; gc->terrainFeatureStage6Count /= 2;
                        gc->weightAccel /= 2; gc->weightAccel2 /= 2; gc->weightAccel3 /= 2; gc->weightAccel4 /= 2;
                    }
                }
            }
            if (gc->loadStep == 3) {
                gc->buildMenuIndex = 0;
                gc->buildMenuMode = 0;
                gc_loadState25Setup(gc, 0);
            }
            if (gc->loadStep <= 3) break;
            gc_loadState25Setup(gc, 1);
            if (gc->mapGenTilesDone != gc->totalTilesToGenerate) break;
            gc->mapGenTilesDone = 0;
            gc_setState(gc, 30);
            break;
        }
        default:
            // TODO port: gameplay state 30 (sim tick) + menus (35/40/45/...).
            break;
    }
}
