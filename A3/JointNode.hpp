// Winter 2019

#pragma once

#include "SceneNode.hpp"
#include <glm/glm.hpp>

struct NotImplementedError {};

class JointNode : public SceneNode {
public:
	JointNode(const std::string & name);
	virtual ~JointNode();

	void set_joint_x(double min, double init, double max);
	void set_joint_y(double min, double init, double max);

	struct JointRange {
		double min, init, max;
	};

	float x_rot, y_rot;

	void rotateLocalClamped(glm::vec3 axis, float angle);

	// Rotates locally
	void rotate(char axis, float angle) override;
	void rotate(glm::vec3 axis, float angle) override;
	void rotateLocal(glm::vec3 axis, float angle) override;

	JointRange m_joint_x, m_joint_y;
};
