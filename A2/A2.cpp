// Winter 2019

#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <math.h>
#include <utility>
using namespace glm;

//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData()
	: numVertices(0),
	  index(0)
{
	positions.resize(kMaxVertices);
	colours.resize(kMaxVertices);
}


//----------------------------------------------------------------------------------------
// Constructor
A2::A2()
	: m_currentLineColour(vec3(0.0f))
{
	// cube = vector<vec3>{vec3(1,1,1), vec3(-1,1,1),	
	// 					vec3(-1,1,1), vec3(-1,-1,1),
	// 					vec3(-1,-1,1), vec3(1,-1,1),
	// 					vec3(1,-1,1), vec3(1,1,1),

	// 					vec3(1,1,-1), vec3(-1,1,-1),	
	// 					vec3(-1,1,-1), vec3(-1,-1,-1),
	// 					vec3(-1,-1,-1), vec3(1,-1,-1),
	// 					vec3(1,-1,-1), vec3(1,1,-1),

	// 					vec3(1,1,1), vec3(1,1,-1),
	// 					vec3(-1,1,1), vec3(-1,1,-1),
	// 					vec3(-1,-1,1), vec3(-1,-1,-1),
	// 					vec3(1,-1,1), vec3(1,-1,-1)};
	
	reset();
}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init()
{
	// Set the background colour.
	glClearColor(0.3, 0.5, 0.7, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao);

	enableVertexAttribIndices();

	generateVertexBuffers();

	mapVboDataToVertexAttributeLocation();
}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();
}

