// Winter 2019

#include <glm/ext.hpp>
#include <vector>
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "SceneNode.hpp"
#include <iostream>
#include <utility>
#include <glm/gtx/vector_angle.hpp>
#include <chrono>

#include "A4.hpp"

using namespace std;
using namespace glm;

vec3 rayColor(vec3 eye, 
			  vec3 ray, 
			  const glm::vec3 & ambient, 
			  const std::list<Light *> & lights, 
			  SceneNode * root,
			  vec3 background = vec3(0.8,0.8,0.8),
			  size_t max_hits = 3);
void pvec(vec3 v) { cout << v.x << ", " << v.y << ", " << v.z << endl; }

int cnt1 = 0;
int cnt2 = 0;

bool ANTI_ALIASING = true;			   	// Enable regular sampling anti aliasing.
const int ANTI_ALIASING_DIVISIONS = 3;	// Number of subdivisions to make at each pixel.

const int subdivisions = ANTI_ALIASING_DIVISIONS;

vec3 background(float x, float y)
{
	y = -y;
	y *= 2;
	// return vec3(0.4*y*(1 - y) + 0.6, 0.3*(0.5*(1 - y)) + 0.7, y);
	vec3 middle_color(1,1,1);
	vec3 bottom_color(0.2,0.2,0);
	vec3 top_color(0.5,0.5,1);


	// if(y >= 0.5 && y)
	if(y >= 0.1) return (y-0.1)*top_color+(1-y+0.1)*middle_color;
	// if(y < -0.5)
	if(y < -0.1) return (sqrt(abs(y+0.1)*0.5))*bottom_color+(1-sqrt(abs(y+0.1)*0.5))*middle_color;

	return middle_color;
}

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

	auto start_time = std::chrono::high_resolution_clock::now();

  // Fill in raytracing code here...  

  float d = 5;
  float nx = image.width();
  float ny = image.height();

//   std::vector<std::vector<vec3>> rays(image.height(), std::vector<vec3>(image.width(), vec3(0,0,0)));

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

//   for(int y = 0; y != image.height(); y++)
//   {
// 	for(int x = 0; x != image.width(); x++)
// 	{
// 		rays[y][x] = vec3(x, y, 0);
// 		rays[y][x] += vec3(-float(image.height())/2,-float(image.height())/2,d);
// 		// TODO Check if scales
// 		//TODO Should be minus?
// 		rays[y][x] *= vec3(-window_h/nx, -window_h/ny, 1);
// 		rays[y][x] = camera_rotation * rays[y][x];
// 		rays[y][x] = normalize(rays[y][x]);
// 		rays[y][x] += eye;
// 	}
//   }

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

	std::vector<std::vector<vec3>> colors(image.height(), std::vector<vec3>(image.width(), vec3(0,0,0)));

	for(int y = 0; y != image.height(); y++)
	{
		for(int x = 0; x != image.width(); x++)
		{

			if(ANTI_ALIASING)
			{
				cout << "hey" << endl;
				vec3 color(0,0,0);

				for(int l = 0; l != subdivisions; l++)
				{
					for(int k = 0; k != subdivisions; k++)
					{
						vec3 ray;// = rays[y][x];


						ray = vec3(x + float(2*k+1)/float(2*subdivisions), y + float(2*l+1)/float(2*subdivisions), 0);
						ray += vec3(-float(image.height())/2,-float(image.height())/2,d);
						// TODO Check if scales
						//TODO Should be minus?
						ray *= vec3(-window_h/nx, -window_h/ny, 1);
						ray = camera_rotation * ray;
						ray = normalize(ray);
						ray += eye;

						// cout << ray.x << ", " << ray.y << ", " << ray.z << endl;
						// vec3 background = vec3(0.4*(1 - float(y)/float(ny)) + 0.6, 0.3*(0.5*(1 - float(y)/float(ny))) + 0.7, float(y)/float(ny));
						// pvec(background);
						// cout << nx << endl;

						color += rayColor(eye, ray, ambient, lights, root, background(float(x)/float(nx)-1.0f/2, float(y)/float(ny)-1.0f/2));
						
						
					}
				}

				colors[y][x] = (1.0f/(subdivisions*subdivisions))* color;
			}

			else
			{
				vec3 ray;// = rays[y][x];

				ray = vec3(x, y, 0);
				ray += vec3(-float(image.height())/2,-float(image.height())/2,d);
				// TODO Check if scales
				//TODO Should be minus?
				ray *= vec3(-window_h/nx, -window_h/ny, 1);
				ray = camera_rotation * ray;
				ray = normalize(ray);
				ray += eye;

				colors[y][x] = rayColor(eye, ray, ambient, lights, root, background(float(x)/float(nx)-1.0f/2, float(y)/float(ny)-1.0f/2));
			}
			
		}
	}

	float max_intensity = 1.0f;
	// Normalize color - desaturates
	// for(int y = 0; y != image.height(); y++)
	// {
	// 	for(int x = 0; x != image.width(); x++)
	// 	{
	// 		if(colors[y][x].x > max_intensity) max_intensity = colors[y][x].x;
	// 		if(colors[y][x].y > max_intensity) max_intensity = colors[y][x].y;
	// 		if(colors[y][x].z > max_intensity) max_intensity = colors[y][x].z;
	// 	}
	// }
	
	for(int y = 0; y != image.height(); y++)
	{
		for(int x = 0; x != image.width(); x++)
		{
			auto color = colors[y][x]*(1.0f/max_intensity);

			image(x,y,0) = color.r;
			image(x,y,1) = color.g;
			image(x,y,2) = color.b;
		}
	}

	// cout << cnt1 << endl;
	// cout << cnt2 << endl;

	auto finish_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish_time - start_time;
	std::cout << "Elapsed time: " << elapsed.count() << " s\n";
}

