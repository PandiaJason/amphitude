#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include "Structs.h"

/**
 * @class Player
 * @brief Represents a playable character in the game.
 * 
 * Handles movement physics, collision detection, animation state,
 * combat logic (health, damage), and rendering.
 */
class Player {
public:
    int id;             ///< Player ID (1 or 2)
    float x, y;         ///< Position
    float vx, vy;       ///< Velocity
    float width, height;
    SDL_Color color;    ///< Tint color for the sprite
    std::string name;   ///< Display name
    float hp, maxHp;    ///< Health points
    bool onGround;      ///< True if standing on a platform
    int facing;         ///< 1 for Right, -1 for Left
    
    // Power-ups
    std::string power;  ///< Current active power-up type
    int powerTimer;     ///< Duration remaining for power-up
    int invincible;     ///< Invincibility frames remaining
    
    // Input State
    bool keyLeft, keyRight, keyJump, keyAttack, keyDown;
    
    // Animation
    SDL_Texture* texture; ///< Current active texture
    SDL_Texture* normalTexture; ///< Standard sprite sheet
    SDL_Texture* dragonTexture; ///< Dragon sprite sheet
    SDL_Texture* rhinoTexture;  ///< Rhino sprite sheet
    int currentFrame = 0; ///< Current animation frame index
    int frameTimer = 0;   ///< Timer to control animation speed
    int attackCooldown = 0; ///< Timer for attack cooldown
    int numFrames = 3;    ///< Total frames in current animation
    int frameWidth = 0;   ///< Width of a single sprite frame
    int frameHeight = 0;  ///< Height of a single sprite frame
    int animRow = 0;      ///< Current row in sprite sheet (0=Run, 1=Jump, 2=Idle)
    int totalColumns = 1;
    int totalRows = 1;

    Player();

    /**
     * @brief Initializes the player with starting values.
     * 
     * @param id Player ID
     * @param x Start X
     * @param y Start Y
     * @param color Tint color
     * @param name Name
     * @param normalTex Standard sprite texture
     * @param dragonTex Dragon sprite texture
     * @param rhinoTex Rhino sprite texture
     * @param fw Frame width
     * @param fh Frame height
     * @param frames Total frames in animation
     * @param speed Animation speed
     */
    void init(int id, float x, float y, SDL_Color color, std::string name, 
              SDL_Texture* normalTex, SDL_Texture* dragonTex, SDL_Texture* rhinoTex,
              int fw, int fh, int frames, int speed);

    /**
     * @brief Applies damage to the player.
     * 
     * Handles invincibility frames and shield power-up logic.
     * 
     * @param damage Amount of damage to take.
     * @param particles Reference to particle list for hit effects.
     */
    void takeDamage(float damage, std::vector<Particle>& particles);

    /**
     * @brief Spawns particles when hit.
     */
    void createHitParticles(std::vector<Particle>& particles);

    /**
     * @brief Updates player state for the current frame.
     * 
     * Handles input processing, physics (gravity, velocity), collision detection,
     * animation updates, and power-up timers.
     * 
     * @param platforms List of platforms for collision.
     * @param projectiles List of projectiles (to fire).
     * @param particles List of particles (for effects).
     */
    void update(const std::vector<Platform>& platforms,
                std::vector<Projectile>& projectiles,
                std::vector<Particle>& particles);

    /**
     * @brief Renders the player to the screen.
     * 
     * Draws the correct sprite frame based on animation state and facing direction.
     * Also draws the health bar and power-up indicators.
     * 
     * @param renderer SDL Renderer.
     */
    void render(SDL_Renderer* renderer) const;
};

#endif // PLAYER_H
