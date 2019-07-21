// Winter 2019

#pragma once
#include <glm/glm.hpp>

class Material {
public:
  virtual ~Material();
  virtual glm::vec3 color(glm::vec3 *pos = nullptr) = 0;
  virtual glm::vec3 ks() = 0;
  virtual float shininess() = 0;
  virtual float tc() = 0;
  virtual float ir() = 0;
  virtual float r_g() = 0;
  virtual float t_g() = 0;

protected:
  Material();
};
