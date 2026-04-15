#ifndef GAMELEVEL_H
#define GAMELEVEL_H

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>

#include "game_object.hpp"
#include "sprite_renderer.hpp"
#include "resource_manager.h"


class GameLevel
{
public:
    // level state  
    std::vector<GameObject> Bricks;
    // constructor
    GameLevel() {}
    // loads level from file
    void Load(const char *file, unsigned int levelWidth, unsigned int levelHeight);
    // render level
    void Draw(SpriteRenderer &renderer);
    // check if the level is completed (all non-solid tiles are destroyed)
    bool IsCompleted();

private:
    // initialize level from tile data
    void init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);
};

#endif