#include "geometry.h"
#include <algorithm>

namespace Geometry
{
	// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
	bool RayTriangleIntersect(const Ray& ray, const Triangle& tri, float& t, glm::vec3& normal)
	{
		const float EPSILON = 0.0000001f;
		auto vertex0 = tri.m_v0;
		auto vertex1 = tri.m_v1;
		auto vertex2 = tri.m_v2;
		glm::vec3 edge1, edge2, h, s, q;
		float a, f, u, v;
		edge1 = vertex1 - vertex0;
		edge2 = vertex2 - vertex0;
		h = glm::cross(ray.m_direction,edge2);
		a = glm::dot(edge1,h);
		if (a > -EPSILON && a < EPSILON)
		{
			return false;    // This ray is parallel to this triangle.
		}
		f = 1.0f / a;
		s = ray.m_origin - vertex0;
		u = f * glm::dot(s,h);
		if (u < 0.0f || u > 1.0f)
		{
			return false;
		}
		q = glm::cross(s,edge1);
		v = f * glm::dot(ray.m_direction,q);
		if (v < 0.0f || u + v > 1.0f)
		{
			return false;
		}
		// At this stage we can compute t to find out where the intersection point is on the line.
		float tFinal = f * glm::dot(edge2,q);
		if (tFinal > EPSILON) // ray intersection
		{
			t = tFinal + 0.001f;
			normal = glm::normalize(glm::cross(edge1,edge2));
			return true;
		}
		else // This means that there is a line intersection but not a ray intersection.
		{
			return false;
		}
	}

	bool RayPlaneIntersect(const Ray& ray, const Plane& plane, float& tOut, glm::vec3& normal)
	{
		float denom = glm::dot(plane.m_normal, ray.m_direction);

		// avoid divide by 0
		if (abs(denom) > 0.0001f)
		{
			float t = glm::dot(plane.m_point - ray.m_origin, plane.m_normal) / denom;
			if (t >= 0.001f && t < 10000.0f)
			{
				tOut = t;
				normal = plane.m_normal;
				return true;
			}
		}

		return false;
	}

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
}