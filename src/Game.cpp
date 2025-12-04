#include "Game.h"
#include "Constants.h"
#include "Utils.h"
#include <iostream>
#include <cmath>
#include <SDL2/SDL_image.h>

Game::Game() : window(nullptr), renderer(nullptr), font(nullptr), titleFont(nullptr),
             boyTexture(nullptr), girlTexture(nullptr), boyDragonTexture(nullptr), girlDragonTexture(nullptr),
    boyRhinoTexture(nullptr), girlRhinoTexture(nullptr), backgroundTexture(nullptr),
             crystalTexture(nullptr),
             currentState(MENU), running(true), p1Character(0), p2Character(1), lastPowerUpTime(0),
             gameTime(GameConstants::GAME_DURATION), inputtingP1(true) {}

Game::~Game() {
    cleanup();
}

bool Game::init() {
    // Initialize SDL Video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return false;
    }
    // Initialize SDL_ttf for text rendering
    if (TTF_Init() < 0) {
        std::cerr << "TTF Init Error: " << TTF_GetError() << std::endl;
        return false;
    }
    // Initialize SDL_image for PNG loading
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG Init Error: " << IMG_GetError() << std::endl;
        return false;
    }

    // Create the game window
    window = SDL_CreateWindow("Amphitude",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        GameConstants::WINDOW_WIDTH, GameConstants::WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return false;

    // Create the renderer (hardware accelerated, vsync enabled)
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) return false;

    loadAssets();
    initGameObjects();

    // Initialize Network
    if (!net.init()) {
        std::cerr << "Failed to init network" << std::endl;
    }

    return true;
}

void Game::run() {
    SDL_Event event;
    // Main Game Loop using for(;;) as requested for industry standard flow
    for (; running; ) {
        Uint32 frameStart = SDL_GetTicks();
        // Network Handshake
        if (isOnline && !net.connected) {
            if (net.isHost) {
                net.acceptClient();
            }
        }

        handleEvents(event); // 1. Input
        update();            // 2. Logic
        render();            // 3. Draw
        
        // Frame Capping: Wait if the frame finished too quickly
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (GameConstants::FRAME_DELAY > frameTime) {
            SDL_Delay(GameConstants::FRAME_DELAY - frameTime);
        }
    }
}

void Game::loadAssets() {
    // Attempt to load fonts from common system paths
    const char* fontPaths[] = {
        "assets/PressStart2P-Regular.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        "/Library/Fonts/Arial.ttf",
        "C:\\Windows\\Fonts\\arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        nullptr
    };
    for (int i = 0; fontPaths[i] != nullptr && !font; ++i) font = TTF_OpenFont(fontPaths[i], 16); // Smaller size for retro font
    for (int i = 0; fontPaths[i] != nullptr && !titleFont; ++i) titleFont = TTF_OpenFont(fontPaths[i], 64); // Large Banner Font

    // Load Character Textures
    SDL_Surface* surf = IMG_Load("assets/Indian boy comp.png");
    if (surf) {
        boyTexture = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_QueryTexture(boyTexture, NULL, NULL, &boyW, &boyH);
        SDL_FreeSurface(surf);
    }
    surf = IMG_Load("assets/Indian girl comp.png");
    if (surf) {
        girlTexture = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_QueryTexture(girlTexture, NULL, NULL, &girlW, &girlH);
        SDL_FreeSurface(surf);
    }
    surf = IMG_Load("assets/Indian boy dragon.png");
    if (surf) {
        boyDragonTexture = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
    }
    surf = IMG_Load("assets/Indian girl dragon.png");
    if (surf) {
        girlDragonTexture = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
    }
    surf = IMG_Load("assets/Indian boy rhino.png");
    if (surf) {
        boyRhinoTexture = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
    }
    surf = IMG_Load("assets/Indian girl rhino.png");
    if (surf) {
        girlRhinoTexture = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
    }

    // Load Background
    surf = IMG_Load("assets/Background Green.png");
    if (surf) {
        backgroundTexture = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
    }

    // Load Mud Tile
    surf = IMG_Load("assets/Mud Tile.png");
    if (surf) {
        mudTileTexture = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
    }
    // Background and Crystal textures removed for minimalist style

}

void Game::initGameObjects() {
    // Define static platforms
    platforms = {
        {0, 500, 800, 20},   // Ground
        {150, 400, 150, 15}, // Left low
        {500, 400, 150, 15}, // Right low
        {325, 300, 150, 15}, // Middle high
        {50, 250, 120, 15},  // Left high
        {630, 250, 120, 15}  // Right high
    };
    players.resize(2);
    spawnPowerUps();
}

void Game::resetGame() {
    // Determine texture based on selection
    SDL_Texture* p1Tex = (p1Character == 0) ? boyTexture : girlTexture;
    SDL_Texture* p1DragonTex = (p1Character == 0) ? boyDragonTexture : girlDragonTexture;
    SDL_Texture* p1RhinoTex = (p1Character == 0) ? boyRhinoTexture : girlRhinoTexture;
    int p1W = (p1Character == 0) ? boyW : girlW;
    int p1H = (p1Character == 0) ? boyH : girlH;
    std::string p1Name = (p1Character == 0) ? "Raju" : "Priya";

    SDL_Texture* p2Tex = (p2Character == 0) ? boyTexture : girlTexture;
    SDL_Texture* p2DragonTex = (p2Character == 0) ? boyDragonTexture : girlDragonTexture;
    SDL_Texture* p2RhinoTex = (p2Character == 0) ? boyRhinoTexture : girlRhinoTexture;
    int p2W = (p2Character == 0) ? boyW : girlW;
    int p2H = (p2Character == 0) ? boyH : girlH;
    
    // Use input names or defaults
    std::string p1FinalName = p1NameInput;
    if (p1FinalName.empty()) p1FinalName = (p1Character == 0) ? "Xeno" : "Zeni";
    
    std::string p2FinalName = p2NameInput;
    if (p2FinalName.empty()) p2FinalName = (p2Character == 0) ? "Xeno" : "Zeni";

    // Initialize players
    // P1 starts on left, P2 on right
    players[0].init(1, 100, 400, {255, 255, 255, 255}, p1FinalName, p1Tex, p1DragonTex, p1RhinoTex, p1W/6, p1H/3, 6, 3);
    players[1].init(2, 700, 400, {255, 255, 255, 255}, p2FinalName, p2Tex, p2DragonTex, p2RhinoTex, p2W/6, p2H/3, 6, 3);

    // Clear dynamic objects
    projectiles.clear();
    particles.clear();
    powerUps.clear();
    
    // Initial Spawn
    spawnPowerUps();
    lastPowerUpTime = SDL_GetTicks();
    
    gameTime = GameConstants::GAME_DURATION;
    winnerId = 0;
}

