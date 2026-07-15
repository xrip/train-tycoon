#include "doctest/doctest.h"

#include "game_methods.h"
#include "stable_digest.h"

#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace {

constexpr int kScreenWidth = 240;
constexpr int kScreenHeight = 320;
constexpr std::int64_t kFixedRandomSeed = 0x5eed1234;
std::uint32_t framebuffer[kScreenWidth * kScreenHeight];

GameCanvas* make_game() {
    auto* game = static_cast<GameCanvas*>(std::malloc(sizeof(GameCanvas)));
    REQUIRE(game != nullptr);
    gc_fields_init(game);
    game->rng.set_seed(kFixedRandomSeed);
    return game;
}

std::uint64_t map_digest(const GameCanvas* game) {
    StableDigest digest;
    digest.add_i32(game->activeScenario);
    digest.add_i32(game->mapWidth);
    digest.add_i32(game->mapHeight);
    digest.add_i32(game->money);
    digest.add_i32(game->loanCeiling);
    digest.add_i32(game->loanBalance);
    for (int x = 0; x < game->mapWidth; ++x) {
        digest.add_bytes(game->tileMap[x], static_cast<std::size_t>(game->mapHeight));
        digest.add_bytes(game->trackMap[x], static_cast<std::size_t>(game->mapHeight));
        digest.add_bytes(game->tileElevation[x], static_cast<std::size_t>(game->mapHeight));
    }
    for (int x = 0; x < game->mapWidth + 2; ++x) {
        digest.add_bytes(game->heightMap[x], static_cast<std::size_t>(game->mapHeight + 2));
    }
    return digest.value();
}

void generate_scenario(GameCanvas* game, int scenario) {
    game->activeScenario = scenario;
    gc_setState(game, 25);
    for (int step = 0; game->gameState == 25 && step < 200000; ++step) {
        gc_update(game);
    }
    REQUIRE(game->gameState == 30);
    REQUIRE(game->tileMap != nullptr);
    REQUIRE(game->trackMap != nullptr);
}

} // namespace

TEST_CASE("fast: Java random values") {
    JavaRandom random{};
    random.set_seed(0);
    CHECK(random.next_int() == -1155484576);
    CHECK(random.next_int() == -723955400);
    CHECK(random.next_int(1000) == 29);
    CHECK(random.next_int(16) == 9);

    random.set_seed(0);
    CHECK(random.next_int() == -1155484576);
}

TEST_CASE("fast: raw resources have fixed digests") {
    struct ExpectedResource {
        const char* name;
        std::uint64_t digest;
    };
    constexpr std::array resources{
        ExpectedResource{"/1", 12333131007334528179ULL},
        ExpectedResource{"/2", 3313215035755421192ULL},
        ExpectedResource{"/3", 10129176710171002926ULL},
        ExpectedResource{"/4", 6400309593563470510ULL},
        ExpectedResource{"/5", 1309244365097902259ULL},
        ExpectedResource{"/6", 6784549803277222961ULL},
        ExpectedResource{"/ico.png", 5309835248505004148ULL},
    };

    for (const auto& resource : resources) {
        std::uint8_t* bytes = nullptr;
        int size = 0;
        REQUIRE(midp_resource_read_all(resource.name, &bytes, &size));
        StableDigest digest;
        digest.add_bytes(bytes, static_cast<std::size_t>(size));
        INFO("resource: ", resource.name);
        CHECK(digest.value() == resource.digest);
        std::free(bytes);
    }
}

