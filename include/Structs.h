#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>
#include <SDL2/SDL.h>

/**
 * @struct Platform
 * @brief Represents a static solid object in the game world.
 */
struct Platform {
    float x, y;       ///< Top-left position
    float width, height; ///< Dimensions
};

/**
 * @struct Projectile
 * @brief Represents a moving object fired by a player.
 */
struct Projectile {
    float x, y;       ///< Current position
    float vx, vy;     ///< Velocity vector
    float width, height;
    int owner;        ///< ID of the player who fired this (0 or 1)
    std::string type; ///< Type of projectile (e.g., "fire")
};

/**
 * @struct PowerUp
 * @brief Represents a collectible item that gives abilities.
 */
struct PowerUp {
    float x, y;
    float width, height;
    std::string type; ///< "fire", "speed", "health", "shield", "star"
    int bobTimer = 0; ///< Used for the floating animation
    int lifetime = 0; ///< Frames until it disappears
};

/**
 * @struct Particle
 * @brief Represents a simple visual effect (confetti, sparks).
 */
struct Particle {
    float x, y;
    float vx, vy;
    int life;         ///< Remaining frames before it disappears
    SDL_Color color;  ///< Color of the particle
    bool active;      ///< Is the particle currently active?

    void update() {
        x += vx;
        y += vy;
        life--;
        if (life <= 0) active = false;
    }
};

#endif // STRUCTS_H