void Game::spawnPowerUps() {
    spawnPowerUp(powerUps, platforms);
}

void Game::handleEvents(SDL_Event& event) {
    // Poll events using for(;;) loop
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) running = false;
        else if (event.type == SDL_TEXTINPUT) {
            if (ignoreInputFrames > 0) {
                // Ignore input
            }
            else if (currentState == MENU) {
                if (enteringCode) {
                    secretCode += event.text.text;
                }
            }
            else if (currentState == CHARACTER_SELECT && typingName) {
                if (!isOnline || net.isHost) {
                    p1NameInput += event.text.text;
                } else {
                    p2NameInput += event.text.text;
                }
            }
            else if (currentState == NAME_INPUT) {
                inputText += event.text.text;
            }
            else if (currentState == SERVER_IP_INPUT) {
                inputText += event.text.text;
            }
        }
        
        if (event.type == SDL_KEYDOWN) {
            if (currentState == MENU) {
                if (!isOnline) {
                    if (event.key.keysym.sym == SDLK_h) {
                        isOnline = true;
                        // Request Code from Signaling Server
                        secretCode = net.requestHostCode(12345);
                        if (secretCode.empty()) {
                            signalingError = "Failed to reach Signaling Server";
                            isOnline = false;
                        } else {
                            net.hostGame(12345);
                            waitingForCode = true; // Waiting for client to join via code
                        }
                    }
                    if (event.key.keysym.sym == SDLK_j) {
                        isOnline = true;
                        enteringCode = true;
                        secretCode = "";
                        SDL_StartTextInput();
                        ignoreInputFrames = 2;
                    }
                    if (event.key.keysym.sym == SDLK_l) {
                        // Local Game
                        isOnline = false;
                        currentState = CHARACTER_SELECT;
                        p1Ready = false;
                        p2Ready = false;
                        p1NameInput = "Player 1";
                        p2NameInput = "Player 2";
                        typingName = false;
                    }
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        currentState = EXIT_CONFIRM;
                    }
                    if (event.key.keysym.sym == SDLK_s) {
                        currentState = SERVER_IP_INPUT;
                        SDL_StartTextInput();
                        inputText = ipInput; // Pre-fill with current
                    }
                } else {
                    // Online Menu
                    if (net.isHost) {
                         // Host Waiting Logic (Handled in update)
                         if (event.key.keysym.sym == SDLK_ESCAPE) {
                             net.disconnect();
                             isOnline = false;
                             waitingForCode = false;
                         }
                    } else {
                        // Client Entering Code
                        if (enteringCode) {
                            if (event.key.keysym.sym == SDLK_RETURN) {
                                SDL_StopTextInput();
                                std::string addr = net.resolveJoinCode(secretCode);
                                if (addr.empty()) {
                                    signalingError = "Invalid or Expired Code";
                                    // Stay in menu to retry
                                } else {
                                    // Parse IP and Port
                                    size_t spacePos = addr.find(' ');
                                    if (spacePos != std::string::npos) {
                                        std::string ip = addr.substr(0, spacePos);
                                        int port = std::stoi(addr.substr(spacePos + 1));
                                        
                                        if (net.joinGame(ip, port)) {
                                            currentState = CHARACTER_SELECT;
                                            connectionFailed = false;
                                            enteringCode = false;
                                        } else {
                                            connectionFailed = true;
                                            signalingError = "Connection Failed";
                                            SDL_StartTextInput();
                                        }
                                    }
                                }
                            }
                            if (event.key.keysym.sym == SDLK_BACKSPACE && secretCode.length() > 0) {
                                secretCode.pop_back();
                            }
                            if (event.key.keysym.sym == SDLK_ESCAPE) {
                                SDL_StopTextInput();
                                isOnline = false;
                                enteringCode = false;
                                signalingError = "";
                            }
                        }
                    }
                }
            }
            else if (currentState == CHARACTER_SELECT) {
                // Typing Name
                if (typingName) {
                    if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_ESCAPE) {
                        typingName = false;
                        SDL_StopTextInput();
                    }
                    else if (event.key.keysym.sym == SDLK_BACKSPACE) {
                        if (!isOnline || net.isHost) {
                            if (p1NameInput.length() > 0) p1NameInput.pop_back();
                        } else {
                            if (p2NameInput.length() > 0) p2NameInput.pop_back();
                        }
                    }
                } else {
                    // Normal Lobby Controls
                    if (event.key.keysym.sym == SDLK_t) {
                        typingName = true;
                        SDL_StartTextInput();
                    }

                    // Toggle Ready
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        if (!isOnline) {
                            // Local: Force both to Ready if not already
                            if (!p1Ready || !p2Ready) {
                                p1Ready = true;
                                p2Ready = true;
                            } else {
                                p1Ready = false;
                                p2Ready = false;
                            }
                        } else {
                            // Online: Toggle my own ready status
                            if (net.isHost) p1Ready = !p1Ready;
                            else p2Ready = !p2Ready;
                        }
                    }
                    if (!isOnline && event.key.keysym.sym == SDLK_r) {
                        p2Ready = !p2Ready;
                    }

                    // Character Toggling
                    if (!isOnline || net.isHost) {
                        if (event.key.keysym.sym == SDLK_1) p1Character = (p1Character + 1) % 2;
                    }
                    if (!isOnline || !net.isHost) {
                        if (event.key.keysym.sym == SDLK_2) p2Character = (p2Character + 1) % 2;
                    }

                    // Disconnect / Back to Menu
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        if (isOnline) {
                            net.disconnect();
                            isOnline = false;
                        }
                    currentState = MENU;
                        // Reset Lobby State
                        p1Ready = false;
                        p2Ready = false;
                        p1NameInput = "Player 1";
                        p2NameInput = "Player 2";
                    }
                }
                
                // Start Game
                if (event.key.keysym.sym == SDLK_SPACE) {
                    if (!isOnline || net.isHost) {
                        if (isOnline) {
                             // Skip Name Input for Online to ensure sync
                             Packet startP;
                             startP.type = 3; // Start Game
                             net.send(startP);
                             resetGame();
                             currentState = PLAYING;
                        } else {
                            // Local Game: Skip Name Input too (User Request)
                            // Use default names set in 'L' key handler or current input
                            resetGame();
                            currentState = PLAYING;
                        }
                    }
                }
            }
            else if (currentState == NAME_INPUT) {
                if (event.key.keysym.sym == SDLK_SPACE) {
                    if (inputtingP1) {
                        p1NameInput = inputText;
                        inputtingP1 = false;
                        inputText = "";
                    } else {
                        p2NameInput = inputText;
                        SDL_StopTextInput();
                        resetGame();
                        currentState = PLAYING;
                    }
                } else if (event.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0) {
                    inputText.pop_back();
                }
            }
            else if (currentState == PLAYING) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    currentState = PAUSED;
                    pauseTime = 10.0f; // 10 seconds auto-forfeit
                }
                // if (event.key.keysym.sym == SDLK_SPACE) currentState = MENU; // Removed to prevent accidental quit

                // Player 1 (WASD)
                if (event.key.keysym.sym == SDLK_a) players[0].keyLeft = true;
                if (event.key.keysym.sym == SDLK_d) players[0].keyRight = true;
                if (event.key.keysym.sym == SDLK_w) players[0].keyJump = true;
                if (event.key.keysym.sym == SDLK_s) players[0].keyDown = true;
                if (event.key.keysym.sym == SDLK_f) players[0].keyAttack = true;

                // Player 2 (Arrows)
                if (event.key.keysym.sym == SDLK_LEFT) players[1].keyLeft = true;
                if (event.key.keysym.sym == SDLK_RIGHT) players[1].keyRight = true;
                if (event.key.keysym.sym == SDLK_UP) players[1].keyJump = true;
                if (event.key.keysym.sym == SDLK_DOWN) players[1].keyDown = true;
                if (event.key.keysym.sym == SDLK_RETURN) players[1].keyAttack = true;
            }
            else if (currentState == EXIT_CONFIRM) {
                if (event.key.keysym.sym == SDLK_y) {
                    running = false;
                }
                if (event.key.keysym.sym == SDLK_n || event.key.keysym.sym == SDLK_ESCAPE) {
                    currentState = MENU;
                }
            }
            else if (currentState == SERVER_IP_INPUT) {
                if (event.key.keysym.sym == SDLK_RETURN) {
                    ipInput = inputText;
                    net.setSignalingServer(ipInput);
                    SDL_StopTextInput();
                    currentState = MENU;
                }
                if (event.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0) {
                    inputText.pop_back();
                }
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    SDL_StopTextInput();
                    currentState = MENU;
                }
            }
            else if (currentState == GAMEOVER) {
                if (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_ESCAPE) {
                    if (isOnline) {
                        net.disconnect();
                        isOnline = false;
                    }
                    currentState = MENU;
                    // Reset Lobby State
                    p1Ready = false;
                    p2Ready = false;
                    p1NameInput = "Player 1";
                    p2NameInput = "Player 2";
                    
                    // Reset Signaling State
                    waitingForCode = false;
                    enteringCode = false;
                    secretCode = "";
                    signalingError = "";
                    connectionFailed = false;
                }
            }
            else if (currentState == PAUSED) {
                if (event.key.keysym.sym == SDLK_y) {
                    if (isOnline) {
                        // Forfeit!
                        currentState = GAMEOVER;
                        // Determine winner (The one who didn't quit)
                        // If Host quits, P2 wins (2). If Client quits, P1 wins (1).
                        int winner = net.isHost ? 2 : 1;
                        
                        // Force HP to 0 to trigger update loop sync logic
                        if (net.isHost) players[0].hp = 0;
                        else players[1].hp = 0;
                        
                    } else {
                    currentState = MENU;
                    }
                }
                if (event.key.keysym.sym == SDLK_n || event.key.keysym.sym == SDLK_ESCAPE) currentState = PLAYING;
            }
        }
        // Key Release Handling
        if (event.type == SDL_KEYUP && currentState == PLAYING) {
            if (event.key.keysym.sym == SDLK_a) players[0].keyLeft = false;
            if (event.key.keysym.sym == SDLK_d) players[0].keyRight = false;
            if (event.key.keysym.sym == SDLK_w) players[0].keyJump = false;
            if (event.key.keysym.sym == SDLK_s) players[0].keyDown = false;
            if (event.key.keysym.sym == SDLK_f) players[0].keyAttack = false;

            if (event.key.keysym.sym == SDLK_LEFT) players[1].keyLeft = false;
            if (event.key.keysym.sym == SDLK_RIGHT) players[1].keyRight = false;
            if (event.key.keysym.sym == SDLK_UP) players[1].keyJump = false;
            if (event.key.keysym.sym == SDLK_DOWN) players[1].keyDown = false;
            if (event.key.keysym.sym == SDLK_RETURN) players[1].keyAttack = false;
        }
    }
}


