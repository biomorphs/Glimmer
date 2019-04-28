#include "traceboi.h"
#include <iostream>
#include <stdint.h>
#include <atomic>

struct RenderParams
{
	glm::vec2 m_imageDimensions;
	glm::mat4 m_cameraToWorld;
	float m_fov;
	float m_scale;
	float m_aspectRatio;
	int m_maxRecursions;
};

float GetAspectRatio(const RenderParams& globals)
{
	return globals.m_imageDimensions.x > globals.m_imageDimensions.y ?
		globals.m_imageDimensions.x / globals.m_imageDimensions.y :
		globals.m_imageDimensions.y / globals.m_imageDimensions.x;
}

glm::vec3 GeneratePrimaryRayDirection(const RenderParams& globals, glm::vec2 pixelPos)
{
	float x = (2.0f * (pixelPos.x + 0.5f) / (float)globals.m_imageDimensions.x - 1.0f) * globals.m_aspectRatio * globals.m_scale;
	float y = (1.0f - 2.0f * (pixelPos.y + 0.5f) / (float)globals.m_imageDimensions.y) * globals.m_scale;
	glm::vec3 direction = (glm::mat3x3)globals.m_cameraToWorld * glm::vec3(x, y, -1);
	direction = glm::normalize(direction);
	return direction;
}

bool RayHitObject(const Geometry::Ray& ray, const TraceParamaters& globals, float& t, glm::vec3& hitNormal, Material& hitMaterial)
{
	float closestT = std::numeric_limits<float>::max();
	Material closestMaterial;
	glm::vec3 closestNormal(0.0f);
	glm::vec3 normal;
	bool hit = false;

	for (auto s : globals.scene.spheres)
	{
		if (Geometry::RaySphereIntersect(ray, s.m_sphere, t, normal))
		{
			if (t < closestT)
			{
				closestNormal = normal;
				closestT = t;
				closestMaterial = s.m_material;
				hit = true;
			}
		}
	}

	for (auto p : globals.scene.planes)
	{
		if (Geometry::RayPlaneIntersect(ray, p.m_plane, t, normal))
		{
			if (t < closestT)
			{
				closestNormal = normal;
				closestT = t;
				closestMaterial = p.m_material;
				hit = true;
			}
		}
	}	

	for (auto m : globals.scene.meshes)
	{
		for (auto tri : m.m_triangles)
		{
			if (Geometry::RayTriangleIntersect(ray, tri, t, normal))
			{
				if (t < closestT)
				{
					closestNormal = normal;
					closestT = t;
					closestMaterial = m.m_material;
					hit = true;
				}
			}
		}
	}

	if (hit)
	{
		t = closestT;
		hitNormal = closestNormal;
		hitMaterial = closestMaterial;
	}
	return hit;
}

float fresnel(const glm::vec3 direction, const glm::vec3 hitnormal, const float refractiveIndex)
{
	float result = 0.0f;
	float cosi = glm::clamp(glm::dot(direction, hitnormal), -1.0f, 1.0f);
	float etai = 1, etat = refractiveIndex;
	if (cosi > 0) { std::swap(etai, etat); }
	// Compute sini using Snell's law
	float sint = etai / etat * sqrtf(glm::max(0.f, 1.0f - cosi * cosi));
	// Total internal reflection
	if (sint >= 1) {
		result = 1;
	}
	else {
		float cost = sqrtf(glm::max(0.f, 1 - sint * sint));
		cosi = fabsf(cosi);
		float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
		float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
		result = (Rs * Rs + Rp * Rp) / 2;
	}
	// As a consequence of the conservation of energy, transmittance is given by:
	// kt = 1 - kr;
	return result;
}

