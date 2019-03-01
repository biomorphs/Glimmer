#include "bitmap_to_dxt1_converter.h"
#include "block_compressed_image.h"
#include "image.h"

BitmapToDXT1Converter::BitmapToDXT1Converter()
{

}

BitmapToDXT1Converter::~BitmapToDXT1Converter()
{
}

// This runs through each pixel in a 4x4 block, finds the closest match in the block
// LUT, and assigns it the correct index
void BitmapToDXT1Converter::BuildPixelIndicesForBlock(const Image& source, uint32_t blockX, uint32_t blockY, BlockCompressedPixels& target)
{
	const uint32_t startX = blockX << 2;
	const uint32_t startY = blockY << 2;
	const uint32_t endX = startX + 4;
	const uint32_t endY = startY + 4;
	const uint32_t maxPixelsY = source.GetHeightPixels();
	ColourRGB sourceColour;
	BlockCompressedPixels::ColourLUT lut(target);
	for (uint32_t py = startY; py < endY; ++py)
	{
		const uint32_t flippedY = maxPixelsY - 1 - py;		// Y-axis is flipped in compressed images
		for (uint32_t px = startX; px < endX; ++px)
		{
			source.GetPixelColour(px, flippedY, sourceColour);
			uint32_t lutIndex = lut.ClosestIndex(sourceColour);
			target.SetPixelColourIndex(px & 3, py & 3, lutIndex);
		}
	}
}

std::unique_ptr<BlockCompressedImage> BitmapToDXT1Converter::Convert(const Image& source, IDXT1BlockRefColourCalculator& compressor)
{
	if (source.GetWidthPixels() == 0 || source.GetHeightPixels() == 0)
	{
		return nullptr;
	}
	if ((source.GetWidthPixels() & 3) != 0 || (source.GetHeightPixels() & 3) != 0)	// Must be multiple of 4
	{
		return nullptr;
	}

	auto newImage = std::make_unique<BlockCompressedImage>(source.GetWidthPixels(), source.GetHeightPixels());
	Assert(newImage != nullptr);

	// For each block, calculate LUT reference colours, then calculate pixel indices
	// based on closest colour to the values in the LUT
	const uint32_t maxBlocksX = source.GetWidthPixels() >> 2;
	const uint32_t maxBlocksY = source.GetHeightPixels() >> 2;
	ColourRGB refColour0, refColour1;
	for (uint32_t blockY = 0; blockY < maxBlocksY; ++blockY)
	{
		for (uint32_t blockX = 0; blockX < maxBlocksX; ++blockX)
		{
			auto targetBlock = newImage->BlockAt(blockX, blockY);
			compressor.CalculateBlockRefColours(source, blockX, blockY, refColour0, refColour1);
			targetBlock->SetRefColour1(refColour0);
			targetBlock->SetRefColour2(refColour1);

			BuildPixelIndicesForBlock(source, blockX, blockY, *targetBlock);
		}
	}

	return newImage;
}