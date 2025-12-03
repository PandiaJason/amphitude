#include "Player.h"
#include "Constants.h"
#include "Utils.h"
#include <cmath>
#include <algorithm>

Player::Player() : id(0), x(0), y(0), vx(0), vy(0), width(0), height(0), hp(100), maxHp(100),
           onGround(false), facing(1), powerTimer(0), invincible(0),
           keyLeft(false), keyRight(false), keyJump(false), keyAttack(false),
           texture(nullptr), normalTexture(nullptr), dragonTexture(nullptr), rhinoTexture(nullptr) {}

void Player::init(int id, float x, float y, SDL_Color color, std::string name, 
                  SDL_Texture* normalTex, SDL_Texture* dragonTex, SDL_Texture* rhinoTex,
                  int fw, int fh, int cols, int rows) {
    this->id = id;
    this->x = x;
    this->y = y;
    width = 40;
    height = 50;
    this->color = color;
    this->name = name;
    this->normalTexture = normalTex;
    this->dragonTexture = dragonTex;
    this->rhinoTexture = rhinoTex;
    this->texture = normalTex; // Start with normal
    frameWidth = fw;
    frameHeight = fh;
    totalColumns = cols;
    totalRows = rows;
    
    vx = 0; vy = 0;
    hp = 100;
    onGround = false;
    facing = (id == 1) ? 1 : -1; // P1 faces right, P2 faces left
    power = "";
    powerTimer = 0;
    invincible = 0;
    keyLeft = false; keyRight = false; keyJump = false; keyAttack = false;
    currentFrame = 0; frameTimer = 0; animRow = 0; attackCooldown = 0;
}

void Player::takeDamage(float damage, std::vector<Particle>& particles) {
    if (invincible > 0) return; // Ignore damage if invincible
    
    // Mario-Style Logic: Lose power-up instead of HP
    if (power != "" && power != "health") {
        power = ""; // Lose the power
        texture = normalTexture; // Revert to normal sprite (e.g. from Dragon)
        
        // Visual feedback
        createParticles(particles, x + width/2, y + height/2, {0, 191, 255, 255}, 15); // Blue/Magic particles
        invincible = 60; // Brief invincibility (1 second)
        return; // NO DAMAGE TAKEN
    }

    hp -= damage;
    createHitParticles(particles);
}

void Player::createHitParticles(std::vector<Particle>& particles) {
    createParticles(particles, x + width/2, y + height/2,
                   {255, 0, 0, 255}, GameConstants::HIT_PARTICLE_COUNT);
}

