#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.hpp"
#include "game_object.hpp"
#include "ball_object.hpp"
#include "particle_generator.h"
#include <algorithm>
#include <iostream>

// Game-related State data
SpriteRenderer *Renderer;
GameObject *Player;
BallObject *Ball;
ParticleGenerator *Particles;

Game::Game(unsigned int width, unsigned int height)
    : m_state(GAME_ACTIVE), m_keys(), m_width(width), m_height(height), Lives(4), Score(0) {}

Game::~Game()
{
    delete Renderer;
    delete Player;
    delete Ball;
    delete Particles;
}

void Game::Init()
{
    // load shaders
    ResourceManager::LoadShader("../shaders/sprite.vs", "../shaders/sprite.frag", nullptr, "sprite");
    ResourceManager::LoadShader("../shaders/particle.vs", "../shaders/particle.frag", nullptr, "particle");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->m_width),
                                      static_cast<float>(this->m_height), 0.0f, -1.0f, 1.0f);

    // Configure Sprite Shader
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

    // Configure Particle Shader (YOU WERE MISSING THIS!)
    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);
    // set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

    // load textures
    ResourceManager::LoadTexture("../images/background.jpg", false, "background");
    // ResourceManager::LoadTexture("textures/awesomeface.png", true, "face");
    ResourceManager::LoadTexture("../images/awesomeface(2).png", true, "face"); // for .png
    // ResourceManager::LoadTexture("../images/awesomeface.jpeg", false, "face"); // for .jpeg
    ResourceManager::LoadTexture("../images/paddle.png", true, "paddle");
    ResourceManager::LoadTexture("../images/block.png", false, "block");
    ResourceManager::LoadTexture("../images/block_solid.png", false, "block_solid");
    // ResourceManager::LoadTexture("textures/particle.png", true, "particle");
    ResourceManager::LoadTexture("../images/particle.png", true, "particle");

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
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));

    Particles = new ParticleGenerator(
        ResourceManager::GetShader("particle"),
        ResourceManager::GetTexture("particle"),
        500);
}

void Game::Update(float dt)
{
    // update objects
    Ball->Move(dt, this->m_width);
    // check for collisions
    this->DoCollisions();
    // update particles
    Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2.0f));

    // --- 1. CHECK LOSS CONDITION (LIVES) ---
    if (Ball->Position.y >= this->m_height) // did ball reach bottom edge?
    {
        --this->Lives;
        std::cout << "Life Lost! Lives remaining: " << this->Lives << std::endl;

        // Player is dead
        if (this->Lives == 0)
        {
            std::cout << "GAME OVER!" << std::endl;
            exit(-1);
        }
        this->ResetPlayer(); // Always reset paddle and ball position
    }

    // --- 2. CHECK WIN CONDITION (LEVEL PROGRESSION) ---
    if (this->m_state == GAME_ACTIVE && this->Levels[this->Level].IsCompleted())
    {
        this->ResetPlayer(); // Put paddle/ball back in center
        this->Level++;       // Move to next level

        std::cout << "Level " << this->Level << " Completed!" << std::endl;

        // If we beat level 4 (index 3)
        if (this->Level >= 4)
        {
            this->m_state = GAME_WIN;
            this->Score = 1000; // Award points!
            std::cout << "===========================" << std::endl;
            std::cout << "YOU WIN! Final Score: " << this->Score << std::endl;
            std::cout << "===========================" << std::endl;
        }
    }
}

void Game::ProcessInput(float dt)
{
    if (m_state == GAME_ACTIVE)
    {
        float velocity = PLAYER_VELOCITY * dt;
        // move playerboard
        if (m_keys[GLFW_KEY_A])
        {
            if (Player->Position.x >= 0.0f)
            {
                Player->Position.x -= velocity;
                if (Ball->Stuck)
                    Ball->Position.x -= velocity;
            }
        }
        if (m_keys[GLFW_KEY_D])
        {
            if (Player->Position.x <= m_width - Player->Size.x)
            {
                Player->Position.x += velocity;
                if (Ball->Stuck)
                    Ball->Position.x += velocity;
            }
        }
        if (m_keys[GLFW_KEY_SPACE])
            Ball->Stuck = false;
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
        Particles->Draw();
        Ball->Draw(*Renderer);
    }
}

