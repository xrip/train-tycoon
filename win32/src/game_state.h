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

struct ScenarioDefinition;

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
    void initialize_map_generator();
    void advance_map_generation_stage();
    void generate_terrain_height_step();
    void generate_terrain_patch_step();
    void generate_terrain_feature_step();
    void scatter_initial_obstacles();
    void generate_one_map_step();
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
    void handle_key(int32_t key_code);
    void handle_dialog_input(int8_t direction);
    void set_state(int32_t state);
    void smooth_tile_at(int32_t x, int32_t y, bool raise);
    void raise_terrain_area(int32_t x, int32_t y, int32_t width,
                            int32_t height, int32_t level);
    void lower_terrain_area(int32_t x, int32_t y, int32_t width,
                            int32_t height, int32_t level);
    void track_build_step(int32_t x, int32_t y, int32_t width,
                          int32_t height);
    void place_terrain_patch(int32_t x, int32_t y);
    void place_terrain_feature(int32_t type);
    void reset_overlay_render_state();
    void load_settings_snapshot();
    void set_canvas_size(int32_t width, int32_t height);
    void render_packed_sprite(Surface& surface, int32_t sprite_id);
    int32_t unload_train_revenue(int32_t car, int32_t service_site);
    void update_train(int32_t head);
    void persist_settings_snapshot();
    int32_t compute_map_train_metric(int32_t score_slot);
    void check_scenario_milestone();
    int32_t scan_map_metric() const;
    void new_year_notify();
    void tick_input_or_ai(bool player_action);
    void simulation_tick();
    void enter_out_of_money_state();
    void apply_background_cost();
    void demolish_at_cursor();
    void pick_trains_at_cursor();
    void build_place_track_piece(int32_t piece);
    void compute_track_build(int32_t start_x, int32_t start_y,
                             int32_t step_x, int32_t step_y,
                             int32_t end_x, int32_t end_y,
                             int32_t tile_code, int32_t cost);
    void revert_track_tiles();
    void place_track_at_cursor(bool first_direction, int32_t x,
                               int32_t y, int32_t step_x, int32_t step_y,
                               int32_t tile_code, int32_t owner,
                               bool charge, int32_t cost);
    void normalize_track_bits();
    void set_track_owner_code(int32_t owner);
    void rebuild_menu_type_indexes();
    void open_build_menu();
    void add_selected_option(int32_t option);
    void pop_selected_option();
    void increment_cargo_option_count(int32_t car_type);
    void decrement_cargo_option_count(int32_t car_type);
    void decrement_train_cargo_option_counts(int32_t car);
    bool buy_train_at_cursor();
    bool apply_train_upgrade(int32_t train, int32_t engine_type,
                             int32_t* wagon_types);
    int32_t draw_menu_row(Surface& surface, int32_t sprite,
                          int32_t item_index, int32_t x, bool spaced);
    void draw_header_bar(Surface& surface);
    void draw_confirm_bar(Surface& surface);
    void paint_main(Surface& surface);
    int32_t draw_number(Surface& surface, int32_t value, int32_t y);
    void draw_money_ticker(Surface& surface);
    void draw_confirm_dialog(Surface& surface);
    void compute_iso_corners(int32_t x, int32_t y, int32_t* screen_x,
                             int32_t* screen_y, bool viewport_relative);
    void draw_build_menu_cursor(Surface& surface);
    void draw_build_menu_row(Surface& surface);
    void handle_build_menu_state(int32_t screen, int32_t option);
    void apply_scenario_definition(const ScenarioDefinition& definition);
    void begin_map_generation();
    void update_boot_splash();
    void update_intro_ready();
    void update_loading_resources();
    void update_generating_map();
    void update_playing();
    void set_minimap_color(int32_t kind, int32_t x, int32_t y);
    void build_minimap_overview();
    void update_building_minimap();
    void update_map_overview();
    void update_building_path();
    void update_saving_game();
    void update_loading_game();
    void update_ai_turn();
    void update_high_score_sync();
    void advance_tile_animation_frames();
    void apply_queued_tile_changes();
    void update_active_trains();
    void advance_month_if_day_ended();
    void apply_new_month_changes();
    void advance_year_if_needed();
    void advance_era_if_needed();
    void advance_game_day();
    void click_bottom_bar(int32_t x);
    void paint_map_overview(Surface& surface);
    void redraw_static_map_if_needed(int32_t origin_x, int32_t origin_y);

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
    int32_t lastCanvasWidth = -1;   // last size passed to set_canvas_size
    int32_t lastCanvasHeight = -1;
    int32_t savedGameState = 0;  // game state stashed for return after an overlay
    int32_t loadingAnimationFrame = 0;
    int32_t dayTickThreshold = 50;  // frames per game-day (dayFrameCounter counts up to this)
} Game;

#endif
