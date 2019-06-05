// Winter 2019

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

#include <vector>

// Set a global maximum number of vertices in order to pre-allocate VBO data
// in one shot, rather than reallocating each frame.
const GLsizei kMaxVertices = 1000;


// Convenience class for storing vertex data in CPU memory.
// Data should be copied over to GPU memory via VBO storage before rendering.
class VertexData {
public:
	VertexData();

	std::vector<glm::vec2> positions;
	std::vector<glm::vec3> colours;
	GLuint index;
	GLsizei numVertices;
};


class A2 : public CS488Window {
public:
	A2();
	virtual ~A2();

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

	void createShaderProgram();
	void enableVertexAttribIndices();
	void generateVertexBuffers();
	void mapVboDataToVertexAttributeLocation();
	void uploadVertexDataToVbos();

	void initLineData();

	void setLineColour(const glm::vec3 & colour);

	void drawLine (
			const glm::vec2 & v0,
			const glm::vec2 & v1
	);

	void drawLine (
			const glm::vec4 & p0,
			const glm::vec4 & p1
	);

	std::vector<glm::vec4> clipLinePrimitive3d(glm::vec4 a, glm::vec4 b);
	std::vector<glm::vec4> clipLinePrimitives3d(std::vector<glm::vec4> data);
	void clipLinePrimitives3d(std::vector<glm::vec4> &data, std::vector<glm::vec3> &colors);

	void reset();

	ShaderProgram m_shader;

	GLuint m_vao;            // Vertex Array Object
	GLuint m_vbo_positions;  // Vertex Buffer Object
	GLuint m_vbo_colours;    // Vertex Buffer Object

	VertexData m_vertexData;

	glm::vec3 m_currentLineColour;

	std::vector<glm::vec3> cube;

	glm::vec3 cube_scale;
	glm::vec3 cube_rot;
	glm::vec3 cube_trans;
	glm::mat4 model_no_scale;

	float perspective_fov;
	float perspective_near;
	float perspective_far;

	glm::vec3 view_rot;
	glm::vec3 view_trans;
	glm::mat4 view;

	glm::vec2 viewport_tl;
	glm::vec2 viewport_br;

	glm::mat4 createScale(glm::vec3 s);
	glm::mat4 createTransform(glm::vec3 t);
	glm::mat4 createRotation(glm::vec3 r);
	glm::mat4 createPerspective(float n, float f, float fov, float aspect);

	struct bvec
	{
		bool x = false;
		bool y = false;
		bool z = false;
	};

	bvec pressed_buttons;
	float mouse_dx = 0.0f;
	int mouse_dragging = 0;
	bvec *current_options;
	glm::vec2 last_mouse_pos;
	glm::vec2 last_mouse_click_pos;

	bvec rotate_view;
	bvec translate_view;
	bvec perspective_change;

	bvec scale_model;
	bvec rotate_model;
	bvec translate_model;

	bool viewport_draw = false;

	int current_mode;
};
