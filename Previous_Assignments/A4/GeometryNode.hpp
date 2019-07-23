// Winter 2019

#pragma once

#include "SceneNode.hpp"
#include "Primitive.hpp"
#include "Material.hpp"
#include <iostream>
#include "Mesh.hpp"
#include <utility>
#include <glm/glm.hpp>

class GeometryNode : public SceneNode {
public:
	GeometryNode( const std::string & name, Primitive *prim, 
		Material *mat = nullptr );

	void setMaterial( Material *material );
	std::pair<float, glm::vec3> collide(glm::vec3 eye, glm::vec3 ray) const { 
		#ifdef RENDER_BOUNDING_VOLUMES
		if(this->m_name == "plane") return std::make_pair(-1.0f, glm::vec3(0,0,0));
		return m_primitive->collide(eye, ray); 
		#endif
		#ifndef RENDER_BOUNDING_VOLUMES
		return m_primitive->collide(eye, ray); 
		
		#endif
	}

	Material *m_material;
	Primitive *m_primitive;
};