struct Collision
{
	float t = -1;
	vec3 hit_point;
	vec3 normal;
	GeometryNode* object = nullptr;

	Collision() = default;
	Collision(float t, vec3 normal, GeometryNode* object): t{t}, normal{normal}, object{object} {}
};

const float EPSILON = 0.0001;

Collision collide(vec3 eye, vec3 ray, SceneNode *root, bool between_points = false)
{
	vec3 orig_eye = eye;
	// pvec(ray);
	// pvec(ray);
	// cout << endl;

	ray = vec3(root->invtrans * vec4(ray, 1.0f));
	eye = vec3(root->invtrans * vec4(eye, 1.0f));

	Collision result;
	// cout << result.t <<endl;

	// std::cout << "hey" << std::endl;

	if(root->m_nodeType == NodeType::GeometryNode)
	{
		GeometryNode * geometryNode = static_cast<GeometryNode *>(root);

		auto collision = geometryNode->collide(eye, ray);
		cnt1++;
		// cout << collision << endl;
		if(collision.first != -1)
		{
			cnt2++;
			if((between_points && collision.first <= length(ray-eye) && collision.first >= EPSILON) || !between_points)
			{
				
				result.t = collision.first;
				result.hit_point = eye + normalize(ray-eye)*(collision.first);
				result.object = geometryNode;
				result.normal = collision.second;

				// if(root->m_name == "s") 
				// {
				// 	cout << collision.first << endl;
				// 	// cout << "eye" << endl;
				// 	// pvec(eye);
				// 	pvec(result.hit_point); // should be a unit vector since s is unit circle
				// 	cout << length(result.hit_point) << endl;
				// 	cout << endl;
				// }

				// cout << result.t << endl;
				// pvec(result.normal);
				// cout << endl;
				// if(result.normal != vec3(0,0,0))pvec(result.normal);
			}
		}
	}

	for ( auto child: root->children)
	{
		auto child_result = collide(eye, ray, child, between_points);
		if(child_result.object && (!result.object || child_result.t < result.t)) result = child_result;
	}

	result.hit_point = vec3(root->trans * vec4(result.hit_point, 1.0f));
	result.t = length(result.hit_point - orig_eye); // TODO Verify
	// if(result.normal != vec3(0,0,0))pvec(result.normal);
	result.normal = mat3{root->invtrans[0][0], root->invtrans[1][0], root->invtrans[2][0], //inv transpose
						 root->invtrans[0][1], root->invtrans[1][1], root->invtrans[2][1],
						 root->invtrans[0][2], root->invtrans[1][2], root->invtrans[2][2] } * result.normal; //[n][m]

	// if(result.normal != vec3(0,0,0))pvec(result.normal);

	return result;
}

