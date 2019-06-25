// Winter 2019

#pragma once
#include <glm/glm.hpp>

class Material {
public:
  virtual ~Material();
  virtual glm::vec3 color() = 0;

protected:
  Material();
};
