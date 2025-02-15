// Winter 2019

#pragma once

#include <glm/glm.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "maze.hpp"
#include "maze_mesh.hpp"

class A1 : public CS488Window {
public:
	A1();
	virtual ~A1();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	void reset_maze();
	void dig();

private:
	void initGrid();

	// Fields related to the shader and uniforms.
	ShaderProgram m_shader;
	GLint P_uni; // Uniform location for Projection matrix.
	GLint V_uni; // Uniform location for View matrix.
	GLint M_uni; // Uniform location for Model matrix.
	GLint col_uni;   // Uniform location for cube colour.

	// Fields related to grid geometry.
	GLuint m_grid_vao; // Vertex Array Object
	GLuint m_grid_vbo; // Vertex Buffer Object

	// Matrices controlling the camera and projection.
	glm::mat4 proj;
	glm::mat4 view;

	float colour[3];
	int current_col;

	Maze* maze = nullptr;
	Maze* original_maze = nullptr;

	MazeMesh* maze_mesh = nullptr;
	Square* floor = nullptr;
	Sphere* avatar = nullptr;
	float avatar_size = 1.0f;

	int dragging = 0;
	glm::vec2 last_mouse_pos;
	float last_dx = 0.0f;
	bool mouse_moving = false;

	glm::vec2 avatar_pos = glm::vec2(-1,-1);
	bool move_commands[4]{false, false, false, false};
	bool wall_break = false;

	bool maze_generated = false;
	float maze_rotation = 0.0f;
	float scale = 1.0f;

	vec3 maze_color = vec3(0,0,0.75);//vec3(0.9,0.5,0);
	vec3 avatar_color = vec3(1,0,0);
	vec3 floor_color = vec3(0.9,0.9,0.9);
};
