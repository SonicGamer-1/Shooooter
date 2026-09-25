#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include "player.h"
#include "weapon.h"
#include "consts.h"

// Particle structure for visual effects (sparks, dust, muzzle smoke)
struct Particle
{
    float x, y;
    float vx, vy;
    float life;
    float maxLife;
    float size;
    Uint8 r, g, b, a;
};

// Muzzle flash visual effect
struct MuzzleFlash
{
    float x, y;
    float timer;
    float duration;
    float size;
};

// Generates a soft circular texture for the player
SDL_Texture *CreateCircleTexture(SDL_Renderer *renderer, int radius)
{
    int diameter = radius * 2;

    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, diameter, diameter, 32, SDL_PIXELFORMAT_RGBA32);

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

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (texture)
    {
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    }

    return texture;
}

// Spawns a burst of sparks/smoke at a location
void SpawnSparks(std::vector<Particle> &particles, float x, float y, float dirX, float dirY, int count, Uint8 r, Uint8 g, Uint8 b)
{
    for (int i = 0; i < count; ++i)
    {
        float angleOffset = ((rand() % 100) / 100.0f - 0.5f) * 1.2f;
        float speed = 80.0f + (rand() % 160);
        float baseAngle = std::atan2(dirY, dirX) + angleOffset;

        Particle p;
        p.x = x;
        p.y = y;
        p.vx = std::cos(baseAngle) * speed;
        p.vy = std::sin(baseAngle) * speed;
        p.life = 0.0f;
        p.maxLife = 0.15f + ((rand() % 100) / 1000.0f);
        p.size = 2.0f + (rand() % 3);
        p.r = r;
        p.g = g;
        p.b = b;
        p.a = 255;
        particles.push_back(p);
    }
}

// Draw a futuristic aim crosshair at mouse coordinates
void RenderCrosshair(SDL_Renderer *renderer, int x, int y, float kickback)
{
    int gap = static_cast<int>(CROSSHAIR_BASE_GAP + kickback * 4.0f);
    int length = CROSSHAIR_TICK_LENGTH;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);

    // Center dot
    SDL_RenderDrawPoint(renderer, x, y);

    // Crosshair ticks
    SDL_RenderDrawLine(renderer, x - gap - length, y, x - gap, y);
    SDL_RenderDrawLine(renderer, x + gap, y, x + gap + length, y);
    SDL_RenderDrawLine(renderer, x, y - gap - length, x, y - gap);
    SDL_RenderDrawLine(renderer, x, y + gap, x, y + gap + length);
}