//---------------------------------------------------------------------------------------- Winter 2019
void A2::enableVertexAttribIndices()
{
	glBindVertexArray(m_vao);

	// Enable the attribute index location for "position" when rendering.
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray(positionAttribLocation);

	// Enable the attribute index location for "colour" when rendering.
	GLint colourAttribLocation = m_shader.getAttribLocation( "colour" );
	glEnableVertexAttribArray(colourAttribLocation);

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers()
{
	// Generate a vertex buffer to store line vertex positions
	{
		glGenBuffers(1, &m_vbo_positions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	// Generate a vertex buffer to store line colors
	{
		glGenBuffers(1, &m_vbo_colours);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao);

	// Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
	// "position" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
	// "colour" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
	GLint colorAttribLocation = m_shader.getAttribLocation( "colour" );
	glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void A2::initLineData()
{
	m_vertexData.numVertices = 0;
	m_vertexData.index = 0;
}

//---------------------------------------------------------------------------------------
void A2::setLineColour (
		const glm::vec3 & colour
) {
	m_currentLineColour = colour;
}

//---------------------------------------------------------------------------------------
void A2::drawLine(
		const glm::vec2 & V0,   // Line Start (NDC coordinate)
		const glm::vec2 & V1    // Line End (NDC coordinate)
) {

	m_vertexData.positions[m_vertexData.index] = V0;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;
	m_vertexData.positions[m_vertexData.index] = V1;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;

	m_vertexData.numVertices += 2;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A2::appLogic()
{
	// Call at the beginning of frame, before drawing lines:
	initLineData();

	const float rotation_rate = 0.01;
	const float translation_rate = 0.05;
	const float scale_rate = 0.01;

	const float near_rate = 0.01;
	const float far_rate = 0.05;
	const float fov_rate = 0.005;

	// Input Logic
	// cout << mouse_dx << endl;
	switch(current_mode)
	{
		case 0: if(rotate_view.x) view_rot.x += mouse_dx*rotation_rate;
				if(rotate_view.y) view_rot.y += mouse_dx*rotation_rate;
				if(rotate_view.z) view_rot.z += mouse_dx*rotation_rate;
				break;
		case 1: if(translate_view.x) view_trans.x += mouse_dx*translation_rate;
				if(translate_view.y) view_trans.y += mouse_dx*translation_rate;
				if(translate_view.z) view_trans.z += mouse_dx*translation_rate;
				break;
		case 2: if(perspective_change.x) { perspective_fov += mouse_dx*fov_rate; 
										   perspective_fov = clamp(perspective_fov, float(M_PI/180.0f*5.0f), float(M_PI/180.0f*160.0f)); }
				if(perspective_change.y) perspective_near += mouse_dx*near_rate;
				if(perspective_change.z) perspective_far += mouse_dx*far_rate;
				break;
		case 3: if(rotate_model.x) cube_rot.x += mouse_dx*rotation_rate;
				if(rotate_model.y) cube_rot.y += mouse_dx*rotation_rate;
				if(rotate_model.z) cube_rot.z += mouse_dx*rotation_rate;
				break;
		case 4: if(translate_model.x) cube_trans.x += mouse_dx*translation_rate;
				if(translate_model.y) cube_trans.y += mouse_dx*translation_rate;
				if(translate_model.z) cube_trans.z += mouse_dx*translation_rate;
				break;
		case 5: if(scale_model.x) cube_scale.x += mouse_dx*scale_rate;
				if(scale_model.y) cube_scale.y += mouse_dx*scale_rate;
				if(scale_model.z) cube_scale.z += mouse_dx*scale_rate;
				break;
		case 6: if(mouse_dragging)
				{
					// viewport_tl = vec2(std::min(last_mouse_click_pos.x, last_mouse_pos.x), std::max(last_mouse_click_pos.y, last_mouse_pos.y));
					// viewport_br = vec2(std::max(last_mouse_click_pos.x, last_mouse_pos.x), std::min(last_mouse_click_pos.y, last_mouse_pos.y));

					vec2 corner1 = last_mouse_click_pos;
					vec2 corner2 = last_mouse_pos;



					corner1.x = corner1.x/ m_windowWidth * 2.0f - 1.0f;
					corner1.y = -(corner1.y/ m_windowHeight * 2.0f - 1.0f);
					corner2.x = corner2.x/ m_windowWidth * 2.0f - 1.0f;
					corner2.y = -(corner2.y/ m_windowHeight * 2.0f - 1.0f);

					viewport_tl = vec2(std::min(corner1.x, corner2.x), std::max(corner1.y, corner2.y));
					viewport_br = vec2(std::max(corner1.x, corner2.x), std::min(corner1.y, corner2.y));

					viewport_tl = clamp(viewport_tl, vec2(-1,-1), vec2(1,1));
					viewport_br = clamp(viewport_br, vec2(-1,-1), vec2(1,1));

					// cout << viewport_br.x << ", " <<  viewport_br.y << endl;
					// cout << viewport_tl.x << ", " <<  viewport_tl.y << endl;
				}
		// cout << mouse_dragging << endl;
	}

	// cout << cube_rot.x << endl;

	// Graphics Logic

	// Cube Drawing
	vector<vec4> data;
	data.reserve(cube.size());
	for(auto p: cube) data.push_back(vec4(p.x, p.y, p.z, 1));

	mat4 model_scale = createScale(cube_scale);
	mat4 model_rot = createRotation(cube_rot);
	mat4 model_trans = createTransform(cube_trans);

	cube_rot = vec3(0,0,0);
	cube_trans = vec3(0,0,0);

	// TODO Make more efficient by only multiplying when needed.

	model_no_scale = model_no_scale*model_trans*model_rot;
	glm::mat4 model = model_no_scale*model_scale;

	mat4 view_matrix_rot = createRotation(view_rot);
	mat4 view_matrix_trans = createTransform(view_trans);

	view_rot = vec3(0,0,0);
	view_trans = vec3(0,0,0);

	view = view_matrix_trans*view_matrix_rot * view;


	for(vec4 &p : data) p = model*p;
	
	glm::mat4 perspective = createPerspective(perspective_near, perspective_far, perspective_fov, 1.0f);

	for(vec4 &p : data) p = view*p;
	for(vec4 &p : data) p = perspective*p;
	// TODO Needed?

	// Clipping line primitives
	data = clipLinePrimitives3d(data);


	// for(vec4 &p : data) p = vec4(p.x/p.w, p.y/p.w, p.z/p.w, 1);

	setLineColour(vec3(1.0f, 0.7f, 0.8f));
	assert(data.size() % 2 == 0);
	for(int i = 0; i < data.size(); i+=2) drawLine(data[i], data[i+1]);


	//Gnomon Drawing
	vector<vec4> world_gnomon_data{vec4(1,0,0,1), vec4(0,0,0,1),
								   vec4(0,1,0,1), vec4(0,0,0,1),
							       vec4(0,0,1,1), vec4(0,0,0,1)};
	vector<vec4> model_gnomon_data = world_gnomon_data;

	mat4 pv = perspective*view;

	for(vec4 &p : world_gnomon_data) p = pv*p;
	for(vec4 &p : model_gnomon_data) p = pv*model_no_scale*p;

	
	// for(vec4 &p : world_gnomon_data) p = vec4(p.x/p.w, p.y/p.w, p.z/p.w, 1);
	// for(vec4 &p : model_gnomon_data) p = vec4(p.x/p.w, p.y/p.w, p.z/p.w, 1);

	vector<vec3> world_gnomon_colors = {vec3(1.0f, 0.0f, 1.0f),
										vec3(1.0f, 1.0f, 0.0f),
										vec3(0.0f, 1.0f, 1.0f)};
	vector<vec3> model_gnomon_colors = {vec3(1.0f, 0.0f, 0.0f),
										vec3(0.0f, 1.0f, 0.0f),
										vec3(0.0f, 0.0f, 1.0f)};

	clipLinePrimitives3d(world_gnomon_data, world_gnomon_colors);
	clipLinePrimitives3d(model_gnomon_data, model_gnomon_colors);

	for(int i = 0; i < world_gnomon_data.size(); i+=2) 
	{
		setLineColour(world_gnomon_colors[i/2]);
		drawLine(world_gnomon_data[i], world_gnomon_data[i+1]);
	}
	for(int i = 0; i < model_gnomon_data.size(); i+=2) 
	{
		setLineColour(model_gnomon_colors[i/2]);
		drawLine(model_gnomon_data[i], model_gnomon_data[i+1]);
	}

	

	// // World Gnomon
	// setLineColour(vec3(1.0f, 0.0f, 1.0f));
	// drawLine(world_gnomon_data[0], world_gnomon_data[1]);
	// setLineColour(vec3(1.0f, 1.0f, 0.0f));
	// drawLine(world_gnomon_data[2], world_gnomon_data[3]);
	// setLineColour(vec3(0.0f, 1.0f, 1.0f));
	// drawLine(world_gnomon_data[4], world_gnomon_data[5]);

	// // World Gnomon
	// setLineColour(vec3(1.0f, 0.0f, 0.0f));
	// drawLine(model_gnomon_data[0], model_gnomon_data[1]);
	// setLineColour(vec3(0.0f, 1.0f, 0.0f));
	// drawLine(model_gnomon_data[2], model_gnomon_data[3]);
	// setLineColour(vec3(0.0f, 0.0f, 1.0f));
	// drawLine(model_gnomon_data[4], model_gnomon_data[5]);

	


	// Draw Viewport
	vec2 viewport_bl = vec2(viewport_tl.x, viewport_br.y);
	vec2 viewport_tr = vec2(viewport_br.x, viewport_tl.y);

	setLineColour(vec3(1.0f, 0.0f, 0.0f));
	drawLine(viewport_bl, viewport_br);
	drawLine(viewport_br, viewport_tr);
	drawLine(viewport_tr, viewport_tl);
	drawLine(viewport_tl, viewport_bl);


	mouse_dx = 0.0f;
}

void A2::clipLinePrimitives3d(vector<vec4> &new_data, vector<vec3> &new_colors)
{
	vector<vec4> data = new_data;
	vector<vec3> colors = new_colors;

	new_data.clear();
	new_colors.clear();

	// cout << data[0].x << ", " << data[0].y << ", " << data[0].z/data[0].w << endl;
	for(int i = 0; i < data.size(); i+=2)
	{
		auto result = clipLinePrimitive3d(data[i], data[i+1]);
		if(!result.empty()) new_colors.push_back(colors[i/2]);


		new_data.insert(new_data.end(), result.begin(), result.end());
	}
}

std::vector<glm::vec4> A2::clipLinePrimitives3d(vector<vec4> data)
{
	vector<vec4> new_data;
	new_data.reserve(data.size());
	// cout << data[0].x << ", " << data[0].y << ", " << data[0].z/data[0].w << endl;
	for(int i = 0; i < data.size(); i+=2)
	{
		auto result = clipLinePrimitive3d(data[i], data[i+1]);


		new_data.insert(new_data.end(), result.begin(), result.end());
	}
	return new_data;
}

std::vector<glm::vec4> A2::clipLinePrimitive3d(glm::vec4 a, glm::vec4 b)
{
	vector<glm::vec4> result;// = {a, b};

	// return vector<vec4>{a, b};

	int a_outcodes = 0;
	int b_outcodes = 0;

	// // if(a.w + a.x < 0) a_outcodes |= 1;
	// // if(a.w - a.x < 0) a_outcodes |= 2;
	// // if(a.w + a.y < 0) a_outcodes |= 4;
	// // if(a.w - a.y < 0) a_outcodes |= 8;
	// if(a.w + a.z < 0) a_outcodes |= 16;
	// if(a.w - a.z < 0) a_outcodes |= 32;

	// // if(b.w + b.x < 0) b_outcodes |= 1;
	// // if(b.w - b.x < 0) b_outcodes |= 2;
	// // if(b.w + b.y < 0) b_outcodes |= 4;
	// // if(b.w - b.y < 0) b_outcodes |= 8;
	// if(b.w + b.z < 0) b_outcodes |= 16;
	// if(b.w - b.z < 0) b_outcodes |= 32;

	if(a.w + a.z < 0) a_outcodes |= 16;
	if(a.w - a.z < 0) a_outcodes |= 32;

	if(b.w + b.z < 0) b_outcodes |= 16;
	if(b.w - b.z < 0) b_outcodes |= 32;

	const int all_ones = 32+16;
	// const int all_ones = 63;
	// cout << "bbbbbb" << endl;

	if((a_outcodes | b_outcodes)==0)
	{ 
		result.push_back(vec4(a.x/a.w, a.y/a.w, a.z/a.w, 1)); 
		result.push_back(vec4(b.x/b.w, b.y/b.w, b.z/b.w, 1)); 

		// cout <<"cccccccccccccc" << endl;

		return result; 
	}// Trivially accept
	if(a_outcodes & b_outcodes) return result; // Trivially reject
	//TODO Make NOT all_ones

	vec3 new_a = vec3(a.x/a.w, a.y/a.w, a.z/a.w);
	vec3 new_b = vec3(b.x/b.w, b.y/b.w, b.z/b.w);
	// Otherwise, not trivial case

	// cout << a_outcodes << " " << b_outcodes << endl;
	// cout << a.x << ", " << a.y << ", " << a.z/a.w << endl;

	// Near 
	if(a_outcodes & 16)
	{

		float t = (new_a - vec3(0, 0, -1)).z / (new_a - new_b).z;

		new_a = new_a + t*(new_b - new_a); // lt

		a_outcodes = 0;
		if(a.w + a.z < 0) a_outcodes |= 16;
		if(a.w - a.z < 0) a_outcodes |= 32;

		// cout <<  a.z << endl;
		// cout <<"a: " <<  new_a.x << ", " << new_a.y << ", " << new_a.z << endl;
	}
	else if(b_outcodes & 16)
	{
		float t = (new_b - vec3(0, 0, -1)).z / (new_b - new_a).z;

		new_b = new_b + t*(new_a - new_b);

		b_outcodes = 0;
		if(b.w + b.z < 0) b_outcodes |= 16;
		if(b.w - b.z < 0) b_outcodes |= 32;

		// cout << a.z << endl;
		// cout <<"b: " <<  new_b.x << ", " << new_b.y << ", " << new_b.z << endl;
	}

	// Far
	if(a_outcodes & 32)
	{

		float t = (new_a - vec3(0, 0, 1)).z*(-1) / (new_a - new_b).z*(-1);

		new_a = new_a + t*(new_b - new_a);

		a_outcodes = 0;
		if(a.w + a.z < 0) a_outcodes |= 16;
		if(a.w - a.z < 0) a_outcodes |= 32;

		// cout <<  a.z << endl;
		// cout <<"a: " <<  new_a.x << ", " << new_a.y << ", " << new_a.z << endl;
	}
	else if(b_outcodes & 32)
	{
		float t = (new_b - vec3(0, 0, 1)).z*(-1) / (new_b - new_a).z*(-1);

		new_b = new_b + t*(new_a - new_b);

		b_outcodes = 0;
		if(b.w + b.z < 0) b_outcodes |= 16;
		if(b.w - b.z < 0) b_outcodes |= 32;

		// cout << a.z << endl;
		// cout <<"b: " <<  new_b.x << ", " << new_b.y << ", " << new_b.z << endl;
	}

	result.push_back(vec4(new_a.x, new_a.y, new_a.z, 1));
	result.push_back(vec4(new_b.x, new_b.y, new_b.z, 1));

	return result;
}

void A2::drawLine( const glm::vec4 & p0, const glm::vec4 & p1)
{
	// vec2 v0 = vec2(p0.x/p0.z, p0.y/p0.z);
	// vec2 v1 = vec2(p1.x/p1.z, p1.y/p1.z);
	vec2 v0 = vec2(p0.x, p0.y);
	vec2 v1 = vec2(p1.x, p1.y);

	// 2D Clipping
	int a_outcodes = 0;
	int b_outcodes = 0;

	if(v0.x < -1) a_outcodes |= 1; // left 
	if(v0.x > 1) a_outcodes |= 2; // right
	if(v0.y < -1) a_outcodes |= 4; // bottom
	if(v0.y > 1) a_outcodes |= 8; // top

	if(v1.x < -1) b_outcodes |= 1;
	if(v1.x > 1) b_outcodes |= 2;
	if(v1.y < -1) b_outcodes |= 4;
	if(v1.y > 1) b_outcodes |= 8;

	const int all_ones = 15;

	if(!(a_outcodes | b_outcodes))
	{ 
		// Trivially accept
	}
	else if(a_outcodes & b_outcodes) return; // Trivially reject
	else
	{
		// Left 
		if(a_outcodes & 1)
		{
			float t = (v0 - vec2(-1,0)).x / (v0 - v1).x;

			v0 = v0 + t*(v1 - v0);

			// cout << lt.x << ", " << lt.y << endl;

			a_outcodes = 0;
			if(v0.x < -1) a_outcodes |= 1; // left 
			if(v0.x > 1) a_outcodes |= 2; // right
			if(v0.y < -1) a_outcodes |= 4; // bottom
			if(v0.y > 1) a_outcodes |= 8; // top
		}
		if(b_outcodes & 1)
		{
			float t = (v1 - vec2(-1,0)).x / (v1 - v0).x;

			v1 = v1 + t*(v0 - v1);

			b_outcodes = 0;
			if(v1.x < -1) b_outcodes |= 1; // left 
			if(v1.x > 1) b_outcodes |= 2; // right
			if(v1.y < -1) b_outcodes |= 4; // bottom
			if(v1.y > 1) b_outcodes |= 8; // top
		}

		// Right
		if(a_outcodes & 2)
		{
			float t = (v0 - vec2(1,0)).x / (v0 - v1).x;

			v0 = v0 + t*(v1 - v0);

			a_outcodes = 0;
			if(v0.x < -1) a_outcodes |= 1; // left 
			if(v0.x > 1) a_outcodes |= 2; // right
			if(v0.y < -1) a_outcodes |= 4; // bottom
			if(v0.y > 1) a_outcodes |= 8; // top
		}
		if(b_outcodes & 2)
		{
			float t = (v1 - vec2(1,0)).x / (v1 - v0).x;

			v1 = v1 + t*(v0 - v1);

			b_outcodes = 0;
			if(v1.x < -1) b_outcodes |= 1; // left 
			if(v1.x > 1) b_outcodes |= 2; // right
			if(v1.y < -1) b_outcodes |= 4; // bottom
			if(v1.y > 1) b_outcodes |= 8; // top
		}

		// Bottom
		if(a_outcodes & 4)
		{
			float t = (v0 - vec2(0,-1)).y / (v0 - v1).y;

			v0 = v0 + t*(v1 - v0);

			a_outcodes = 0;
			if(v0.x < -1) a_outcodes |= 1; // left 
			if(v0.x > 1) a_outcodes |= 2; // right
			if(v0.y < -1) a_outcodes |= 4; // bottom
			if(v0.y > 1) a_outcodes |= 8; // top
		}
		if(b_outcodes & 4)
		{
			float t = (v1 - vec2(0,-1)).y / (v1 - v0).y;

			v1 = v1 + t*(v0 - v1);

			b_outcodes = 0;
			if(v1.x < -1) b_outcodes |= 1; // left 
			if(v1.x > 1) b_outcodes |= 2; // right
			if(v1.y < -1) b_outcodes |= 4; // bottom
			if(v1.y > 1) b_outcodes |= 8; // top
		}

		// Top
		if(a_outcodes & 8)
		{
			float t = (v0 - vec2(0,1)).y / (v0 - v1).y;

			v0 = v0 + t*(v1 - v0); //lt

			a_outcodes = 0;
			if(v0.x < -1) a_outcodes |= 1; // left 
			if(v0.x > 1) a_outcodes |= 2; // right
			if(v0.y < -1) a_outcodes |= 4; // bottom
			if(v0.y > 1) a_outcodes |= 8; // top
		}
		if(b_outcodes & 8)
		{
			float t = (v1 - vec2(0,1)).y*(-1) / (v1 - v0).y*(-1);

			v1 = v1 + t*(v0 - v1); //lt

			b_outcodes = 0;
			if(v1.x < -1) b_outcodes |= 1; // left 
			if(v1.x > 1) b_outcodes |= 2; // right
			if(v1.y < -1) b_outcodes |= 4; // bottom
			if(v1.y > 1) b_outcodes |= 8; // top
		}
	}

	// Window to Viewport mapping
	v0.x = (viewport_br.x - viewport_tl.x)/(2.0f)*(v0.x + 1.0f) + viewport_tl.x;
	v0.y = (viewport_tl.y - viewport_br.y)/(2.0f)*(v0.y + 1.0f) + viewport_br.y;
	v1.x = (viewport_br.x - viewport_tl.x)/(2.0f)*(v1.x + 1.0f) + viewport_tl.x;
	v1.y = (viewport_tl.y - viewport_br.y)/(2.0f)*(v1.y + 1.0f) + viewport_br.y;

	drawLine(v0,v1);
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic()
{
	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);

		viewport_draw = false;
	
		if(ImGui::RadioButton( "Rotate View", &current_mode, 0 )) current_options = &rotate_view;
		if(ImGui::RadioButton( "Translate View", &current_mode, 1 )) current_options = &translate_view;
		if(ImGui::RadioButton( "Perspective", &current_mode, 2 )) current_options = &perspective_change;
		if(ImGui::RadioButton( "Rotate Model", &current_mode, 3 )) current_options = &rotate_model;
		if(ImGui::RadioButton( "Translate Model", &current_mode, 4 )) current_options = &translate_model;
		if(ImGui::RadioButton( "Scale Model", &current_mode, 5 )) current_options = &scale_model;
		if(ImGui::RadioButton( "Viewport", &current_mode, 6 )) viewport_draw = true;

		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Reset" ) ) {
			reset();
		}

		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		//ImGui::Text( "Far: %.2f", perspective_far );

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );
		ImGui::Text( "Near: %.2f, Far: %.2f", perspective_near, perspective_far);
		ImGui::Text( "FOV: %.1f degrees", float(perspective_fov/M_PI*180));

	ImGui::End();
}

//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos() {

	//-- Copy vertex position data into VBO, m_vbo_positions:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
				m_vertexData.positions.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	//-- Copy vertex colour data into VBO, m_vbo_colours:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
				m_vertexData.colours.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw()
{
	uploadVertexDataToVbos();

	glBindVertexArray(m_vao);

	m_shader.enable();
		glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A2::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		if(mouse_dragging){
			if(mouse_dragging == 2)
			{
				mouse_dragging = 1;
				last_mouse_pos = glm::vec2(xPos, yPos);
				last_mouse_click_pos = last_mouse_pos;
			}
			else if (mouse_dragging == 1)
			{
				float d_x = xPos - last_mouse_pos.x;
				last_mouse_pos = glm::vec2(xPos, yPos);
				mouse_dx = d_x;
				
			}
		}
	}

	eventHandled = true;

	// TODO Ensure this is reset when not moving
	// cout << mouse_dx << endl;

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A2::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// The user clicked in the window.  If it's the left
		// mouse button, initiate a rotation.

		bvec all_false;

		if (button == GLFW_MOUSE_BUTTON_LEFT){
			// Respond to some key events.
			if(actions == GLFW_PRESS){ mouse_dragging = 2; pressed_buttons.x = true; current_options->x = true;}
			else if(actions == GLFW_RELEASE){ pressed_buttons.x = false; 
											  //  cout << pressed_buttons.x << ", " << pressed_buttons.y << ", " << pressed_buttons.z << endl;
											  if(!pressed_buttons.x && !pressed_buttons.y && !pressed_buttons.z) mouse_dragging = 0; 
											  mouse_dx = 0.0f; 
											  current_options->x = false;}
		}
		else if (button == GLFW_MOUSE_BUTTON_MIDDLE){
			// Respond to some key events.
			if(actions == GLFW_PRESS){ mouse_dragging = 2; pressed_buttons.y = true; current_options->y = true;}
			else if(actions == GLFW_RELEASE){ pressed_buttons.y = false; 
											  if(!pressed_buttons.x && !pressed_buttons.y && !pressed_buttons.z)mouse_dragging = 0; 
											  mouse_dx = 0.0f; 
											  current_options->y = false;}
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT){
			// Respond to some key events.
			if(actions == GLFW_PRESS){ mouse_dragging = 2; pressed_buttons.z = true; current_options->z = true;}
			else if(actions == GLFW_RELEASE){ pressed_buttons.z = false; 
											  if(!pressed_buttons.x && !pressed_buttons.y && !pressed_buttons.z)mouse_dragging = 0; 
											  mouse_dx = 0.0f; 
											  current_options->z = false;}
		}

		// cout << mouse_dragging << endl;
	}

	eventHandled = true;

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A2::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A2::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS )
	{
		if(key == GLFW_KEY_O) { current_mode = 0; current_options = &rotate_view; }
		if(key == GLFW_KEY_E) { current_mode = 1; current_options = &translate_view; }
		if(key == GLFW_KEY_P) { current_mode = 2; current_options = &perspective_change; }
		if(key == GLFW_KEY_R) { current_mode = 3; current_options = &rotate_model; }
		if(key == GLFW_KEY_T) { current_mode = 4; current_options = &translate_model; }
		if(key == GLFW_KEY_S) { current_mode = 5; current_options = &scale_model; }
		if(key == GLFW_KEY_V) { current_mode = 6; viewport_draw = true; }
		if(key == GLFW_KEY_A) reset();
		if(key == GLFW_KEY_Q) glfwSetWindowShouldClose(m_window, GL_TRUE);
	}

	eventHandled = true;

	return eventHandled;
}

