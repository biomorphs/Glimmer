#pragma once
#include <vector>
#include "math/glm_headers.h"

struct Sphere
{
	glm::vec4 m_posAndRadius;
};

struct TraceParamaters
{
	std::vector<uint8_t>& outputBuffer;
	std::vector<Sphere> spheres;
	uint32_t width;
	uint32_t height;
};

void TraceMeSomethingNice(const TraceParamaters& parameters);