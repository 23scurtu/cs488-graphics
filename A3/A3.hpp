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
#include "JointNode.hpp"

#include <chrono>

class GeometryNode;

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

class Command
{
public:
  virtual ~Command() {}
  virtual void execute() = 0;
  virtual void undo() = 0;
};
class RotateCommand: Command
{
	JointNode *node;
  	char axis;
	float angle;
	glm::mat4 prev_trans;
	glm::mat4 prev_invtrans;
public:
	RotateCommand(JointNode *node, char axis, float angle)
		: node{node}, axis{axis}, angle{angle} {}

	virtual void execute()
	{
		prev_trans = node->trans;
		prev_invtrans = node->invtrans;

		node->rotate(axis, angle);
		// unit_->moveTo(x_, y_);
	}

	virtual void undo()
	{
		node->trans = prev_trans;
		node->invtrans = prev_invtrans;
	}
};

class JointChangeCommand: public Command
{
	JointNode *node;
	JointTransform n_trans;
	JointTransform p_trans;
public:
	JointChangeCommand(JointNode *node, JointTransform p_trans)
		: node{node}, n_trans{node->getTransformPack()}, p_trans{p_trans} {}

	virtual void execute()
	{
		node->setTransformPack(n_trans);//  new_node;
	}

	virtual void undo()
	{
		node->setTransformPack(p_trans);
	}
};

// class JointChangeCommand: public Command
// {
// 	JointNode *node;
// 	// JointNode new_node;
// 	// JointNode prev_node;

// 	glm::mat4 n_trans;
// 	glm::mat4 n_invtrans;
// 	float n_x_rot;
// 	float n_y_rot;

// 	glm::mat4 p_trans;
// 	glm::mat4 p_invtrans;
// 	float p_x_rot;
// 	float p_y_rot;

// public:
// 	JointChangeCommand(JointNode *node, JointNode &prev_node)
// 		: node{node}, 
// 		n_trans{node->trans}, 
// 		n_invtrans{node->invtrans}
// 		n_

// 	virtual void execute()
// 	{
// 		*node = new_node;//  new_node;
// 	}

// 	virtual void undo()
// 	{
// 		*node = prev_node;
// 	}
// };

struct CommandFrame
{
	std::vector<Command*> commands;
	
	CommandFrame() = default;
	~CommandFrame()
	{
		// for(auto command: commands) delete command;
	}
	//TODO Write copy constructor

	// Commands will get executed in order theyre added 
	void addCommand(Command* command)
	{ 
		commands.push_back(command); 
	}

	void execute()
	{
		for(auto command: commands) command->execute();
	}

	void undo()
	{
		for(auto command: commands) command->undo();
	}

	void deleteCommands()
	{
		for(auto command: commands) delete command;
	}

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
	SceneNode* getHeadNode(SceneNode *root);

	void undo(bool human_called = false);
	void redo(bool human_called = false);
	// void add_action();

	void resetPosition();
	void resetOrientation();
	void resetJoints();

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

	int current_mode = 0;
	bool do_picking = false;
	std::unordered_map<unsigned int, JointSelection> selected_geometry_nodes;

	int mouse_dragging = 0;
	float mouse_dy = 0;
	// glm::vec2 last_mouse_click_pos;
	glm::vec2 mouse_pos;
	glm::vec2 last_mouse_pos;

	glm::vec2 trackball_pos;
	float trackball_diam;

	std::vector<CommandFrame> command_stack;
	int stack_ptr = -1;
	// std::vector<JointNode> changed_joints;
	// std::vector<JointNode*> changed_nodes;
	std::unordered_map<JointNode*, JointTransform> changed_joints;

	bool draw_trackball = false;
	bool zbuffer = true;
	bool backface_culling = false;
	bool frontface_culling = false;

	glm::mat4 root_translation;
	glm::mat4 root_invtranslation;
	glm::mat4 root_rotation;
	glm::mat4 root_invrotation;
	glm::mat4 root_original;
	glm::mat4 root_invoriginal;

	JointNode *head_joint = nullptr;
	GeometryNode *head_mesh = nullptr;

	float warning_time = 0; //seconds
	float warning_duration = 1; //seconds
	std::chrono::system_clock::time_point last_time;

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
