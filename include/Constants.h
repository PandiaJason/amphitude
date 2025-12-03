#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SDL2/SDL.h>

/**
 * @namespace GameConstants
 * @brief Contains all the global configuration values for the game.
 * 
 * Using a namespace prevents naming collisions and keeps global constants organized.
 * These values control physics, rendering, gameplay mechanics, and balancing.
 */
namespace GameConstants {
    // ==========================================
    // Physics & Movement
    // ==========================================
    /** @brief Downward acceleration applied every frame (pixels/frame^2). */
    const float GRAVITY = 0.6f;
    
    /** @brief Velocity multiplier for ground friction (0.0 - 1.0). Lower = slippery. */
    const float FRICTION = 0.85f;
    
    /** @brief Initial upward velocity when jumping (pixels/frame). */
    const float JUMP_STRENGTH = 12.0f;
    
    /** @brief Horizontal speed added per frame when moving (pixels/frame^2). */
    const float MOVE_ACCELERATION = 1.2f;
    
    /** @brief Maximum horizontal speed cap (pixels/frame). */
    const float MAX_VELOCITY_X = 8.0f;

    // ==========================================
    // Window & Rendering
    // ==========================================
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 520;
    
    /** @brief Target frames per second. */
    const int TARGET_FPS = 60;
    
    /** @brief Milliseconds per frame (1000 / 60 = ~16ms). Used for frame capping. */
    const int FRAME_DELAY = 1000 / TARGET_FPS;

    // ==========================================
    // Combat & Gameplay
    // ==========================================
    const float PROJECTILE_DAMAGE = 15.0f;
    const float COLLISION_DAMAGE = 5.0f;
    
    /** @brief Damage taken when falling off the bottom of the screen. */
    const float FALL_DAMAGE = 20.0f;
    
    const float HEALTH_PICKUP = 30.0f;
    const float PROJECTILE_SPEED = 8.0f;
    const float SPEED_BOOST = 20.0f;
    
    /** @brief Horizontal force applied when players collide. */
    const float KNOCKBACK_FORCE = 10.0f;

    // ==========================================
    // Timers (in frames)
    // ==========================================
    const float GAME_DURATION = 180.0f; // Seconds
    const int FIRE_COOLDOWN = 30;
    const int SPEED_COOLDOWN = 60;
    
    /** @brief Duration of invincibility after taking damage (300 frames = 5 seconds at 60FPS). */
    const int INVINCIBILITY_DURATION = 60; // Frames
    const int POWER_DURATION = 1800; // 30 seconds at 60 FPS
    
    /** @brief Lifetime of a power-up crate on screen (frames). */
    const int POWER_UP_LIFETIME = 300; // 5 seconds
    
    /** @brief Interval between power-up spawns (milliseconds). */
    const int POWER_UP_SPAWN_INTERVAL = 10000; // 10 seconds
    const float RHINO_SPEED_MULTIPLIER = 1.5f;
    const float RHINO_DAMAGE_MULTIPLIER = 2.0f;

    /** @brief Time between power-up spawns in milliseconds. */

    
    const int MAX_POWER_UPS = 3;

    // ==========================================
    // Visual Effects
    // ==========================================
    const int PARTICLE_LIFETIME = 30;
    const int HIT_PARTICLE_COUNT = 20;
    const int COLLECT_PARTICLE_COUNT = 15;
}

#endif // CONSTANTS_H
