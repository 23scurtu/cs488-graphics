// Winter 2019

#pragma once

#include <vector>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

#include "Primitive.hpp"

struct Triangle
{
	size_t v1;
	size_t v2;
	size_t v3;

	Triangle( size_t pv1, size_t pv2, size_t pv3 )
		: v1( pv1 )
		, v2( pv2 )
		, v3( pv3 )
	{}
};

// #define RENDER_BOUNDING_VOLUMES
#define USE_BOUNDING_VOLUMES

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh( const std::string& fname );
  
private:
	friend class NonhierBox;
	friend class Cube;

  	glm::vec3 min, max;
	float bounding_radius = 0;
	glm::vec3 bounding_center;
	NonhierSphere* bounding_sphere = nullptr;

	glm::vec3 min_components(glm::vec3 a, glm::vec3 b)
	{
		return glm::vec3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
	}

	glm::vec3 max_components(glm::vec3 a, glm::vec3 b)
	{
		return glm::vec3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
	}

	std::vector<glm::vec3> m_vertices;
	std::vector<Triangle> m_faces;

    friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
	std::pair<float, glm::vec3> collide(glm::vec3 eye, glm::vec3 ray) override;
};
