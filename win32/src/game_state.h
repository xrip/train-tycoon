// Complete Train Tycoon game state and shared values.
#ifndef GAME_STATE_H
#define GAME_STATE_H
#include "game_constants.h"
#include "world.h"
#include "pathfinder.h"
#include "fleet.h"
#include "economy.h"
#include "offer_board.h"
#include "scenario.h"
#include "build_controller.h"
#include "assets.h"
#include "renderer.h"
#include "ui_controller.h"
#include "save_codec.h"
#include "music_controller.h"
#include "click_map.h"
#include "game_settings.h"
#include "midp.h"
#include "support/grid.h"
#include "support/java_random.h"
#include "support/resource_reader.h"
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

typedef struct Game {
    Game(std::unique_ptr<MusicOutput> music_output, int64_t random_seed);

    void start();
    void advance_frame();
    void render(Surface& surface);
    void key_pressed(int32_t key_code);
    void key_released(int32_t key_code);
    void pointer_pressed(int32_t x, int32_t y);
    void pointer_moved(int32_t x, int32_t y);
    void pointer_edge_scroll(int32_t x, int32_t y);
    void update_audio();
    void load_music();
    void play_music();
    void stop_music();
    void set_music_volume(int32_t level);
    bool music_available() const;
    bool music_playing() const;
    std::string_view music_error() const;
    void update_state_machine();
    void apply_offer_effect(int32_t row);
    void duplicate_offer(int32_t row);
    void monthly_offer_tick();
    void copy_offer_to_summary(int32_t row);
    void charge_train_running_costs();
    void remove_train_from_map(int32_t car, bool queue_tile);
    void merge_train_at(int32_t car);
    void maybe_take_ai_turn();
    void upgrade_one_ai_train_for_era();
    void reset_ai_depot_train_ages();
    void record_delivered_resource(int32_t resource, int32_t amount);
    void record_train_revenue(int32_t amount);
    void run_map_generation_step(int32_t mode);
    void spawn_train_at(int32_t x, int32_t y);
    void place_cities_and_industry();
    int8_t add_city_entry(int32_t direction);
    int8_t register_map_train(int8_t zone_population, int32_t y, int32_t x);
    void record_money_change(int32_t amount);
    bool matches_resource_condition(int32_t group, int32_t value);
    bool spend_money(int32_t amount);
    void update_ten_day_metric();
    bool sync_high_scores(bool upload);
    void viewport_zoom_key();
    void compute_viewport_bounds(int32_t anchor_x, int32_t anchor_y);
    std::optional<std::vector<int8_t>> resolve_dialog_value(int32_t screen,
                                                            int32_t field);
    void render_map(Surface& surface);

    bool stopRequested = false;
    int32_t simSpeed = 1;
    int32_t frameIndex = 0;
    bool displayInitialized = false;
    bool confirmCloseRequested = false;  // one-shot: close confirm dialog to the menu overlay
    bool snapshotLoadSucceeded = false;
    Pathfinder pathfinder;
    int32_t serviceVehicleAtCursor = 0;
    Fleet fleet;
    int32_t trainAtCursor = 0;
    bool compareToPrevious = false;
    bool monthRolloverFlag = false;
    int32_t savedLegacyMetricBq = 0;
    MusicController music;
    int32_t gameState = 0;
    World world;  // owns tileMap, trackMap, tileElevation, heightMap
    Economy economy;
    OfferBoard offerBoard;
    Scenario scenario;
    BuildController build;
    Assets assets;
    Renderer renderer;
    UiController ui;
    SaveCodec saveCodec;
    ClickMap clickMap;  // captured overlay menu-row hit-rects, filled in game_paint
    GameSettings settings;
    int32_t cachedHeaderMoney = 0;
    int32_t cachedHeaderDay = 0;
    int32_t cachedHeaderMonth = 0;
    int32_t cachedHeaderYear = 0;
    int32_t dayFrameCounter = 0;
    JavaRandom rng;
    int64_t loadingStartTime = 0;
    bool loadingTimeout = false;
    bool ready = true;
    int32_t stateWorkStep = 0;
    int32_t tile50AnimationPhase = 0;
    int32_t tile50AnimationFrame = 0;
    int32_t tile64AnimationPhase = 0;
    int32_t tile64AnimationFrame = 0;
    int32_t tile55AnimationPhase = 0;
    int32_t tile55AnimationFrame = 0;
    int32_t graphColors[kGraphColorCount]{};  // 3 series x 3 colors for report graphs
    int32_t lastCanvasWidth = -1;   // last size passed to game_setCanvasSize
    int32_t lastCanvasHeight = -1;
    int32_t savedGameState = 0;  // game state stashed for return after an overlay
    int32_t loadingAnimationFrame = 0;
    int32_t dayTickThreshold = 50;  // frames per game-day (dayFrameCounter counts up to this)
} Game;

#endif
