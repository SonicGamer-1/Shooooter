#include "weapon.h"
#include "consts.h"
#include <cmath>
#include <algorithm>

Weapon::Weapon(
    const std::string &name,
    float reloadSpeed,
    float range,
    float bulletSpeed,
    int magSize,
    float fireRate,
    float width,
    float height,
    SDL_Texture *texture
)
    : name(name),
      reloadSpeed(reloadSpeed),
      range(range),
      bulletSpeed(bulletSpeed),
      magSize(magSize),
      currentAmmo(magSize),
      fireRate(fireRate),
      fireCooldown(0.0f),
      reloadTimer(0.0f),
      isReloading(false),
      x(0.0f),
      y(0.0f),
      angle(0.0f),
      dirX(1.0f),
      dirY(0.0f),
      width(width),
      height(height),
      barrelOffset(width),
      texture(texture),
      ownsTexture(false)
{
}

void Weapon::setStats(float newReloadSpeed, float newRange, float newBulletSpeed, int newMagSize, float newFireRate)
{
    reloadSpeed = newReloadSpeed;
    range = newRange;
    bulletSpeed = newBulletSpeed;
    magSize = newMagSize;
    fireRate = newFireRate;
    currentAmmo = std::min(currentAmmo, magSize);
}

void Weapon::update(float deltaTime, float ownerCenterX, float ownerCenterY, int mouseX, int mouseY)
{
    // Update pivot origin to owner's center
    x = ownerCenterX;
    y = ownerCenterY;

    // Aim calculation towards mouse pointer
    float dx = static_cast<float>(mouseX) - x;
    float dy = static_cast<float>(mouseY) - y;
    float len = std::sqrt(dx * dx + dy * dy);

    if (len > 0.001f)
    {
        dirX = dx / len;
        dirY = dy / len;
        angle = std::atan2(dy, dx) * RAD_TO_DEG;
    }

    // Cooldown timer
    if (fireCooldown > 0.0f)
    {
        fireCooldown -= deltaTime;
    }

    // Reload timer
    if (isReloading)
    {
        reloadTimer += deltaTime;
        if (reloadTimer >= reloadSpeed)
        {
            currentAmmo = magSize;
            isReloading = false;
            reloadTimer = 0.0f;
        }
    }

    // Update active bullets
    for (auto &b : bullets)
    {
        if (!b.active)
            continue;

        float stepX = b.vx * deltaTime;
        float stepY = b.vy * deltaTime;
        b.x += stepX;
        b.y += stepY;
        b.distanceTraveled += std::sqrt(stepX * stepX + stepY * stepY);

        if (b.distanceTraveled >= b.maxRange)
        {
            b.active = false;
        }
    }

    // Remove inactive bullets
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(), [](const Bullet &b) {
            return !b.active;
        }),
        bullets.end()
    );
}

bool Weapon::shoot()
{
    if (isReloading)
    {
        return false;
    }

    if (fireCooldown > 0.0f)
    {
        return false;
    }

    if (currentAmmo <= 0)
    {
        reload();
        return false;
    }

    // Spawn bullet from muzzle end
    float muzzleX = x + dirX * barrelOffset;
    float muzzleY = y + dirY * barrelOffset;
    spawnBullet(muzzleX, muzzleY, dirX, dirY);

    currentAmmo--;
    fireCooldown = fireRate;

    if (currentAmmo <= 0)
    {
        reload();
    }

    return true;
}

void Weapon::spawnBullet(float startX, float startY, float dX, float dY)
{
    Bullet b;
    b.x = startX;
    b.y = startY;
    b.vx = dX * bulletSpeed;
    b.vy = dY * bulletSpeed;
    b.distanceTraveled = 0.0f;
    b.maxRange = range;
    b.size = BULLET_DEFAULT_SIZE;
    b.active = true;

    bullets.push_back(b);
}

void Weapon::reload()
{
    if (isReloading || currentAmmo == magSize)
    {
        return;
    }

    isReloading = true;
    reloadTimer = 0.0f;
}

void Weapon::createDefaultTexture(SDL_Renderer *renderer)
{
    if (texture != nullptr && ownsTexture)
    {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    int texW = static_cast<int>(width);
    int texH = static_cast<int>(height);
    if (texW <= 0) texW = 32;
    if (texH <= 0) texH = 12;

    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, texW, texH, 32, SDL_PIXELFORMAT_RGBA32);
    if (!surface)
        return;

    // Transparent background
    SDL_FillRect(surface, nullptr, SDL_MapRGBA(surface->format, 0, 0, 0, 0));

    // Gun body (dark metallic grey)
    SDL_Rect bodyRect = { 0, texH / 4, texW * 3 / 4, texH / 2 };
    SDL_FillRect(surface, &bodyRect, SDL_MapRGBA(surface->format, 70, 75, 85, 255));

    // Barrel extension
    SDL_Rect barrelRect = { texW * 3 / 4, texH * 3 / 8, texW / 4, texH / 4 };
    SDL_FillRect(surface, &barrelRect, SDL_MapRGBA(surface->format, 45, 50, 60, 255));

    // Handle/Grip
    SDL_Rect gripRect = { 2, texH / 2, texW / 5, texH / 2 };
    SDL_FillRect(surface, &gripRect, SDL_MapRGBA(surface->format, 110, 65, 40, 255));

    // Highlight top rail
    SDL_Rect railRect = { 0, texH / 4, texW * 3 / 4, 2 };
    SDL_FillRect(surface, &railRect, SDL_MapRGBA(surface->format, 130, 140, 155, 255));

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (texture)
    {
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        ownsTexture = true;
    }
}

void Weapon::render(SDL_Renderer *renderer) const
{
    if (texture)
    {
        SDL_Rect dstRect = {
            static_cast<int>(x),
            static_cast<int>(y - height / 2.0f),
            static_cast<int>(width),
            static_cast<int>(height)
        };

        // Pivot around (0, height / 2) so the weapon rotates from the player center/grip
        SDL_Point pivot = { 0, static_cast<int>(height / 2.0f) };

        // Flip vertically if aiming left so the weapon doesn't render upside down
        SDL_RendererFlip flip = (angle > 90.0f || angle < -90.0f) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;

        SDL_RenderCopyEx(renderer, texture, nullptr, &dstRect, angle, &pivot, flip);
    }
    else
    {
        // Fallback procedural line rendering if no texture exists
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawLine(
            renderer,
            static_cast<int>(x),
            static_cast<int>(y),
            static_cast<int>(x + dirX * width),
            static_cast<int>(y + dirY * width)
        );
    }
}

void Weapon::renderBullets(SDL_Renderer *renderer) const
{
    // Draw all active bullets
    for (const auto &b : bullets)
    {
        if (!b.active)
            continue;

        SDL_Rect bulletRect = {
            static_cast<int>(b.x - b.size / 2.0f),
            static_cast<int>(b.y - b.size / 2.0f),
            static_cast<int>(b.size),
            static_cast<int>(b.size)
        };

        // Inner glowing bullet
        SDL_SetRenderDrawColor(renderer, BULLET_COLOR_R, BULLET_COLOR_G, BULLET_COLOR_B, BULLET_COLOR_A);
        SDL_RenderFillRect(renderer, &bulletRect);

        // Core highlight
        SDL_SetRenderDrawColor(renderer, BULLET_CORE_COLOR_R, BULLET_CORE_COLOR_G, BULLET_CORE_COLOR_B, BULLET_CORE_COLOR_A);
        SDL_RenderDrawPoint(renderer, static_cast<int>(b.x), static_cast<int>(b.y));
    }
}
