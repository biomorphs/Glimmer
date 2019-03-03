#pragma once
#include <vector>

struct TraceParamaters
{
	std::vector<uint8_t>& outputBuffer;
	uint32_t width;
	uint32_t height;
};

void TraceMeSomethingNice(TraceParamaters& parameters);