#include "geometry.h"
#include <algorithm>

namespace Geometry
{
	bool RaySphereIntersect(const Ray& ray, const Sphere& sphere, float &t, glm::vec3& normal)
	{
		float radius2 = sphere.m_posAndRadius.w * sphere.m_posAndRadius.w;
		glm::vec3 l = glm::vec3(sphere.m_posAndRadius) - ray.m_origin;
		float tca = glm::dot(l, ray.m_direction);
		if (tca < 0)
		{
			return false;
		}
		float d2 = glm::dot(l, l) - tca * tca;
		if (d2 > radius2)
		{
			return false;
		}
		float thc = sqrt(radius2 - d2);
		float t0 = tca - thc;
		float t1 = tca + thc;
		if (t0 > t1)
		{
			std::swap(t0, t1);
		}
		if (t0 < 0)
		{
			t0 = t1;
			if (t0 < 0)
			{
				return false;
			}
		}
		t = t0;
		auto spherePos = glm::vec3(sphere.m_posAndRadius);
		auto hitPos = ray.m_origin + ray.m_direction * t;
		auto hitLength = glm::length(hitPos - spherePos);
		normal = glm::normalize(hitPos - spherePos);
		return true;
	}

	bool RayBoxIntersect(const Ray& ray, const Box& box, float& t, glm::vec3& normal)
	{
		float tmin = (box.m_min.x - ray.m_origin.x) / ray.m_direction.x;
		float tmax = (box.m_max.x - ray.m_origin.x) / ray.m_direction.x;

		if (tmin > tmax) std::swap(tmin, tmax);

		float tymin = (box.m_min.y - ray.m_origin.y) / ray.m_direction.y;
		float tymax = (box.m_max.y - ray.m_origin.y) / ray.m_direction.y;

		if (tymin > tymax) std::swap(tymin, tymax);

		if ((tmin > tymax) || (tymin > tmax))
			return false;

		if (tymin > tmin)
			tmin = tymin;

		if (tymax < tmax)
			tmax = tymax;

		float tzmin = (box.m_min.z - ray.m_origin.z) / ray.m_direction.z;
		float tzmax = (box.m_max.z - ray.m_origin.z) / ray.m_direction.z;

		if (tzmin > tzmax) std::swap(tzmin, tzmax);

		if ((tmin > tzmax) || (tzmin > tmax))
			return false;

		if (tzmin > tmin)
			tmin = tzmin;

		if (tzmax < tmax)
			tmax = tzmax;

		return true;
	}
}