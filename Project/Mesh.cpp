// Winter 2019

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	std::ifstream ifs( fname.c_str() );
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
			min = min_components(min, glm::vec3( vx, vy, vz ));
			max = max_components(max, glm::vec3( vx, vy, vz ));
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}

	bounding_center = min + (max-min)/2;
	bounding_radius = length(max-min)/2 + 0.00001;

	bounding_sphere = new NonhierSphere(bounding_center, bounding_radius);
	// std::cout <<"mesh size " << m_faces.size() << std::endl;
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}

std::pair<float, glm::vec3> Mesh::collide(glm::vec3 eye, glm::vec3 ray)
{
	std::pair<float, glm::vec3> result(-1,glm::vec3(0,0,0));

	#ifdef USE_BOUNDING_VOLUMES
		//First collide with bounding sphere!
		#ifdef RENDER_BOUNDING_VOLUMES
			return bounding_sphere->collide(eye, ray);
		#endif
		#ifndef RENDER_BOUNDING_VOLUMES
			if(bounding_sphere->collide(eye, ray).first == -1) return result;
		#endif
	#endif

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