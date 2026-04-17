#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.hpp"
#include "game_object.hpp"

// Game-related State data
SpriteRenderer *Renderer;
GameObject *Player;

Game::Game(unsigned int width, unsigned int height)
    : m_state(GAME_ACTIVE), m_keys(), m_width(width), m_height(height)
{
}

Game::~Game()
{
    delete Renderer;
}

void Game::Init()
{
    // load shaders
    ResourceManager::LoadShader("../shaders/sprite.vs", "../shaders/sprite.frag", nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->m_width),
                                      static_cast<float>(this->m_height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

    // load textures
    ResourceManager::LoadTexture("../images/background.jpg", false, "background");
    // ResourceManager::LoadTexture("textures/awesomeface.png", true, "face");
    // ResourceManager::LoadTexture("../images/awesomeface(2).png", true, "face"); // for .png
    ResourceManager::LoadTexture("../images/awesomeface.jpeg", false, "face"); // for .jpeg
    ResourceManager::LoadTexture("../images/paddle.png", true, "paddle");       
    ResourceManager::LoadTexture("../images/block.png", false, "block");
    ResourceManager::LoadTexture("../images/block_solid.png", false, "block_solid");
    // load levels
    GameLevel one;
    one.Load("../src/levels/one.lvl", m_width, m_height / 2);
    GameLevel two;
    two.Load("../src/levels/two.lvl", m_width, m_height / 2);
    GameLevel three;
    three.Load("../src/levels/three.lvl", m_width, m_height / 2);
    GameLevel four;
    four.Load("../src/levels/four.lvl", m_width, m_height / 2);
    Levels.push_back(one);
    Levels.push_back(two);
    Levels.push_back(three);
    Levels.push_back(four);
    Level = 0;

    glm::vec2 playerPos = glm::vec2(m_width / 2.0f - PLAYER_SIZE.x / 2.0f, m_height - PLAYER_SIZE.y);
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
}

void Game::Update(float dt)
{
}

void Game::ProcessInput(float dt)
{
    if (this->m_state == GAME_ACTIVE)
    {
        float velocity = PLAYER_VELOCITY * dt;
        // move playerboard
        if (this->m_keys[GLFW_KEY_A])
        {
            if (Player->Position.x >= 0.0f)
                Player->Position.x -= velocity;
        }
        if (this->m_keys[GLFW_KEY_D])
        {
            if (Player->Position.x <= this->m_width - Player->Size.x)
                Player->Position.x += velocity;
        }
    }
}

void Game::Render()
{
    if (m_state == GAME_ACTIVE)
    {
        // draw background
        Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(m_width, m_height), 0.0f);
        // draw level
        Levels[Level].Draw(*Renderer);
        Player->Draw(*Renderer);
    }
}