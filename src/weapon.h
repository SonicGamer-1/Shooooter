#ifndef WEAPON_H
#define WEAPON_H

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include "consts.h"

// Structure representing an active projectile/bullet fired from a weapon
struct Bullet
{
    float x;
    float y;
    float vx;
    float vy;
    float distanceTraveled;
    float maxRange;
    float size;
    bool active;
};

// Base class for all weapons (inheritable)
class Weapon
{
public:
    Weapon(
        const std::string &name = WEAPON_DEFAULT_NAME,
        float reloadSpeed = WEAPON_DEFAULT_RELOAD_SPEED,
        float range = WEAPON_DEFAULT_RANGE,
        float bulletSpeed = WEAPON_DEFAULT_BULLET_SPEED,
        int magSize = WEAPON_DEFAULT_MAG_SIZE,
        float fireRate = WEAPON_DEFAULT_FIRE_RATE,
        float width = WEAPON_SPRITE_WIDTH,
        float height = WEAPON_SPRITE_HEIGHT,
        SDL_Texture *texture = nullptr // Weapon sprite texture
    );

    virtual ~Weapon() = default;

    // Updates weapon position, aims sprite at mouse pointer, and updates active bullets
    virtual void update(float deltaTime, float ownerCenterX, float ownerCenterY, int mouseX, int mouseY);

    // Attempts to shoot. Returns true if bullet was spawned, false otherwise (e.g. empty mag or cooling down)
    virtual bool shoot();

    // Initiates weapon reloading
    virtual void reload();

    // Renders the weapon sprite rotated towards the pointer
    virtual void render(SDL_Renderer *renderer) const;

    // Renders all active bullets fired by this weapon
    virtual void renderBullets(SDL_Renderer *renderer) const;

    // Generates a default procedural sprite if no custom image texture is provided
    virtual void createDefaultTexture(SDL_Renderer *renderer);

    // Getters
    const std::string &getName() const { return name; }
    int getCurrentAmmo() const { return currentAmmo; }
    int getMagSize() const { return magSize; }
    float getReloadSpeed() const { return reloadSpeed; }
    float getRange() const { return range; }
    float getBulletSpeed() const { return bulletSpeed; }
    float getAngle() const { return angle; }
    bool isReloadingState() const { return isReloading; }
    float getReloadProgress() const { return isReloading ? (reloadTimer / reloadSpeed) : 1.0f; }
    const std::vector<Bullet> &getBullets() const { return bullets; }

    // Setters
    void setTexture(SDL_Texture *tex) { texture = tex; }
    void setStats(float newReloadSpeed, float newRange, float newBulletSpeed, int newMagSize, float newFireRate);

protected:
    // Core weapon stats
    std::string name;
    float reloadSpeed;  // In seconds
    float range;        // In pixels
    float bulletSpeed;  // In pixels/sec
    int magSize;        // Max magazine capacity
    int currentAmmo;    // Current bullets in magazine
    float fireRate;     // Cooldown between shots in seconds

    // Timers and states
    float fireCooldown; // Remaining cooldown before next shot
    float reloadTimer;  // Elapsed reload time
    bool isReloading;   // Whether reload is in progress

    // Transform and aiming
    float x;            // World pivot X position
    float y;            // World pivot Y position
    float angle;        // Aim angle in degrees
    float dirX;         // Normalized aim direction X
    float dirY;         // Normalized aim direction Y
    float width;        // Weapon sprite width
    float height;       // Weapon sprite height
    float barrelOffset; // Distance from pivot to muzzle/barrel tip

    // Visuals & Projectiles
    SDL_Texture *texture;
    bool ownsTexture;
    std::vector<Bullet> bullets;

    // Internal helper to spawn a projectile
    virtual void spawnBullet(float startX, float startY, float dirX, float dirY);
};

#endif // WEAPON_H
