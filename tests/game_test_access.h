#ifndef GAME_TEST_ACCESS_H
#define GAME_TEST_ACCESS_H

#include "game.h"

struct GameTestAccess {
    static World& world(Game& game) { return game.world; }
    static const World& world(const Game& game) { return game.world; }
    static Fleet& fleet(Game& game) { return game.fleet; }
    static const Fleet& fleet(const Game& game) { return game.fleet; }
    static Economy& economy(Game& game) { return game.economy; }
    static const Economy& economy(const Game& game) { return game.economy; }
    static OfferBoard& offer_board(Game& game) { return game.offerBoard; }
    static const OfferBoard& offer_board(const Game& game) { return game.offerBoard; }
    static Scenario& scenario(Game& game) { return game.scenario; }
    static const Scenario& scenario(const Game& game) { return game.scenario; }
    static BuildController& build(Game& game) { return game.build; }
    static const BuildController& build(const Game& game) { return game.build; }
    static Assets& assets(Game& game) { return game.assets; }
    static const Assets& assets(const Game& game) { return game.assets; }
    static Renderer& renderer(Game& game) { return game.renderer; }
    static const Renderer& renderer(const Game& game) { return game.renderer; }
    static UiController& ui(Game& game) { return game.ui; }
    static const UiController& ui(const Game& game) { return game.ui; }
    static SaveCodec& save_codec(Game& game) { return game.saveCodec; }
    static const SaveCodec& save_codec(const Game& game) { return game.saveCodec; }
    static MusicController& music(Game& game) { return game.music; }
    static const MusicController& music(const Game& game) { return game.music; }
    static Pathfinder& pathfinder(Game& game) { return game.pathfinder; }
    static const Pathfinder& pathfinder(const Game& game) { return game.pathfinder; }
    static JavaRandom& random(Game& game) { return game.rng; }

    static int32_t& state(Game& game) { return game.gameState; }
    static const int32_t& state(const Game& game) { return game.gameState; }
    static int32_t& simulation_speed(Game& game) { return game.simSpeed; }
    static int32_t& day_tick_threshold(Game& game) { return game.dayTickThreshold; }
    static int32_t& day_frame_counter(Game& game) { return game.dayFrameCounter; }
    static int32_t& service_site_at_cursor(Game& game) {
        return game.serviceVehicleAtCursor;
    }
    static int32_t& last_canvas_width(Game& game) { return game.lastCanvasWidth; }
    static int32_t& last_canvas_height(Game& game) { return game.lastCanvasHeight; }
    static int64_t& loading_start_time(Game& game) { return game.loadingStartTime; }
    static bool& ready(Game& game) { return game.ready; }

    static void update_state_machine(Game& game) { game.update_state_machine(); }
    static void set_state(Game& game, int32_t state) { game.set_state(state); }
    static void simulation_tick(Game& game) { game.simulation_tick(); }
    static void update_train(Game& game, int32_t head) {
        game.update_train(head);
    }
    static void pick_trains_at_cursor(Game& game) { game.pick_trains_at_cursor(); }
    static void open_build_menu(Game& game) { game.open_build_menu(); }
    static bool buy_train_at_cursor(Game& game) {
        return game.buy_train_at_cursor();
    }
    static void build_place_track_piece(Game& game, int32_t piece) {
        game.build_place_track_piece(piece);
    }
};

#endif  // GAME_TEST_ACCESS_H
