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

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh( const std::string& fname );
  
private:
	friend class NonhierBox;
	friend class Cube;

	std::vector<glm::vec3> m_vertices;
	std::vector<Triangle> m_faces;

    friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
	std::pair<float, glm::vec3> collide(glm::vec3 eye, glm::vec3 ray) override 
	{
		 std::pair<float, glm::vec3> result(-1,glm::vec3(0,0,0));


		glm::vec3 d = normalize(ray-eye);
		// std::cout << m_faces.size() << std::endl;

		for(Triangle tri: m_faces)
		{
			const glm::vec3 &a = m_vertices[tri.v1];
			const glm::vec3 &b = m_vertices[tri.v2];
			const glm::vec3 &c = m_vertices[tri.v3];
			glm::mat3 A = glm::transpose(glm::mat3{a.x-b.x, a.x-c.x, d.x,
												   a.y-b.y, a.y-c.y, d.y,
												   a.z-b.z, a.z-c.z, d.z});
			float det_A = glm::determinant(A);

			float det_gamma = glm::determinant(glm::transpose(
				glm::mat3{a.x-b.x, a.x-eye.x, d.x,
						  a.y-b.y, a.y-eye.y, d.y,
						  a.z-b.z, a.z-eye.z, d.z,}
			));
			float gamma = det_gamma/det_A;

			float det_t = glm::determinant(glm::transpose(
				glm::mat3{a.x-b.x, a.x-c.x, a.x-eye.x,
						  a.y-b.y, a.y-c.y, a.y-eye.y,
						  a.z-b.z, a.z-c.z, a.z-eye.z,}
			));

			if(gamma < 0 || gamma > 1) continue;

			float det_beta = glm::determinant(glm::transpose(
				glm::mat3{a.x-eye.x, a.x-c.x, d.x,
						  a.y-eye.y, a.y-c.y, d.y,
						  a.z-eye.z, a.z-c.z, d.z,}
			));

			float beta = det_beta/det_A;
			float t = det_t/det_A;

			// std::cout << result.first << std::endl;
			// std::cout << beta << std::endl;

			if(beta > 0 && gamma > 0 && beta + gamma < 1)
			{
				if(result.first == -1 || t < result.first)
				{
					//TODO Check that cross product is correct order.
					result = std::make_pair(t, normalize(glm::cross(c-b, a-b)));
				}
			}
		}

		return result;
	}
};
