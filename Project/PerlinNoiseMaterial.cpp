#include "PerlinNoiseMaterial.hpp"
#include <iostream>

using namespace std;
using namespace glm;

PerlinNoiseMaterial::PerlinNoiseMaterial(size_t grid_size, vec3 texture_scale, const glm::vec3& kd, const glm::vec3& ks, double shininess, double tc, double n_i, double r_g, double t_g):
PhongMaterial(kd, ks, shininess, tc, n_i, r_g, t_g), texture_scale{texture_scale}, grid_size{grid_size}
{
    assert(grid_size % 2 == 0);
    size_t total_gradients = grid_size*grid_size*grid_size;
    gradients.reserve(total_gradients);

    for(size_t i = 0; i < total_gradients; i++)
    {
        vec3 gradient = vec3(1,1,1);

        while(length(gradient) > 1) gradient = vec3(2*uni()-1, 2*uni()-1, 2*uni()-1);

        gradients.push_back(normalize(gradient));
    }
}

int euclidean_reminder(int a, int b)
{
  assert(b != 0);
  int r = a % b;
  return r >= 0 ? r : r + std::abs(b);
}

glm::vec3 PerlinNoiseMaterial::gradient(int x, int y, int z)
{
    x = euclidean_reminder( x , grid_size);
    y = euclidean_reminder( y , grid_size);
    z = euclidean_reminder( z , grid_size);

    return gradients[z*grid_size*grid_size + y*grid_size + x];
}

PerlinNoiseMaterial::~PerlinNoiseMaterial()
{
}

float PerlinNoiseMaterial::noise(glm::vec3 p)
{
    vec3 scaled_pos = vec3(p.x/texture_scale.x, p.y/texture_scale.y, p.z/texture_scale.z);
    
    p = scaled_pos;

    vec3 i = vec3(floor(p.x), floor(p.y), floor(p.z));
    
    float c1 = dot(gradient(i.x, i.y, i.z),       p - i);
    float c2 = dot(gradient(i.x, i.y, i.z+1),     p - vec3(i.x, i.y, i.z+1));
    float c3 = dot(gradient(i.x, i.y+1, i.z),     p - vec3(i.x, i.y+1, i.z));
    float c4 = dot(gradient(i.x, i.y+1, i.z+1),   p - vec3(i.x, i.y+1, i.z+1));
    float c5 = dot(gradient(i.x+1, i.y, i.z),     p - vec3(i.x+1, i.y, i.z));
    float c6 = dot(gradient(i.x+1, i.y, i.z+1),   p - vec3(i.x+1, i.y, i.z+1));
    float c7 = dot(gradient(i.x+1, i.y+1, i.z),   p - vec3(i.x+1, i.y+1, i.z));
    float c8 = dot(gradient(i.x+1, i.y+1, i.z+1), p - vec3(i.x+1, i.y+1, i.z+1));

    float u = p.x - floor(p.x);
    float v = p.y - floor(p.y);
    float w = p.z - floor(p.z);

    u = smooth(u);
    v = smooth(v);
    w = smooth(w);

    float x1 = lerp(c1, c5, u);
    float x2 = lerp(c2, c6, u);
    float x3 = lerp(c3, c7, u);
    float x4 = lerp(c4, c8, u);

    float y1 = lerp(x1, x3, v);
    float y2 = lerp(x2, x4, v);

    float avg = lerp(y1, y2, w);

    return avg; // Temp
}

float PerlinNoiseMaterial::turbulence(glm::vec3 p, float size)
{
    float t = 0;
    float scale = size;
    // TODO Make texture scale float
    float pixel_size = texture_scale.x/grid_size;

    while(scale >= pixel_size)
    {
        t += noise(p/scale)*scale;
        scale /= 2;
    }

    return t;

    ///////////////////////////////////

    // float t = 0;
    // float initial_size = size;
    // // TODO Make texture scale float

    // int i = 0;

    // while(size >= 1)
    // {
    //     i++;
    //     t += noise(p*(1.0f/size))*size;
    //     size /= 2;
    // }

    // cout << t/i << endl;

    // return t/i;

    ///////////////////////////////////

    // float t = 0;
    // float initial_size = size;
    // // TODO Make texture scale float
    // float pixel_size = 1.0f/grid_size;

    // int i = 0;

    // while(size >= grid_size)
    // {
    //     i++;
    //     t += noise(p*(1.0f/size))*size;
    //     size /= 2;
    // }

    // cout << t/i << endl;

    // return t/i;
}

glm::vec3 PerlinNoiseMaterial::color(glm::vec3 *pos)
{
    if(!pos) return PhongMaterial::color();
    // float n = turbulence(*pos);
    float x_period = 1;
    float y_period = 2;
    float turb_power = 2;

    float pattern_period = 0.5;

    float xyz_val = pos->x*x_period + pos->y*y_period + turb_power*turbulence(*pos, 32);

    float n = (sin(xyz_val * M_PI * pattern_period) + 1)/2;

    // n = turbulence(*pos, 32);
    // n = noise(*pos);

    n = 1.0f - n*n*n;

    float marble_color = 0.65;

    return vec3(n,n,n)*(1.0f - marble_color) + marble_color; // Temp
}