#include "AABB.h"

glm::vec3 min_components(glm::vec3 a, glm::vec3 b)
{ return glm::vec3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z)); }

glm::vec3 max_components(glm::vec3 a, glm::vec3 b)
{ return glm::vec3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z)); }