TEST_CASE("fast: record store keeps record bytes and ids") {
    constexpr std::array<std::uint8_t, 5> record{3, 1, 4, 1, 5};
    MidpRecordStore* store = midp_rms_open("RulesStore", true);
    REQUIRE(store != nullptr);
    CHECK(midp_rms_num_records(store) == 0);
    const int record_id = midp_rms_add_record(
        store, record.data(), 0, static_cast<int>(record.size()));
    CHECK(record_id == 1);
    midp_rms_close(store);

    store = midp_rms_open("RulesStore", false);
    REQUIRE(store != nullptr);
    CHECK(midp_rms_num_records(store) == 1);
    CHECK(midp_rms_first_record_id(store) == 1);
    CHECK(midp_rms_record_size(store, 1) == static_cast<int>(record.size()));
    std::uint8_t* restored = nullptr;
    int restored_size = 0;
    REQUIRE(midp_rms_get_record(store, 1, &restored, &restored_size));
    CHECK(restored_size == static_cast<int>(record.size()));
    CHECK(std::memcmp(restored, record.data(), record.size()) == 0);
    std::free(restored);
    midp_rms_delete_record(store, 1);
    midp_rms_close(store);

    store = midp_rms_open("RulesStore", false);
    REQUIRE(store != nullptr);
    CHECK(midp_rms_num_records(store) == 0);
    midp_rms_close(store);
}

TEST_CASE("fast: small path search") {
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

TEST_CASE("fast: save buffer bytes and round trip") {
    GameCanvas* game = make_game();
    game->money = 123456;
    game->dayOfMonth = 17;
    game->monthOfYear = 8;
    game->year = 1973;
    game->loanBalance = -7654321;
    game->visibleTrains[0] = -1234;
    gc_initSaveOrLoad(game, 4);

    const int plain_size = game->snapshotSize;
    REQUIRE(plain_size > 0);
    StableDigest save_digest;
    save_digest.add_bytes(game->saveBuffer, static_cast<std::size_t>(plain_size));
    CHECK(plain_size == 19242);
    CHECK(save_digest.value() == 16273675477449773753ULL);
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
    game->loanBalance = 0;
    game->visibleTrains[0] = 0;
    gc_initSaveOrLoad(game, 5);
    CHECK(game->money == 123456);
    CHECK(game->dayOfMonth == 17);
    CHECK(game->monthOfYear == 8);
    CHECK(game->year == 1973);
    CHECK(game->loanBalance == -7654321);
    CHECK(game->visibleTrains[0] == -1234);

    std::free(plain);
    std::free(game);
}

TEST_CASE("fast: MIDI resource is valid") {
    CHECK(midp_media_validate_midi("/6"));
}

TEST_CASE("full: all scenario maps have fixed digests") {
    constexpr std::array<std::uint64_t, 13> expected{
        4307122233410706396ULL,
        7410989599457244056ULL,
        14660043004649095592ULL,
        1298331539406995531ULL,
        13704524898869083524ULL,
        2397133757916587733ULL,
        2733903835998681015ULL,
        2254306706094717035ULL,
        12476545952634235763ULL,
        5618243249801725703ULL,
        9569871328048813870ULL,
        5566282860187776653ULL,
        46185107338736850ULL,
    };
    for (int scenario = 0; scenario < static_cast<int>(expected.size()); ++scenario) {
        GameCanvas* game = make_game();
        generate_scenario(game, scenario);
        INFO(scenario);
        CHECK(map_digest(game) == expected[scenario]);
        std::free(game);
    }
}

TEST_CASE("full: scenario zero input and frames have fixed digests") {
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

    generate_scenario(game, 0);

    std::memset(framebuffer, 0, sizeof(framebuffer));
    MidpGraphics screen;
    midp_gfx_init_screen(&screen, framebuffer, kScreenWidth, kScreenHeight);
    gc_paint(game, &screen);
    StableDigest first_frame;
    first_frame.add_pixels(framebuffer, std::size(framebuffer));
    CHECK(first_frame.value() == 13699798000239068498ULL);

    const int old_viewport_x = game->viewportCenterX;
    gc_keyPressed(game, 54);
    gc_run(game);
    gc_paint(game, &screen);
    CHECK(game->gameState == 30);
    CHECK(game->backbuffer != nullptr);
    CHECK(game->viewportCenterX != old_viewport_x);
    StableDigest moved_frame;
    moved_frame.add_pixels(framebuffer, std::size(framebuffer));
    CHECK(moved_frame.value() == 2749731419861486662ULL);

    midp_media_shutdown();
    std::free(game);
}
