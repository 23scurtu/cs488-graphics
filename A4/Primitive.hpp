// Fall 2018

#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <utility>

class Primitive {
public:
  virtual ~Primitive();
  virtual std::pair<float, glm::vec3> collide(glm::vec3 eye, glm::vec3 ray){ return std::make_pair(-1.0f, glm::vec3(0,0,0)); };
};

class Sphere : public Primitive {
public:
  virtual ~Sphere();
};

class Cube : public Primitive {
public:
  virtual ~Cube();
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();
  std::pair<float, glm::vec3> collide(glm::vec3 eye, glm::vec3 ray) override 
  {
    // vec3 d = ray - eye;
    float A = glm::dot(ray - eye, ray - eye);
    float B = 2*glm::dot(ray - eye, eye - m_pos);//glm::dot(ray - eye, eye - m_pos)*2;
    float C = glm::dot(eye - m_pos, eye - m_pos) - m_radius*m_radius;

    // std::cout << B*B - 4*A*C << std::endl;
    // std::cout << "hey" << std::endl;

    if(B*B - 4*A*C < 0) return std::make_pair(-1.0f, glm::vec3(0,0,0));
    else
    {
      float discriminant = std::sqrt(B*B-4*A*C);
      // std::cout << std::min(-2.0f*C/(B+discriminant), -2.0f*C/(B-discriminant)) << std::endl;
      float t = std::min(-2.0f*C/(B+discriminant), -2.0f*C/(B-discriminant));
      return std::make_pair(t, glm::normalize(eye + normalize(ray-eye)*t - m_pos));
    } 
  }

private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size)
  {
  }
  
  virtual ~NonhierBox();

private:
  glm::vec3 m_pos;
  double m_size;
};
