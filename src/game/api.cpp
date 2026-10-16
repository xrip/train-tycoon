#include "game.h"

bool Game::music_available() const { return music.available; }
bool Game::music_playing() const { return music.playing; }
std::string_view Game::music_error() const {
    return music.output ? music.output->last_error() : std::string_view{};
}
