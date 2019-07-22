#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <utility>
#include <cmath>
#include "Primitive.hpp"
#include <vector>
#include <glm/ext.hpp>
#include "GeometryNode.hpp"
#include "Light.hpp"
// class Mesh;

class AreaLight : public Primitive {
    // Mesh mesh;
    // vector<vec3> corners;
    // Unit Square, corners are implied to be (0,0,0) and (1,1,0)
    GeometryNode *node = nullptr;
    friend class GeometryNode;

public:
    glm::vec3 color;
    double falloff[3];

    AreaLight();
    void setNode(GeometryNode* node){ this->node = node; }

    virtual ~AreaLight() = default;
    std::pair<float, glm::vec3> collide(glm::vec3 eye, glm::vec3 ray) override;

    bool is_light() override { return true; }
    glm::vec3 surfacePoint() override;
};