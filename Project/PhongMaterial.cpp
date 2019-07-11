// Winter 2019

#include "PhongMaterial.hpp"

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess , double tc, double ir)
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
	, m_tc(tc)
	, m_ir(ir)
{}

PhongMaterial::~PhongMaterial()
{}
