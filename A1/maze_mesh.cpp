#include "maze_mesh.hpp"
#include <stddef.h>
#include <cstdio>
#include <algorithm>
#include <vector>
#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;
using namespace std;

MazeMesh::MazeMesh(Maze &maze)
{
    setMaze(maze);
}

void MazeMesh::setMaze(Maze &maze)
{
    walls.clear();
    int m_dim = maze.getDim();
    int i,j;

    for (i=0; i<m_dim; i++) {
		for (j=0; j<m_dim; j++) { 
			if ( maze.getValue(i,j)==1 ) {
                walls.push_back(Cube());
                walls.back().applyTranslation(glm::vec3(i-m_dim/2,0,j-m_dim/2));     
			} else {
				// Do Nothing
			}
		}
	}

    setWallColor(wall_color);
}

void MazeMesh::setWallColor(glm::vec3 color)
{ 
    wall_color = color; 
    for(Cube &wall : walls) wall.setColor(color);
}

glm::vec3 MazeMesh::getWallColor()
{ 
    return wall_color;
}

void MazeMesh::draw(ShaderProgram &shader, mat4 parent)
{
    // For now create cube
    // Cube cube;

    // cube.applyTranslation(glm::vec3(5,0,0));
    // cube.draw(shader);

    // TODO Remove maze_rotation - no longer needed
    // glm::mat4 rotation = glm::rotate(maze_rotation, glm::vec3(0.0f, 1.0f, 0.0f));

    for (Cube &wall: walls) wall.draw(shader, parent);
}