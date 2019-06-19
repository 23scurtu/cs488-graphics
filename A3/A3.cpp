// Winter 2019

#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

#include <math.h>

#include "trackball.hpp"

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

void pvec(vec3 v) { cout << v.x << ", " << v.y << ", " << v.z << endl; }
void pvec(vec2 v) { cout << v.x << ", " << v.y << endl; }

//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0)
{

}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Set the background colour.
	// glClearColor(0.85, 0.85, 0.85, 1.0);
	glClearColor(0.35, 0.35, 0.35, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);
	head_mesh = static_cast<GeometryNode*>( getHeadNode(m_rootNode.get()));
	if(head_mesh) head_joint = static_cast<JointNode*>(head_mesh->parent);

	// cout << head->m_nodeId << endl;

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();


	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.

	last_time =  chrono::system_clock::now();
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could Not Open " << filename << std::endl;
	}

	root_original = m_rootNode->trans;
	root_invoriginal = m_rootNode->invtrans;
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	m_light.position = vec3(-2.0f, 5.0f, 0.0f);
	// m_light.position = vec3(0.0f, 0.0f, 0.0f);
	m_light.rgbIntensity = vec3(0.9f); // light
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;

		location = m_shader.getUniformLocation("picking");
		glUniform1i( location, do_picking ? 1 : 0 );

		if(!do_picking)
		{
			//-- Set LightSource uniform for the scene:
			{
				location = m_shader.getUniformLocation("light.position");
				glUniform3fv(location, 1, value_ptr(m_light.position));
				location = m_shader.getUniformLocation("light.rgbIntensity");
				glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
				CHECK_GL_ERRORS;
			}

			//-- Set background light ambient intensity
			{
				location = m_shader.getUniformLocation("ambientIntensity");
				vec3 ambientIntensity(0.05f);
				glUniform3fv(location, 1, value_ptr(ambientIntensity));
				CHECK_GL_ERRORS;
			}
		}
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	// Place per frame, application logic here ...
	switch(current_mode)
	{
		// Position/Orientation
		case 0: if(mouse_dragging == 1)
				{
					if(pressed_buttons.x)
					{
						float trans_rate = 0.02;
						vec2 trans = mouse_pos - last_mouse_pos;
						// m_rootNode->translate(vec3(trans.x*trans_rate, -trans.y*trans_rate, 0));

						auto amount = vec3(trans.x*trans_rate, -trans.y*trans_rate, 0);

						root_translation = glm::translate(amount) * root_translation;
						root_invtranslation = root_invtranslation * glm::translate(-amount);
					}

					if(pressed_buttons.y)
					{
						float trans_rate = 0.02;
						vec2 trans = mouse_pos - last_mouse_pos;
						// m_rootNode->translate(vec3(0, 0, trans.y*trans_rate));
						auto amount = vec3(0, 0, trans.y*trans_rate);

						root_translation = glm::translate(amount) * root_translation;
						root_invtranslation = root_invtranslation * glm::translate(-amount);
					}

					if(pressed_buttons.z)
					{

						vec2 normalized_mouse_pos = mouse_pos - trackball_pos;
						vec2 normalized_last_mouse_pos = last_mouse_pos - trackball_pos;

						
					
						vec3 rot = vCalcRotVec(normalized_mouse_pos.x, normalized_mouse_pos.y,
											normalized_last_mouse_pos.x, normalized_last_mouse_pos.y,
											trackball_diam);

						// cout << trackball_diam << endl;
						// cout << m_framebufferHeight<< endl;

						

						// pvec(rot);
						// cout << glm::length(rot) << endl;

						if(rot != vec3(0,0,0))
						{
							// apply rotation to root
							// TODO INQUIRE ABOUT THE ROTATION OF A3MARK
							// Assume (center of the puppet) is the scene center.
							// m_rootNode->rotateLocal(rot, glm::length(rot)*100);
							float angle = glm::length(rot)*100;
							mat4 rot_matrix = glm::rotate(degreesToRadians(angle), rot);
							mat4 invrot_matrix = glm::rotate(degreesToRadians(-angle), rot);

							root_rotation = root_rotation * rot_matrix;
							root_invrotation = invrot_matrix * root_invrotation ;

							
						}
					}

					// Keeping track of rot and trans seperatly to be able to reset them.
					m_rootNode->trans = root_translation * root_original * root_rotation;
					m_rootNode->invtrans = root_invrotation * root_invoriginal * root_invtranslation;
				}
			    break;
		// Joints
		case 1: if(mouse_dragging == 1)
				{
					if(pressed_buttons.y)
					{
						float trans_rate = 1;
						// vec2 trans = mouse_pos - last_mouse_pos;
						// m_rootNode->translate(vec3(trans.x*trans_rate, -trans.y*trans_rate, 0));

						for (std::pair<unsigned int, JointSelection> selection : selected_geometry_nodes)
						{
							// std::cout << element.first << " :: " << element.second << std::endl;
							selection.second.jnode->clampedRotate('x', -trans_rate*(mouse_pos - last_mouse_pos).y);
						}
						//TODO Add HEAD rotation!
					}

					if(pressed_buttons.z)
					{
						float trans_rate = 1;
							// std::cout << element.first << " :: " << element.second << std::endl;

						if(head_joint && head_mesh)
						{
							if(selected_geometry_nodes.find(head_mesh->m_nodeId) != selected_geometry_nodes.end())
								head_joint->clampedRotate('y', -trans_rate*(mouse_pos - last_mouse_pos).y);
						}
					}
				}
				break;

	}

	mouse_dy = 0;

	uploadCommonSceneUniforms();

	auto current_time = chrono::system_clock::now();

	warning_time -= std::chrono::duration<double>(current_time -  last_time).count();
	if(warning_time < 0) warning_time = 0;

	last_mouse_pos = mouse_pos;
	last_time = current_time;
}

