// Winter 2019

#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>

#include <sys/types.h>
#include <unistd.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include "maze_mesh.hpp"
#include <algorithm>

using namespace glm;
using namespace std;

static const size_t DIM = 16;

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: current_col( 0 )
{
	colour[0] = 0.0f;
	colour[1] = 0.0f;
	colour[2] = 0.0f;
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
	// Initialize random number generator
	int rseed=getpid();
	srandom(rseed);
	// Print random number seed in case we want to rerun with
	// same random numbers
	cout << "Random number seed = " << rseed << endl;

	dig();
	
	// Set the background colour.
	glClearColor( 0.3, 0.5, 0.7, 1.0 );

	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath( "VertexShader.vs" ).c_str() );
	m_shader.attachFragmentShader(
		getAssetFilePath( "FragmentShader.fs" ).c_str() );
	m_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation( "P" );
	V_uni = m_shader.getUniformLocation( "V" );
	M_uni = m_shader.getUniformLocation( "M" );
	col_uni = m_shader.getUniformLocation( "colour" );

	initGrid();

	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
	view = glm::lookAt( 
		glm::vec3( 0.0f, 2.*float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2 ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );

	proj = glm::perspective( 
		glm::radians( 30.0f ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );
}

void A1::initGrid()
{
	size_t sz = 3 * 2 * 2 * (DIM+3);

	float *verts = new float[ sz ];
	size_t ct = 0;
	for( int idx = 0; idx < DIM+3; ++idx ) {
		verts[ ct ] = -1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = idx-1;
		verts[ ct+3 ] = DIM+1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = idx-1;
		ct += 6;

		verts[ ct ] = idx-1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = -1;
		verts[ ct+3 ] = idx-1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = DIM+1;
		ct += 6;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_grid_vao );
	glBindVertexArray( m_grid_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_grid_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_grid_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	// avatar_pos = glm::vec2(avatar->getTranslation().x, avatar->getTranslation().z);

	// Place per frame, application logic here ...
	if(move_commands[0] && (!maze->getValue(avatar_pos.x - 1, avatar_pos.y) || wall_break)) avatar_pos.x -= 1;
	else if(move_commands[1] && (!maze->getValue(avatar_pos.x, avatar_pos.y + 1)  || wall_break)) avatar_pos.y += 1;
	else if(move_commands[2] && (!maze->getValue(avatar_pos.x + 1, avatar_pos.y)  || wall_break)) avatar_pos.x += 1;
	else if(move_commands[3] && (!maze->getValue(avatar_pos.x, avatar_pos.y - 1)  || wall_break)) avatar_pos.y -= 1;

	avatar_pos = vec2(clamp(avatar_pos.x, -1.0f,float(DIM)), clamp(avatar_pos.y, -1.0f, float(DIM)));

	if(wall_break){
		maze->setValue(avatar_pos.x, avatar_pos.y, 0);

		// TODO Make better update function, store cubes in 2d vector internally.
		maze_mesh->setMaze(*maze);
	}

	// cout << "(" << avatar_pos.x << ", " << avatar_pos.y << ")" << endl;
	// cout << maze->getValue(avatar_pos.x,avatar_pos.y);

	// TODO Pull out avatar radius
	avatar->rotation *= quat(vec3(0,PI/2,0));
	avatar->setTranslation(glm::vec3(avatar_pos.x + 0.5f, 0.5f, avatar_pos.y + 0.5f)); 
	
	// avatar->setTranslation(glm::vec3(-5, 0.0f,-5)); 


	// avatar->setTranslation();

	for(size_t i = 0; i < 4; i++) move_commands[i] = false;

	if(dragging == 0 && last_dx){
		maze_mesh->maze_rotation += 0.006*last_dx;
		last_dx *= 0.85f;
		cout << last_dx << endl;
		if(last_dx < 0.0001f && last_dx > -0.0001f) last_dx = 0.0f;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
	// We already know there's only going to be one window, so for 
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A1 running simultaneously, this would break; you'd want to make
	// this into instance fields of A1.
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100,100), opacity, windowFlags);
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		if( ImGui::Button( "Reset" ) ) {
			reset_maze();
		}

		// Eventually you'll create multiple colour widgets with
		// radio buttons.  If you use PushID/PopID to give them all
		// unique IDs, then ImGui will be able to keep them separate.
		// This is unnecessary with a single colour selector and
		// radio button, but I'm leaving it in as an example.

		// Prefixing a widget name with "##" keeps it from being
		// displayed.

		ImGui::PushID( 0 );
		ImGui::ColorEdit3( "##Colour", colour );

		ImGui::SameLine();
		if( ImGui::RadioButton( "Maze Block Color", &current_col, 0 ) ) {
			// Select this colour.
			// current_col = 0;
			auto maze_color = maze_mesh->getWallColor();
			colour[0] = maze_color.r;
			colour[1] = maze_color.g;
			colour[2] = maze_color.b;
			
		}
		ImGui::SameLine();
		if( ImGui::RadioButton( "Floor Color", &current_col, 1 ) ) {
			// Select this colour.
			auto floor_color = floor->getColor();
			colour[0] = floor_color.r;
			colour[1] = floor_color.g;
			colour[2] = floor_color.b;

		}
		ImGui::SameLine();
		if( ImGui::RadioButton( "Avatar Color", &current_col, 2 ) ) {
			// Select this colour.
			// current_col = 2;
			auto avatar_color = avatar->getColor();
			colour[0] = avatar_color.r;
			colour[1] = avatar_color.g;
			colour[2] = avatar_color.b;
		}

		ImGui::PopID();

		switch(current_col)
		{
			case 0: 
				maze_mesh->setWallColor(glm::vec3(colour[0], colour[1], colour[2]));
				break;
			case 1:
				floor->setColor(glm::vec3(colour[0], colour[1], colour[2]));
				break;
			case 2:
				avatar->setColor(glm::vec3(colour[0], colour[1], colour[2]));
				break;
		}

/*
		// For convenience, you can uncomment this to show ImGui's massive
		// demonstration window right in your application.  Very handy for
		// browsing around to get the widget you want.  Then look in 
		// shared/imgui/imgui_demo.cpp to see how it's done.
		if( ImGui::Button( "Test Window" ) ) {
			showTestWindow = !showTestWindow;
		}
*/

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();

	if( showTestWindow ) {
		ImGui::ShowTestWindow( &showTestWindow );
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{

	// Create a global transformation for the model (centre it).
	mat4 W, W_trans;
	// W = glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );
	W = glm::rotate(maze_mesh->maze_rotation, glm::vec3(0.0f, 1.0f, 0.0f)) * 
		// glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) ) *
		glm::scale(mat4(1.0f), vec3(scale, scale, scale));;

	W_trans = 
			  glm::rotate(maze_mesh->maze_rotation, glm::vec3(0.0f, 1.0f, 0.0f)) * 
			  glm::scale(mat4(1.0f), vec3(scale, scale, scale)) *
			  glm::translate( W_trans, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );;
			  // TODO Doing translate first is hacky but needed since dont have seperate model transform, find better way

	maze_mesh->draw(m_shader, W);
	floor->rotation = glm::quat(glm::vec3(PI/2.0f, 0.0f, 0.0f));
	floor->draw(m_shader, W);

	avatar->draw(m_shader, W_trans);

	m_shader.enable();
		glEnable( GL_DEPTH_TEST );

		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W_trans ) );

		// // Just draw the grid for now.
		glBindVertexArray( m_grid_vao );
		glUniform3f( col_uni, 1, 1, 1 );
		glDrawArrays( GL_LINES, 0, (3+DIM)*4 );

		// Draw the cubes
		// Highlight the active square.
	m_shader.disable();

	// Restore defaults
	glBindVertexArray( 0 );

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent (
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
bool A1::mouseMoveEvent(double xPos, double yPos) 
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// Put some code here to handle rotations.  Probably need to
		// check whether we're *dragging*, not just moving the mouse.
		// Probably need some instance variables to track the current
		// rotation amount, and maybe the previous X position (so 
		// that you can rotate relative to the *change* in X.

		if(dragging){
			if(dragging == 2)
			{
				dragging = 1;
				last_mouse_pos = glm::vec2(xPos, yPos);
			}
			else if (dragging == 1)
			{
				// cout << last_mouse_pos.x << endl;
				float d_x = xPos - last_mouse_pos.x;
				//Factor out maze rotation rate
				maze_mesh->maze_rotation += 0.006*d_x;
				last_mouse_pos = glm::vec2(xPos, yPos);
				last_dx = d_x;
				
			}
			

		}
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// The user clicked in the window.  If it's the left
		// mouse button, initiate a rotation.

		if (button == GLFW_MOUSE_BUTTON_LEFT){
			// Respond to some key events.
			if(actions == GLFW_PRESS){ dragging = 2; }
			else if(actions == GLFW_RELEASE){ dragging = 0; }
		}

		// cout << dragging << endl;
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet) {
	bool eventHandled(false);

	// Zoom in or out.

	scale *= 1.0f - yOffSet * 0.1f;

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if( action == GLFW_PRESS ) {
		// Respond to some key events.
		if(key == GLFW_KEY_LEFT) move_commands[0] = true;
		if(key == GLFW_KEY_DOWN) move_commands[1] = true;
		if(key == GLFW_KEY_RIGHT) move_commands[2] = true;
		if(key == GLFW_KEY_UP) move_commands[3] = true;
		if(key == GLFW_KEY_Q) glfwSetWindowShouldClose(m_window, GL_TRUE);
		if(key == GLFW_KEY_R) reset_maze();
		if(key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT)  wall_break = true;
	}
	if (action == GLFW_RELEASE)
	{
		if(key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT)  wall_break = false;
	}

	return eventHandled;
}

void A1::reset_maze()
{
	delete maze;
	maze = new Maze(*original_maze);
	maze_mesh->setMaze(*maze);
	maze = new Maze(*original_maze);

	avatar_pos = vec2(-1,-1);
}

void A1::dig()
{
	// DELETE FROM HERE...
	maze = new Maze(DIM);
	maze->digMaze();
	maze->printMaze();
	// ...TO HERE

	original_maze = new Maze(*maze);

	maze_mesh = new MazeMesh(*maze);
	maze_mesh->setWallColor(glm::vec3(0,0,1));

	avatar = new Sphere(0.5f, 16, 16);
	avatar->applyTranslation(glm::vec3(0.5f, 0, 0.5f));
	avatar->setTranslation(vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ));

	floor = new Square(16);
	// floor->rotation *= glm::quat(glm::vec3(PI/2, PI, 0));
	floor->setColor(glm::vec3(1,1,1));
	// floor->applyTranslation(glm::vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );
}