glm::mat4 A2::createScale(glm::vec3 s)
{
	return  glm::transpose(glm::mat4{s.x, 0, 0, 0,
									 0, s.y, 0, 0,
									 0, 0, s.z, 0,
									 0, 0, 0, 1});
}

glm::mat4 A2::createTransform(glm::vec3 t)
{
	return glm::transpose(glm::mat4{1, 0, 0, t.x,
									0, 1, 0, t.y,
									0, 0, 1, t.z,
									0, 0, 0, 1});
}

glm::mat4 A2::createRotation(glm::vec3 r)
{
	return glm::transpose(glm::mat4{cos(r.z), -sin(r.z), 0, 0,
									sin(r.z), cos(r.z), 0, 0,
									0, 0, 1, 0,
									0, 0, 0, 1}*
						  glm::mat4{1, 0, 0, 0,
									0, cos(r.x), -sin(r.x), 0,
									0, sin(r.x), cos(r.x), 0,
									0, 0, 0, 1}*
						  glm::mat4{cos(r.y), 0, sin(r.y), 0,
									0, 1, 0, 0,
									-sin(r.y), 0, cos(r.y), 0,
									0, 0, 0, 1});
}

float cotan(float i) { return(1 / tan(i)); }

glm::mat4 A2::createPerspective(float n, float f, float fov, float aspect)
{
	const int z_look_down = 1; // Constant, must be 1 or -1, represents if we look down +z or -z axis
	float cot = cotan(fov/2);
	return glm::transpose(glm::mat4{cot/aspect, 0, 0, 0,
							0, cot, 0, 0,
							0, 0, z_look_down*(f+n)/(f-n), -2*f*n/(f-n),
							0, 0, z_look_down, 0});
}

