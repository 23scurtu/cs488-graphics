// Winter 2019

#include <glm/ext.hpp>
#include <vector>
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "SceneNode.hpp"
#include <iostream>
#include <utility>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <chrono>
#include "uniform.hpp"
#include "AreaLight.hpp"
#include "BVH.hpp"
#include <thread>
#include <utility>
#include <atomic>

#include "A4.hpp"

using namespace std;
using namespace glm;

struct SceneState
{
	const glm::vec3 & ambient;
	const std::list<Light *> & lights;
	const vector<AreaLight*> & area_lights;
	const SceneNode * root;
	const BVH * bvh;

	SceneState(const glm::vec3 & ambient, 
			   const std::list<Light *> & lights, 
			   const vector<AreaLight*> & area_lights, 
			   const SceneNode * root,
			   const BVH * bvh): ambient{ambient}, lights{lights}, area_lights{area_lights}, root{root}, bvh{bvh} {}
};

vec3 rayColor(vec3 eye, 
			  vec3 ray, 
			  SceneState* s,
			  vec3 background,
			  size_t max_hits = 5, 
			  bool inside_solid = false);
void pvec(vec3 v) { cout << v.x << ", " << v.y << ", " << v.z << endl; }

int cnt1 = 0;
int cnt2 = 0;

bool ANTI_ALIASING = true;			   	// Enable regular sampling anti aliasing.
const int ANTI_ALIASING_DIVISIONS = 4;	// Number of subdivisions to make at each pixel.

// Number of cores used in first*second
pair<size_t, size_t> multithreading_kernel(4,2);

const int subdivisions = ANTI_ALIASING_DIVISIONS;

bool PRINT_PROGRESS = true;
const int progress_prints = 250;
atomic_int pixels_processed(0);

#define BVH_COLLISION
// #define RENDER_BVH

vec3 background(float x, float y)
{
	// y = -y;
	// y *= 2;

	// vec3 middle_color(1,1,1);
	// vec3 bottom_color(0.2,0.2,0);
	// vec3 top_color(0.5,0.5,1);


	// // if(y >= 0.5 && y)
	// if(y >= 0.1) return (y-0.1)*top_color+(1-y+0.1)*middle_color;
	// // if(y < -0.5)
	// if(y < -0.1) return (sqrt(abs(y+0.1)*0.5))*bottom_color+(1-sqrt(abs(y+0.1)*0.5))*middle_color;

	// return middle_color;
	// return vec3(0.65,0.825,1);
	return vec3(0,0,0);
}