void A3::resetPosition()
{
	root_translation = mat4(1.0f);
	root_invtranslation = mat4(1.0f);
	
	m_rootNode->trans = root_translation * root_original * root_rotation;
	m_rootNode->invtrans = root_invrotation * root_invoriginal * root_invtranslation;
}

void A3::resetOrientation()
{
	root_rotation = mat4(1.0f);
	root_invrotation = mat4(1.0f);
	
	m_rootNode->trans = root_translation * root_original * root_rotation;
	m_rootNode->invtrans = root_invrotation * root_invoriginal * root_invtranslation;
}

void A3::resetJoints()
{
	while(stack_ptr != -1)
	{
		undo();
	}

	// TODO Release pointers!?
	command_stack.clear();
	changed_joints.clear();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
{
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Application")) {
            if( ImGui::MenuItem( "Reset Position (I)" ) ) resetPosition();
			if( ImGui::MenuItem( "Reset Orientation (O)" ) ) resetOrientation();
			if( ImGui::MenuItem( "Reset Joints (S)" ) ) resetJoints();
			if( ImGui::MenuItem( "Reset All (A)" ) ){ resetPosition(); resetOrientation(); resetJoints();}
			if( ImGui::MenuItem( "Quit Application (Q)" ) ) {
				glfwSetWindowShouldClose(m_window, GL_TRUE);
			}

            ImGui::EndMenu();
        }

		if (ImGui::BeginMenu("Options")) {
				ImGui::Checkbox("Circle (C)", &draw_trackball);
				ImGui::Checkbox("Z-buffer (Z)", &zbuffer);
				ImGui::Checkbox("Backface culling (B)", &backface_culling);
				ImGui::Checkbox("Frontface culling (F)", &frontface_culling);
            ImGui::EndMenu();
        }

		if (ImGui::BeginMenu("Edit")) {
				if( ImGui::Button( "Undo (U)" ) ) undo(true);
				if( ImGui::Button( "Redo (R)" ) ) redo(true);
            ImGui::EndMenu();
        }

		if (ImGui::BeginMenu("Interaction Mode")) {
				if(ImGui::RadioButton( "Position/Orientation (P)", &current_mode, 0 ));
				if(ImGui::RadioButton( "Joints (J)", &current_mode, 1 ));
            ImGui::EndMenu();
        }

		// if(ImGui::RadioButton( "Position/Orientation (P)", &current_mode, 0 ));
		// if(ImGui::RadioButton( "Joints (J)", &current_mode, 1 ));

        ImGui::EndMainMenuBar();
    }

	// ImGui::SetCursorPos(ImVec2(200,100));
	if(warning_time != 0.0f) 
	{
		// ImGui::SetCursorPos(ImVec2(200,100));
		ImGui::Begin("Warning!", &showDebugWindow, ImVec2(100, 100), opacity, windowFlags);
				ImGui::Text( "Cannot undo or redo past \n the end of the stack!");
		ImGui::End();
	}

	// ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
	// 		windowFlags);


	// 	// Add more gui elements here here ...

	// 	ImGui::Text( "Application");
	// 	if( ImGui::Button( "Reset Position (I)" ) ) resetPosition();
	// 	if( ImGui::Button( "Reset Orientation (O)" ) ) resetOrientation();
	// 	if( ImGui::Button( "Reset Joints (S)" ) ) resetJoints();
	// 	if( ImGui::Button( "Reset All (A)" ) ){ resetPosition(); resetOrientation(); resetJoints();}
	// 	if( ImGui::Button( "Quit Application (Q)" ) ) {
	// 		glfwSetWindowShouldClose(m_window, GL_TRUE);
	// 	}

	// 	ImGui::Text( "Options");
	// 	ImGui::Checkbox("Circle (C)", &draw_trackball);
	// 	ImGui::Checkbox("Z-buffer (Z)", &zbuffer);
	// 	ImGui::Checkbox("Backface culling (B)", &backface_culling);
	// 	ImGui::Checkbox("Frontface culling (F)", &frontface_culling);

	// 	ImGui::Text( "Edit");
	// 	if( ImGui::Button( "Undo (U)" ) ) undo(true);
	// 	if( ImGui::Button( "Redo (R)" ) ) redo(true);

	// 	ImGui::Text( "Interaction Mode");
	// 	if(ImGui::RadioButton( "Position/Orientation (P)", &current_mode, 0 ));
	// 	if(ImGui::RadioButton( "Joints (J)", &current_mode, 1 ));

	// 	ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	// 	// if(warning_time != 0.0f) 
	// 	// {
	// 	// 	ImGui::Text( "" );
	// 	// 	ImGui::Text( "Cannot undo or redo past \n the end of the stack!");
	// 	// }

	// ImGui::End();

	///////////////////////////////////////////

	// 	// Add more gui elements here here ...

	// 	ImGui::Text( "Application");
	// 	if( ImGui::Button( "Reset Position (I)" ) ) resetPosition();
	// 	if( ImGui::Button( "Reset Orientation (O)" ) ) resetOrientation();
	// 	if( ImGui::Button( "Reset Joints (S)" ) ) resetJoints();
	// 	if( ImGui::Button( "Reset All (A)" ) ){ resetPosition(); resetOrientation(); resetJoints();}
	// 	if( ImGui::Button( "Quit Application (Q)" ) ) {
	// 		glfwSetWindowShouldClose(m_window, GL_TRUE);
	// 	}

	// 	ImGui::Text( "Options");
	// 	ImGui::Checkbox("Circle (C)", &draw_trackball);
	// 	ImGui::Checkbox("Z-buffer (Z)", &zbuffer);
	// 	ImGui::Checkbox("Backface culling (B)", &backface_culling);
	// 	ImGui::Checkbox("Frontface culling (F)", &frontface_culling);

	// 	ImGui::Text( "Edit");
	// 	if( ImGui::Button( "Undo (U)" ) ) undo(true);
	// 	if( ImGui::Button( "Redo (R)" ) ) redo(true);

	// 	ImGui::Text( "Interaction Mode");
	// 	if(ImGui::RadioButton( "Position/Orientation (P)", &current_mode, 0 ));
	// 	if(ImGui::RadioButton( "Joints (J)", &current_mode, 1 ));

	// 	ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	// 	if(warning_time != 0.0f) 
	// 	{
	// 		ImGui::Text( "" );
	// 		ImGui::Text( "Cannot undo or redo past \n the end of the stack!");
	// 	}

	// ImGui::End();
}

