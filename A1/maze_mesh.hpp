#pragma once
#include "maze.hpp"
#include "mesh.hpp"
#include <vector>
#include "graphics_object.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "cs488-framework/ShaderProgram.hpp"

using namespace std;

class MazeMesh
{
    size_t wall_height = 1;
    Cube* wall_mesh;
    vector<GraphicsObject> walls;
    glm::vec3 wall_color;

public:
    MazeMesh(Maze &maze);
    ~MazeMesh() = default;

    void setWallColor(glm::vec3 color);
    void setMaze(Maze &maze);
    glm::vec3 getWallColor();

    void draw(ShaderProgram &shader, mat4 parent=mat4(1.0f));
    void changeWallHeight(int d_height);
};