void get_lights(SceneNode* root, vector<AreaLight*> &result)
{
	if(root->m_nodeType == NodeType::GeometryNode)
	{
		GeometryNode * geometryNode = static_cast<GeometryNode *>(root);
		if(geometryNode->m_primitive && geometryNode->m_primitive->is_light()) result.push_back(static_cast<AreaLight *>(geometryNode->m_primitive));
	}

	for(SceneNode * child : root->children) {
		get_lights(child, result);
	}
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
	// Calculate all scene graph world transforms before rendering. 
	// TODO Do outside of rendering function
	root->calc_world_transforms();

	// BVH Specific preproccessing
	root->calc_world_inv_transforms();
	root->calc_world_normal_inv_transforms();

	BVH bvh(root);

	vector<AreaLight*> area_lights;

	get_lights(root, area_lights);

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

	std::vector<std::vector<vec3>> colors(image.height(), std::vector<vec3>(image.width(), vec3(0,0,0)));

	SceneState state(ambient, lights, area_lights, root, &bvh);

	auto RenderThread = [&](size_t c_h, size_t c_w, size_t t_i, size_t t_j){
		if(ANTI_ALIASING)
		{
			for(int y = t_i; y < h; y+=c_h)
			{
				for(int x = t_j; x < w; x+=c_w)
				{
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

							color += rayColor(eye, ray, &state, background(float(x)/float(nx)-1.0f/2, float(y)/float(ny)-1.0f/2));
						}
					}

					colors[y][x] = (1.0f/(subdivisions*subdivisions))* color;

					pixels_processed++;

					if(pixels_processed % (w*h/progress_prints) == 0)
					{
						cout << float(pixels_processed)/float(w*h)*100 << "% completed." << endl;
					}
				}
			}
		}
		else
		{
			for(int y = t_i; y < h; y+=c_h)
			{
				for(int x = t_j; x < w; x+=c_w)
				{
						vec3 ray;// = rays[y][x];

						// 1.715*10^-6
						ray = vec3(x, y, 0);
						ray += vec3(-float(h)/2,-float(image.height())/2,d);
						// TODO Check if scales
						//TODO Should be minus?
						ray *= vec3(-window_h/nx, -window_h/ny, 1);
						ray = camera_rotation * ray;
						ray = normalize(ray);
						ray += eye;

						// 1.18*10^-5 s
						colors[y][x] = rayColor(eye, ray, &state, background(float(x)/float(nx)-1.0f/2, float(y)/float(ny)-1.0f/2));
						
						pixels_processed++;

						if(pixels_processed % (w*h/progress_prints) == 0)
						{
							cout << float(pixels_processed)/float(w*h)*100 << "% completed." << endl;
						}
				}
			}
		}
	};

	auto RenderMultithread = [&](size_t c_h, size_t c_w)
	{
		vector<thread*> threads;

		for(size_t j = 0; j < c_h; j++)
		{
			for(size_t i = 0; i < c_w; i++)
			{
				threads.push_back(std::move(new thread(RenderThread, c_h, c_w, j, i)));
			}
		}

		for(auto thread: threads) thread->join();
	};

	RenderMultithread(multithreading_kernel.first, multithreading_kernel.second);

	// COMMENT ABOVE LINE AND UNCOMMENT BELOW FOR SINGLETHREADING CODE
	// for(int y = 0; y != image.height(); y++)
	// {
	// 	for(int x = 0; x != image.width(); x++)
	// 	{
	// 		if(ANTI_ALIASING)
	// 		{
	// 			vec3 color(0,0,0);

	// 			for(int l = 0; l != subdivisions; l++)
	// 			{
	// 				for(int k = 0; k != subdivisions; k++)
	// 				{
	// 					vec3 ray;// = rays[y][x];

	// 					ray = vec3(x + float(2*k+1)/float(2*subdivisions), y + float(2*l+1)/float(2*subdivisions), 0);
	// 					ray += vec3(-float(image.height())/2,-float(image.height())/2,d);
	// 					// TODO Check if scales
	// 					//TODO Should be minus?
	// 					ray *= vec3(-window_h/nx, -window_h/ny, 1);
	// 					ray = camera_rotation * ray;
	// 					ray = normalize(ray);
	// 					ray += eye;

	// 					color += rayColor(eye, ray, &state, background(float(x)/float(nx)-1.0f/2, float(y)/float(ny)-1.0f/2));
	// 				}
	// 			}

	// 			colors[y][x] = (1.0f/(subdivisions*subdivisions))* color;
	// 		}
	// 		else
	// 		{
	// 			vec3 ray;// = rays[y][x];

	// 			// 1.715*10^-6
	// 			ray = vec3(x, y, 0);
	// 			ray += vec3(-float(image.height())/2,-float(image.height())/2,d);
	// 			// TODO Check if scales
	// 			//TODO Should be minus?
	// 			ray *= vec3(-window_h/nx, -window_h/ny, 1);
	// 			ray = camera_rotation * ray;
	// 			ray = normalize(ray);
	// 			ray += eye;

	// 			// 1.18*10^-5 s
	// 			colors[y][x] = rayColor(eye, ray, &state, background(float(x)/float(nx)-1.0f/2, float(y)/float(ny)-1.0f/2));
	// 		}
	// 	}
	// }

	float max_intensity = 1.0f;
	
	// //Normalize color - desaturates
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

	auto finish_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish_time - start_time;
	std::cout << "Elapsed time: " << elapsed.count() << " s\n";
}

struct Collision
{
	float t = -1;
	vec3 hit_point;
	vec3 local_hit_point;
	vec3 normal;
	const GeometryNode* object = nullptr;

