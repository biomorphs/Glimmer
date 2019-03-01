#ifndef IMGCONVERTER_IMAGE_TO_BLOCK_COMPRESSED_INCLUDED
#define IMGCONVERTER_IMAGE_TO_BLOCK_COMPRESSED_INCLUDED

#include <memory>

class BlockCompressedImage;
class BlockCompressedPixels;
class Image;
class ColourRGB;

// Interface for calculating LUT ref colours for blocks
class IDXT1BlockRefColourCalculator
{
public:
	virtual ~IDXT1BlockRefColourCalculator() { }
	virtual void CalculateBlockRefColours(const Image& sourceImg, uint32_t blockX, uint32_t blockY, ColourRGB& ref0, ColourRGB& ref1) = 0;
};

// This takes a Image object and compresses it to DXT1 format, using the 
// IDXT1BlockRefColourCalculator to determine how block reference colours
// are calculated.
class BitmapToDXT1Converter
{
public:
	BitmapToDXT1Converter();
	~BitmapToDXT1Converter();

	std::unique_ptr<BlockCompressedImage> Convert(const Image& source, IDXT1BlockRefColourCalculator& compressor);

private:
	void BuildPixelIndicesForBlock(const Image& source, uint32_t blockX, uint32_t blockY, BlockCompressedPixels& target);
};

#endif