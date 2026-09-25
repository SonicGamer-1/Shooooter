#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include "consts.h"

class Player
{
public:
    Player(float x = 0.0f, float y = 0.0f, float size = PLAYER_DEFAULT_SIZE, float speed = PLAYER_DEFAULT_SPEED, SDL_Texture *tex = nullptr);

    void handleInput(const Uint8 *keystate);
    void update(float deltaTime, int screenWidth, int screenHeight);
    void render(SDL_Renderer *renderer) const;

    // Getters
    float getX() const { return x; }
    float getY() const { return y; }
    float getSize() const { return size; }

    // Setters
    void setPosition(float newX, float newY);

private:
    float x, y;
    float size;
    float speed;
    float vx, vy;
    float ax, ay;
    SDL_Texture *texture;
    SDL_Rect src;
};

#endif // PLAYER_H