bool A3::selected(const GeometryNode* gnode)
{
	// TODO Make selected into a set?
	return selected_geometry_nodes.find(gnode->m_nodeId) != selected_geometry_nodes.end();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
void A3::updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix,
		const glm::mat4 & modelMatrix
) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * modelMatrix;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		if( do_picking ) {
			unsigned int idx = node.m_nodeId;
			// TODO Encode in idx

			float r = float(idx&0xff) / 255.0f;
			float g = float((idx>>8)&0xff) / 255.0f;
			float b = float((idx>>16)&0xff) / 255.0f;

			location = shader.getUniformLocation("material.kd");
			glUniform3f( location, r, g, b );
			CHECK_GL_ERRORS;
		} else {
			//-- Set NormMatrix:
			location = shader.getUniformLocation("NormalMatrix");
			mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
			glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
			CHECK_GL_ERRORS;


			Material selected_material = Material();
			selected_material.kd = vec3(1,1,0);

			auto old_material = node.material;

			//-- Set Material values:
			location = shader.getUniformLocation("material.kd");
			vec3 kd = node.material.kd;
			if(selected(&node)) kd = selected_material.kd;
			glUniform3fv(location, 1, value_ptr(kd));
			CHECK_GL_ERRORS;
			location = shader.getUniformLocation("material.ks");
			vec3 ks = node.material.ks;
			glUniform3fv(location, 1, value_ptr(ks));
			CHECK_GL_ERRORS;
			location = shader.getUniformLocation("material.shininess");
			glUniform1f(location, node.material.shininess);
			CHECK_GL_ERRORS;
		}

	}
	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {

	glEnable( GL_DEPTH_TEST );

	if(!zbuffer) glDisable(GL_DEPTH_TEST);

	glDisable(GL_CULL_FACE);
	if(backface_culling || frontface_culling) glEnable(GL_CULL_FACE);
	if(backface_culling && frontface_culling) glCullFace(GL_FRONT_AND_BACK);
	else if(backface_culling) glCullFace(GL_BACK);
	else if(frontface_culling) glCullFace(GL_FRONT);

	renderSceneGraph(*m_rootNode);


	glDisable( GL_DEPTH_TEST );
	renderArcCircle();
}

