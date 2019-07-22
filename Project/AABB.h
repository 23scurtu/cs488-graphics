#pragma once

#include <vector>
#include <glm/ext.hpp>

using namespace std;
using namespace glm;

glm::vec3 min_components(glm::vec3 a, glm::vec3 b);
glm::vec3 max_components(glm::vec3 a, glm::vec3 b);

struct AABB
{
    vec3 min, max;
    vec3 midpoint;

    vector<vec3> verts;

    AABB() = default;

    AABB(vec3 min, vec3 max): min{min}, max{max} 
    {
        verts.reserve(8);
        verts.push_back(vec3(min.x,min.y,min.z));
        verts.push_back(vec3(min.x,min.y,max.z));
        verts.push_back(vec3(min.x,max.y,min.z));
        verts.push_back(vec3(min.x,max.y,max.z));
        verts.push_back(vec3(max.x,min.y,min.z));
        verts.push_back(vec3(max.x,min.y,max.z));
        verts.push_back(vec3(max.x,max.y,min.z));
        verts.push_back(vec3(max.x,max.y,max.z));

        midpoint = vec3( min.x + (max.x - min.x)/2,
                         min.y + (max.y - min.y)/2,
                         min.z + (max.z - min.z)/2);
    }

    AABB transform(mat4 trans)
    {
        vec3 new_min, new_max;
        vector<vec3> new_verts;
        new_verts.reserve(8);

        bool first = true;
        for(auto vert: verts)
        {
            auto new_vert = vec3(trans*vec4(vert, 1));

            if(first)
            {
                new_min = new_vert;
                new_max = new_vert;
                first = false;
            }
            else
            {
                new_min = min_components(new_min, new_vert);
                new_max = max_components(new_max, new_vert);
            }
        }

        return AABB(new_min, new_max);
    }

    bool collide(glm::vec3 eye, glm::vec3 ray)
    {
        // Modified from https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms

        // v is unit direction vector of ray
        vec3 v = normalize(ray - eye);
        vec3 dirfrac(1.0f / v.x, 1.0f / v.y, 1.0f / v.z);

        // min is the corner of AABB with minimal coordinates - left bottom, max is maximal corner
        // eye is origin of ray
        float t1 = (min.x - eye.x)*dirfrac.x;
        float t2 = (max.x - eye.x)*dirfrac.x;
        float t3 = (min.y - eye.y)*dirfrac.y;
        float t4 = (max.y - eye.y)*dirfrac.y;
        float t5 = (min.z - eye.z)*dirfrac.z;
        float t6 = (max.z - eye.z)*dirfrac.z;

        float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
        float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

        return !((tmax < 0) || (tmin > tmax));
    }
};