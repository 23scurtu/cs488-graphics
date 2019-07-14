// Winter 2019

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

using namespace glm;
using namespace std;

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
	// std::string code;
	// double vx, vy, vz;
	// size_t s1, s2, s3;

	// std::ifstream ifs( fname.c_str() );
	// while( ifs >> code ) {
	// 	if( code == "v" ) {
	// 		ifs >> vx >> vy >> vz;
	// 		m_vertices.push_back( glm::vec3( vx, vy, vz ) );
	// 		min = min_components(min, glm::vec3( vx, vy, vz ));
	// 		max = max_components(max, glm::vec3( vx, vy, vz ));
	// 	} else if( code == "f" ) {
	// 		ifs >> s1 >> s2 >> s3;
	// 		m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
	// 	}
	// }

	// bounding_center = min + (max-min)/2;
	// bounding_radius = length(max-min)/2 + 0.00001;

	// bounding_sphere = new NonhierSphere(bounding_center, bounding_radius);

	//////////////////////////////////////////////////////////////////////////

	std::string inputfile = fname;
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	// Refer to https://github.com/syoyo/tinyobjloader for tinyobjloader help.

	std::string warn;
	std::string err;

	std::size_t found = inputfile.find_last_of("/\\");
	string basedir = inputfile.substr(0,found);

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str(), basedir.c_str());

	if (!warn.empty()) {
	std::cout << warn << std::endl;
	}

	if (!err.empty()) {
	std::cerr << err << std::endl;
	}

	if (!ret) {
	exit(1);
	}

	for(size_t i = 0; i < attrib.vertices.size(); i+=3)
	{
		tinyobj::real_t vx = attrib.vertices[i+0];
		tinyobj::real_t vy = attrib.vertices[i+1];
		tinyobj::real_t vz = attrib.vertices[i+2];

		m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		// cout << vx << ", " << vy << ", " << vz << endl;
	}

	for(size_t i = 0; i < attrib.texcoords.size(); i+=2)
	{
		tinyobj::real_t u = attrib.texcoords[i+0];
		tinyobj::real_t v = attrib.texcoords[i+1];

		m_texture_coords.push_back( glm::vec2( u, v ) );
		// cout << u << ", " << v << ", " << endl;
	}

	for(auto material: materials)
	{
		m_textures.emplace_back( basedir + "/" + material.diffuse_texname );
	}

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) 
	{
		// cout << shapes[s].mesh.indices.size() << endl;
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) 
		{
			int fv = shapes[s].mesh.num_face_vertices[f];

			if(fv != 3)
			{
				cerr << "Attempting to load non-triangular polygon faces from obj file " << fname << endl;
				exit(1);
			}

			// Loop over vertices in the face.
			m_faces.push_back( Triangle( shapes[s].mesh.indices[index_offset].vertex_index,
										 shapes[s].mesh.indices[index_offset+1].vertex_index,
										 shapes[s].mesh.indices[index_offset+2].vertex_index));

			m_face_textures.push_back( Triangle( shapes[s].mesh.indices[index_offset].texcoord_index,
												 shapes[s].mesh.indices[index_offset + 1].texcoord_index,
												 shapes[s].mesh.indices[index_offset + 2].texcoord_index ));

			// TODO Load in texture!

			// for (size_t v = 0; v < fv; v++) {
			// // access to vertex
			// tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
			// tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
			// tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
			// tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];

			// tinyobj::real_t nx = attrib.normals[3*idx.normal_index+0];
			// tinyobj::real_t ny = attrib.normals[3*idx.normal_index+1];
			// tinyobj::real_t nz = attrib.normals[3*idx.normal_index+2];

			// tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
			// tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];

			// Optional: vertex colors
			// tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
			// tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
			// tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
			// }

			index_offset += fv;

			// per-face material
			int material_id = shapes[s].mesh.material_ids[f];
			// if(material_id != -1) // cout << shapes[s].mesh.material_ids[f] << endl;
				// cout << materials[material_id].diffuse_texname << endl;
				// cout << materials.size() << endl;
				m_texture_ids.push_back(material_id);
			// cout << "aaaaaaaaaaaaaaaaaaaaaa" << material_id << endl;
		}
	}

	if(!m_texture_coords.empty() && !materials.empty())
	{
		m_textured = true;
	} 
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

	for(int tri_i = 0; tri_i != m_faces.size(); tri_i++)
	{
		Triangle tri = m_faces[tri_i];

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
				// cout << u << ", " << v << endl;

				if(m_textured)
				{
					Triangle tri_texture = m_face_textures[tri_i];

					// last_hit_uv_coords = vec2(u,v);
					last_hit_uv_coords = (1 - u - v) * m_texture_coords[tri_texture.v1] + 
										 u * m_texture_coords[tri_texture.v2] + 
										 v * m_texture_coords[tri_texture.v3];

					last_hit_index = tri_i;
				}

			}
		}
		// else continue
	}

	return result;
}

glm::vec3 Mesh::getLastHitColor()
{ 
	// cout << last_hit_uv_coords.x << ", " << last_hit_uv_coords.y << endl;
	// TODO No interpolation, simply casts float to int
	if(last_hit_index == -1) return vec3(0,0,0);
	Texture & tex = m_textures[m_texture_ids[ last_hit_index]];

	// cout << "cccccccccccccc" << last_hit_index << endl;

	return tex.color(last_hit_uv_coords.x*tex.width, last_hit_uv_coords.y*tex.height); 
	// TODO Seg faulting due to image not having any elements!
	// cout << m_mat << endl;
	// return vec3(last_hit_uv_coords.x, last_hit_uv_coords.y, 1 - last_hit_uv_coords.x - last_hit_uv_coords.y); 
}