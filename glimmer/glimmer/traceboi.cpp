#include "traceboi.h"
#include <iostream>
#include <stdint.h>
#include "../../imglib/image.h"
#include "../../imglib/raw_file_buffer.h"
#include "../../imglib/bitmap_file_writer.h"
#include "../../imglib/raw_file_io.h"

void TraceMeSomethingNice(TraceParamaters& parameters)
{
	const uint32_t width = parameters.width;
	const uint32_t height = parameters.height;
	uint8_t* outBuffer = parameters.outputBuffer.data();

	for (uint32_t y = 0; y < height; ++y)
	{
		for (uint32_t x = 0; x < width; ++x)
		{
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