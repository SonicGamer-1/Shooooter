#include <SDL2/SDL.h>
#include <iostream>
#include "player.h"
#include "weapon.h"
#include "consts.h"

SDL_Texture *CreateCircleTexture(SDL_Renderer *renderer, int radius)
{
    int diameter = radius * 2;

    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(
        0,
        diameter,
        diameter,
        32,
        SDL_PIXELFORMAT_RGBA32);

    if (!surface)
        return nullptr;

    // Make background transparent
    SDL_FillRect(surface, nullptr, SDL_MapRGBA(surface->format, 0, 0, 0, 0));

    // Draw filled circle
    Uint32 *pixels = static_cast<Uint32 *>(surface->pixels);

    for (int y = 0; y < diameter; y++)
    {
        for (int x = 0; x < diameter; x++)
        {
            int dx = x - radius;
            int dy = y - radius;

            if (dx * dx + dy * dy <= radius * radius)
            {
                pixels[y * (surface->pitch / 4) + x] =
                    SDL_MapRGBA(surface->format, PLAYER_COLOR_R, PLAYER_COLOR_G, PLAYER_COLOR_B, PLAYER_COLOR_A);
            }
        }
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(
        renderer,
        surface);

    SDL_FreeSurface(surface);

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    return texture;
}

int main(int argc, char *argv[])
{
    // Suppress unused parameter warnings
    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window)
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer)
    {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    const float playerSize = PLAYER_DEFAULT_SIZE;
    const float playerSpeed = PLAYER_DEFAULT_SPEED;

    SDL_Texture *playerTex = CreateCircleTexture(renderer, PLAYER_TEXTURE_RADIUS);

    Player player((SCREEN_WIDTH - playerSize) / 2.0f, (SCREEN_HEIGHT - playerSize) / 2.0f,
                  playerSize, playerSpeed, playerTex);

    // Create weapon instance (can also instantiate any derived weapon class)
    Weapon weapon("Pistol", 1.2f, 600.0f, 800.0f, 10, 0.18f);
    weapon.createDefaultTexture(renderer);

    bool isRunning = true;
    SDL_Event event;

    Uint64 lastCounter = SDL_GetPerformanceCounter();
    const Uint64 perfFreq = SDL_GetPerformanceFrequency();

    while (isRunning)
    {
        // Handle window/quit events
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                isRunning = false;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    isRunning = false;
                }
                else if (event.key.keysym.sym == SDLK_r)
                {
                    weapon.reload();
                }
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    weapon.shoot();
                }
            }
        }

        // Calculate delta time
        Uint64 currentCounter = SDL_GetPerformanceCounter();
        float deltaTime = static_cast<float>(currentCounter - lastCounter) / static_cast<float>(perfFreq);
        lastCounter = currentCounter;

        // Cap maximum delta time to prevent large physics jumps if lag occurs
        if (deltaTime > MAX_DELTA_TIME)
        {
            deltaTime = MAX_DELTA_TIME;
        }

        // Mouse position and continuous shooting if held
        int mouseX = 0;
        int mouseY = 0;
        Uint32 mouseButtons = SDL_GetMouseState(&mouseX, &mouseY);
        if (mouseButtons & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            weapon.shoot();
        }

        // Input
        const Uint8 *keystate = SDL_GetKeyboardState(nullptr);
        player.handleInput(keystate);

        // Update player
        player.update(deltaTime, SCREEN_WIDTH, SCREEN_HEIGHT);

        // Update weapon (aims at pointer and updates bullets)
        float playerCenterX = player.getX() + player.getSize() / 2.0f;
        float playerCenterY = player.getY() + player.getSize() / 2.0f;
        weapon.update(deltaTime, playerCenterX, playerCenterY, mouseX, mouseY);

        // Render
        SDL_SetRenderDrawColor(renderer, CLEAR_COLOR_R, CLEAR_COLOR_G, CLEAR_COLOR_B, CLEAR_COLOR_A);
        SDL_RenderClear(renderer);

        // Render bullets, player, and weapon
        weapon.renderBullets(renderer);
        player.render(renderer);
        weapon.render(renderer);

        SDL_RenderPresent(renderer);
    }

    // Cleanup
    SDL_DestroyTexture(playerTex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