vec3 exp_n(vec3 i, float n)
{
	return vec3(pow( i.x, n), pow( i.y, n), pow( i.z, n));
}

vec3 rayColor(vec3 eye, vec3 ray, const glm::vec3 & ambient, const std::list<Light *> & lights, SceneNode * root, vec3 background, size_t max_hits)
{
	vec3 result(0,0,0);
	vec3 diffuse_light(0,0,0);
	vec3 specular_light(0,0,0);

	const GeometryNode * geometryNode = static_cast<const GeometryNode *>(root);
	// cout << "hey" << endl;

	auto collision = collide(eye, ray, root);// geometryNode->collide(eye, ray);
	collision.normal = normalize(collision.normal);

	if(collision.object)
	{
		vec3 collision_point = collision.hit_point; 
		// vec3 collision_point = eye + normalize(ray-eye)*(collision.t);//*(1.0f-EPSILON);
		// collision_point = collision_point - collision.normal*EPSILON;
		result += collision.object->m_material->color()*ambient;
		// collision.normal = normalize(collision.normal); 

		for(auto light: lights)
		{
			cnt1++;
			auto light_collision = collide(collision_point, light->position, root, true);
			float light_dist = length(light->position - collision_point);// light_collision.t;

			if(!(light_collision.object))
			{
				
				float light_attenuation = 1.0f/(light->falloff[2]*light_dist*light_dist + 
												light->falloff[1]*light_dist + 
												light->falloff[0]);
				// cout << length(collision.normal) << endl;

				diffuse_light += //(1.0f/lights.size())*
								 light_attenuation*light->colour * 
								 std::max(0.0f, dot(collision.normal, normalize(light->position - collision_point)));
				vec3 v = normalize(eye-ray);
				vec3 l = normalize(light->position - collision_point);
				vec3 r = -l + 2*collision.normal*dot(l, collision.normal); // ggReflection
				// vec3 v = normalize(eye - collision_point);
				// diffuse_light += light_attenuation*light->colour * collision.object->m_material->ks() *
				// 				 exp_n(r+v, collision.object->m_material->shininess());
				// specular_light += light_attenuation*light->colour * collision.object->m_material->ks() * 
				// 				 std::max(0.0f, dot(normalize(eye - collision_point), r));// exp_n(r+v, collision.object->m_material->shininess())

				// if(max_hits - 1 == 0)
				// {
				specular_light += //(1.0f/lights.size())*
									collision.object->m_material->ks() *
									// TODO Why does no max here create concentric circle pattern?
									pow(std::max(0.0f, dot(r,v)), collision.object->m_material->shininess())*light_attenuation*light->colour;
				
				// else
				// {
				// if(max_hits - 1 != 0)
				// {
				// 	vec3 reflected_color = rayColor(collision_point, collision_point + r, ambient, lights, root, background, max_hits-1);
				// 	reflected_color.x = std::max(0.0f, reflected_color.x);
				// 	reflected_color.y = std::max(0.0f, reflected_color.y);
				// 	reflected_color.z = std::max(0.0f, reflected_color.z);

				// 	specular_light += //(1.0f/lights.size())*
				// 						collision.object->m_material->ks() *
				// 						// // TODO Why does no max here create concentric circle pattern?
				// 						// pow(std::max(0.0f, dot(r,v)), collision.object->m_material->shininess())*light_attenuation*light->colour *
				// 						reflected_color;
				// }
				
				// cout << dot(r,v) << endl;
				// pvec(light_attenuation*light->colour * collision.object->m_material->ks());
				// cout << std::max(0.0f, dot(normalize(eye - collision_point), r))<< endl;

			}	
		}

		result += diffuse_light;
		result += specular_light;
	}
	else
	{
		result = background;
	}

	return result;
}


