// Winter 2019

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

using namespace glm;
using namespace std;

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

	// #ifdef USE_BOUNDING_VOLUMES
	// 	//First collide with bounding sphere!
	// 	#ifdef RENDER_BOUNDING_VOLUMES
	// 		return bounding_sphere->collide(eye, ray);
	// 	#endif
	// 	#ifndef RENDER_BOUNDING_VOLUMES
	// 		if(bounding_sphere->collide(eye, ray).first == -1) return result;
	// 	#endif
	// #endif

	glm::vec3 d = normalize(ray-eye);
	// std::cout << m_faces.size() << std::endl;

	for(Triangle tri: m_faces)
	{
		const glm::vec3 &v1 = m_vertices[tri.v1];
		const glm::vec3 &v2 = m_vertices[tri.v2];
		const glm::vec3 &v3 = m_vertices[tri.v3];

		// Möller–Trumbore intersection algorithm

		const float EPSILON = 0.00001;

		vec3 ray_vec = normalize(ray - eye);

		vec3 e1 = v2 - v1;
		vec3 e2 = v3 - v1;

		float a, f, u, v;

		vec3 h = cross(ray_vec, e2);
		a = dot(e1, h);

		if(a > -EPSILON && a < EPSILON) continue;
		f = 1.0/a;
		vec3 s = eye - v1;
		u = f * dot(s, h);

		if(u < 0.0f || u > 1.0f) continue;

		vec3 q = cross(s, e1);
		v = f * dot(ray_vec, q);

		if(v < 0.0f || u + v > 1.0f) continue;

		float t = f * dot(e2, q);
		if(t > EPSILON)
		{
			// cout << "aaaaaa" << endl;

			if(result.first == -1 || t < result.first)
			{
				//TODO Check that cross product is correct order.
				result = std::make_pair(t, normalize(cross(e1, e2)));
			}
		}
		// else continue
	}

	return result;
}