// Winter 2019

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material {
  float m_tc = 0.0f;
  float m_ir = 1.0f;
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess, double tc = 0.0f, double n_i = 1.0f);
  virtual ~PhongMaterial();
  glm::vec3 color(){ return m_kd; }; // TODO Temporary
  glm::vec3 ks(){ return m_ks; }
  float shininess(){ return m_shininess; }
  float tc(){ return m_tc; }
  float ir(){ return m_ir; }
private:
  glm::vec3 m_kd;
  glm::vec3 m_ks;

  double m_shininess;
};
