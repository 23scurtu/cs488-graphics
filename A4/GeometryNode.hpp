// Winter 2019

#pragma once

#include "SceneNode.hpp"
#include "Primitive.hpp"
#include "Material.hpp"
#include <iostream>

class GeometryNode : public SceneNode {
public:
	GeometryNode( const std::string & name, Primitive *prim, 
		Material *mat = nullptr );

	void setMaterial( Material *material );
	std::pair<float, glm::vec3> collide(glm::vec3 eye, glm::vec3 ray) const { return m_primitive->collide(eye, ray); }

	Material *m_material;
	Primitive *m_primitive;
};
