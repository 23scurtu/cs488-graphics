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
#include <iostream>

using namespace glm;
using namespace std;

MazeMesh::MazeMesh(Maze &maze)
{
    wall_mesh = new Cube();
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
                mat4 W;
                W = glm::translate( W, glm::vec3(i-m_dim/2,0,j-m_dim/2));
                walls.push_back(GraphicsObject(wall_mesh, W)); 
			} else {
				// Do Nothing
			}
		}
	}

    setWallColor(wall_color);
}

void MazeMesh::setWallColor(glm::vec3 color)
{ 
    // cout << color.b << endl;
    wall_color = color; 

    // TODO Make color part of graphics object?
    //for(GraphicsObject &wall : walls) wall.setColor(color);
    wall_mesh->setColor(color);
}

glm::vec3 MazeMesh::getWallColor()
{ 
    return wall_color;
}

void MazeMesh::draw(ShaderProgram &shader, mat4 parent)
{
    for (GraphicsObject &wall: walls) wall.draw(shader, parent);
}

void MazeMesh::changeWallHeight(int d_height)
{
    if(int(wall_height) + d_height <= 0) return;
    wall_height += d_height;

    wall_mesh->setScale(vec3(1,wall_height,1));
    // wall_mesh->applyTranslation(vec3(0, d_height, 0));
}