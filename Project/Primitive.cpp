// Winter 2019

#include "Primitive.hpp"
#include "Mesh.hpp"
#include "A4.hpp"

const float EPSILON = 0.01;

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

std::pair<float, glm::vec3> Sphere::collide(glm::vec3 eye, glm::vec3 ray)
{
    glm::vec3 d = normalize(ray - eye);

    float A = glm::dot(d, d);
    float B = 2*glm::dot(d, eye);//glm::dot(ray - eye, eye - m_pos)*2;
    float C = glm::dot(eye, eye) - 1.0f;

    if(B*B - 4*A*C < 0.0f) return std::make_pair(-1.0f, glm::vec3(0,0,0));
    else if (B*B - 4.0f*A*C == 0.0f)
    {
        float t = -2.0f*C/B;
        return std::make_pair(t, eye + d*t);
    }
    else
    {
        float discriminant = std::sqrt(B*B-4*A*C);
        // std::cout << discriminant << std::endl;

        // std::cout << std::min(-2.0f*C/(B+discriminant), -2.0f*C/(B-discriminant)) << std::endl;
        float a = B+discriminant;
        float b = B-discriminant;

        if(a == 0.0f) a = b;
        if(b == 0.0f) b = a;
        if(a == b && a == 0.0f) return std::make_pair(-1.0f, glm::vec3(0,0,0));

        // TODO Check if correct to be pos
        // if(a == 0.0f) a = 0.00001f;
        // if(b == 0.0f) b = 0.00001f;

        float first_int = -2.0f*C/a;
        float second_int = -2.0f*C/b;

        float t;
        if(first_int < EPSILON && second_int < EPSILON) return std::make_pair(-1.0f, glm::vec3(0,0,0));
        else if(first_int < EPSILON) t = second_int;
        else if(second_int <  EPSILON) t = first_int;
        else t = std::min(first_int, second_int);

        // float t = std::min(-2.0f*C/a, -2.0f*C/b);

        return std::make_pair(t, (eye + d*t)); //float(1.0f/m_radius)
    } 
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

std::pair<float, glm::vec3> NonhierSphere::collide(glm::vec3 eye, glm::vec3 ray)
{
    glm::vec3 d = normalize(ray - eye);
    // if(ray - eye == glm::vec3(0,0,0)) return std::make_pair(-1.0f, glm::vec3(0,0,0));
    // glm::vec3 d = glm::normalize(ray - eye);

    float A = glm::dot(d, d);
    float B = 2*glm::dot(d, eye - m_pos);//glm::dot(ray - eye, eye - m_pos)*2;
    float C = glm::dot(eye - m_pos, eye - m_pos) - m_radius*m_radius;
    float inv_radius = 1.0f/m_radius;

    if(B*B - 4*A*C < 0.0f) return std::make_pair(-1.0f, glm::vec3(0,0,0));
    else if (B*B - 4.0f*A*C == 0.0f)
    {
        float t = -2.0f*C/B;
        return std::make_pair(t, inv_radius*(eye + d*t - m_pos));
    }
    else
    {
        float discriminant = std::sqrt(B*B-4*A*C);
        // std::cout << discriminant << std::endl;

        // std::cout << std::min(-2.0f*C/(B+discriminant), -2.0f*C/(B-discriminant)) << std::endl;
        float a = B+discriminant;
        float b = B-discriminant;

        // TODO Check if correct to be pos
        // if(a == 0.0f) a = 0.00001f;
        // if(b == 0.0f) b = 0.00001f;
        if(a == 0.0f) a = b;
        if(b == 0.0f) b = a;
        if(a == b && a == 0.0f) return std::make_pair(-1.0f, glm::vec3(0,0,0));

        float first_int = -2.0f*C/a;
        float second_int = -2.0f*C/b;

        // std::cout << first_int << std::endl;

        float t;
        if(first_int < EPSILON && second_int < EPSILON) return std::make_pair(-1.0f, glm::vec3(0,0,0));
        else if(first_int < EPSILON) t = second_int;
        else if(second_int < EPSILON) t = first_int;
        else t = std::min(first_int, second_int);

        // float t = std::min(-2.0f*C/a, -2.0f*C/b);

        return std::make_pair(t, inv_radius*(eye + d*t - m_pos)); //float(1.0f/m_radius)
    } 
}

NonhierBox::~NonhierBox()
{
    delete mesh; 
}
