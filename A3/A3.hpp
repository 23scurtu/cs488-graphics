// Winter 2019

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "SceneNode.hpp"

#include <glm/glm.hpp>
#include <memory>

#include <unordered_map>

class GeometryNode;
class JointNode;

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};

struct JointSelection
{
	GeometryNode* gnode = nullptr;
	JointNode* jnode = nullptr;

	JointSelection() = default;
	JointSelection(GeometryNode *gnode, JointNode *jnode): gnode{gnode}, jnode{jnode} {}
};


class A3 : public CS488Window {
public:
	A3(const std::string & luaSceneFile);
	virtual ~A3();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void processLuaSceneFile(const std::string & filename);
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void initViewMatrix();
	void initLightSources();

	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	void updateShaderUniforms(const ShaderProgram & shader, 
							  const GeometryNode & node, 
							  const glm::mat4 & viewMatrix, 
							  const glm::mat4 & modelMatrix);
	void renderSceneGraph(const SceneNode &node);
	void renderSceneNode(const SceneNode &node, glm::mat4 parent_transform);
	void renderArcCircle();

	SceneNode* getSceneNode(unsigned int id, SceneNode* root);
	bool selected(const GeometryNode* gnode);

	glm::mat4 m_perpsective;
	glm::mat4 m_view;

	LightSource m_light;

	struct bvec
	{
		bool x = false;
		bool y = false;
		bool z = false;
	};

	bvec pressed_buttons;

	int current_mode = 1;
	bool do_picking = false;
	std::unordered_map<unsigned int, JointSelection> selected_geometry_nodes;

	int mouse_dragging = 0;
	float mouse_dy = 0;
	// glm::vec2 last_mouse_click_pos;
	glm::vec2 mouse_pos;
	glm::vec2 last_mouse_pos;

	glm::vec2 trackball_pos;
	float trackball_diam;

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
	ShaderProgram m_shader;

	//-- GL resources for trackball circle geometry:
	GLuint m_vbo_arcCircle;
	GLuint m_vao_arcCircle;
	GLint m_arc_positionAttribLocation;
	ShaderProgram m_shader_arcCircle;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::string m_luaSceneFile;

	std::shared_ptr<SceneNode> m_rootNode;
};