void Game::ResetLevel()
{
    if (this->Level == 0)
        this->Levels[0].Load("../src/levels/one.lvl", this->m_width, this->m_height / 2);
    else if (this->Level == 1)
        this->Levels[1].Load("../src/levels/two.lvl", this->m_width, this->m_height / 2);
    else if (this->Level == 2)
        this->Levels[2].Load("../src/levels/three.lvl", this->m_width, this->m_height / 2);
    else if (this->Level == 3)
        this->Levels[3].Load("../src/levels/four.lvl", this->m_width, this->m_height / 2);
}

void Game::ResetPlayer()
{
    // reset player/ball stats
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(this->m_width / 2.0f - PLAYER_SIZE.x / 2.0f, this->m_height - PLAYER_SIZE.y);
    Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
}

// collision detection
bool CheckCollision(GameObject &one, GameObject &two);
Collision CheckCollision(BallObject &one, GameObject &two);
Direction VectorDirection(glm::vec2 closest);

void Game::DoCollisions()
{
    for (GameObject &box : this->Levels[this->Level].Bricks)
    {
        if (!box.Destroyed)
        {
            Collision collision = CheckCollision(*Ball, box);
            if (std::get<0>(collision)) // if collision is true
            {
                // destroy block if not solid
                if (!box.IsSolid)
                    box.Destroyed = true;
                // collision resolution
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (dir == LEFT || dir == RIGHT) // horizontal collision
                {
                    Ball->Velocity.x = -Ball->Velocity.x; // reverse horizontal velocity
                    // relocate
                    float penetration = Ball->Radius - std::abs(diff_vector.x);
                    if (dir == LEFT)
                        Ball->Position.x += penetration; // move ball to right
                    else
                        Ball->Position.x -= penetration; // move ball to left;
                }
                else // vertical collision
                {
                    Ball->Velocity.y = -Ball->Velocity.y; // reverse vertical velocity
                    // relocate
                    float penetration = Ball->Radius - std::abs(diff_vector.y);
                    if (dir == UP)
                        Ball->Position.y -= penetration; // move ball bback up
                    else
                        Ball->Position.y += penetration; // move ball back down
                }
            }
        }
    }
    // check collisions for player pad (unless stuck)
    Collision result = CheckCollision(*Ball, *Player);
    if (!Ball->Stuck && std::get<0>(result))
    {
        // check where it hit the board, and change velocity based on where it hit the board
        float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
        float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
        float percentage = distance / (Player->Size.x / 2.0f);
        // then move accordingly
        float strength = 2.0f;
        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        // Ball->Velocity.y = -Ball->Velocity.y;
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity); // keep speed consistent over both axes (multiply by length of old velocity, so total strength is not changed)
        // fix sticky paddle
        Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);
    }
}

bool CheckCollision(GameObject &one, GameObject &two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
                      two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
                      two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}

Collision CheckCollision(BallObject &one, GameObject &two) // AABB - Circle collision
{
    // get center point circle first
    glm::vec2 center(one.Position + one.Radius);
    // calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);
    // get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // now that we know the clamped values, add this to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;
    // now retrieve vector between center circle and closest point AABB and check if length < radius
    difference = closest - center;

    if (glm::length(difference) < one.Radius) // not <= since in that case a collision also occurs when object one exactly touches object two, which they are at the end of each collision resolution stage.
        return std::make_tuple(true, VectorDirection(difference), difference);
    else
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

// calculates which direction a vector is facing (N,E,S or W)
Direction VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),  // up
        glm::vec2(1.0f, 0.0f),  // right
        glm::vec2(0.0f, -1.0f), // down
        glm::vec2(-1.0f, 0.0f)  // left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}