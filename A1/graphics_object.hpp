#pragma once

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
#include <glm/gtx/quaternion.hpp>

#include "mesh.hpp"

class GraphicsObject
{
    Mesh* mesh;
    glm::mat4 transform;

    public:
    // TODO Create transform object to wrap trans, rot, and scale so they can be changed independently.
    GraphicsObject(Mesh* mesh, const glm::mat4 &transform): mesh{mesh}, transform{transform} {}
    void setTransform(const glm::mat4 &transform){ this->transform = transform; }
    glm::mat4 getTransform(){ return transform; }

    void draw(ShaderProgram &shader, glm::mat4 parent = glm::mat4(1.0))
    {
        mesh->draw(shader, parent*transform );
    }
};