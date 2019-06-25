// Winter 2019

#include <glm/ext.hpp>
#include <vector>
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "SceneNode.hpp"
#include <iostream>
#include <utility>

#include "A4.hpp"

using namespace std;
using namespace glm;

vec3 rayColor(vec3 eye, vec3 ray, const glm::vec3 & ambient, const std::list<Light *> & lights, SceneNode * root);
void pvec(vec3 v) { cout << v.x << ", " << v.y << ", " << v.z << endl; }

int cnt1 = 0;
int cnt2 = 0;

void A4_Render(
		// What to render  
		SceneNode * root,

		// Image to write to, set to a given width and height  
		Image & image,

		// Viewing parameters  
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters  
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
) {

  // Fill in raytracing code here...  

  float d = 5;
  float nx = image.width();
  float ny = image.height();

  std::vector<std::vector<vec3>> rays(image.height(), std::vector<vec3>(image.width(), vec3(0,0,0)));

  float window_h = 2.0f*d*tan(degreesToRadians(fovy)/2);

  vec3 camera_w = normalize(view - eye);
  vec3 camera_u = normalize(cross(up, camera_w));
  vec3 camera_v = normalize(cross(camera_w, camera_u));

  mat3 camera_rotation = mat3{
	  camera_u.x, camera_v.x, camera_w.x,
	  camera_u.y, camera_v.y, camera_w.y,
	  camera_u.z, camera_v.z, camera_w.z,
  };
  camera_rotation = transpose(camera_rotation);

  for(int y = 0; y != image.height(); y++)
  {
	for(int x = 0; x != image.width(); x++)
	{
		rays[y][x] = vec3(x, y, 0);
		rays[y][x] += vec3(-float(image.height())/2,-float(image.height())/2,d);
		// TODO Check if scales
		rays[y][x] *= vec3(-window_h/nx, window_h/ny, 1);
		rays[y][x] = camera_rotation * rays[y][x];
		rays[y][x] = normalize(rays[y][x]);
		rays[y][x] += eye;
	}
  }

  std::cout << "Calling A4_Render(\n" <<
		  "\t" << *root <<
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
          "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
		  "\t" << "view: " << glm::to_string(view) << std::endl <<
		  "\t" << "up:   " << glm::to_string(up) << std::endl <<
		  "\t" << "fovy: " << fovy << std::endl <<
          "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
		  "\t" << "lights{" << std::endl;

	for(const Light * light : lights) {
		std::cout << "\t\t" <<  *light << std::endl;
	}
	std::cout << "\t}" << std::endl;
	std:: cout <<")" << std::endl;

	size_t h = image.height();
	size_t w = image.width();

	// for (uint y = 0; y < h; ++y) {
	// 	for (uint x = 0; x < w; ++x) {
	// 		// Red: 
	// 		image(x, y, 0) = (double)1.0;
	// 		// Green: 
	// 		image(x, y, 1) = (double)1.0;
	// 		// Blue: 
	// 		image(x, y, 2) = (double)1.0;
	// 	}
	// }

	for(int y = 0; y != image.height(); y++)
	{
		for(int x = 0; x != image.width(); x++)
		{
			vec3 ray = rays[y][x];

			// cout << ray.x << ", " << ray.y << ", " << ray.z << endl;

			vec3 color = rayColor(eye, ray, ambient, lights, root);

			image(x,y,0) = color.r;
			image(x,y,1) = color.g;
			image(x,y,2) = color.b;
		}
	}

	cout << cnt1 << endl;
	cout << cnt2 << endl;
}

struct Collision
{
	float t = -1;
	vec3 normal;
	GeometryNode* object = nullptr;

	Collision() = default;
	Collision(float t, vec3 normal, GeometryNode* object): t{t}, normal{normal}, object{object} {}
};

Collision collide(vec3 eye, vec3 ray, SceneNode *root, bool between_points = false)
{
	Collision result;
	// cout << result.t <<endl;

	// std::cout << "hey" << std::endl;

	if(root->m_nodeType == NodeType::GeometryNode)
	{
		GeometryNode * geometryNode = static_cast<GeometryNode *>(root);

		auto collision = geometryNode->collide(eye, ray);
		// cout << collision << endl;
		if(collision.first != -1)
		{
			if((between_points && collision.first <= length(ray-eye) && collision.first >= 0) || !between_points)
			{
				result.t = collision.first;
				result.object = geometryNode;
				result.normal = collision.second;
			}
		}
		else if (between_points && collision.first <= length(ray-eye) && collision.first >= 0)
		{
			
		}
	}

	for ( auto child: root->children)
	{
		auto child_result = collide(eye, ray, child, between_points);
		if(child_result.object && (!result.object || child_result.t < result.t)) result = child_result;
	}

	return result;
}

vec3 rayColor(vec3 eye, vec3 ray, const glm::vec3 & ambient, const std::list<Light *> & lights, SceneNode * root)
{
	vec3 result(0,0,0);
	vec3 diffuse_light(0,0,0);

	const GeometryNode * geometryNode = static_cast<const GeometryNode *>(root);
	// cout << "hey" << endl;

	auto collision = collide(eye, ray, root);// geometryNode->collide(eye, ray);
	// cout << collision.first << endl;

	if(collision.object)
	{
		
		vec3 collision_point = eye + normalize(ray-eye)*(collision.t-0.00001);
		result += collision.object->m_material->color()*ambient;

		for(auto light: lights)
		{
			cnt1++;
			auto light_collision = collide(collision_point, light->position, root, true);
			float light_dist = length(light->position - collision_point);

			// cout << std::max(0.0f, dot(collision.normal, normalize(light->position - collision_point))) << endl;

			if(!(light_collision.object))
			{
				cnt2++;
				float light_attenuation = 1.0f/(light->falloff[2]*light_dist*light_dist + 
												light->falloff[1]*light_dist + 
												light->falloff[0]);
				// light->colour = vec3(1.0/light->colour.x, 1.0/light->colour.y, 1.0/light->colour.z);

				// cout << light_dist << endl;
				// cout << light_attenuation << endl;
				// cout << light_color.x << ", " << light_color.y << ", " << light_color.z << endl;
				diffuse_light += light_attenuation*light->colour * 
								 std::max(0.0f, dot(collision.normal, normalize(light->position - collision_point)));
				// cout << std::max(0.0f, dot(collision.normal, normalize(light->position - collision_point))) << endl;
				// pvec(collision.normal);
				// pvec(light->position - collision_point);
			}
		}

		result += diffuse_light;
	}
	else
	{
		result = vec3(0.8,0.8,0.8);
	}

	return result;
}


