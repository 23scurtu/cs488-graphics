// Fall 2018

#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <utility>
#include <cmath>
class Mesh;

// const float EPSILON = 0.00001;

class Primitive {
public:
  virtual ~Primitive();
  virtual std::pair<float, glm::vec3> collide(glm::vec3 eye, glm::vec3 ray){ return std::make_pair(-1.0f, glm::vec3(0,0,0)); };
};

class Sphere : public Primitive {
public:
  virtual ~Sphere();
  std::pair<float, glm::vec3> collide(glm::vec3 eye, glm::vec3 ray) override; 
};

class Cube : public Primitive {
  Mesh *mesh;
public:
  Cube();
  virtual ~Cube();
  std::pair<float, glm::vec3> collide(glm::vec3 eye, glm::vec3 ray) override;
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();
  std::pair<float, glm::vec3> collide(glm::vec3 eye, glm::vec3 ray) override;

private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
  Mesh *mesh;
public:
  NonhierBox(const glm::vec3& pos, double size);

  std::pair<float, glm::vec3> collide(glm::vec3 eye, glm::vec3 ray) override;
  
  virtual ~NonhierBox();

private:
  glm::vec3 m_pos;
  double m_size;
};
