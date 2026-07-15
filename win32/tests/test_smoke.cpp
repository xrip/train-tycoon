#include "doctest/doctest.h"

#include "game_methods.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace {

constexpr int kScreenWidth = 240;
constexpr int kScreenHeight = 320;
std::uint32_t framebuffer[kScreenWidth * kScreenHeight];

GameCanvas* make_game() {
    auto* game = static_cast<GameCanvas*>(std::malloc(sizeof(GameCanvas)));
    REQUIRE(game != nullptr);
    gc_fields_init(game);
    return game;
}

} // namespace

TEST_CASE("small path search") {
    GameCanvas* game = make_game();
    game->mapWidth = 5;
    game->mapHeight = 5;
    game->tileMap = GC_NEW(int8_t*, game->mapWidth);
    game->trackMap = GC_NEW(int8_t*, game->mapWidth);
    game->tileElevation = GC_NEW(int8_t*, game->mapWidth);
    for (int x = 0; x < game->mapWidth; ++x) {
        game->tileMap[x] = GC_NEW(int8_t, game->mapHeight);
        game->trackMap[x] = GC_NEW(int8_t, game->mapHeight);
        game->tileElevation[x] = GC_NEW(int8_t, game->mapHeight);
    }
    game->placementX = 1;
    game->placementY = 2;
    game->demolishX = 3;
    game->demolishY = 2;
    game->accel1 = 10;

    CHECK(gc_tryBuildPath(game));
    CHECK(game->trackTileCount > 0);
    CHECK(game->selectionCost > 0);
    std::free(game);
}

TEST_CASE("save buffer round trip") {
    GameCanvas* game = make_game();
    game->money = 123456;
    game->dayOfMonth = 17;
    game->monthOfYear = 8;
    game->year = 1973;
    gc_initSaveOrLoad(game, 4);

    const int plain_size = game->snapshotSize;
    REQUIRE(plain_size > 0);
    auto* plain = static_cast<int8_t*>(std::malloc(static_cast<std::size_t>(plain_size)));
    REQUIRE(plain != nullptr);
    std::memcpy(plain, game->saveBuffer, static_cast<std::size_t>(plain_size));

    gc_compressSaveBuffer(game);
    gc_decompressSaveBuffer(game);
    CHECK(game->snapshotSize == plain_size);
    CHECK(std::memcmp(game->saveBuffer, plain, static_cast<std::size_t>(plain_size)) == 0);

    game->money = 0;
    game->dayOfMonth = 0;
    game->monthOfYear = 0;
    game->year = 0;
    gc_initSaveOrLoad(game, 5);
    CHECK(game->money == 123456);
    CHECK(game->dayOfMonth == 17);
    CHECK(game->monthOfYear == 8);
    CHECK(game->year == 1973);

    std::free(plain);
    std::free(game);
}

TEST_CASE("MIDI resource is valid") {
    CHECK(midp_media_validate_midi("/6"));
}

TEST_CASE("boot, generate scenario zero, update, and paint") {
    GameCanvas* game = make_game();
    gc_start(game, nullptr);

    game->loadingStartTime = 0;
    gc_update(game);
    game->loadingStartTime = 0;
    gc_update(game);
    gc_update(game);
    REQUIRE(game->gameState == 20);

    gc_setState(game, 10);
    for (int step = 0; game->gameState == 10 && step < 1000; ++step) {
        gc_update(game);
    }
    REQUIRE(game->gameState == 20);

    game->activeScenario = 0;
    gc_setState(game, 25);
    for (int step = 0; game->gameState == 25 && step < 200000; ++step) {
        gc_update(game);
    }
    REQUIRE(game->gameState == 30);
    REQUIRE(game->tileMap != nullptr);
    REQUIRE(game->trackMap != nullptr);

    MidpGraphics screen;
    midp_gfx_init_screen(&screen, framebuffer, kScreenWidth, kScreenHeight);
    gc_paint(game, &screen);
    gc_keyPressed(game, 54);
    gc_run(game);
    gc_paint(game, &screen);
    CHECK(game->gameState == 30);
    CHECK(game->backbuffer != nullptr);

    midp_media_shutdown();
    std::free(game);
}
