#pragma once
#include <vector>
#include "math/glm_headers.h"

struct Sphere
{
	glm::vec4 m_posAndRadius;
};

struct Light
{
	glm::vec3 m_position;
	glm::vec4 m_diffuse;
};

struct TraceParamaters
{
	std::vector<uint8_t>& outputBuffer;
	std::vector<Sphere> spheres;
	std::vector<Light> lights;
	uint32_t width;
	uint32_t height;
};

void TraceMeSomethingNice(const TraceParamaters& parameters);