	Collision() = default;
	Collision(float t, vec3 normal, GeometryNode* object): t{t}, normal{normal}, object{object} {}
};

const float EPSILON = 0.0002;

// This epsilon varies in effectiveness depending on the general scale of the scene
// To account for this, its later multiplied by intersection distance which roughly
// generates the scale of the scene.
const float DOUBLE_COLLISION_EPSILON = 0.0001f;

// Calculate intersection without any optimization
Collision collide(vec3 eye, vec3 ray, const SceneNode *root, bool between_points = false)
{
	vec3 orig_eye = eye;
	vec3 orig_ray = ray;

	ray = vec3(root->invtrans * vec4(ray, 1.0f));
	eye = vec3(root->invtrans * vec4(eye, 1.0f));

	Collision result;
	Collision second_result;

	if(root->m_nodeType == NodeType::GeometryNode)
	{
		const GeometryNode * geometryNode = static_cast<const GeometryNode *>(root);

		auto collision = geometryNode->collide(eye, ray);
		// cout << collision << endl;
		if(collision.first != -1)
		{
			if((between_points && collision.first <= length(ray-eye) && collision.first >= EPSILON) || 
			   (!between_points && collision.first >= EPSILON))
			{
				
				result.t = collision.first;
				result.hit_point = eye + normalize(ray-eye)*(collision.first);
				result.local_hit_point = result.hit_point;

				result.object = geometryNode;
				result.normal = collision.second;
			}
		}
	}

	for ( auto child: root->children)
	{
		auto child_result = collide(eye, ray, child, between_points);
		
		if(child_result.object) 
		{
			if((!result.object)) result = child_result;
			else 
			{
				if(abs(result.t - child_result.t) < DOUBLE_COLLISION_EPSILON*result.t)
					second_result = result;

				if(child_result.t < result.t + DOUBLE_COLLISION_EPSILON*result.t)
					result = child_result;
			}
		}
	}

	result.hit_point = vec3(root->trans * vec4(result.hit_point, 1.0f));
	result.t = length(result.hit_point - orig_eye); // TODO Verify
	result.normal = root->normal_invtrans * result.normal; //[n][m]

	return result;
}

// Calculate intersection using the BVH hierarchy
Collision collide(vec3 eye, vec3 ray, SceneState * s, bool between_points = false)
{
	vector<GeometryNode*> candidates = s->bvh->candidate_geometry_nodes(eye, ray);
	// cout << candidates.size() << endl;
	
	Collision result;
	Collision second_result;

	for(GeometryNode* c: candidates)
	{
		vec3 local_eye = vec3(c->world_inv_trans * vec4(eye,1.0f));
		vec3 local_ray = vec3(c->world_inv_trans * vec4(ray,1.0f));

		auto collision = c->collide(local_eye, local_ray);
		
		if(collision.first != -1)
		{
			if(((between_points && collision.first <= length(local_ray-local_eye) && collision.first >= EPSILON) || 
				(!between_points && collision.first >= EPSILON)))
				// If within the bounds epsilon and ray/inf and closer than previous collisions
			{			
				Collision c_collision;

				c_collision.t = collision.first;
				c_collision.hit_point = local_eye + normalize(local_ray-local_eye)*(collision.first);
				c_collision.local_hit_point = c_collision.hit_point;

				c_collision.object = c;
				c_collision.normal = collision.second;

				// Bring back to global coords!

				c_collision.hit_point = vec3(c->world_trans * vec4(c_collision.hit_point, 1.0f));
				c_collision.t = length(c_collision.hit_point - eye); // TODO Verify

				// TODO Move more under if statement!
				if(c_collision.object) 
				{
					if((!result.object))
					{
						c_collision.normal = c->world_normal_invtrans * c_collision.normal; //[n][m]
						result = c_collision;
					}
					else 
					{
						if(abs(result.t - c_collision.t) < DOUBLE_COLLISION_EPSILON*result.t) second_result = result;
						if(c_collision.t < result.t + DOUBLE_COLLISION_EPSILON*result.t)
						{
							c_collision.normal = c->world_normal_invtrans * c_collision.normal; //[n][m]
							result = c_collision;
						}
					}
				}
			}
		}
	}

	return result;
}

