#ifndef IMGCONVERTER_BLOCK_COMPRESSED_IMAGE_INCLUDED
#define IMGCONVERTER_BLOCK_COMPRESSED_IMAGE_INCLUDED

#include "block_compressed_pixels.h"
#include <stdint.h>
#include <vector>

// An image containing 4x4 blocks of DXT1 pixel data
// Note! Data is stored with flipped y-axis (i.e. top left = 0,0; bottom right = w, h)
class BlockCompressedImage
{
public:
	BlockCompressedImage(uint32_t widthPixels, uint32_t heightPixels);
	BlockCompressedImage(BlockCompressedImage&& other);
	~BlockCompressedImage();
	BlockCompressedImage(const BlockCompressedImage& other) = delete;
	BlockCompressedImage& operator=(const BlockCompressedImage& other) = delete;
	BlockCompressedImage& operator=(BlockCompressedImage&& other);

	inline uint32_t GetWidthPixels() const { return m_widthPixels; }
	inline uint32_t GetHeightPixels() const { return m_heightPixels; }
	inline uint32_t GetWidthBlocks() const { return m_widthBlocks; }
	inline uint32_t GetHeightBlocks() const { return m_heightBlocks; }

	// Access at pixel level is slow since the LUT for a block must be generated each time
	// However, this is useful for testing
	void GetPixelColour(uint32_t x, uint32_t y, ColourRGB& colour) const;

	// Block access
	BlockCompressedPixels* BlockAt(uint32_t x, uint32_t y);
	const BlockCompressedPixels* BlockAt(uint32_t x, uint32_t y) const;

	// Raw data accessors for fast serialisation
	const uint8_t* BlockData() const { return reinterpret_cast<const uint8_t*>(m_pixelData.data()); }
	uint8_t* BlockData()	{ return reinterpret_cast<uint8_t*>(m_pixelData.data()); }

private:
	BlockCompressedPixels& GetBlock(uint32_t blckX, uint32_t blckY);
	const BlockCompressedPixels& GetBlock(uint32_t blckX, uint32_t blckY) const;
	uint32_t m_widthPixels;
	uint32_t m_heightPixels;
	uint32_t m_widthBlocks;
	uint32_t m_heightBlocks;
	std::vector<BlockCompressedPixels> m_pixelData;
};

#endif