void A3::renderSceneNode(const SceneNode &root, mat4 parent_transform)
{
	if (root.m_nodeType == NodeType::GeometryNode)
	{
		// cout << "hey" << endl;
		const GeometryNode * geometryNode = static_cast<const GeometryNode *>(&root);

		updateShaderUniforms(m_shader, *geometryNode, m_view, parent_transform * root.get_transform());

		// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
		BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

		//-- Now render the mesh:
		m_shader.enable();
		glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
		m_shader.disable();
	}

	parent_transform = parent_transform * root.get_transform();

	for (const SceneNode * node : root.children) {
		renderSceneNode(*node, parent_transform);
	}
}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph(const SceneNode & root) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	// This is emphatically *not* how you should be drawing the scene graph in
	// your final implementation.  This is a non-hierarchical demonstration
	// in which we assume that there is a list of GeometryNodes living directly
	// underneath the root node, and that we can draw them in a loop.  It's
	// just enough to demonstrate how to get geometry and materials out of
	// a GeometryNode and onto the screen.

	// You'll want to turn this into recursive code that walks over the tree.
	// You can do that by putting a method in SceneNode, overridden in its
	// subclasses, that renders the subtree rooted at every node.  Or you
	// could put a set of mutually recursive functions in this class, which
	// walk down the tree from nodes of different types.

	renderSceneNode(root, mat4(1.0f));

	// for (const SceneNode * node : root.children) {

	// 	if (node->m_nodeType != NodeType::GeometryNode)
	// 		continue;

	// 	const GeometryNode * geometryNode = static_cast<const GeometryNode *>(node);

	// 	updateShaderUniforms(m_shader, *geometryNode, m_view);


	// 	// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
	// 	BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

	// 	//-- Now render the mesh:
	// 	m_shader.enable();
	// 	glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
	// 	m_shader.disable();
	// }

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle() {
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
		GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
		float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
		glm::mat4 M;
		if( aspect > 1.0 ) {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
		} else {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
		}

		trackball_pos = vec2(m_framebufferWidth/2, m_framebufferHeight/2);
		trackball_diam = std::min(0.5f*m_framebufferWidth, 0.5f*m_framebufferHeight);

		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
		// Only draw circle if visual is enabled.
		if(draw_trackball) glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
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
bool A3::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(true);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		if(mouse_dragging){
			if(mouse_dragging == 2)
			{
				mouse_dragging = 1;
				last_mouse_pos = glm::vec2(xPos, yPos);
				// last_mouse_click_pos = last_mouse_pos;
				mouse_pos = last_mouse_pos;
			}
			else if (mouse_dragging == 1)
			{
				float d_y = xPos - last_mouse_pos.y;
				last_mouse_pos = mouse_pos;
				mouse_pos = glm::vec2(xPos, yPos);
				mouse_dy = d_y;
			}
		}
	}

	return eventHandled;
}

