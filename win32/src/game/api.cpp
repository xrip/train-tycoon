#include "game_internal.h"

void Game::start() { game_start(this); }
void Game::advance_frame() { game_run(this); }
void Game::render(Surface& surface) { game_paint(this, &surface); }
void Game::key_pressed(int32_t key_code) { game_keyPressed(this, key_code); }
void Game::key_released(int32_t key_code) { game_keyReleased(this, key_code); }
void Game::pointer_pressed(int32_t x, int32_t y) {
    game_pointerPressed(this, x, y);
}
void Game::pointer_moved(int32_t x, int32_t y) {
    game_pointerMoved(this, x, y);
}
void Game::pointer_edge_scroll(int32_t x, int32_t y) {
    game_pointerEdgeScroll(this, x, y);
}
void Game::update_audio() { music.update(); }
void Game::load_music() { game_loadMusic(this); }
void Game::play_music() { game_playMusic(this); }
void Game::stop_music() { game_stopMusic(this); }
void Game::set_music_volume(int32_t level) {
    game_setMusicVolume(this, level);
}
bool Game::music_available() const { return music.available; }
bool Game::music_playing() const { return music.playing; }
std::string_view Game::music_error() const {
    return music.output ? music.output->last_error() : std::string_view{};
}
