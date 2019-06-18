// Winter 2019

#include "JointNode.hpp"
#include <glm/gtx/transform.hpp>
#include "cs488-framework/MathUtils.hpp"
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string& name)
	: SceneNode(name)
{
	m_nodeType = NodeType::JointNode;
}

//---------------------------------------------------------------------------------------
JointNode::~JointNode() {

}
 //---------------------------------------------------------------------------------------
void JointNode::set_joint_x(double min, double init, double max) {
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;
}

//---------------------------------------------------------------------------------------
void JointNode::set_joint_y(double min, double init, double max) {
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;
}

// void JointNode::rotateLocalClamped(vec3 axis, float angle) {
// 	mat4 rot_matrix = glm::rotate(degreesToRadians(angle), axis);
// 	mat4 invrot_matrix = glm::rotate(degreesToRadians(-angle), axis);

// 	trans = trans * rot_matrix;
// 	invtrans = invrot_matrix * invtrans ;
// }

void JointNode::rotate(char axis, float angle) {
	vec3 rot_axis;
	float rot = 0.0f;

	switch (axis) {
		// TODO Store rotation matrix seperatly and overwrite gettransform?
		case 'x':
			{
			rot_axis = vec3(1,0,0);
			auto old_x_rot = x_rot;
			x_rot += degreesToRadians(angle);
			x_rot = clamp(x_rot,
						  degreesToRadians(float(m_joint_x.min)), 
						  degreesToRadians(float(m_joint_x.max)));
			rot = x_rot - old_x_rot;
			break;
			}
		case 'y':
			{
			rot_axis = vec3(0,1,0);
			auto old_y_rot = y_rot;
			y_rot += degreesToRadians(angle);
			y_rot = clamp(y_rot, 
						  degreesToRadians(float(m_joint_y.min)), 
						  degreesToRadians(float(m_joint_y.max)));
			rot = y_rot - old_y_rot;
	        break;
			}
		case 'z':
			rot_axis = vec3(0,0,1);
			rot = degreesToRadians(angle);
	        break;
		default:
			break;
	}
	mat4 rot_matrix = glm::rotate(rot, rot_axis);
	mat4 invrot_matrix = glm::rotate(-rot, rot_axis);

	trans =  trans * rot_matrix;
	invtrans = invrot_matrix * invtrans ;
}

void JointNode::rotate(vec3 axis, float angle) {
	// mat4 rot_matrix = glm::rotate(degreesToRadians(angle), axis);
	// mat4 invrot_matrix = glm::rotate(degreesToRadians(-angle), axis);

	// trans = rot_matrix * trans;
	// invtrans = invtrans * invrot_matrix;

	throw NotImplementedError();
}

void JointNode::rotateLocal(vec3 axis, float angle) {
	// mat4 rot_matrix = glm::rotate(degreesToRadians(angle), axis);
	// mat4 invrot_matrix = glm::rotate(degreesToRadians(-angle), axis);

	// trans = trans * rot_matrix;
	// invtrans = invrot_matrix * invtrans ;

	throw NotImplementedError();
}