vec3 exp_n(vec3 i, float n)
{
	return vec3(pow( i.x, n), pow( i.y, n), pow( i.z, n));
}

inline vec3 specular_color(vec3 v,
						   vec3 light_pos, 
						   vec3 light_color,
						   vec3 collision_point, 
						   Collision collision,
						   float light_attenuation, 
						   float PHONG_EXPONENT, 
						   float REFLECTIVE_GLOSSINESS,
						   size_t reflection_rays)
{
	vec3 specular_light(0,0,0);
	vec3 l = normalize(light_pos - collision_point);
	vec3 r = -l + 2*collision.normal*dot(l, collision.normal); // ggReflection
	
	float exponent = (1.0f/(PHONG_EXPONENT+1))*REFLECTIVE_GLOSSINESS;

	if(exponent > 0.000001 && reflection_rays) // Otherwise glossiness does nothing PHONG_EXPONENT > 0.0f && REFLECTIVE_GLOSSINESS > 0.00001f && 
	{
		for(size_t i = 1; i <= reflection_rays; i++)
		{
			// Apply glossy reflection to lights
			float alpha = acos(pow(1-uni(), exponent ));
			float beta = 2.0f*M_PI*uni();

			vec3 alpha_axis = cross(r, collision.normal);
			if(alpha_axis == vec3(0,0,0)) alpha_axis = vec3(r.y,-r.x,0); // Will always be perpendicular

			vec3 temp = rotate(r, alpha, alpha_axis);
			r = rotate(temp, beta, r);

			// If pertubation causes refraction, skip.
			if(dot(r, collision.normal) <= 0.001) {i--; continue;}

			specular_light += collision.object->m_material->ks() *
								// TODO Why does no max here create concentric circle pattern?
								pow(std::max(0.0f, dot(r,v)), collision.object->m_material->shininess())*light_attenuation*light_color;
		}
		specular_light *= 1.0f/reflection_rays;
	}
	else
	{
		specular_light += collision.object->m_material->ks() *
							// TODO Why does no max here create concentric circle pattern?
							pow(std::max(0.0f, dot(r,v)), collision.object->m_material->shininess())*light_attenuation*light_color;
	}

	return specular_light;
}