SceneNode* A3::getSceneNode(unsigned int id, SceneNode* root)
{
	if(root->m_nodeId == id) return root;

	for (SceneNode * node : root->children) {
		SceneNode* result = getSceneNode(id, node);
		if(result) return result;
	}

	return nullptr;
}

SceneNode* A3::getHeadNode(SceneNode *root)
{
	if(root->m_name == "head") return root;

	for (SceneNode * node : root->children) {
		SceneNode* result = getHeadNode(node);

		if(result) return result;
	}

	return nullptr;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(true);

	// Fill in with event handling code...

	if (current_mode == 1 && button == GLFW_MOUSE_BUTTON_LEFT && actions == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos( m_window, &xpos, &ypos );

		do_picking = true;

		uploadCommonSceneUniforms();
		// TODO Make default clear color field?
		glClearColor(1.0, 1.0, 1.0, 1.0 );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glClearColor(0.35, 0.35, 0.35, 1.0);
		// glClearColor(0.85, 0.85, 0.85, 1.0);

		draw();

		// I don't know if these are really necessary anymore.
		// glFlush();
		// glFinish();

		CHECK_GL_ERRORS;

		// Ugly -- FB coordinates might be different than Window coordinates
		// (e.g., on a retina display).  Must compensate.
		xpos *= double(m_framebufferWidth) / double(m_windowWidth);
		// WTF, don't know why I have to measure y relative to the bottom of
		// the window in this case.
		ypos = m_windowHeight - ypos;
		ypos *= double(m_framebufferHeight) / double(m_windowHeight);

		GLubyte buffer[ 4 ] = { 0, 0, 0, 0 };
		// A bit ugly -- don't want to swap the just-drawn false colours
		// to the screen, so read from the back buffer.
		glReadBuffer( GL_BACK );
		// Actually read the pixel at the mouse location.
		glReadPixels( int(xpos), int(ypos), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
		CHECK_GL_ERRORS;

		// Reassemble the object ID.
		unsigned int what = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16);

		// cout << what << endl;
		SceneNode* selected = getSceneNode(what,m_rootNode.get());
		// cout << "pointer " << selected << endl;
		if(selected && selected->parent->m_nodeType == NodeType::JointNode) 
		{
			if(selected_geometry_nodes.find(what) == selected_geometry_nodes.end())
			{
				selected_geometry_nodes[what] = JointSelection(static_cast<GeometryNode *>(selected), 
															   static_cast<JointNode *>(selected->parent));
			}
			else
				selected_geometry_nodes.erase(what);
			// cout << "selected count: " << selected_geometry_nodes.size() << endl;
		}

		// if( what < xforms.size() ) {
		// 	selected[what] = !selected[what];
		// }

		do_picking = false;

		CHECK_GL_ERRORS;
	}

	bool dragging_stopped = false;
	bool rotation_drag_start = false;
	bool rotation_drag_stop = false;

	if (!ImGui::IsMouseHoveringAnyWindow()) {

		if (button == GLFW_MOUSE_BUTTON_LEFT){
			// Respond to some key events.
			if(actions == GLFW_PRESS){ mouse_dragging = 2; pressed_buttons.x = true;/* current_options->x = true;*/}
			else if(actions == GLFW_RELEASE){ pressed_buttons.x = false; 
											  //  cout << pressed_buttons.x << ", " << pressed_buttons.y << ", " << pressed_buttons.z << endl;
											  if(!pressed_buttons.x && !pressed_buttons.y && !pressed_buttons.z){ mouse_dragging = 0; dragging_stopped = true; }
											  mouse_dy = 0.0f; 
											  /*current_options->x = false;*/}
		}
		else if (button == GLFW_MOUSE_BUTTON_MIDDLE){
			// Respond to some key events.
			if(actions == GLFW_PRESS){ mouse_dragging = 2; pressed_buttons.y = true; rotation_drag_start = true;}
			else if(actions == GLFW_RELEASE){ pressed_buttons.y = false; 
											  if(!pressed_buttons.x && !pressed_buttons.y && !pressed_buttons.z){ mouse_dragging = 0; dragging_stopped = true; }
											  mouse_dy = 0.0f; 
											  rotation_drag_stop = true;
											  /*current_options->y = false;*/}
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT){
			// Respond to some key events.
			if(actions == GLFW_PRESS){ mouse_dragging = 2; pressed_buttons.z = true; rotation_drag_start = true;}
			else if(actions == GLFW_RELEASE){ pressed_buttons.z = false; 
											  if(!pressed_buttons.x && !pressed_buttons.y && !pressed_buttons.z){ mouse_dragging = 0; dragging_stopped = true; }
											  mouse_dy = 0.0f; 
											  rotation_drag_stop = true;
											  /*current_options->z = false;*/}
		}
	}

	if( rotation_drag_start && current_mode == 1 )
	{
		// cout << "Drag started" << endl;
		//Dragging started in jointmode
		for (std::pair<unsigned int, JointSelection> selection : selected_geometry_nodes)
		{
			// Store initial values of joints
			changed_joints[ selection.second.jnode ] = selection.second.jnode->getTransformPack();
		}


		// cout << "eee" << endl;
		// If new actions done, erase possible redos.
		if(!command_stack.empty()) command_stack.erase (command_stack.begin() + stack_ptr + 1, command_stack.end());
		// cout << "aaaa" << endl;

	}

	if( rotation_drag_stop && current_mode == 1 )
	{
		// cout << "Drag stopped" << endl;
		//Set changes in joints as a new command.

		CommandFrame new_frame;

		for (std::pair<JointNode*, JointTransform> joint : changed_joints)
		{
			// std::cout << element.first << " :: " << element.second << std::endl;
			// selection.second.jnode->rotate('x', -trans_rate*(mouse_pos - last_mouse_pos).y);
			
			new_frame.addCommand(new JointChangeCommand(joint.first, joint.second));
		}
		// cout << "eee" << endl;

		command_stack.push_back(new_frame);

		// cout << "aaaa" << endl;

		stack_ptr += 1;


		changed_joints.clear();
		
	}

	// cout << "command_stack_size: " << command_stack.size() << endl;

	return eventHandled;
}