// Render Ammo & Reload HUD
void RenderHUD(SDL_Renderer *renderer, const Weapon &weapon)
{
    int startX = HUD_START_X;
    int startY = SCREEN_HEIGHT - HUD_START_Y_OFFSET;
    int pipWidth = HUD_PIP_WIDTH;
    int pipHeight = HUD_PIP_HEIGHT;
    int pipGap = HUD_PIP_GAP;

    // Draw ammo magazine pips
    for (int i = 0; i < weapon.getMagSize(); ++i)
    {
        SDL_Rect pipRect = {
            startX + i * (pipWidth + pipGap),
            startY,
            pipWidth,
            pipHeight};

        if (i < weapon.getCurrentAmmo())
        {
            SDL_SetRenderDrawColor(renderer, HUD_AMMO_COLOR_R, HUD_AMMO_COLOR_G, HUD_AMMO_COLOR_B, HUD_AMMO_COLOR_A);
            SDL_RenderFillRect(renderer, &pipRect);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 60, 70, 85, 150); // Spent slot
            SDL_RenderDrawRect(renderer, &pipRect);
        }
    }

    // Reload progress bar
    if (weapon.isReloadingState())
    {
        int barW = weapon.getMagSize() * (pipWidth + pipGap) - pipGap;
        int barH = 5;
        int barY = startY - 10;

        SDL_Rect bgBar = {startX, barY, barW, barH};
        SDL_SetRenderDrawColor(renderer, 40, 45, 55, 200);
        SDL_RenderFillRect(renderer, &bgBar);

        int fillW = static_cast<int>(barW * weapon.getReloadProgress());
        SDL_Rect fillBar = {startX, barY, fillW, barH};
        SDL_SetRenderDrawColor(renderer, HUD_RELOAD_BAR_COLOR_R, HUD_RELOAD_BAR_COLOR_G, HUD_RELOAD_BAR_COLOR_B, HUD_RELOAD_BAR_COLOR_A);
        SDL_RenderFillRect(renderer, &fillBar);
    }
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Enable linear scaling for sharp visuals on resize
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    // Create resizable window
    SDL_Window *window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (!window)
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer)
    {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Lock rendering to logical resolution (auto-scales with letterboxing on window resize)
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Hide system cursor to use custom crosshair
    SDL_ShowCursor(SDL_DISABLE);

    const float playerSize = PLAYER_DEFAULT_SIZE;
    const float playerSpeed = PLAYER_DEFAULT_SPEED;

    SDL_Texture *playerTex = CreateCircleTexture(renderer, PLAYER_TEXTURE_RADIUS);

    Player player(
        (SCREEN_WIDTH - playerSize) / 2.0f,
        (SCREEN_HEIGHT - playerSize) / 2.0f,
        playerSize,
        playerSpeed,
        playerTex);

    Weapon weapon;
    weapon.createDefaultTexture(renderer);

    // Visual effect states
    std::vector<Particle> particles;
    MuzzleFlash muzzleFlash = {0.0f, 0.0f, 0.0f, MUZZLE_FLASH_DURATION, MUZZLE_FLASH_SIZE};
    float screenShake = 0.0f;
    float crosshairKickback = 0.0f;

    bool isRunning = true;
    bool isMouseDown = false;
    int mouseX = SCREEN_WIDTH / 2;
    int mouseY = SCREEN_HEIGHT / 2;
    SDL_Event event;

    Uint64 lastCounter = SDL_GetPerformanceCounter();
    const Uint64 perfFreq = SDL_GetPerformanceFrequency();

    while (isRunning)
    {
        // Event processing
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
            else if (event.type == SDL_MOUSEMOTION)
            {
                // In logical size mode, motion coordinates map directly to logical space
                mouseX = event.motion.x;
                mouseY = event.motion.y;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    mouseX = event.button.x;
                    mouseY = event.button.y;
                    isMouseDown = true;
                }
            }
            else if (event.type == SDL_MOUSEBUTTONUP)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    isMouseDown = false;
                }
            }
        }

        // Calculate delta time
        Uint64 currentCounter = SDL_GetPerformanceCounter();
        float deltaTime = static_cast<float>(currentCounter - lastCounter) / static_cast<float>(perfFreq);
        lastCounter = currentCounter;

        if (deltaTime > MAX_DELTA_TIME)
        {
            deltaTime = MAX_DELTA_TIME;
        }

        // Handle continuous shooting
        if (isMouseDown)
        {
            if (weapon.shoot())
            {
                // Visual & physical juice on shoot
                screenShake = SCREEN_SHAKE_INTENSITY;
                crosshairKickback = 1.0f;

                float rad = weapon.getAngle() * (PI / 180.0f);
                float dirX = std::cos(rad);
                float dirY = std::sin(rad);

                float playerCenterX = player.getX() + player.getSize() / 2.0f;
                float playerCenterY = player.getY() + player.getSize() / 2.0f;
                float muzzleX = playerCenterX + dirX * WEAPON_SPRITE_WIDTH;
                float muzzleY = playerCenterY + dirY * WEAPON_SPRITE_WIDTH;

                // Trigger muzzle flash
                muzzleFlash.x = muzzleX;
                muzzleFlash.y = muzzleY;
                muzzleFlash.timer = muzzleFlash.duration;

                // Spawn muzzle sparks
                SpawnSparks(particles, muzzleX, muzzleY, dirX, dirY, MUZZLE_SPARKS_COUNT, 255, 210, 80);
            }
        }

        // Input and physics
        const Uint8 *keystate = SDL_GetKeyboardState(nullptr);
        player.handleInput(keystate);
        player.update(deltaTime);

        float playerCenterX = player.getX() + player.getSize() / 2.0f;
        float playerCenterY = player.getY() + player.getSize() / 2.0f;
        weapon.update(deltaTime, playerCenterX, playerCenterY, mouseX, mouseY);

        // Spawn movement dust particles behind player
        static float dustTimer = 0.0f;
        dustTimer += deltaTime;
        if (dustTimer >= DUST_SPAWN_INTERVAL)
        {
            dustTimer = 0.0f;
            if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_A] ||
                keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_D])
            {
                Particle p;
                p.x = playerCenterX + ((rand() % 16) - 8);
                p.y = playerCenterY + ((rand() % 16) - 8);
                p.vx = ((rand() % 40) - 20) * 0.5f;
                p.vy = ((rand() % 40) - 20) * 0.5f;
                p.life = 0.0f;
                p.maxLife = 0.25f;
                p.size = 2.5f;
                p.r = 0;
                p.g = 180;
                p.b = 240;
                p.a = 140;
                particles.push_back(p);
            }
        }

        // Update particles
        for (auto &p : particles)
        {
            p.x += p.vx * deltaTime;
            p.y += p.vy * deltaTime;
            p.life += deltaTime;
            float progress = p.life / p.maxLife;
            if (progress < 1.0f)
            {
                p.a = static_cast<Uint8>((1.0f - progress) * 200.0f);
            }
        }
        particles.erase(
            std::remove_if(particles.begin(), particles.end(), [](const Particle &p)
                           { return p.life >= p.maxLife; }),
            particles.end());

        // Update effect timers
        if (screenShake > 0.0f)
        {
            screenShake = std::max(0.0f, screenShake - SCREEN_SHAKE_DECAY * deltaTime);
        }
        if (crosshairKickback > 0.0f)
        {
            crosshairKickback = std::max(0.0f, crosshairKickback - CROSSHAIR_KICKBACK_DECAY * deltaTime);
        }
        if (muzzleFlash.timer > 0.0f)
        {
            muzzleFlash.timer -= deltaTime;
        }

        // Apply screen shake via logical viewport offset
        int shakeOffsetX = 0;
        int shakeOffsetY = 0;
        if (screenShake > 0.01f)
        {
            shakeOffsetX = static_cast<int>(((rand() % 100) / 50.0f - 1.0f) * screenShake);
            shakeOffsetY = static_cast<int>(((rand() % 100) / 50.0f - 1.0f) * screenShake);
        }

        SDL_Rect sceneViewport = {shakeOffsetX, shakeOffsetY, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderSetViewport(renderer, &sceneViewport);

        // Render Background
        SDL_SetRenderDrawColor(renderer, CLEAR_COLOR_R, CLEAR_COLOR_G, CLEAR_COLOR_B, CLEAR_COLOR_A);
        SDL_RenderClear(renderer);

        // Subtle background grid for motion depth
        SDL_SetRenderDrawColor(renderer, GRID_COLOR_R, GRID_COLOR_G, GRID_COLOR_B, GRID_COLOR_A);
        for (int gx = 0; gx < SCREEN_WIDTH; gx += GRID_CELL_SIZE)
        {
            SDL_RenderDrawLine(renderer, gx, 0, gx, SCREEN_HEIGHT);
        }
        for (int gy = 0; gy < SCREEN_HEIGHT; gy += GRID_CELL_SIZE)
        {
            SDL_RenderDrawLine(renderer, 0, gy, SCREEN_WIDTH, gy);
        }

        // Render particles
        for (const auto &p : particles)
        {
            SDL_SetRenderDrawColor(renderer, p.r, p.g, p.b, p.a);
            SDL_Rect pRect = {
                static_cast<int>(p.x - p.size / 2.0f),
                static_cast<int>(p.y - p.size / 2.0f),
                static_cast<int>(p.size),
                static_cast<int>(p.size)};
            SDL_RenderFillRect(renderer, &pRect);
        }

        // Render game objects
        weapon.renderBullets(renderer);
        player.render(renderer);
        weapon.render(renderer);

        // Render Muzzle Flash
        if (muzzleFlash.timer > 0.0f)
        {
            float flashScale = muzzleFlash.timer / muzzleFlash.duration;
            int flashSize = static_cast<int>(muzzleFlash.size * flashScale);
            SDL_Rect flashRect = {
                static_cast<int>(muzzleFlash.x - flashSize / 2),
                static_cast<int>(muzzleFlash.y - flashSize / 2),
                flashSize,
                flashSize};
            SDL_SetRenderDrawColor(renderer, 255, 235, 120, 220);
            SDL_RenderFillRect(renderer, &flashRect);
        }

        // Reset viewport for UI overlays (crosshair and HUD remain stable without shake)
        SDL_RenderSetViewport(renderer, nullptr);

        // Render UI & Effects
        RenderHUD(renderer, weapon);
        RenderCrosshair(renderer, mouseX, mouseY, crosshairKickback);

        SDL_RenderPresent(renderer);
    }

    // Cleanup
    SDL_DestroyTexture(playerTex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