void Player::update(const std::vector<Platform>& platforms,
            std::vector<Projectile>& projectiles,
            std::vector<Particle>& particles) {
    // ============================================================
    // 1. Input Handling
    // ============================================================
    if (keyLeft) {
        vx -= GameConstants::MOVE_ACCELERATION;
        facing = -1;
    }
    if (keyRight) {
        vx += GameConstants::MOVE_ACCELERATION;
        facing = 1;
    }

    // ============================================================
    // 2. Animation State Logic
    // ============================================================
    int row = 0; // Default to run (row 0)
    int framesInRow = 6; // Default frames for run/jump

    if (!onGround) {
        row = 1; // Jump (row 1)
        framesInRow = 6;
    } else if (std::abs(vx) < 0.1f) {
        row = 2; // Idle (row 2)
        framesInRow = 1; // Stand simply (1 frame)
    }

    // Switch animation row if state changed
    if (animRow != row) {
        animRow = row;
        currentFrame = 0;
        frameTimer = 0;
    }

    // Advance animation frame
    // Advance animation frame
    if (power == "fire" && !onGround) {
        currentFrame = 1; // Static flight frame
        frameTimer = 0;
    } else {
        frameTimer++;
        if (frameTimer >= 10) { // Change frame every 10 ticks (slows down animation)
            currentFrame = (currentFrame + 1) % framesInRow;
            frameTimer = 0;
        }
    }

    // ============================================================
    // 3. Physics (Velocity)
    // ============================================================
    // Clamp horizontal speed

    float maxSpeed = (power == "shield") ? GameConstants::MAX_VELOCITY_X * GameConstants::RHINO_SPEED_MULTIPLIER : GameConstants::MAX_VELOCITY_X;
    vx = clamp(vx, -maxSpeed, maxSpeed);
    // Apply friction (slow down when no input)
    vx *= GameConstants::FRICTION;

    // Jumping / Flying
    if (keyJump) {
        if (power == "fire") {
            // Dragon Flight: Levitate upwards against gravity
            vy = -5.0f; // Constant upward speed
            onGround = false;
        } else if (onGround) {
            // Normal Jump
            vy = -GameConstants::JUMP_STRENGTH;
            onGround = false;
        }
    }

    // ============================================================
    // 4. Combat & Abilities
    // ============================================================
    // Texture Switching based on Power-up
    if (power == "fire") {
        texture = dragonTexture;
    } else if (power == "shield") {
        texture = rhinoTexture;
    } else {
        texture = normalTexture;
    }

    if (keyAttack && attackCooldown <= 0) {
        if (power == "fire") {
            projectiles.push_back({
                x + (facing > 0 ? width : 0),
                y + height / 2,
                facing * GameConstants::PROJECTILE_SPEED,
                0,
                15, 15,
                id - 1, // Owner ID (0 or 1)
                "fire"
            });
            attackCooldown = GameConstants::FIRE_COOLDOWN;
        }
        else if (power == "shield") {
            vx = facing * GameConstants::SPEED_BOOST; // Charge
            createParticles(particles, x + width/2, y + height/2, color, 10);
            attackCooldown = GameConstants::SPEED_COOLDOWN;
        }
        else {
            // Basic Attack (Lunge)
            vx = facing * 3.0f;
            attackCooldown = 30; // 0.5s cooldown
        }
    }

    // Update timers
    if (powerTimer > 0) {
        powerTimer--;
        if (powerTimer <= 0) {
            power = "";
            texture = normalTexture;
        }
    }
    if (attackCooldown > 0) attackCooldown--;
    if (invincible > 0) invincible--;

    // ============================================================
    // 5. Physics Integration & Collision
    // ============================================================
    if (power == "fire" && !onGround && !keyJump && !keyDown) {
        // Gliding logic: Reduced gravity and terminal velocity
        vy += GameConstants::GRAVITY * 0.1f;
        if (vy > 2.0f) vy = 2.0f;
    } else {
        vy += GameConstants::GRAVITY;
    }

    // Move X Axis
    x += vx;
    // Check X Collision
    for (const auto& platform : platforms) {
        if (checkCollision(x, y, width, height, platform.x, platform.y, platform.width, platform.height)) {
            if (vx > 0) { // Moving right -> Hit left side of platform
                x = platform.x - width;
            } else if (vx < 0) { // Moving left -> Hit right side of platform
                x = platform.x + platform.width;
            }
            vx = 0;
        }
    }

    // Move Y Axis
    y += vy;
    onGround = false;
    // Check Y Collision
    for (const auto& platform : platforms) {
        if (checkCollision(x, y, width, height, platform.x, platform.y, platform.width, platform.height)) {
            if (vy > 0) { // Falling down -> Land on top
                y = platform.y - height;
                onGround = true;
                vy = 0;
            } else if (vy < 0) { // Jumping up -> Hit bottom
                y = platform.y + platform.height;
                vy = 0;
            }
        }
    }

    // Screen bounds
    x = clamp(x, 0, GameConstants::WINDOW_WIDTH - width);
    // Prevent flying off the top
    if (y < 0) {
        y = 0;
        vy = 0; // Stop upward velocity
    }

    // Fall damage (falling off screen)
    if (y > GameConstants::WINDOW_HEIGHT) {
        takeDamage(GameConstants::FALL_DAMAGE, particles);
        // Respawn
        y = 100;
        x = (id == 1) ? 100 : 700;
        vy = 0;
    }
}

void Player::render(SDL_Renderer* renderer) const {
    SDL_Color playerColor = color;

    // Invincibility flash effect (flicker alpha)
    if (invincible > 0 && (invincible / 5) % 2 == 0) {
        playerColor.a = 128;
        SDL_SetTextureAlphaMod(texture, 128);
    } else {
        SDL_SetTextureAlphaMod(texture, 255);
    }

    if (texture) {
        // Apply color tint (e.g., for Player 2)
        SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
        
        // Select the correct frame from the sprite sheet
        SDL_Rect srcRect = {
            currentFrame * frameWidth,
            animRow * frameHeight,
            frameWidth,
            frameHeight
        };
        // Destination on screen
        SDL_Rect dstRect = {
            static_cast<int>(x), 
            static_cast<int>(y),
            static_cast<int>(width), 
            static_cast<int>(height)
        };
        
        // Flip sprite if facing left
        SDL_RendererFlip flip = (facing == -1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        SDL_RenderCopyEx(renderer, texture, &srcRect, &dstRect, 0, NULL, flip);
    } else {
        // Fallback rectangle if texture fails
        drawRect(renderer, x, y, width, height, playerColor, invincible > 0);
    }

    // Power-up indicator bar above head
    if (power != "" && power != "health") {
        SDL_Color powerColor;
        if (power == "fire") powerColor = {255, 69, 0, 255};
        else if (power == "shield") powerColor = {0, 191, 255, 255};

        drawRect(renderer, x, y - 10, width, 5, powerColor);
    }
}