void A2::reset()
{
	cube = vector<vec3>{vec3(1,1,1), vec3(-1,1,1),	
						vec3(-1,1,1), vec3(-1,-1,1),
						vec3(-1,-1,1), vec3(1,-1,1),
						vec3(1,-1,1), vec3(1,1,1),

						vec3(1,1,-1), vec3(-1,1,-1),	
						vec3(-1,1,-1), vec3(-1,-1,-1),
						vec3(-1,-1,-1), vec3(1,-1,-1),
						vec3(1,-1,-1), vec3(1,1,-1),

						vec3(1,1,1), vec3(1,1,-1),
						vec3(-1,1,1), vec3(-1,1,-1),
						vec3(-1,-1,1), vec3(-1,-1,-1),
						vec3(1,-1,1), vec3(1,-1,-1)};

	current_options = &rotate_model;
	current_mode = 3;

	cube_scale= glm::vec3(1, 1, 1);
	cube_rot = vec3(0,0,0);
	cube_trans = glm::vec3(0, 0, 0);
	model_no_scale = mat4(1.0f);

	perspective_fov = M_PI/6;
	perspective_near = 1;
	perspective_far = 20;

	view_rot = vec3(0,0,0);
	view_trans = glm::vec3(0,0,10);
	view = mat4(1.0f);

	viewport_tl = glm::vec2(-0.9,0.9);
	viewport_br = glm::vec2(0.9,-0.9);


	// mouse_dx = 0.0f;
	// mouse_dragging = 0;

	// glm::vec2 last_mouse_pos;
	// glm::vec2 last_mouse_click_pos;
}