#pragma once
#include <vector>
#include "math/glm_headers.h"
#include "geometry.h"

struct Light
{
	glm::vec3 m_position;
	glm::vec4 m_diffuse;
};

enum MaterialType
{
	ReflectRefract,
	Diffuse
};

struct SceneMaterial
{
	float m_refractiveIndex;
	MaterialType m_type;
};

struct SceneSphere
{
	Sphere m_sphere;
	SceneMaterial m_material;
};

struct TraceParamaters
{
	std::vector<uint8_t>& outputBuffer;
	std::vector<SceneSphere> spheres;
	std::vector<Light> lights;
	glm::vec4 skyColour;
	uint32_t width;
	uint32_t height;
	int maxRecursions;
};

void TraceMeSomethingNice(const TraceParamaters& parameters);