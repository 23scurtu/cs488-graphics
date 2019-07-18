// Winter 2019

#include "PhongMaterial.hpp"

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess , double tc, double ir, double r_g, double t_g)
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
	, m_tc(tc)
	, m_ir(ir)
	, m_r_g(r_g)
	, m_t_g(t_g)
{}

PhongMaterial::~PhongMaterial()
{}
