#include "traceboi.h"
#include <iostream>
#include <stdint.h>
#include "math/glm_headers.h"
#include "geometry.h"

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
	glm::vec3 direction = (glm::vec3)(globals.m_cameraToWorld * glm::vec4(x, y, -1, 0));
	direction = glm::normalize(direction);
	return direction;
}

bool RayHitObject(const Ray& ray, const TraceParamaters& globals, float& t, glm::vec3& hitNormal, SceneMaterial& hitMaterial)
{
	float closestT = std::numeric_limits<float>::max();
	SceneMaterial closestMaterial;
	glm::vec3 closestNormal(0.0f);
	glm::vec3 normal;
	bool hit = false;

	for (auto s : globals.spheres)
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

glm::vec4 CastRay(const Ray& ray, const TraceParamaters& globals, int depth)
{
	if (depth >= globals.maxRecursions)
	{
		return glm::vec4(0.0f);
	}

	float hitT = 0.0f;
	glm::vec3 hitNormal(0.0f);
	SceneMaterial hitMaterial;
	bool rayHitObject = RayHitObject(ray, globals, hitT, hitNormal, hitMaterial);
	if (rayHitObject)
	{
		auto hitPosition = ray.m_origin + ray.m_direction * hitT;
		if (hitMaterial.m_type == Diffuse)
		{
			glm::vec4 accumColour(0.0f);
			for (auto l : globals.lights)
			{
				auto pointToLight = glm::normalize(l.m_position - hitPosition);
				auto nDotL = glm::dot(hitNormal, pointToLight);

				// shadow
				Ray pointToLightRay = { hitPosition, pointToLight };
				float shadowT = 0.0f;
				glm::vec3 shadowNormal(0.0f);
				SceneMaterial shadowMaterial;
				bool inShadow = RayHitObject(pointToLightRay, globals, shadowT, shadowNormal, shadowMaterial);
				accumColour += (nDotL * l.m_diffuse) * (inShadow ? 0.3f : 1.0f);
			}
			return glm::clamp(accumColour, { 0.0f }, { 1.0f });
		}
		else if (hitMaterial.m_type == ReflectRefract)
		{
			static float bias = 0.001f;
			float frenelFactor = fresnel(ray.m_direction, hitNormal, hitMaterial.m_refractiveIndex);
			bool outside = glm::dot(ray.m_direction, hitNormal) < 0;
			glm::vec4 refractionColor(0.0f);
			glm::vec3 biasVec = bias * hitNormal;

			if (frenelFactor < 1.0f)	// Not total internal reflection
			{
				glm::vec3 refractionDir = glm::normalize(refract(ray.m_direction, hitNormal, hitMaterial.m_refractiveIndex));
				glm::vec3 refractionOrig = outside ? hitPosition - biasVec : hitPosition + biasVec;
				refractionColor = CastRay({ refractionOrig, refractionDir }, globals, depth + 1);
			}
			auto reflectionDir = glm::reflect(ray.m_direction, hitNormal);
			auto reflectionOrigin = outside ? hitPosition + biasVec : hitPosition - biasVec;

			Ray reflection = { reflectionOrigin, reflectionDir };
			glm::vec4 reflectionColour = CastRay(reflection, globals, depth + 1);
			return glm::clamp(reflectionColour * frenelFactor + refractionColor * (1.0f - frenelFactor), { 0.0f }, { 1.0f });
		}
	}

	return globals.skyColour;
}

void TraceMeSomethingNice(const TraceParamaters& parameters)
{
	const uint32_t width = parameters.width;
	const uint32_t height = parameters.height;
	uint8_t* outBuffer = parameters.outputBuffer.data();

	RenderParams globals;
	globals.m_fov = 51.52f;
	globals.m_scale = tan(glm::radians(globals.m_fov * 0.5f));
	globals.m_imageDimensions = { width, height };
	globals.m_aspectRatio = GetAspectRatio(globals);
	globals.m_cameraToWorld = glm::lookAt(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,0.0f,-1.0f), glm::vec3(0.0f,1.0f,0.0f));
	glm::vec3 origin = (glm::vec3)(globals.m_cameraToWorld * glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	globals.m_maxRecursions = parameters.maxRecursions;

	for (uint32_t y = 0; y < height; ++y)
	{
		for (uint32_t x = 0; x < width; ++x)
		{
			Ray primaryRay;
			primaryRay.m_origin = origin;
			primaryRay.m_direction = GeneratePrimaryRayDirection(globals, { (float)x, (float)y });
			glm::vec4 outColour = CastRay(primaryRay, parameters, 0);
			*(uint32_t*)outBuffer = (uint8_t)(outColour.r * 255.0f) |
				(uint8_t)(outColour.g * 255.0f) << 8 |
				(uint8_t)(outColour.b * 255.0f) << 16 |
				(uint8_t)(outColour.a * 255.0f) << 24;
			outBuffer+=4;
		}
	}
}