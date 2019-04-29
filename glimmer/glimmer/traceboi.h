#pragma once
#include <vector>
#include "render/camera.h"
#include "geometry.h"
#include <sol.hpp>

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

struct Mesh
{
	std::vector<Geometry::Triangle> m_triangles;
	Material m_material;
};

struct Scene
{
	std::vector<Sphere> spheres;
	std::vector<Plane> planes;
	std::vector<Mesh> meshes;
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

namespace TraceBoi
{
	void TraceMeSomethingNice(const TraceParamaters& parameters);

	// adds glimmer.scene.*(addSphere, addPlane, addLight, setSkyColour) to scripts
	// they will operate on the target scene
	template<class ScriptScope>
	static inline void RegisterScriptTypes(ScriptScope& globals, Scene& targetScene)
	{
		auto glimmer = globals["glimmer"].get_or_create<sol::table>();
		auto scene = glimmer["scene"].get_or_create<sol::table>();
		scene["addSphere"] = [&targetScene](float x, float y, float z, float radius, bool reflect)
		{
			Material m = reflect ? Material{ 0.001f, ReflectRefract } : Material{ 1.0f, Diffuse };
			targetScene.spheres.push_back(
				{ { { x, y, z, radius} }, m }
			);
		};
		scene["addPlane"] = [&targetScene](float nx, float ny, float nz, float px, float py, float pz, bool reflect)
		{
			Material m = reflect ? Material{ 0.001f, ReflectRefract } : Material{ 1.0f, Diffuse };
			targetScene.planes.push_back(
				{ { {nx,ny,nz}, {px,py,pz} }, m }
			);
		};
		scene["addLight"] = [&targetScene](float px, float py, float pz, float r, float g, float b)
		{
			targetScene.lights.push_back({ {px,py,pz},{r,g,b} });
		};
		scene["setSkyColour"] = [&targetScene](float r, float g, float b)
		{
			targetScene.skyColour = glm::vec3(r, g, b);
		};
	}
}
