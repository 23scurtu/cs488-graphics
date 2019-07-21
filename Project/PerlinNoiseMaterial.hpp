#include "PhongMaterial.hpp"
#include <glm/glm.hpp>
#include <vector>
#include "uniform.hpp"
#include <math.h>

class PerlinNoiseMaterial : public PhongMaterial {
    glm::vec3 texture_scale;
    size_t grid_size;
    
    std::vector<glm::vec3> gradients;

    glm::vec3 gradient(int x, int y, int z);
    float lerp(float a0, float a1, float w) 
    { return (1.0f - w)*a0 + w*a1; }
    float smooth(float t)
    { return t * t * t * (t * (t * 6 - 15) + 10); }

public:
  PerlinNoiseMaterial(size_t grid_size, glm::vec3 texture_scale, const glm::vec3& kd, const glm::vec3& ks, double shininess, double tc = 0.0f, double n_i = 1.0f, double r_g = 0.0f, double t_g = 0.0f);
  virtual ~PerlinNoiseMaterial();
  float noise(glm::vec3 p);
  float turbulence(glm::vec3 p, float size);
  glm::vec3 color(glm::vec3 *pos = nullptr) override;
};
