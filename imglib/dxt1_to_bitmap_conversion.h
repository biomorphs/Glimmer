#ifndef IMGCONVERTER_DXT1_TO_BITMAP_CONVERSION_INCLUDED
#define IMGCONVERTER_DXT1_TO_BITMAP_CONVERSION_INCLUDED

#include <memory>

class Image;
class BlockCompressedImage;

// This takes a block-compressed image (DXT1 format), and returns a decompressed Image object
class DXT1ToBitmapConverter
{
public:
	DXT1ToBitmapConverter();
	~DXT1ToBitmapConverter();

	std::unique_ptr<Image> Convert(const BlockCompressedImage& sourceImage);
};

#endif