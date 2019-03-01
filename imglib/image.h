#ifndef IMGCONVERTER_IMAGE_INCLUDED
#define IMGCONVERTER_IMAGE_INCLUDED

#include "colour_rgb.h"
#include <stdint.h>
#include <vector>

// Class representing an image containing RGB data (0,0=bottom left pixel)
class Image
{
public:
	Image(uint32_t widthPixels, uint32_t heightPixels);
	Image(Image&& other);
	~Image();

	Image(const Image& other) = delete;
	Image& operator=(const Image& other) = delete;
	Image& operator=(Image&& other);

	inline uint32_t GetWidthPixels() const { return m_widthPixels; }
	inline uint32_t GetHeightPixels() const { return m_heightPixels; }

	inline void SetPixelColour(uint32_t xCoord, uint32_t yCoord, const ColourRGB& value);
	inline void GetPixelColour(uint32_t xCoord, uint32_t yCoord, ColourRGB& value) const;
	
private:

	inline ColourRGB& PixelAt(uint32_t x, uint32_t y) { return m_pixelData.data()[ (y * m_widthPixels) + x ]; }
	inline const ColourRGB& PixelAt(uint32_t x, uint32_t y) const { return m_pixelData.data()[(y * m_widthPixels) + x]; }

	uint32_t m_widthPixels;
	uint32_t m_heightPixels;
	std::vector<ColourRGB> m_pixelData;
};

void Image::SetPixelColour(uint32_t xCoord, uint32_t yCoord, const ColourRGB& value)
{
	if (xCoord < m_widthPixels && yCoord < m_heightPixels)
	{
		PixelAt(xCoord, yCoord) = value;
	}
}

void Image::GetPixelColour(uint32_t xCoord, uint32_t yCoord, ColourRGB& value) const
{
	if (xCoord < m_widthPixels && yCoord < m_heightPixels)
	{
		value = PixelAt(xCoord, yCoord);
	}
}

#endif