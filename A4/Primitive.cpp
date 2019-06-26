// Winter 2019

#include "Primitive.hpp"
#include "Mesh.hpp"

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

Cube::~Cube()
{
}

NonhierBox::NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size), mesh(new Mesh("cube.obj"))
    //TODO Change to not be assets folder
{
    for(glm::vec3 &p: mesh->m_vertices)
    {
        p *= size;
        p += pos;
    }
}

Cube::Cube(): mesh(new Mesh("cube.obj"))
    //TODO Change to not be assets folder
{
}

std::pair<float, glm::vec3> Cube::collide(glm::vec3 eye, glm::vec3 ray)
{
    auto result = mesh->collide(eye, ray);
    // std::cout << result.first << std::endl;
    return result;
}

std::pair<float, glm::vec3> NonhierBox::collide(glm::vec3 eye, glm::vec3 ray)
{
    auto result = mesh->collide(eye, ray);
    // std::cout << result.first << std::endl;
    return result;
}

NonhierSphere::~NonhierSphere()
{
}

NonhierBox::~NonhierBox()
{
    delete mesh; 
}
