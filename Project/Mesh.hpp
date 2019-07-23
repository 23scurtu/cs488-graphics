// Winter 2019

#pragma once

#include <vector>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

#include "Primitive.hpp"
#include "lodepng/lodepng.h"
#include <unordered_map>
#include <thread>
#include <mutex>

// enum TextureType
// {
// 	DIFFUSE_COLOR,
// 	NORMAL_MAP
// };

struct Texture
{
	const float inverse_pixels = 1.0f/255.0f;
	unsigned width = 0;
	unsigned height = 0;
	std::vector<unsigned char> data;

	Texture(std::string filename/*, TextureType texture_type = DIFFUSE_COLOR*/)
	{
		//decode
		unsigned error = lodepng::decode(data, width, height, filename.c_str());

		//if there's an error, display it
		if(error)
		{
			std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
			width = 0;
			height = 0;
			data.clear();
		} 
	}

	glm::vec3 color(size_t x, size_t y)
	{
		
		if(height != 0 && width != 0)
		{
			size_t i = (height - 1 - y)*width*4 + 4*x;
			return inverse_pixels*glm::vec3(data[i], data[i+1], data[i+2]);
		} 
		return glm::vec3(0,0,0);
	}
};

struct OBJMaterial
{
	Texture* texture_map = nullptr;
	Texture* normal_map = nullptr;
 
	OBJMaterial() = default;
	OBJMaterial(Texture* texture_map, Texture* normal_map): texture_map{texture_map}, normal_map{normal_map} {}
};

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

struct TangentBasis
{
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 bitangent;

	glm::mat3 TBN;

	TangentBasis() = default;
	TangentBasis( glm::vec3 normal, glm::vec3 tangent, glm::vec3 bitangent )
		: normal( normal )
		, tangent( tangent )
		, bitangent( bitangent )
	{}

	void calcTBN()
	{
		TBN = glm::transpose(glm::mat3{tangent.x, bitangent.x, normal.x,
						tangent.y, bitangent.y, normal.y,
						tangent.z, bitangent.z, normal.z}); 
						//TBN Should be whats visually seen here transposed (glm is already transposed)
	}
};

// #define RENDER_BOUNDING_VOLUMES
#define USE_BOUNDING_VOLUMES

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh( const std::string& fname , bool PHONG_SHADING = false);
  
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

	bool PHONG_SHADING = false;

	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec3> m_normals;

	std::vector<glm::vec2> m_texture_coords;

	// TODO Make names more descriptive
	std::vector<Triangle> m_face_textures;
	std::vector<Triangle> m_faces;
	std::vector<Triangle> m_face_normals;

	std::vector<int> m_material_ids;
	// std::vector<int> m_normal_map_ids;
	// std::vector<Texture> m_textures;
	// std::vector<Texture> m_normal_maps;
	std::vector<OBJMaterial> m_materials;

	std::vector<TangentBasis> m_tangents;

	mutex last_hit_mutex;
	unordered_map<thread::id, glm::vec2> thread_last_hit_uv_coords;
	unordered_map<thread::id, int> thread_last_hit_index;

    friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
	std::pair<float, glm::vec3> collide(glm::vec3 eye, glm::vec3 ray) override;
	glm::vec3 getLastHitColor() override;
	glm::vec3 getLastHitNormal(glm::vec3 *interpolated_normal = nullptr) override;
};
