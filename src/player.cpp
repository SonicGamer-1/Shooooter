#include "player.h"
#include <cmath>

Player::Player(float x, float y, float size, float speed, SDL_Texture *tex)
    : x(x), y(y), size(size), speed(speed), vx(0.0f), vy(0.0f), texture(tex) { src = {(int)x, (int)y, (int)size, (int)size}; }

void Player::handleInput(const Uint8 *keystate)
{
    int left = keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT];
    int right = keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT];

    int up = keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP];
    int down = keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_DOWN];

    ax = right - left;
    ay = down - up;

    float length = std::sqrt(ax * ax + ay * ay);

    if (length > 0.0f)
    {
        ax /= length;
        ay /= length;
    }
}

void Player::update(float deltaTime, int screenWidth, int screenHeight)
{
    vx += ax * speed * deltaTime;
    vy += ay * speed * deltaTime;

    vx *= std::exp(-PLAYER_FRICTION * deltaTime);
    vy *= std::exp(-PLAYER_FRICTION * deltaTime);

    x += vx * deltaTime;
    y += vy * deltaTime;

    src = {(int)x, (int)y, (int)size, (int)size};

    // Boundary collision checks
    if (x < 0.0f)
        x = 0.0f;
    if (y < 0.0f)
        y = 0.0f;
    if (x + size > static_cast<float>(SCREEN_WIDTH))
        x = static_cast<float>(SCREEN_WIDTH) - size;
    if (y + size > static_cast<float>(SCREEN_HEIGHT))
        y = static_cast<float>(SCREEN_HEIGHT) - size;
}

void Player::render(SDL_Renderer *renderer) const
{
    SDL_RenderCopy(renderer, texture, nullptr, &src);
}

void Player::setPosition(float newX, float newY)
{
    x = newX;
    y = newY;
}
