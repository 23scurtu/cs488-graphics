// Winter 2019

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess);
  virtual ~PhongMaterial();
  glm::vec3 color(){ return m_kd; }; // TODO Temporary
  glm::vec3 ks(){ return m_ks; }
  float shininess(){ return m_shininess; }

private:
  glm::vec3 m_kd;
  glm::vec3 m_ks;

  double m_shininess;
};
