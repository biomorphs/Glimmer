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
	float m_fov;
	glm::vec2 m_imageDimensions;
	glm::mat4x4 m_cameraToWorld;
};

float GetAspectRatio(const RenderParams& globals)
{
	return globals.m_imageDimensions.x > globals.m_imageDimensions.y ?
		globals.m_imageDimensions.x / globals.m_imageDimensions.y :
		globals.m_imageDimensions.y / globals.m_imageDimensions.x;
}

Ray GeneratePrimaryRay(const RenderParams& globals, glm::vec2 pixelPos)
{
	float aspectRatio = GetAspectRatio(globals);
	float px = (2.0f * ((pixelPos.x + 0.5f) / globals.m_imageDimensions.x) - 1.f) * tan(globals.m_fov / 2.0f * glm::pi<float>() / 180.f) * aspectRatio;
	float py = (1.f - 2.f * ((pixelPos.y + 0.5f) / globals.m_imageDimensions.y) * tan(globals.m_fov / 2.f * glm::pi<float>() / 180.f));
	glm::vec3 origin = {0.0f,0.0f,0.0f};
	glm::vec3 direction = glm::vec3(px, py, -1.0f) - origin;
	
	// should w be 1 or zero here?
	glm::vec3 originWorld = glm::vec3(globals.m_cameraToWorld * glm::vec4(origin, 0.0f));
	glm::vec3 rayPointWorld = glm::vec3(globals.m_cameraToWorld * glm::vec4( px, py, -1.0f, 0.0f ));
	glm::vec3 worldDirection = glm::normalize(rayPointWorld - originWorld);

	return { originWorld, worldDirection };
}

void TraceMeSomethingNice(TraceParamaters& parameters)
{
	const uint32_t width = parameters.width;
	const uint32_t height = parameters.height;
	uint8_t* outBuffer = parameters.outputBuffer.data();

	RenderParams globals;
	globals.m_fov = 90.0f;
	globals.m_imageDimensions = { width, height };

	for (uint32_t y = 0; y < height; ++y)
	{
		for (uint32_t x = 0; x < width; ++x)
		{
			Ray primaryRay = GeneratePrimaryRay(globals, {(float)x, (float)y});
			*outBuffer++ = 255;
			*outBuffer++ = 255;
			*outBuffer++ = 255;
			*outBuffer++ = 255;
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