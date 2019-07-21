#include "AreaLight.hpp"
#include <random>
#include "uniform.hpp"
#include <iostream>

using namespace glm;
using namespace std;

std::pair<float, glm::vec3> AreaLight::collide(glm::vec3 eye, glm::vec3 ray) 
{
    // TODO Abstract into a plane primitive!?
    vec3 v = ray - eye;

    std::pair<float, glm::vec3> result = make_pair(-1.0f, vec3(0,0,0));
    // Find where x,y plane intersection occurs, if at all.
    if((eye.z < 0 && v.z <= 0) || ( eye.z > 0 && v.z >= 0 ) || eye.z == 0.0f) return result;
    // else it must collide somewhere

    // eye and v have opposing sign z values
    vec3 collision_point = eye + v*(-1.0f/v.z)*eye.z + vec3(0,0,0.1);
    

    if(collision_point.x <= 0.0f ||
        collision_point.x >= 1.0f ||
        collision_point.y <= 0.0f ||
        collision_point.y >= 1.0f) return result;

    result.first = length(collision_point - eye);
    // TODO Make able to emit on both sides!
    result.second =  vec3(0,0,1);// eye.z >= 0 ? vec3(0,0,1) : vec3(0,0,-1); // Normal is from w/e side eye was on

    // cout << result.second.z << endl;

    return result;
}

vec3 AreaLight::surfacePoint()
{
    if(node) return vec3(node->world_trans * vec4(uni(), uni(), 0, 1));
    return vec3(uni(), uni(), 0);
}

// AreaLight::AreaLight()
// {
// }