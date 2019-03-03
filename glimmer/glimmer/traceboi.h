#pragma once
#include <vector>
#include "math/glm_headers.h"
#include "geometry.h"

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
	glm::vec4 skyColour;
	uint32_t width;
	uint32_t height;
	int maxRecursions;
};

void TraceMeSomethingNice(const TraceParamaters& parameters);