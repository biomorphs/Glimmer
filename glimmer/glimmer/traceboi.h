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

struct Material
{
	float m_refractiveIndex;
	MaterialType m_type;
};

struct Sphere
{
	Geometry::Sphere m_sphere;
	Material m_material;
};

struct Scene
{
	std::vector<Sphere> spheres;
	std::vector<Light> lights;
	glm::vec4 skyColour;
};

struct ImageParameters
{
	glm::ivec2 m_dimensions;
};

struct TraceParamaters
{
	std::vector<uint32_t>& outputBuffer;
	Scene scene;
	glm::ivec2 imageDimensions;
	glm::ivec2 outputOrigin;
	glm::ivec2 outputDimensions;
	int maxRecursions;
};

void TraceMeSomethingNice(const TraceParamaters& parameters);