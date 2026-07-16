// Public app-facing game API.
#ifndef GAME_H
#define GAME_H

#include "game_state.h"

void game_start(Game* game);
void game_run(Game* game);
void game_paint(Game* game, Surface* graphics);

void game_keyPressed(Game* game, int32_t keyCode);
void game_keyReleased(Game* game, int32_t keyCode);
void game_pointerPressed(Game* game, int32_t x, int32_t y);
void game_pointerMoved(Game* game, int32_t x, int32_t y);
void game_pointerEdgeScroll(Game* game, int32_t x, int32_t y);

void game_loadMusic(Game* game);
void game_playMusic(Game* game);
void game_setMusicVolume(Game* game, int32_t level);
void game_stopMusic(Game* game);

#endif  // GAME_H
