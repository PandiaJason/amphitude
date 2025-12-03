#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>
#include <map>
#include "Player.h"
#include "Structs.h"
#include "NetworkManager.h"

/**
 * @class Game
 * @brief The main engine class that manages the game loop and state.
 * 
 * Responsibilities:
 * - Initializing SDL and loading assets.
 * - Handling user input (keyboard).
 * - Updating game logic (physics, collisions, AI).
 * - Rendering the frame.
 * - Managing game states (Menu, Playing, GameOver).
 */
class Game {
public:
    /** @brief Game states */
    enum GameState { MENU, CHARACTER_SELECT, NAME_INPUT, PLAYING, PAUSED, GAMEOVER };

    Game();
    ~Game();

    /**
     * @brief Initializes SDL, creates window/renderer, and loads assets.
     * @return true if successful, false otherwise.
     */
    bool init();

    /**
     * @brief Starts the main game loop.
     * 
     * This loop runs until the user closes the window.
     * It enforces a fixed frame rate (60 FPS).
     */
    void run();

private:
    // SDL Core
    SDL_Window* window;
    SDL_Renderer* renderer;
    
    // Resources
    TTF_Font* font;
    TTF_Font* titleFont;
    SDL_Texture* boyTexture;
    SDL_Texture* girlTexture;
    SDL_Texture* boyDragonTexture;
    SDL_Texture* girlDragonTexture;
    SDL_Texture* boyRhinoTexture;
    SDL_Texture* girlRhinoTexture;
    SDL_Texture* backgroundTexture;
    SDL_Texture* mudTileTexture;
    SDL_Texture* crystalTexture;
    int boyW = 0, boyH = 0;
    int girlW = 0, girlH = 0;

    // Game State
    GameState currentState;
    bool running;
    
    // Network
    NetworkManager net;
    bool isOnline = false;
    bool connectionFailed = false;
    std::string ipInput = "127.0.0.1";
    int p1Character; // 0 = Boy, 1 = Girl
    int p2Character; // 0 = Boy, 1 = Girl
    bool p1Ready = false;
    bool p2Ready = false;
    std::string p1NameInput = "Player 1";
    std::string p2NameInput = "Player 2";
    bool typingName = false; // To toggle typing mode
    
    // Name Input
    std::string inputText;
    bool inputtingP1;


    // Timer
    float gameTime;
    float pauseTime;

    Uint32 lastPowerUpTime;

    // Game Objects
    std::vector<Player> players;
    std::vector<Platform> platforms;
    std::vector<Projectile> projectiles;
    std::vector<PowerUp> powerUps;
    std::vector<Particle> particles;
    std::map<std::string, SDL_Texture*> textCache;

    // Internal Methods
    void loadAssets();
    void initGameObjects();
    void resetGame();
    void spawnPowerUps();
    void handleEvents(SDL_Event& event);
    void update();
    void render();
    
    /**
     * @brief Helper to render text to the screen.
     */
    /**
     * @brief Helper to render text to the screen.
     */
    void renderText(int x, int y, const std::string& text, SDL_Color color, TTF_Font* f);

    /**
     * @brief Helper to render centered text.
     */
    void renderCenteredText(int y, const std::string& text, SDL_Color color, TTF_Font* f);
    
    /**
     * @brief Frees all SDL resources.
     */
    void cleanup();
};

#endif // GAME_H