vec3 rayColor(vec3 eye, vec3 ray, SceneState* s,
			  vec3 background,
			  size_t max_hits,
			  bool inside_solid)
{
	// auto start_time = std::chrono::high_resolution_clock::now();
	vec3 result(0,0,0);
	vec3 diffuse_light(0,0,0);
	vec3 specular_light(0,0,0);
	vec3 reflected_light(0,0,0);
	vec3 transmitted_light(0,0,0);

	const GeometryNode * geometryNode = static_cast<const GeometryNode *>(s->root);

	#ifndef BVH_COLLISION
	auto collision = collide(eye, ray, s->root);// geometryNode->collide(eye, ray);
	#endif
	#ifdef BVH_COLLISION
	auto collision = collide(eye, ray, s);
	#endif
	
	collision.normal = normalize(collision.normal);

	if(collision.object)
	{
		// TODO Attenuate?
		// If the collision is with an area light, return the color of the light
		if(collision.object->m_primitive->is_light())
		{
			return static_cast<AreaLight *>(collision.object->m_primitive)->color;
		}

		vec3 collision_point = collision.hit_point; 
		// vec3 collision_point = eye + normalize(ray-eye)*(collision.t);//*(1.0f-EPSILON);
		// collision_point = collision_point - collision.normal*EPSILON;

		vec3 kd = collision.object->m_material->color(&collision.local_hit_point);
		if(collision.object->m_primitive->textured())
			kd = collision.object->m_primitive->getLastHitColor();
		// TODO Uncomment for texture

		result += kd*s->ambient;
		
		collision.normal = normalize(collision.normal); 

		vec3 v = normalize(ray - eye);

		const float PHONG_EXPONENT = collision.object->m_material->shininess();
		float REFLECTIVE_GLOSSINESS = collision.object->m_material->r_g(); //0.1f;
		float REFRACTIVE_GLOSSINESS = collision.object->m_material->t_g(); //0.003f; // TODO Debug black dot in middle!
		
		size_t reflection_rays = 2; // For glossy // TODO Causes exponential time increase
		size_t transmission_rays = 2;

		if(!inside_solid)
		{
			for(AreaLight* area_light: s->area_lights)
			{
				// TODO Make lua param to set samples
				const int samples = 3;
				const float sample_inv = 1.0f/float(samples);

				vec3 area_diffuse(0,0,0);
				vec3 area_specular(0,0,0);

				// Sample the area light multiple times and add together contributions
				for(int i = 0; i < samples; i++)
				{
					vec3 position = area_light->surfacePoint();

					#ifndef BVH_COLLISION
					auto light_collision = collide(collision_point, position, s->root, true);
					#endif

					#ifdef BVH_COLLISION
					auto light_collision = collide(collision_point, position, s, true);
					#endif

					float light_dist = length(position - collision_point);

					if(!(light_collision.object))
					{
						float light_attenuation = 1.0f/(area_light->falloff[2]*light_dist*light_dist + 
														area_light->falloff[1]*light_dist + 
														area_light->falloff[0]);

						area_diffuse += light_attenuation*area_light->color * 
										std::max(0.0f, dot(collision.normal, normalize(position - collision_point))) *
										kd;

						vec3 v = normalize(eye-ray);
						area_specular += specular_color(v, position, area_light->color, collision_point, collision, 
														light_attenuation, PHONG_EXPONENT, REFLECTIVE_GLOSSINESS, reflection_rays);
					}
				}

				area_diffuse *= sample_inv;
				area_specular *= sample_inv;

				diffuse_light += area_diffuse;
				specular_light += area_specular;
			}

			for(auto light: s->lights)
			{
				#ifndef BVH_COLLISION
				auto light_collision = collide(collision_point, light->position, s->root, true);
				#endif

				#ifdef BVH_COLLISION
				auto light_collision = collide(collision_point, light->position, s, true);
				#endif
				float light_dist = length(light->position - collision_point);// light_collision.t;

				if(!(light_collision.object))
				{
					float light_attenuation = 1.0f/(light->falloff[2]*light_dist*light_dist + 
													light->falloff[1]*light_dist + 
													light->falloff[0]);

					diffuse_light += light_attenuation*light->colour * 
									std::max(0.0f, dot(collision.normal, normalize(light->position - collision_point))) *
									kd;

					vec3 v = normalize(eye-ray);
					specular_light += specular_color(v, light->position, light->colour, collision_point, collision, 
														light_attenuation, PHONG_EXPONENT, REFLECTIVE_GLOSSINESS, reflection_rays);
				}	
			}

			if(max_hits > 0)
			{
				
				vec3 r = (v - 2*collision.normal*dot(v, collision.normal)); // ggReflection
				vec3 new_collision_point = collision_point + EPSILON * collision.normal;
				// TODO Causes artifacting if objects are too small???

				float exponent = (1.0f/(PHONG_EXPONENT+1))*REFLECTIVE_GLOSSINESS;

				if(exponent > 0.000001 && reflection_rays) // Otherwise glossiness does nothing
				{
					for(size_t i = 1; i <= reflection_rays; i++)
					{
						// Apply glossy reflection
						float alpha = acos(pow(1-uni(), exponent ));
						float beta = 2.0f*M_PI*uni();

						vec3 alpha_axis = cross(r, collision.normal);
						if(alpha_axis == vec3(0,0,0)) alpha_axis = vec3(r.y,-r.x,0); // Will always be perpendicular

						vec3 temp = rotate(r, alpha, alpha_axis);
						r = rotate(temp, beta, r);

						// If pertubation causes refraction, skip.
						if(dot(r, collision.normal) <= 0.001) {i--; continue;}

						vec3 reflected_color = rayColor(new_collision_point, new_collision_point + r, s, vec3(0,0,0), max_hits-1);
						// If bounce and miss do not add background color! ^

						reflected_light += collision.object->m_material->ks() * reflected_color;
					}

					reflected_light *= 1.0f/reflection_rays;
				}
				else // TODO Threshold this
				{
					vec3 reflected_color = rayColor(new_collision_point, new_collision_point + r, s, vec3(0,0,0), max_hits-1);
					reflected_light += collision.object->m_material->ks() * reflected_color;
				}
				
			}
		}

		float TRANSMISSION_COEFFICIENT = collision.object->m_material->tc(); //0.4;

		if(TRANSMISSION_COEFFICIENT > 0.01f && max_hits > 0)
		{
			// Transmittence
			// temp constants
			float n_i = 1.0f;
			float n_t = collision.object->m_material->ir();

			if (inside_solid) std::swap(n_i, n_t);

			vec3 N = collision.normal;
			if(inside_solid) N = -N; // TODO Fix determining whether or not inside solid
			
			float n_frac = n_i/n_t;
			float v_dot_N = dot(v, N);

			vec3 transmitted_ray = (-n_frac*(v_dot_N) - sqrt(1.0f-n_frac*n_frac*(1.0f-v_dot_N*v_dot_N)))*N + n_frac*v;

			// EPSILON NEEDS TO BE FAR TOO HIGH
			vec3 new_collision_point = collision_point + 0.5 * -N; /// EPSILON

			if(0.5f* REFRACTIVE_GLOSSINESS > 0.00000001f  && transmission_rays) // Otherwise glossiness does nothing
			{
				for(size_t i = 1; i <= transmission_rays; i++)
				{
					// Apply glossy transmission
					float alpha = acos(pow(1-uni(), 0.5f* REFRACTIVE_GLOSSINESS) );//(1.0f/(PHONG_EXPONENT+1));
					float beta = 2.0f*M_PI*uni();

					vec3 alpha_axis = cross(transmitted_ray, N);
					if(alpha_axis == vec3(0,0,0)) alpha_axis = vec3(transmitted_ray.y,-transmitted_ray.x,0); // Will always be perpendicular

					vec3 temp = rotate(transmitted_ray, alpha, alpha_axis);
					transmitted_ray = rotate(temp, beta, transmitted_ray);

					// If pertubation causes a reflection, skip.
					if(dot(transmitted_ray, N) >= -0.01) {i--; continue;} // TODO SHOULD BE -0.01?

					// Transmit light
					transmitted_light += TRANSMISSION_COEFFICIENT* (vec3(1,1,1)-collision.object->m_material->ks()) *
										rayColor(new_collision_point, new_collision_point + transmitted_ray, s, vec3(0,0,0), max_hits - 1, !inside_solid);
				}

				// transmitted_light += TRANSMISSION_COEFFICIENT* (vec3(1,1,1)-collision.object->m_material->ks()) *
				// 						rayColor(new_collision_point, new_collision_point + transmitted_ray, ambient, lights, root, vec3(0,0,0), max_hits - 1, !inside_solid);

				// transmitted_light *= (1.0f/(transmission_rays + 1));
				// Have at least one transmitted ray straight through?

				transmitted_light *= 1.0f/transmission_rays;
			}
			else
			{
				transmitted_light += TRANSMISSION_COEFFICIENT* (vec3(1,1,1)-collision.object->m_material->ks()) *
									 rayColor(new_collision_point, new_collision_point + transmitted_ray, s, vec3(0,0,0), max_hits - 1, !inside_solid);
			}
			
		}

		result += diffuse_light + specular_light + reflected_light;	
		result *= (1-TRANSMISSION_COEFFICIENT);
		result += transmitted_light;
	}
	else result = background;
	

	// auto finish_time = std::chrono::high_resolution_clock::now();
	// std::chrono::duration<double> elapsed = finish_time - start_time;
	// std::cout << "Elapsed time: " << elapsed.count() << " s\n";
	// exit(0);


	// Quick and dirty method of visualizing bounding boxes (will be slower due to recalculating candidate_geometry_nodes)
	#ifdef RENDER_BVH
	for(auto c: s->bvh->candidate_geometry_nodes(eye, ray))
	{
		AABB b = c->m_primitive->aabb.transform(c->world_trans);
		result += vec3(1,0,0)*0.1f;
	}
	#endif

	return result;
}
