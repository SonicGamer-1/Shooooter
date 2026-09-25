#ifndef CONSTS_H
#define CONSTS_H

#include <cstdint>

// Math constants
inline constexpr float PI = 3.14159265358979323846f;
inline constexpr float RAD_TO_DEG = 180.0f / PI;

// Window settings
inline constexpr const char* WINDOW_TITLE = "SDL2 Moving Player Template";
inline constexpr int SCREEN_WIDTH = 800;
inline constexpr int SCREEN_HEIGHT = 600;

// Timing settings
inline constexpr float MAX_DELTA_TIME = 0.05f;

// Player settings
inline constexpr float PLAYER_DEFAULT_SIZE = 50.0f;
inline constexpr float PLAYER_DEFAULT_SPEED = 300.0f;
inline constexpr int PLAYER_TEXTURE_RADIUS = 50;
inline constexpr float PLAYER_FRICTION = 0.9f;

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

// Background grid settings
inline constexpr int GRID_CELL_SIZE = 40;
inline constexpr uint8_t GRID_COLOR_R = 32;
inline constexpr uint8_t GRID_COLOR_G = 34;
inline constexpr uint8_t GRID_COLOR_B = 44;
inline constexpr uint8_t GRID_COLOR_A = 120;

// Weapon default stats
inline constexpr const char* WEAPON_DEFAULT_NAME = "Pistol";
inline constexpr float WEAPON_DEFAULT_RELOAD_SPEED = 1.2f;
inline constexpr float WEAPON_DEFAULT_RANGE = 600.0f;
inline constexpr float WEAPON_DEFAULT_BULLET_SPEED = 850.0f;
inline constexpr int WEAPON_DEFAULT_MAG_SIZE = 12;
inline constexpr float WEAPON_DEFAULT_FIRE_RATE = 0.16f;
inline constexpr float WEAPON_SPRITE_WIDTH = 36.0f;
inline constexpr float WEAPON_SPRITE_HEIGHT = 12.0f;
inline constexpr float BULLET_DEFAULT_SIZE = 4.0f;

// Bullet colors (RGBA)
inline constexpr uint8_t BULLET_COLOR_R = 255;
inline constexpr uint8_t BULLET_COLOR_G = 230;
inline constexpr uint8_t BULLET_COLOR_B = 80;
inline constexpr uint8_t BULLET_COLOR_A = 255;

inline constexpr uint8_t BULLET_CORE_COLOR_R = 255;
inline constexpr uint8_t BULLET_CORE_COLOR_G = 255;
inline constexpr uint8_t BULLET_CORE_COLOR_B = 200;
inline constexpr uint8_t BULLET_CORE_COLOR_A = 255;

// Visual effects & Juice
inline constexpr float SCREEN_SHAKE_INTENSITY = 3.5f;
inline constexpr float SCREEN_SHAKE_DECAY = 18.0f;
inline constexpr float CROSSHAIR_KICKBACK_DECAY = 6.0f;
inline constexpr float MUZZLE_FLASH_DURATION = 0.06f;
inline constexpr float MUZZLE_FLASH_SIZE = 14.0f;
inline constexpr int MUZZLE_SPARKS_COUNT = 5;
inline constexpr float DUST_SPAWN_INTERVAL = 0.04f;

// Crosshair settings
inline constexpr int CROSSHAIR_BASE_GAP = 5;
inline constexpr int CROSSHAIR_TICK_LENGTH = 8;

// HUD settings
inline constexpr int HUD_START_X = 25;
inline constexpr int HUD_START_Y_OFFSET = 35;
inline constexpr int HUD_PIP_WIDTH = 6;
inline constexpr int HUD_PIP_HEIGHT = 16;
inline constexpr int HUD_PIP_GAP = 4;

inline constexpr uint8_t HUD_AMMO_COLOR_R = 0;
inline constexpr uint8_t HUD_AMMO_COLOR_G = 220;
inline constexpr uint8_t HUD_AMMO_COLOR_B = 255;
inline constexpr uint8_t HUD_AMMO_COLOR_A = 230;

inline constexpr uint8_t HUD_RELOAD_BAR_COLOR_R = 255;
inline constexpr uint8_t HUD_RELOAD_BAR_COLOR_G = 180;
inline constexpr uint8_t HUD_RELOAD_BAR_COLOR_B = 40;
inline constexpr uint8_t HUD_RELOAD_BAR_COLOR_A = 255;

#endif // CONSTS_H
