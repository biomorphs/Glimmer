#ifndef IMGCONVERTER_COMPRESS_BLOCK_MINMAX_INCLUDED
#define IMGCONVERTER_COMPRESS_BLOCK_MINMAX_INCLUDED

#include "bitmap_to_dxt1_converter.h"

// This calculates DXT block reference colours by simply taking
// the min/max colour in the source image block. Not very accurate, but
// its fairly fast
class CompressBlocksMinMax : public IDXT1BlockRefColourCalculator
{
public:
	virtual ~CompressBlocksMinMax() { }
	void CalculateBlockRefColours(const Image& sourceImg, uint32_t blockX, uint32_t blockY, ColourRGB& ref0, ColourRGB& ref1);
};

#endif