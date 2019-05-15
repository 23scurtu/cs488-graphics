#pragma once
#include "maze.hpp"
#include "mesh.hpp"
#include <vector>

#include "cs488-framework/ShaderProgram.hpp"

using namespace std;

class MazeMesh
{
    vector<Cube> walls;
    glm::vec3 wall_color;

public:
    float maze_rotation = 0.0f;

    MazeMesh(Maze &maze);
    ~MazeMesh() = default;

    void setWallColor(glm::vec3 color);
    void setMaze(Maze &maze);
    glm::vec3 getWallColor();

    void draw(ShaderProgram &shader, mat4 parent=mat4(1.0f));
};