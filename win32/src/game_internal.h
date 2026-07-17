// Calls shared by game-core translation units and behavior tests.
#ifndef GAME_INTERNAL_H
#define GAME_INTERNAL_H
#include "game.h"
#include "support/byte_buffer.h"

#include <optional>
#include <span>
#include <vector>

// Resolve the tile under a map-screen-space point (anchorX/Y) and refresh the
// tile cursor, its screen position, and the selection diamond. anchorX/Y are in
// the same space as viewportBaseX/Y (absolute map-pixel coords, not view-relative).
// The keyboard zoom path passes viewportBase (screen centre); the pointer path
// passes the mouse position translated into map space.
#endif  // GAME_INTERNAL_H
