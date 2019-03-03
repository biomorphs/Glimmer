#pragma once
#include "math/glm_headers.h"

struct Ray
{
	glm::vec3 m_origin;
	glm::vec3 m_direction;
};

struct Sphere
{
	glm::vec4 m_posAndRadius;
};

struct Box
{
	glm::vec3 m_min;
	glm::vec3 m_max;
};

namespace Geometry
{
	bool RaySphereIntersect(const Ray& ray, const Sphere& sphere, float &t, glm::vec3& normal);
	bool RayBoxIntersect(const Ray& ray, const Box& box, float& t, glm::vec3& normal);
}