void Game::update() {

    // Timer Logic
    if (ignoreInputFrames > 0) ignoreInputFrames--;

    // Auto-transition Host to Lobby
    if (currentState == MENU && isOnline && net.isHost && net.connected) {
        SDL_Delay(500); // Brief delay to see "Connected" message
        currentState = CHARACTER_SELECT;
    }

    if (currentState == CHARACTER_SELECT) {
        if (isOnline) {
            if (!net.connected) {
                // Peer disconnected
                isOnline = false;
                currentState = MENU;
                net.disconnect(); // Ensure clean state
                p1Ready = false;
                p2Ready = false;
                p1NameInput = "Player 1";
                p2NameInput = "Player 2";
            }
            else if (net.isHost) {
                // Host sends P1 choice and Game State (to trigger start)
                // Host sends P1 choice, Name, Ready Status
                Packet p;
                p.type = 2; // State (using same type for lobby sync)
                p.gameState = CHARACTER_SELECT;
                p.p1Char = p1Character;
                strncpy(p.p1Name, p1NameInput.c_str(), 19); p.p1Name[19] = '\0';
                p.p1Ready = p1Ready;
                
                // Host also echoes P2 data back to P2 (so P2 knows Host knows)
                // Actually P2 sends its data to Host. Host just needs to receive it.
                // But Host is Authority.
                
                // Let's just send P1 data for now.
                net.send(p);

                // Receive P2 choice, Name, Ready Status
                Packet p2P;
                // Process all incoming packets
                for (; net.receive(p2P); ) {
                    if (p2P.type == 2) {
                        p2Character = p2P.p2Char;
                        p2NameInput = p2P.p2Name;
                        p2Ready = p2P.p2Ready;
                    }
                }
                
                // Check Start Condition
                if (p1Ready && p2Ready) {
                    // Both Ready! Start Game!
                    Packet startP;
                    startP.type = 3; // Start Game
                    net.send(startP);
                    
                    // Set names in Player objects
                    players[0].name = p1NameInput;
                    players[1].name = p2NameInput;
                    
                    resetGame();
                    currentState = PLAYING;
                }

            } else {
                // Client sends P2 choice, Name, Ready Status
                Packet p;
                p.type = 2;
                p.p2Char = p2Character;
                strncpy(p.p2Name, p2NameInput.c_str(), 19); p.p2Name[19] = '\0';
                p.p2Ready = p2Ready;
                net.send(p);

                // Receive P1 choice, Name, Ready Status
                Packet hostP;
                for (; net.receive(hostP); ) {
                    if (hostP.type == 2) {
                        // Check if Host has already started the game
                        if (hostP.gameState == PLAYING) {
                             players[0].name = p1NameInput; // Use last known name
                             players[1].name = p2NameInput;
                             resetGame();
                             currentState = PLAYING;
                             return;
                        }

                        p1Character = hostP.p1Char;
                        p1NameInput = hostP.p1Name;
                        p1Ready = hostP.p1Ready;
                    } else if (hostP.type == 3) {
                         // Host started the game!
                         players[0].name = p1NameInput;
                         players[1].name = p2NameInput;
                         resetGame();
                         currentState = PLAYING;
                         return;
                    }
                }
            }
        } else {
            // Local Play Start Logic
            if (p1Ready && p2Ready) { // Require both ready for local too
                 isOnline = false; // Ensure we are offline
                 players[0].name = p1NameInput;
                 players[1].name = p2NameInput;
                 resetGame();
                 currentState = PLAYING;
            }
        }
    }

    if (currentState == PLAYING) {
        if (isOnline) {
            if (!net.connected) {
                 // Peer Disconnected
                 isOnline = false;
                 currentState = MENU;
                 net.disconnect();
                 p1Ready = false;
                 p2Ready = false;
                 p1NameInput = "Player 1";
                 p2NameInput = "Player 2";
                 return;
            }

            if (net.isHost) {
                // HOST: Receive P2 Input, Update Physics, Send State
                Packet p2Input;
                // Drain the socket to get the LATEST input packet
                // This prevents input lag if packets pile up in the buffer
                for (; net.receive(p2Input); ) {
                    if (p2Input.type == 1) {
                        // Apply P2 Input
                        players[1].keyLeft = p2Input.keys[0]; // Left
                        players[1].keyRight = p2Input.keys[1]; // Right
                        players[1].keyJump = p2Input.keys[2]; // Jump
                        players[1].keyDown = p2Input.keys[3]; // Down
                        players[1].keyAttack = p2Input.keys[4]; // Attack
                    }
                }
                
                if (!net.connected) {
                     isOnline = false;
                     currentState = MENU;
                     net.disconnect();
                     p1Ready = false;
                     p2Ready = false;
                     waitingForCode = false;
                     enteringCode = false;
                     secretCode = "";
                     signalingError = "";
                     connectionFailed = false;
                     return;
                }
                
                // Update Game Logic (Host Authority)
                // ... (Existing update logic below) ...
                Packet stateP;
                stateP.type = 2; // Game State
                stateP.gameTime = gameTime;
                
                stateP.p1X = players[0].x;
                stateP.p1Y = players[0].y;
                stateP.p1VX = players[0].vx;
                stateP.p1VY = players[0].vy;
                stateP.p1HP = players[0].hp;
                strncpy(stateP.p1Power, players[0].power.c_str(), 19); stateP.p1Power[19] = '\0';
                stateP.p1PowerTimer = players[0].powerTimer;
                stateP.p1Invincible = players[0].invincible;
                stateP.p1AttackCooldown = players[0].attackCooldown;
                stateP.p1FacingLeft = (players[0].facing == -1);

                stateP.p2X = players[1].x;
                stateP.p2Y = players[1].y;
                stateP.p2VX = players[1].vx;
                stateP.p2VY = players[1].vy;
                stateP.p2HP = players[1].hp;
                strncpy(stateP.p2Power, players[1].power.c_str(), 19); stateP.p2Power[19] = '\0';
                stateP.p2PowerTimer = players[1].powerTimer;
                stateP.p2Invincible = players[1].invincible;
                stateP.p2AttackCooldown = players[1].attackCooldown;
                stateP.p2FacingLeft = (players[1].facing == -1);

                // Sync PowerUps
                stateP.numPowerUps = 0;
                for (const auto& pu : powerUps) {
                    if (stateP.numPowerUps < 5) {
                        stateP.powerUps[stateP.numPowerUps].x = pu.x;
                        stateP.powerUps[stateP.numPowerUps].y = pu.y;
                        strncpy(stateP.powerUps[stateP.numPowerUps].type, pu.type.c_str(), 19);
                        stateP.powerUps[stateP.numPowerUps].type[19] = '\0';
                        stateP.numPowerUps++;
                    }
                }

                // Sync Projectiles
                stateP.numProjectiles = 0;
                for (const auto& proj : projectiles) {
                    if (stateP.numProjectiles < 10) {
                        stateP.projectiles[stateP.numProjectiles].x = proj.x;
                        stateP.projectiles[stateP.numProjectiles].y = proj.y;
                        stateP.projectiles[stateP.numProjectiles].vx = proj.vx;
                        stateP.projectiles[stateP.numProjectiles].vy = proj.vy;
                        stateP.projectiles[stateP.numProjectiles].owner = proj.owner;
                        strncpy(stateP.projectiles[stateP.numProjectiles].type, proj.type.c_str(), 9);
                        stateP.projectiles[stateP.numProjectiles].type[9] = '\0';
                        stateP.numProjectiles++;
                    }
                }

                // Sync Game State (Game Over)
                stateP.gameState = currentState;
                if (currentState == GAMEOVER) {
                    if (players[0].hp <= 0) stateP.winnerId = 2;
                    else if (players[1].hp <= 0) stateP.winnerId = 1;
                    else stateP.winnerId = 0; // Time out?
                }

                net.send(stateP);
                
            } else {
                // CLIENT: Send P2 Input, Receive State
                Packet p2Input;
                p2Input.type = 1; // Input
                p2Input.keys[0] = players[1].keyLeft;
                p2Input.keys[1] = players[1].keyRight;
                p2Input.keys[2] = players[1].keyJump;
                p2Input.keys[3] = players[1].keyDown;
                p2Input.keys[4] = players[1].keyAttack;
                net.send(p2Input);

                Packet hostState;
                // Drain socket to get LATEST state
                for (; net.receive(hostState); ) {
                    if (hostState.type == 2) { // Assuming type 2 is game state
                        // Check for State Change (e.g. Back to Lobby)
                        if (hostState.gameState == CHARACTER_SELECT) {
                            currentState = CHARACTER_SELECT;
                            p1Ready = false;
                            p2Ready = false;
                            return; // Exit update to prevent applying game state
                        }

                        // Apply Host State
                        players[0].x = hostState.p1X;
                        players[0].y = hostState.p1Y;
                        players[0].vx = hostState.p1VX;
                        players[0].vy = hostState.p1VY;
                        players[0].hp = hostState.p1HP;
                        players[0].power = hostState.p1Power;
                        players[0].powerTimer = hostState.p1PowerTimer;
                        players[0].invincible = hostState.p1Invincible;
                        players[0].attackCooldown = hostState.p1AttackCooldown;
                        players[0].facing = hostState.p1FacingLeft ? -1 : 1;

                        players[1].x = hostState.p2X;
                        players[1].y = hostState.p2Y;
                        players[1].vx = hostState.p2VX;
                        players[1].vy = hostState.p2VY;
                        players[1].hp = hostState.p2HP;
                        players[1].power = hostState.p2Power;
                        players[1].powerTimer = hostState.p2PowerTimer;
                        players[1].invincible = hostState.p2Invincible;
                        players[1].attackCooldown = hostState.p2AttackCooldown;
                        players[1].facing = hostState.p2FacingLeft ? -1 : 1;

                        // Sync Game Time
                        gameTime = hostState.gameTime;

                        // Sync PowerUps
                        powerUps.clear();
                        for (int i = 0; i < hostState.numPowerUps; i++) {
                            PowerUp pu;
                            pu.x = hostState.powerUps[i].x;
                            pu.y = hostState.powerUps[i].y;
                            pu.width = 30; // Default size
                            pu.height = 30;
                            pu.type = hostState.powerUps[i].type;
                            pu.bobTimer = 0; // Visuals can be local
                            pu.lifetime = 600; // Assume fresh or keep host's if we synced it
                            powerUps.push_back(pu);
                        }

                        // Sync Projectiles
                        projectiles.clear();
                        for (int i = 0; i < hostState.numProjectiles; i++) {
                            Projectile proj;
                            proj.x = hostState.projectiles[i].x;
                            proj.y = hostState.projectiles[i].y;
                            proj.vx = hostState.projectiles[i].vx;
                            proj.vy = hostState.projectiles[i].vy;
                            proj.owner = hostState.projectiles[i].owner;
                            proj.type = hostState.projectiles[i].type;
                            proj.width = 15; // Default size
                            proj.height = 15;
                            projectiles.push_back(proj);
                        }

                        // Sync Game State
                        if (hostState.gameState == GAMEOVER) {
                            currentState = GAMEOVER;
                            winnerId = hostState.winnerId;
                        }
                    }
                }
            }
            }
            
            if (isOnline && !net.connected) {
                 isOnline = false;
                 currentState = MENU;
                 net.disconnect();
                 p1Ready = false;
                 p2Ready = false;
                 waitingForCode = false;
                 enteringCode = false;
                 secretCode = "";
                 signalingError = "";
                 connectionFailed = false;
                 return;
            }
    }
    else if (currentState == GAMEOVER) {
        if (isOnline) {
            if (net.isHost) {
                // HOST: Continue sending Game Over state so Client knows
                Packet stateP;
                stateP.type = 2; // Game State
                stateP.gameTime = gameTime;
                stateP.gameState = GAMEOVER;
                
                // Ensure winner is consistent
                if (players[0].hp <= 0) winnerId = 2;
                else if (players[1].hp <= 0) winnerId = 1;
                // If forfeit, winnerId should have been set in handleEvents. 
                // If not set (0), check HP. If still 0, maybe draw?
                // But handleEvents sets HP to 0 for quitter.
                
                stateP.winnerId = winnerId;

                // Sync HP too just in case
                stateP.p1HP = players[0].hp;
                stateP.p2HP = players[1].hp;
                
                net.send(stateP);
                
                if (!net.connected) {
                     isOnline = false;
                     currentState = MENU;
                     net.disconnect();
                     p1Ready = false;
                     p2Ready = false;
                     waitingForCode = false;
                     enteringCode = false;
                     secretCode = "";
                     signalingError = "";
                     connectionFailed = false;
                }
            } else {
                // Client listens for state change (Back to Lobby)
                Packet p;
                for (; net.receive(p); ) {
                    if (p.type == 2) {
                        if (p.gameState == CHARACTER_SELECT) {
                            currentState = CHARACTER_SELECT;
                            p1Ready = false;
                            p2Ready = false;
                        }
                        // Update HP/Winner if we missed it?
                        players[0].hp = p.p1HP;
                        players[1].hp = p.p2HP;
                        if (p.gameState == GAMEOVER) {
                             winnerId = p.winnerId;
                        }
                    }
                }
                if (!net.connected) {
                    isOnline = false;
                    currentState = MENU;
                     net.disconnect();
                     p1Ready = false;
                     p2Ready = false;
                     
                     // Reset Signaling State
                     waitingForCode = false;
                     enteringCode = false;
                     secretCode = "";
                     signalingError = "";
                     connectionFailed = false;
                }
            }
        } else {
             // Local Game Over Logic
             if (players[0].hp <= 0) winnerId = 2;
             else if (players[1].hp <= 0) winnerId = 1;
        }
    }
    


    if (currentState == PAUSED) {
        pauseTime -= 1.0f / 60.0f;
        if (pauseTime <= 0) {
            currentState = GAMEOVER;
        }
        return;
    }
    if (currentState != PLAYING) return;

    // Update Players
    for (auto& player : players) {
        player.update(platforms, projectiles, particles);
    }

    // Spawn Power-ups periodically
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastPowerUpTime > GameConstants::POWER_UP_SPAWN_INTERVAL) {
        spawnPowerUps();
        lastPowerUpTime = currentTime;
    }

    // Power-up collection
    for (auto it = powerUps.begin(); it != powerUps.end(); ) {
        // Bobbing animation
        it->bobTimer++;
        it->y += sin(it->bobTimer * 0.1f) * 0.5f;
        
        // Lifetime Check
        it->lifetime--;
        if (it->lifetime <= 0) {
            it = powerUps.erase(it);
            continue;
        }

        bool collected = false;
        for (auto& player : players) {
            if (checkCollision(player.x, player.y, player.width, player.height,
                             it->x, it->y, it->width, it->height)) {
                // Apply Effect
                player.power = it->type;
                player.powerTimer = GameConstants::POWER_DURATION;
                
                if (it->type == "health") player.hp = std::min(player.maxHp, player.hp + GameConstants::HEALTH_PICKUP);
                
                // Visuals
                createParticles(particles, it->x, it->y, {255, 215, 0, 255}, GameConstants::COLLECT_PARTICLE_COUNT);
                collected = true;
                break;
            }
        }
        if (collected) it = powerUps.erase(it);
        else ++it;
    }

    // PvP Collision (Player vs Player)
    if (checkCollision(players[0].x, players[0].y, players[0].width, players[0].height,
                     players[1].x, players[1].y, players[1].width, players[1].height)) {
        if (players[0].invincible == 0 && players[1].invincible == 0) {
            // Bounce back
            float knockback = GameConstants::KNOCKBACK_FORCE;
            if (players[0].x < players[1].x) {
                players[0].vx = -knockback; players[1].vx = knockback;
            } else {
                players[0].vx = knockback; players[1].vx = -knockback;
            }
            // Deal damage
            float p1Damage = GameConstants::COLLISION_DAMAGE;
            float p2Damage = GameConstants::COLLISION_DAMAGE;

            // Rhino Logic (Shield = Rhino)
            bool p1Rhino = (players[0].power == "shield");
            bool p2Rhino = (players[1].power == "shield");
            
            // Check for Attacking (Any active attack: Fire, Dash, Lunge)
            // Rhino is attacking if cooldown is active OR moving faster than normal max speed (Charging)
            bool p1Attacking = players[0].attackCooldown > 0 || (p1Rhino && std::abs(players[0].vx) > GameConstants::MAX_VELOCITY_X);
            bool p2Attacking = players[1].attackCooldown > 0 || (p2Rhino && std::abs(players[1].vx) > GameConstants::MAX_VELOCITY_X);

            if (p1Rhino && p2Rhino) {
                if (p1Attacking && p2Attacking) {
                    // CLASH! Both lose power
                    players[0].power = ""; players[0].texture = players[0].normalTexture;
                    players[1].power = ""; players[1].texture = players[1].normalTexture;
                    p1Damage = 0; p2Damage = 0;
                    // Visual effect
                    createParticles(particles, players[0].x + players[0].width, players[0].y + players[0].height/2, {255, 255, 255, 255}, 20);
                } else {
                    // Just bumping into each other
                    p1Damage = 0; p2Damage = 0;
                }
            }
            else if (p1Rhino) {
                if (p2Attacking && !p1Attacking) {
                    // Counter! P2 attacks Passive Rhino -> Rhino loses power
                    players[0].power = ""; players[0].texture = players[0].normalTexture;
                    p1Damage = 0; // Rhino takes no HP damage from the hit that breaks shield
                }
                
                if (p1Attacking) {
                    // Rhino Charge! P2 takes double damage
                    p2Damage *= GameConstants::RHINO_DAMAGE_MULTIPLIER;
                    p1Damage = 0; // Rhino takes no damage while charging
                } else if (!p2Attacking) {
                     // Passive Rhino bump
                     p1Damage = 0;
                }
            }
            else if (p2Rhino) {
                if (p1Attacking && !p2Attacking) {
                    // Counter! P1 attacks Passive Rhino -> Rhino loses power
                    players[1].power = ""; players[1].texture = players[1].normalTexture;
                    p2Damage = 0;
                }
                
                if (p2Attacking) {
                    // Rhino Charge! P1 takes double damage
                    p1Damage *= GameConstants::RHINO_DAMAGE_MULTIPLIER;
                    p2Damage = 0;
                } else if (!p1Attacking) {
                    // Passive Rhino bump
                    p2Damage = 0;
                }
            }
            else {
                // Normal vs Normal
                if (p1Attacking && !p2Attacking) {
                    // P1 Hits P2
                    p1Damage = 0;
                    p2Damage = GameConstants::COLLISION_DAMAGE;
                } else if (!p1Attacking && p2Attacking) {
                    // P2 Hits P1
                    p1Damage = GameConstants::COLLISION_DAMAGE;
                    p2Damage = 0;
                } else {
                    // Both Attacking (Clash) or Both Passive (Bump) -> Both take damage
                    p1Damage = GameConstants::COLLISION_DAMAGE;
                    p2Damage = GameConstants::COLLISION_DAMAGE;
                }
            }

            players[0].takeDamage(p1Damage, particles);
            players[1].takeDamage(p2Damage, particles);
        }
    }

    // Projectiles Logic
    for (auto it = projectiles.begin(); it != projectiles.end(); ) {
        it->x += it->vx; it->y += it->vy;
        


        bool hit = false;
        for (auto& player : players) {
            // Don't hit self
            if ((player.id - 1) != it->owner &&
                checkCollision(it->x - it->width/2, it->y - it->height/2, it->width, it->height,
                             player.x, player.y, player.width, player.height) &&
                player.invincible == 0) {
                
                player.takeDamage(GameConstants::PROJECTILE_DAMAGE, particles);
                player.vx = it->vx * 0.5f; player.vy = -5; // Knockback
                hit = true;
                break;
            }
        }
        // Remove if hit or out of bounds
        if (hit || it->x < 0 || it->x > GameConstants::WINDOW_WIDTH) it = projectiles.erase(it);
        else ++it;
    }

    // Update Particles
    for (auto& p : particles) p.update();
    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](const Particle& p) { return !p.active; }), particles.end());

    // Timer Logic
    if (gameTime > 0) {
        gameTime -= 1.0f / 60.0f; // Assuming 60 FPS
        if (gameTime <= 0) {
            gameTime = 0;
            currentState = GAMEOVER;
        }
    }

    // Check Win Condition (Death)
    if (players[0].hp <= 0 || players[1].hp <= 0) {
        currentState = GAMEOVER;
    }
}

