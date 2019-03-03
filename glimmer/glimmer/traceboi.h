#pragma once
#include <vector>
#include "math/glm_headers.h"

struct Sphere
{
	glm::vec4 m_posAndRadius;
};

struct Plane
{
	glm::vec3 m_normal;
	glm::vec3 m_point;
};

struct TraceParamaters
{
	std::vector<uint8_t>& outputBuffer;
	std::vector<Sphere> spheres;
	std::vector<Plane> planes;
	uint32_t width;
	uint32_t height;
};

void TraceMeSomethingNice(const TraceParamaters& parameters);