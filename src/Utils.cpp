#include "Utils.h"
#include "Constants.h"
#include <cstdlib>
#include <cmath>

// Implementation of collision detection
bool checkCollision(float x1, float y1, float w1, float h1,
                   float x2, float y2, float w2, float h2) {
    // AABB Collision Rule:
    // Rect1 is to the left of Rect2 OR
    // Rect1 is to the right of Rect2 OR
    // Rect1 is above Rect2 OR
    // Rect1 is below Rect2
    // If ANY of these are true, there is NO collision.
    // We return the negation (collision exists).
    return x1 < x2 + w2 && x1 + w1 > x2 &&
           y1 < y2 + h2 && y1 + h1 > y2;
}

float clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

void createParticles(std::vector<Particle>& particles, float x, float y,
                    SDL_Color color, int count) {
    for (int i = 0; i < count; ++i) {
        particles.push_back({
            x, y,
            // Random velocity between -4 and 4
            (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 8,
            (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 8,
            GameConstants::PARTICLE_LIFETIME,
            color
        });
    }
}

void drawRect(SDL_Renderer* renderer, float x, float y, float w, float h,
             SDL_Color color, bool blend) {
    if (blend) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    }
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect rect = {static_cast<int>(x), static_cast<int>(y),
                    static_cast<int>(w), static_cast<int>(h)};
    SDL_RenderFillRect(renderer, &rect);
    if (blend) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
}

void drawCircle(SDL_Renderer* renderer, int cx, int cy, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    // Midpoint Circle Algorithm
    // Efficiently draws a circle using integer arithmetic
    int x = radius;
    int y = 0;
    int err = 0;

    for (; x >= y; ) {
        // Draw 8 octants
        SDL_RenderDrawLine(renderer, cx - x, cy + y, cx + x, cy + y);
        SDL_RenderDrawLine(renderer, cx - x, cy - y, cx + x, cy - y);
        SDL_RenderDrawLine(renderer, cx - y, cy + x, cx + y, cy + x);
        SDL_RenderDrawLine(renderer, cx - y, cy - x, cx + y, cy - x);

        y++;
        if (err <= 0) {
            err += 2 * y + 1;
        }
        if (err > 0) {
            x--;
            err -= 2 * x + 1;
        }
    }
}

void spawnPowerUp(std::vector<PowerUp>& powerUps,
                 const std::vector<Platform>& platforms) {
    if (powerUps.size() >= GameConstants::MAX_POWER_UPS) return;

    std::vector<std::string> types = {"fire", "shield", "health"};
    std::string type = types[rand() % types.size()];
    
    // Pick a random platform to spawn on
    const Platform& platform = platforms[rand() % platforms.size()];

    powerUps.push_back({
        // Random X position on the platform
        platform.x + (static_cast<float>(rand()) / RAND_MAX) * (platform.width - 30),
        platform.y - 30, // Just above the platform
        25, 25,
        type,
        0,
        GameConstants::POWER_UP_LIFETIME
    });
}
