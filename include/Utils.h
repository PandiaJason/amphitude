#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL.h>
#include <vector>
#include "Structs.h"

/**
 * @brief Checks for Axis-Aligned Bounding Box (AABB) collision between two rectangles.
 * 
 * @param x1 Top-left X of rect 1
 * @param y1 Top-left Y of rect 1
 * @param w1 Width of rect 1
 * @param h1 Height of rect 1
 * @param x2 Top-left X of rect 2
 * @param y2 Top-left Y of rect 2
 * @param w2 Width of rect 2
 * @param h2 Height of rect 2
 * @return true if the rectangles overlap, false otherwise.
 */
bool checkCollision(float x1, float y1, float w1, float h1,
                   float x2, float y2, float w2, float h2);

/**
 * @brief Clamps a value between a minimum and maximum.
 * 
 * Useful for keeping players on screen or limiting speed.
 * 
 * @param value The value to clamp
 * @param min The minimum allowed value
 * @param max The maximum allowed value
 * @return The clamped value
 */
float clamp(float value, float min, float max);

/**
 * @brief Spawns a burst of particles at a location.
 * 
 * @param particles Reference to the particle list to add to.
 * @param x X position of the burst center.
 * @param y Y position of the burst center.
 * @param color Color of the particles.
 * @param count Number of particles to spawn.
 */
void createParticles(std::vector<Particle>& particles, float x, float y,
                    SDL_Color color, int count);

/**
 * @brief Draws a filled rectangle using SDL.
 * 
 * @param renderer The SDL renderer.
 * @param x Top-left X.
 * @param y Top-left Y.
 * @param w Width.
 * @param h Height.
 * @param color Fill color.
 * @param blend If true, enables alpha blending (transparency).
 */
void drawRect(SDL_Renderer* renderer, float x, float y, float w, float h,
             SDL_Color color, bool blend = false);

/**
 * @brief Draws a hollow circle using the Midpoint Circle Algorithm.
 * 
 * @param renderer The SDL renderer.
 * @param cx Center X.
 * @param cy Center Y.
 * @param radius Radius in pixels.
 * @param color Line color.
 */
void drawCircle(SDL_Renderer* renderer, int cx, int cy, int radius, SDL_Color color);

/**
 * @brief Spawns a random power-up on a random platform.
 * 
 * @param powerUps Reference to the power-up list.
 * @param platforms Reference to the platform list (to choose a location).
 */
void spawnPowerUp(std::vector<PowerUp>& powerUps,
                 const std::vector<Platform>& platforms);

#endif // UTILS_H