std::atomic<int> m_raycastCount = 0;
glm::vec3 CastRay(const Geometry::Ray& ray, const TraceParamaters& globals, int depth)
{
	if (depth >= globals.maxRecursions)
	{
		return globals.scene.skyColour;
	}

	++m_raycastCount;

	float hitT = 0.0f;
	glm::vec3 hitNormal(0.0f);
	Material hitMaterial;
	bool rayHitObject = RayHitObject(ray, globals, hitT, hitNormal, hitMaterial);
	glm::vec3 outColour(0.0f);
	if (rayHitObject)
	{
		auto hitPosition = ray.m_origin + ray.m_direction * hitT;
		if (hitMaterial.m_type == Diffuse)
		{
			glm::vec3 diffuse(0.0f);
			glm::vec3 specular(0.0f);
			for (auto l : globals.scene.lights)
			{
				auto pointToLight = glm::normalize(l.m_position - hitPosition);
				auto nDotL = glm::dot(hitNormal, pointToLight);

				// shadow
				float shadowBias = 0.000f;	// to avoid hitting the same object
				Geometry::Ray pointToLightRay = { hitPosition + hitNormal * shadowBias, pointToLight };
				float shadowT = 0.0f;
				glm::vec3 shadowNormal(0.0f);
				Material shadowMaterial;
				bool inShadow = RayHitObject(pointToLightRay, globals, shadowT, shadowNormal, shadowMaterial);
				glm::vec3 shadow(inShadow ? 0.0f : 1.0f);
				diffuse += (nDotL * l.m_diffuse) * shadow;

				//specular
				glm::vec3 idealReflection = glm::reflect(pointToLight, hitNormal);
				float specularPow = glm::pow(glm::max(0.0f, glm::dot(idealReflection, ray.m_direction)), 10.0f);
				specular += /*shadow **/ l.m_diffuse * specularPow;
			}
			outColour = diffuse + specular;
		}
		else if (hitMaterial.m_type == ReflectRefract)
		{
			static float bias = 0.001f;
			float frenelFactor = fresnel(ray.m_direction, hitNormal, hitMaterial.m_refractiveIndex);
			float dirDotNormal = glm::dot(ray.m_direction, hitNormal);
			bool outside = dirDotNormal < 0;
			glm::vec3 biasVec = bias * hitNormal;

			auto reflectionDir = glm::reflect(ray.m_direction, hitNormal);
			auto reflectionOrigin = outside ? hitPosition + biasVec : hitPosition - biasVec;
			Geometry::Ray reflection = { reflectionOrigin, reflectionDir };
			glm::vec3 reflectionColour = CastRay(reflection, globals, depth + 1);

			glm::vec3 refractionColor(0.0f);
			if (frenelFactor < 1.0f)	// Not total internal reflection
			{
				glm::vec3 refractionDir = (refract(ray.m_direction, hitNormal, hitMaterial.m_refractiveIndex));
				glm::vec3 refractionOrig = outside ? hitPosition - biasVec : hitPosition + biasVec;
				refractionColor = CastRay({ refractionOrig, refractionDir }, globals, depth + 1);
			}
			
			glm::vec3 mixed = (reflectionColour * frenelFactor) + (refractionColor * (1.0f - frenelFactor));

			glm::vec3 specular(0.0f);
			for (auto l : globals.scene.lights)
			{
				auto pointToLight = glm::normalize(l.m_position - hitPosition);
				auto nDotL = glm::dot(hitNormal, pointToLight);

				// shadow
				Geometry::Ray pointToLightRay = { hitPosition, pointToLight };
				float shadowT = 0.0f;
				glm::vec3 shadowNormal(0.0f);
				Material shadowMaterial;
				bool inShadow = RayHitObject(pointToLightRay, globals, shadowT, shadowNormal, shadowMaterial);
				glm::vec3 shadow(inShadow ? 0.0f : 1.0f);

				glm::vec3 idealReflection = glm::reflect(pointToLight, hitNormal);
				float specularPow = glm::pow(glm::max(0.0f, glm::dot(idealReflection, ray.m_direction)), 10.0f);
				specular += /*shadow **/ l.m_diffuse * specularPow;
			}
			outColour = mixed + specular;
		}
	}
	else
	{
		outColour = globals.scene.skyColour;
	}

	return glm::clamp(outColour, 0.0f,1.0f);
}

void WritePixel(uint32_t* buffer, glm::ivec2 dims, glm::ivec2 pos, glm::vec3 colour)
{
	glm::ivec3 quantised = (glm::ivec3)(colour * 255.0f);
	size_t pixelIndex = (pos.y * dims.x) + pos.x;
	buffer[pixelIndex] = quantised.r | quantised.g << 8 | quantised.b << 16 | 0xff000000;
}

void TraceMeSomethingNice(const TraceParamaters& parameters)
{
	const glm::ivec2 imageMin = parameters.outputOrigin;
	const glm::ivec2 imageMax = parameters.outputOrigin + parameters.outputDimensions;

	RenderParams globals;
	globals.m_fov = parameters.camera.FOV();
	globals.m_scale = tan(glm::radians(globals.m_fov * 0.5f));
	globals.m_imageDimensions = parameters.imageDimensions;
	globals.m_aspectRatio = GetAspectRatio(globals);
	globals.m_cameraToWorld = glm::inverse(parameters.camera.ViewMatrix());
	glm::vec3 origin = (glm::vec3)(globals.m_cameraToWorld * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	globals.m_maxRecursions = parameters.maxRecursions;

	Geometry::Ray primaryRay;
	primaryRay.m_origin = origin;
	for (int y = imageMin.y; y < imageMax.y; ++y)
	{
		for (int x = imageMin.x; x < imageMax.x; ++x)
		{
			primaryRay.m_direction = GeneratePrimaryRayDirection(globals, { (float)x, (float)y });
			glm::vec3 outColour = CastRay(primaryRay, parameters, 0);
			WritePixel(parameters.outputBuffer.data(), parameters.imageDimensions, { x, y }, outColour);
		}
	}
}