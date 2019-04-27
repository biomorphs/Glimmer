#pragma once
#include "math/glm_headers.h"

namespace Geometry
{
	struct Ray
	{
		glm::vec3 m_origin;
		glm::vec3 m_direction;
	};

	struct Sphere
	{
		glm::vec4 m_posAndRadius;
	};

	struct Plane
	{
		glm::vec3 m_normal;
		glm::vec3 m_point;
	};

	struct Triangle
	{
		glm::vec3 m_v0;
		glm::vec3 m_v1;
		glm::vec3 m_v2;
	};

	bool RayPlaneIntersect(const Ray& ray, const Plane& plane, float& t, glm::vec3& normal);
	bool RaySphereIntersect(const Ray& ray, const Sphere& sphere, float &t, glm::vec3& normal);
	bool RayTriangleIntersect(const Ray& ray, const Triangle& tri, float& t, glm::vec3& normal);
}