void A3::undo(bool human_called)
{
	if(!command_stack.empty() && stack_ptr >= 0) 
	{
		command_stack[stack_ptr].undo();
		stack_ptr -= 1;
	}
	else if(human_called)
	{
		warning_time = warning_duration;
	}
	
}

void A3::redo(bool human_called)
{
	// cout << "stack_ptr " << stack_ptr << endl;
	if(!command_stack.empty()  && stack_ptr < int(command_stack.size()) - 1) 
	{
		// cout << "Redo" << endl;
		stack_ptr += 1;
		command_stack[stack_ptr].execute();
	}
	else if(human_called)
	{
		warning_time = warning_duration;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
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
bool A3::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(true);

	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_M ) {
			show_gui = !show_gui;
			eventHandled = true;
		}

		if(key == GLFW_KEY_I) resetPosition();
		if(key == GLFW_KEY_O) resetOrientation();
		if(key == GLFW_KEY_S) resetJoints();
		if(key == GLFW_KEY_A) { resetPosition(); resetOrientation(); resetJoints();}
		if(key == GLFW_KEY_Q) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		// ImGui::Text( "Options");

		if(key == GLFW_KEY_C) draw_trackball = !draw_trackball;
		if(key == GLFW_KEY_Z) zbuffer = !zbuffer;
		if(key == GLFW_KEY_B) backface_culling = !backface_culling;
		if(key == GLFW_KEY_F) frontface_culling = !frontface_culling;

		// ImGui::Text( "Edit");
		if( key == GLFW_KEY_U ) undo(true);
		if( key == GLFW_KEY_R ) redo(true);

		if( key == GLFW_KEY_P ) current_mode = 0;
		if( key == GLFW_KEY_J ) current_mode = 1;

		// ImGui::Text( "Interaction Mode");
		// if(ImGui::RadioButton( "Position/Orientation", &current_mode, 0 ));
		// if(ImGui::RadioButton( "Joints", &current_mode, 1 ));
	}
	// Fill in with event handling code...

	return eventHandled;
}
