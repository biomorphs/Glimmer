#include "dxt1_to_bitmap_conversion.h"
#include "image.h"
#include "block_compressed_image.h"

DXT1ToBitmapConverter::DXT1ToBitmapConverter()
{

}

DXT1ToBitmapConverter::~DXT1ToBitmapConverter()
{

}

std::unique_ptr<Image> DXT1ToBitmapConverter::Convert(const BlockCompressedImage& sourceImage)
{
	if (sourceImage.GetWidthBlocks() == 0 || sourceImage.GetHeightBlocks() == 0)
	{
		return nullptr;
	}

	// Make a new image object to contain the converted data
	auto newImage = std::make_unique<Image>(sourceImage.GetWidthPixels(), sourceImage.GetHeightPixels());
	Assert(newImage != nullptr);

	// Convert one block at a time
	const uint32_t c_imgWidthBlocks = sourceImage.GetWidthBlocks();
	const uint32_t c_imgHeightBlocks = sourceImage.GetHeightBlocks();
	const uint32_t c_imgHeightPx = sourceImage.GetHeightPixels();
	for (uint32_t yBlock = 0; yBlock < c_imgHeightBlocks; ++yBlock)
	{
		for (uint32_t xBlock = 0; xBlock < c_imgWidthBlocks; ++xBlock)
		{
			const uint32_t flippedYBlock = c_imgHeightBlocks - 1 - yBlock;		// Y-axis is flipped in DXT image
			const auto thisBlock = sourceImage.BlockAt(xBlock, flippedYBlock);
			
			BlockCompressedPixels::ColourLUT lut(*thisBlock);
			for (uint32_t py = 0; py < 4; ++py)
			{
				for (uint32_t px = 0; px < 4; ++px)
				{
					auto pixelColour = lut.GetColour(thisBlock->GetPixelColourIndex(px, py));

					// Y-axis is flipped in DXT blocks
					const uint32_t flippedYPx = 3 - py;
					newImage->SetPixelColour((xBlock * 4) + px, (yBlock * 4) + flippedYPx, pixelColour);
				}
			}
		}
	}

	return newImage;
}