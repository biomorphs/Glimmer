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
	//direction = glm::fastNormalize(direction); 
	direction = glm::normalize(direction);
	return direction;
}

bool RayHitObject(const Ray& ray, const TraceParamaters& globals, float& t, glm::vec3& hitNormal)
{
	float closestT = std::numeric_limits<float>::max();
	glm::vec3 closestNormal(0.0f);
	glm::vec3 normal;
	bool hit = false;

	for (auto s : globals.spheres)
	{
		if (Geometry::RaySphereIntersect(ray, s, t, normal))
		{
			if (t < closestT)
			{
				closestNormal = normal;
				closestT = t;
				hit = true;
			}
		}
	}

	if (hit)
	{
		t = closestT;
		hitNormal = closestNormal;
	}
	return hit;
}

glm::vec4 CastRay(const Ray& ray, const TraceParamaters& globals, int depth)
{
	float hitT = 0.0f;
	glm::vec3 hitNormal(0.0f);
	bool rayHitObject = RayHitObject(ray, globals, hitT, hitNormal);

	if (rayHitObject)
	{
		glm::vec4 accumColour( 0.0f );
		auto hitPosition = ray.m_origin + ray.m_direction * hitT;
		for (auto l : globals.lights)
		{
			auto pointToLight = glm::normalize(l.m_position - hitPosition);
			auto nDotL = glm::dot(hitNormal, pointToLight);

			// shadow
			Ray pointToLightRay = { hitPosition, pointToLight };
			float shadowT = 0.0f;
			glm::vec3 shadowNormal(0.0f);
			bool inShadow = RayHitObject(pointToLightRay, globals, shadowT, shadowNormal);
			glm::vec4 shadow = glm::vec4(1.0f) * (inShadow ? 0.5f : 1.0f);
			accumColour += glm::clamp(nDotL * l.m_diffuse * shadow, { 0.0f }, { 1.0f });
		}
		
		if (depth < globals.maxRecursions)
		{
			Ray reflection = { hitPosition, glm::reflect(ray.m_direction, hitNormal) };
			glm::vec4 reflectionColour = CastRay(reflection, globals, depth + 1);
			float reflectionFactor = glm::dot(reflection.m_direction, hitNormal) * 0.9f;
			return glm::clamp(accumColour + reflectionColour * reflectionFactor, { 0.0f }, { 1.0f });
		}
		else
		{
			return glm::clamp(accumColour, { 0.0f }, { 1.0f });
		}
	}

	return glm::vec4(0.0f,0.0f,0.0f,1.0f);
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