void Game::render() {
    // Clear screen (Dark Slate Blue - High contrast    // Clear screen (Forest Green fallback)
    SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
    SDL_RenderClear(renderer);

    // Clear screen (Dark Slate Blue - High contrast for warm skin tones)
    SDL_SetRenderDrawColor(renderer, 30, 40, 50, 255);
    SDL_RenderClear(renderer);

    // Render Background Texture (Only during Gameplay)
    if (backgroundTexture && (currentState == PLAYING || currentState == PAUSED)) {
        // Dimming ("Little Blacky")
        SDL_SetTextureColorMod(backgroundTexture, 150, 150, 150); // Darken to ~60% brightness

        // Parallax Effect
        // Calculate average player position to shift background
        float avgX = (players[0].x + players[1].x) / 2.0f;
        float avgY = (players[0].y + players[1].y) / 2.0f;
        
        // Calculate offset (inverse to movement for depth)
        // Max shift is small relative to screen size
        float offsetX = (GameConstants::WINDOW_WIDTH / 2.0f - avgX) * 0.05f;
        float offsetY = (GameConstants::WINDOW_HEIGHT / 2.0f - avgY) * 0.05f;

        // Draw slightly larger than screen to cover edges during shift
        SDL_Rect bgRect = {
            static_cast<int>(-40 + offsetX),
            static_cast<int>(-26 + offsetY),
            880, // 1.1x Width
            572  // 1.1x Height
        };
        SDL_RenderCopy(renderer, backgroundTexture, NULL, &bgRect);
    }

    if (currentState == MENU) {
        // Connection Menu
        if (titleFont) renderCenteredText(100, "amphitude", {150, 255, 150, 255}, titleFont);
        if (font) {
            if (!isOnline) {
                renderCenteredText(250, "Press H to HOST Game", {255, 255, 255, 255}, font);
                renderCenteredText(300, "Press J to JOIN Game", {255, 255, 255, 255}, font);
                renderCenteredText(350, "Press L for LOCAL Game", {200, 200, 200, 255}, font);
                renderCenteredText(400, "Press S to Set Server IP", {100, 100, 100, 255}, font);
                if (!signalingError.empty()) {
                    renderCenteredText(450, signalingError, {255, 0, 0, 255}, font);
                }
                renderCenteredText(490, "amphitainments", {150, 150, 150, 255}, font);
            } else {
                if (net.isHost) {
                    if (waitingForCode) {
                         renderCenteredText(250, "Hosting Game...", {255, 255, 255, 255}, font);
                         renderCenteredText(300, "Secret Code: " + secretCode, {0, 255, 0, 255}, font);
                         renderCenteredText(400, "Share this code with your friend!", {200, 200, 200, 255}, font);
                         renderCenteredText(450, "Waiting for connection...", {255, 255, 0, 255}, font);
                         

                    } else if (net.connected) {
                         renderCenteredText(300, "Client Connected! Entering Lobby...", {0, 255, 0, 255}, font);
                    }
                } else {
                    if (enteringCode) {
                        renderCenteredText(250, "Enter Secret Code:", {255, 255, 255, 255}, font);
                        renderCenteredText(300, secretCode + "_", {0, 255, 255, 255}, font);
                        if (!signalingError.empty()) {
                            renderCenteredText(400, signalingError, {255, 0, 0, 255}, font);
                        } else {
                            renderCenteredText(350, "Press ENTER to Connect", {200, 200, 200, 255}, font);
                        }
                    }
                }
            }
        }
    }
    else if (currentState == EXIT_CONFIRM) {
        if (font) {
            renderCenteredText(250, "Quitting...", {255, 255, 255, 255}, font);
            renderCenteredText(350, "Are you sure? (Y/N)", {255, 0, 0, 255}, font);
        }
    }
    else if (currentState == SERVER_IP_INPUT) {
        if (font) {
            renderCenteredText(200, "Signaling Server IP", {255, 255, 255, 255}, font);
            renderCenteredText(250, "(Default: 127.0.0.1)", {150, 150, 150, 255}, font);
            renderCenteredText(300, inputText + "_", {0, 255, 255, 255}, font);
            renderCenteredText(400, "Press ENTER to Save", {200, 200, 200, 255}, font);
        }
    }
    else if (currentState == CHARACTER_SELECT) {
        // Background
        SDL_SetRenderDrawColor(renderer, 30, 30, 40, 255);
        SDL_RenderClear(renderer);

        renderCenteredText(50, "LOBBY", {255, 255, 0, 255}, titleFont);

        // Player 1 Section (Left)
        std::string p1Str = (p1Character == 0) ? "< Boy >" : "< Girl >";
        renderText(100, 150, "Player 1 (Host)", {0, 255, 255, 255}, font);
        renderText(100, 200, p1Str, {255, 255, 255, 255}, font);
        renderText(100, 250, "Name: " + p1NameInput + (typingName && (isOnline ? net.isHost : true) ? "_" : ""), {255, 255, 255, 255}, font);
        renderText(100, 300, p1Ready ? "READY!" : "Not Ready", p1Ready ? SDL_Color{0, 255, 0, 255} : SDL_Color{255, 0, 0, 255}, font);

        // Player 2 Section (Right)
        std::string p2Str = (p2Character == 0) ? "< Boy >" : "< Girl >";
        renderText(500, 150, "Player 2 (Client)", {255, 100, 100, 255}, font);
        renderText(500, 200, p2Str, {255, 255, 255, 255}, font);
        renderText(500, 250, "Name: " + p2NameInput + (typingName && (isOnline ? !net.isHost : false) ? "_" : ""), {255, 255, 255, 255}, font);
        renderText(500, 300, p2Ready ? "READY!" : "Not Ready", p2Ready ? SDL_Color{0, 255, 0, 255} : SDL_Color{255, 0, 0, 255}, font);

        // Instructions
        renderCenteredText(450, "Press 'T' to Type Name", {200, 200, 200, 255}, font);
        renderCenteredText(500, "Press 'ENTER' to Toggle Ready", {200, 200, 200, 255}, font);
        
        if (p1Ready && p2Ready) {
             if (!isOnline || net.isHost) {
                renderCenteredText(550, "Starting Game...", {0, 255, 0, 255}, font);
             } else {
                renderCenteredText(550, "Waiting for Host...", {0, 255, 0, 255}, font);
             }
        }
    }
    else if (currentState == NAME_INPUT) {
        drawRect(renderer, 100, 100, 600, 300, {255, 255, 255, 255});
        if (font) {
            if (inputtingP1) {
                renderText(200, 150, "Enter Player 1 Name:", {0, 0, 0, 255}, font);
                renderText(200, 200, inputText + "_", {0, 0, 255, 255}, font);
                renderText(200, 300, "(Default: Xeno/Zeni)", {100, 100, 100, 255}, font);
            } else {
                renderText(200, 150, "Enter Player 2 Name:", {0, 0, 0, 255}, font);
                renderText(200, 200, inputText + "_", {255, 0, 0, 255}, font);
                renderText(200, 300, "(Default: Xeno/Zeni)", {100, 100, 100, 255}, font);
            }
            renderText(250, 350, "Press SPACE to Confirm", {0, 0, 0, 255}, font);
        }
    }
    else if (currentState == PLAYING || currentState == PAUSED) {
        // Draw Platforms
        for (size_t i = 0; i < platforms.size(); ++i) {
            const auto& p = platforms[i];
            if (i == 0 && mudTileTexture) { // Ground Platform (Index 0)
                // Tile the texture
                int tileW = 32; // Assuming 32x32 tile, adjust if needed
                int tileH = 32;
                SDL_QueryTexture(mudTileTexture, NULL, NULL, &tileW, &tileH);
                
                for (int x = p.x; x < p.x + p.width; x += tileW) {
                    SDL_Rect destRect = { x, static_cast<int>(p.y), tileW, static_cast<int>(p.height) };
                    // Clip the last tile if it exceeds platform width
                    if (x + tileW > p.x + p.width) {
                        destRect.w = static_cast<int>((p.x + p.width) - x);
                    }
                    SDL_RenderCopy(renderer, mudTileTexture, NULL, &destRect);
                }
            } else {
                // Other platforms (Pixelated White Transparent)
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 180); // White, slightly more opaque for pixels

                int pixelSize = 3;
                for (int y = p.y; y < p.y + p.height; y += pixelSize) {
                    for (int x = p.x; x < p.x + p.width; x += pixelSize) {
                        // Checkerboard pattern
                        if (((x / pixelSize) + (y / pixelSize)) % 2 == 0) {
                            drawRect(renderer, x, y, pixelSize, pixelSize, {255, 255, 255, 180});
                        }
                    }
                }
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            }
        }
        // Draw Power-ups (with bobbing effect)
        for (const auto& pu : powerUps) {
            // Assuming drawPowerUp function exists or similar logic
            // For now, just draw a rect
            drawRect(renderer, pu.x, pu.y, pu.width, pu.height, {255, 215, 0, 255});
        }

        // Draw Players
        for (auto& player : players) player.render(renderer);

        // Draw Projectiles
        for (const auto& p : projectiles) {
            drawCircle(renderer, p.x, p.y, 5, {255, 255, 0, 255});
        }
        // Draw Particles
        for (const auto& p : particles) {
            drawRect(renderer, p.x, p.y, 3, 3, p.color);
        }

        // HUD: Timer & Health Bars
        if (font) {
            // Timer
            std::string timeStr = "Time: " + std::to_string((int)gameTime);
            renderText(350, 20, timeStr, {255, 255, 255, 255}, font);

            // P1 Health Bar
            drawRect(renderer, 20, 20, 200, 20, {100, 0, 0, 255}); // Back
            drawRect(renderer, 20, 20, (int)(players[0].hp * 2), 20, {0, 255, 0, 255}); // Front
            renderText(20, 45, players[0].name, {255, 255, 255, 255}, font);

            // P2 Health Bar
            drawRect(renderer, 580, 20, 200, 20, {100, 0, 0, 255}); // Back
            drawRect(renderer, 580, 20, (int)(players[1].hp * 2), 20, {0, 255, 0, 255}); // Front
            renderText(580, 45, players[1].name, {255, 255, 255, 255}, font);
        }
    }

    if (currentState == PAUSED) {
        // Semi-transparent overlay
        drawRect(renderer, 0, 0, GameConstants::WINDOW_WIDTH, GameConstants::WINDOW_HEIGHT, {0, 0, 0, 150}, true);
        
        drawRect(renderer, 200, 200, 400, 200, {50, 50, 50, 255});
        if (font) {
            renderText(350, 250, "PAUSED", {255, 255, 255, 255}, font);
            renderText(250, 300, "Quit to Menu? (Y/N)", {255, 255, 0, 255}, font);
            
            std::string autoEnd = "Auto-End in: " + std::to_string((int)pauseTime);
            renderText(300, 350, autoEnd, {255, 0, 0, 255}, font);
        }
    }

    if (currentState == GAMEOVER) {
        // No box, just text on the Dark Slate Blue background
        if (titleFont) renderCenteredText(150, "GAME OVER", {255, 50, 50, 255}, titleFont); // Red Title
        
        if (font) {
            std::string winner;
            if (winnerId == 1) winner = players[0].name + " Wins!";
            else if (winnerId == 2) winner = players[1].name + " Wins!";
            else winner = "It's a Draw!";
            
            renderCenteredText(300, winner, {255, 255, 255, 255}, font);
            renderCenteredText(350, "Press SPACE to Restart", {200, 200, 200, 255}, font);
        }
    }

    SDL_RenderPresent(renderer);
}

