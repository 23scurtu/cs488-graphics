// Winter 2019

#pragma once

#include "SceneNode.hpp"
#include <glm/glm.hpp>

struct NotImplementedError {};

struct JointTransform
{
	glm::mat4 trans;
	glm::mat4 invtrans;
	float x_rot;
	float y_rot;
};

class JointNode : public SceneNode {
public:
	JointNode();
	JointNode(const std::string & name);
	virtual ~JointNode();

	void set_joint_x(double min, double init, double max);
	void set_joint_y(double min, double init, double max);

	struct JointRange {
		double min, init, max;
	};

	float x_rot = 0;
	float y_rot = 0;

	void rotateLocalClamped(glm::vec3 axis, float angle);

	// Rotates locally
	void rotate(char axis, float angle) override;
	void clampedRotate(char axis, float angle);
	void rotate(glm::vec3 axis, float angle) override;
	void rotateLocal(glm::vec3 axis, float angle) override;

	void setTransformPack(const JointTransform &t);
	JointTransform getTransformPack();

	// JointNode shallowCopy();

	JointRange m_joint_x, m_joint_y;
};
