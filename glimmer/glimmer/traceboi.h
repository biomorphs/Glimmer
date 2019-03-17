#pragma once
#include <vector>
#include "render/camera.h"
#include "math/glm_headers.h"
#include "geometry.h"

struct Light
{
	glm::vec3 m_position;
	glm::vec3 m_diffuse;
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

struct Plane
{
	Geometry::Plane m_plane;
	Material m_material;
};

struct Scene
{
	std::vector<Sphere> spheres;
	std::vector<Plane> planes;
	std::vector<Light> lights;
	glm::vec3 skyColour;
};

struct ImageParameters
{
	glm::ivec2 m_dimensions;
};

struct TraceParamaters
{
	std::vector<uint32_t>& outputBuffer;
	Scene scene;
	Render::Camera camera;
	glm::ivec2 imageDimensions;
	glm::ivec2 outputOrigin;
	glm::ivec2 outputDimensions;
	int maxRecursions;
	int raycastCount = 0;
};

void TraceMeSomethingNice(const TraceParamaters& parameters);