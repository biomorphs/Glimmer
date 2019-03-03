#include "traceboi.h"
#include <iostream>
#include <stdint.h>
#include "../../imglib/image.h"
#include "../../imglib/raw_file_buffer.h"
#include "../../imglib/bitmap_file_writer.h"
#include "../../imglib/raw_file_io.h"
#include "math/glm_headers.h"

struct Ray
{
	glm::vec3 m_origin;
	glm::vec3 m_direction;
};

struct RenderParams
{
	glm::vec2 m_imageDimensions;
	glm::mat4 m_cameraToWorld;
	float m_fov;
	float m_scale;
	float m_aspectRatio;
};

bool RayPlaneIntersect(const Ray& r, const Plane& p, float &t, glm::vec3& normal)
{
	float denom = glm::dot(p.m_normal, r.m_direction);
	if (denom > 1e-6f) {
		glm::vec3 p0l0 = p.m_point - r.m_origin;
		t = glm::dot(p0l0, p.m_normal) / denom;
		normal = p.m_normal;
		return (t >= 0);
	}

	return false;
}

bool RaySphereIntersect(const Ray& ray, const Sphere& sphere, float &t, glm::vec3& normal)
{
	float radius2 = sphere.m_posAndRadius.w * sphere.m_posAndRadius.w;
	glm::vec3 l = glm::vec3(sphere.m_posAndRadius) - ray.m_origin;
	float tca = glm::dot(l, ray.m_direction);
	if (tca < 0) 
	{
		return false;
	}
	float d2 = glm::dot(l, l) - tca * tca;
	if (d2 > radius2)
	{
		return false;
	}
	float thc = sqrt(radius2 - d2);
	float t0 = tca - thc;
	float t1 = tca + thc;
	if (t0 > t1)
	{
		std::swap(t0, t1);
	}
	if (t0 < 0)
	{
		t0 = t1; 
		if (t0 < 0)
		{
			return false;
		}
	}
	t = t0;
	auto spherePos = glm::vec3(sphere.m_posAndRadius);
	auto hitPos = ray.m_origin + ray.m_direction * t;
	auto hitLength = glm::length(hitPos - spherePos);
	normal = glm::normalize(hitPos - spherePos);
	return true;
}

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

glm::vec4 CastRay(const Ray& ray, const TraceParamaters& globals)
{
	float t = std::numeric_limits<float>::max();
	glm::vec3 normal;
	for(auto s : globals.spheres)
	{
		if (RaySphereIntersect(ray, s, t, normal))
		{
			return glm::vec4(normal,1.0f);
		}
	}
	for (auto p : globals.planes)
	{
		if (RayPlaneIntersect(ray, p, t, normal))
		{
			return glm::vec4(normal, 1.0f);
		}
	}
	return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
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

	for (uint32_t y = 0; y < height; ++y)
	{
		for (uint32_t x = 0; x < width; ++x)
		{
			Ray primaryRay;
			primaryRay.m_origin = origin;
			primaryRay.m_direction = GeneratePrimaryRayDirection(globals, { (float)x, height - (float)y });
			glm::vec4 outColour = CastRay(primaryRay, parameters);
			*outBuffer++ = (uint8_t)(outColour.r * 255.0f);
			*outBuffer++ = (uint8_t)(outColour.g * 255.0f);
			*outBuffer++ = (uint8_t)(outColour.b * 255.0f);
			*outBuffer++ = (uint8_t)(outColour.a * 255.0f);
		}
	}
}

void Trace()
{
	const char* c_outputPath = "test.bmp";
	const ColourRGB c_outputColour(0, 0, 255);

	std::cout << "Glimmer!\n";
	uint32_t outputResX = 1024;
	uint32_t outputResY = 1024;

	auto outputImage = std::make_unique<Image>(outputResX, outputResY);
	for (uint32_t y = 0; y < outputResY; ++y)
	{
		for (uint32_t x = 0; x < outputResX; ++x)
		{
			outputImage->SetPixelColour(x, y, c_outputColour);
		}
	}

	RawFileBuffer outputRawBuffer(outputResX * outputResY * 4);
	BitmapFileWriter bitmapConverter;
	if (!bitmapConverter.WriteFile(*outputImage, outputRawBuffer))
	{
		std::cout << "Nope";
		return;
	}

	RawFileBufferWriter fileWriter;
	fileWriter.WriteTofile(c_outputPath, outputRawBuffer);
}