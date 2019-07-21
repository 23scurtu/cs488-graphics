// Winter 2019

#include "SceneNode.hpp"

#include "cs488-framework/MathUtils.hpp"

#include <iostream>
#include <sstream>
using namespace std;

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;


// Static class variable
unsigned int SceneNode::nodeInstanceCount = 0;


//---------------------------------------------------------------------------------------
SceneNode::SceneNode(const std::string& name)
  : m_name(name),
	m_nodeType(NodeType::SceneNode),
	trans(mat4(1.0f)),
	invtrans(mat4(1.0f)),
	m_nodeId(nodeInstanceCount++)
{

}

//---------------------------------------------------------------------------------------
// Deep copy
SceneNode::SceneNode(const SceneNode & other)
	: m_nodeType(other.m_nodeType),
	  m_name(other.m_name),
	  trans(other.trans),
	  invtrans(other.invtrans)
{
	for(SceneNode * child : other.children) {
		SceneNode* new_child = new SceneNode(*child);
		new_child->parent = this;
		this->children.push_front(new_child);
	}
}

//---------------------------------------------------------------------------------------
SceneNode::~SceneNode() {
	for(SceneNode * child : children) {
		delete child;
	}
}

//---------------------------------------------------------------------------------------
void SceneNode::set_transform(const glm::mat4& m) {
	trans = m;
	invtrans = glm::inverse(m);
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_transform() const {
	return trans;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_inverse() const {
	return invtrans;
}

//---------------------------------------------------------------------------------------
void SceneNode::add_child(SceneNode* child) {
	child->parent = this;
	children.push_back(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::remove_child(SceneNode* child) {
	child->parent = nullptr;
	children.remove(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::rotate(char axis, float angle) {
	vec3 rot_axis;

	switch (axis) {
		case 'x':
			rot_axis = vec3(1,0,0);
			break;
		case 'y':
			rot_axis = vec3(0,1,0);
	        break;
		case 'z':
			rot_axis = vec3(0,0,1);
	        break;
		default:
			break;
	}
	mat4 rot_matrix = glm::rotate(degreesToRadians(angle), rot_axis);
	mat4 invrot_matrix = glm::rotate(degreesToRadians(-angle), rot_axis);
	// set_transform( rot_matrix * trans );

	trans = rot_matrix * trans;
	invtrans = invtrans * invrot_matrix;
	// invtrans = inverse(trans);

	reset_normal_transform();
}

void SceneNode::reset_normal_transform()
{
	normal_trans = mat3{trans[0][0], trans[1][0], trans[2][0], //inv transpose
						trans[0][1], trans[1][1], trans[2][1],
						trans[0][2], trans[1][2], trans[2][2] };

	normal_invtrans = mat3{invtrans[0][0], invtrans[1][0], invtrans[2][0], //inv transpose
						   invtrans[0][1], invtrans[1][1], invtrans[2][1],
						   invtrans[0][2], invtrans[1][2], invtrans[2][2] };
}

//---------------------------------------------------------------------------------------
void SceneNode::scale(const glm::vec3 & amount) {
	// set_transform( glm::scale(amount) * trans );
	trans = glm::scale(amount) * trans;
	invtrans = invtrans * glm::scale(vec3(1.0f/amount.x, 1.0f/amount.y, 1.0f/amount.z));
	// invtrans = inverse(trans);
	reset_normal_transform();
}

//---------------------------------------------------------------------------------------
void SceneNode::translate(const glm::vec3& amount) {
	// set_transform( glm::translate(amount) * trans );
	trans = glm::translate(amount) * trans;
	invtrans = invtrans * glm::translate(-amount);
	// invtrans = inverse(trans);
	reset_normal_transform();
}

void SceneNode::calc_world_transforms()
{
	if(!parent) world_trans = trans;
	else world_trans = parent->world_trans*trans;
	
	for(SceneNode * child : children) {
		child->calc_world_transforms();
	}
}


//---------------------------------------------------------------------------------------
int SceneNode::totalSceneNodes() const {
	return nodeInstanceCount;
}

//---------------------------------------------------------------------------------------
std::ostream & operator << (std::ostream & os, const SceneNode & node) {

	//os << "SceneNode:[NodeType: ___, name: ____, id: ____, isSelected: ____, transform: ____"
	switch (node.m_nodeType) {
		case NodeType::SceneNode:
			os << "SceneNode";
			break;
		case NodeType::GeometryNode:
			os << "GeometryNode";
			break;
		case NodeType::JointNode:
			os << "JointNode";
			break;
	}
	os << ":[";

	os << "name:" << node.m_name << ", ";
	os << "id:" << node.m_nodeId;

	os << "]\n";
	return os;
}