void Game::renderText(int x, int y, const std::string& text, SDL_Color color, TTF_Font* f) {
    SDL_Surface* surf = TTF_RenderText_Blended(f, text.c_str(), color);
    if (surf) {
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        if (tex) {
            SDL_Rect dst = {x, y, surf->w, surf->h};
            SDL_RenderCopy(renderer, tex, NULL, &dst);
            SDL_DestroyTexture(tex);
        }
        SDL_FreeSurface(surf);
    }
}

void Game::renderCenteredText(int y, const std::string& text, SDL_Color color, TTF_Font* f) {
    int w, h;
    TTF_SizeText(f, text.c_str(), &w, &h);
    int x = (GameConstants::WINDOW_WIDTH - w) / 2;
    renderText(x, y, text, color, f);
}

void Game::cleanup() {
    if (boyTexture) SDL_DestroyTexture(boyTexture);
    if (girlTexture) SDL_DestroyTexture(girlTexture);
    if (boyDragonTexture) SDL_DestroyTexture(boyDragonTexture);
    if (girlDragonTexture) SDL_DestroyTexture(girlDragonTexture);
    if (boyRhinoTexture) SDL_DestroyTexture(boyRhinoTexture);
    if (girlRhinoTexture) SDL_DestroyTexture(girlRhinoTexture);
    if (backgroundTexture) SDL_DestroyTexture(backgroundTexture);
    if (crystalTexture) SDL_DestroyTexture(crystalTexture);
    if (font) TTF_CloseFont(font);
    if (titleFont) TTF_CloseFont(titleFont);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    
    for (auto& pair : textCache) {
        SDL_DestroyTexture(pair.second);
    }
    textCache.clear();

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    net.cleanup();
}
