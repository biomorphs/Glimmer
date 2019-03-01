#ifndef IMGCONVERTER_BLOCK_COMPRESSED_PIXELS_INCLUDED
#define IMGCONVERTER_BLOCK_COMPRESSED_PIXELS_INCLUDED

#include <stdint.h>
#include "colour_rgb.h"

// A pixel block consists of 2 colours stored in R5G6B5, followed by
// 16 2-bit indices used to linearly interpolate between the two reference points
// A lookup table can be generated from the block to speed-up pixel colour calculation
// Note! data is stored with flipped y-axis (so 0 = top, height = bottom)
class BlockCompressedPixels
{
public:
	inline BlockCompressedPixels();
	inline BlockCompressedPixels(const BlockCompressedPixels& other);
	inline BlockCompressedPixels(BlockCompressedPixels&& other);
	inline ~BlockCompressedPixels();
	inline BlockCompressedPixels& operator=(const BlockCompressedPixels& other);
	inline BlockCompressedPixels& operator=(BlockCompressedPixels&& other);

	inline void SetRefColour1(const ColourRGB& col);
	inline void SetRefColour2(const ColourRGB& col);
	inline uint16_t GetRefColour1() const { return m_refColour1; }
	inline uint16_t GetRefColour2() const { return m_refColour2; }

	inline void SetPixelColourIndex(uint32_t x, uint32_t y, uint32_t id);
	inline uint32_t GetPixelColourIndex(uint32_t x, uint32_t y) const;

	class ColourLUT	// Lookup table helper for getting pixel colours
	{
	public:
		inline ColourLUT(const BlockCompressedPixels& block);
		inline const ColourRGB& GetColour(uint32_t index);
		inline uint32_t ClosestIndex(const ColourRGB& testColour);	// Calculate best index for this colour
	private:
		ColourRGB m_lut[4];
	};

private:
	inline uint32_t GetIndexShiftPosition(uint32_t x, uint32_t y) const;
	union
	{
		uint64_t m_rawData;
		struct
		{
			uint16_t m_refColour1;
			uint16_t m_refColour2;
			uint32_t m_indices;
		};
	};
};

#include "block_compressed_pixels.inl"

#endif