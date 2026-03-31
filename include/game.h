#ifndef GAME_H // GAME_H isn't variable, doesn't store  data, doesn't exist at run time
#define GAME_H
// are header guards, used to prevent multiple inclusion of the same header file.
// When the preprocessor sees:
// #define GAME_H
// it simply remembers:
// “GAME_H is now defined.”
// Later, when it encounters:
// #ifndef GAME_H
// it checks:
// “Has GAME_H been defined already?”
// If no → include the code
// If yes → skip the code

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game { 
    public: 
    // game state
    GameState m_state;
    bool m_keys[1024];
    unsigned int m_width, m_height;
    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void init();
    // game loop
    void processInput(float dt);
    void update(float dt);
    void render();

};

#endif