#ifndef CONSTS_H
#define CONSTS_H

#include <cstdint>

// Window settings
inline constexpr const char* WINDOW_TITLE = "SDL2 Moving Player Template";
inline constexpr int SCREEN_WIDTH = 800;
inline constexpr int SCREEN_HEIGHT = 600;

// Timing settings
inline constexpr float MAX_DELTA_TIME = 0.05f;

// Player default settings
inline constexpr float PLAYER_DEFAULT_SIZE = 50.0f;
inline constexpr float PLAYER_DEFAULT_SPEED = 300.0f;
inline constexpr int PLAYER_TEXTURE_RADIUS = 50;

// Background clear color (RGBA)
inline constexpr uint8_t CLEAR_COLOR_R = 24;
inline constexpr uint8_t CLEAR_COLOR_G = 24;
inline constexpr uint8_t CLEAR_COLOR_B = 32;
inline constexpr uint8_t CLEAR_COLOR_A = 255;

// Player color (RGBA)
inline constexpr uint8_t PLAYER_COLOR_R = 0;
inline constexpr uint8_t PLAYER_COLOR_G = 200;
inline constexpr uint8_t PLAYER_COLOR_B = 255;
inline constexpr uint8_t PLAYER_COLOR_A = 255;

#endif // CONSTS_H
