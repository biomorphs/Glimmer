#ifndef IMGCONVERTER_COLOUR_RGB_INCLUDED
#define IMGCONVERTER_COLOUR_RGB_INCLUDED

#include <stdint.h>

// RGB colour represented as 1 byte per channel
// We store a 'hidden' alpha channel, just to ensure 4-byte alignment for faster read/write,
// and enable eventual SIMD access if required
class ColourRGB
{
public:
	inline ColourRGB();
	inline ColourRGB(const ColourRGB& other);
	inline ColourRGB(ColourRGB&& other);
	inline explicit ColourRGB(const uint8_t* b);
	inline explicit ColourRGB(int32_t r, int32_t g, int32_t b);
	inline explicit ColourRGB(uint8_t r, uint8_t g, uint8_t b);
	inline explicit ColourRGB(float r, float g, float b);
	inline explicit ColourRGB(uint16_t colR5G6B5);
	inline ~ColourRGB();

	inline ColourRGB& operator=(const ColourRGB& other);
	inline ColourRGB& operator=(ColourRGB&& other);

	inline bool operator==(const ColourRGB& other) const;
	inline bool operator!=(const ColourRGB& other) const;

	inline void SetRed(uint8_t r) { m_rgb[0] = r; }
	inline void SetGreen(uint8_t g) { m_rgb[1] = g; }
	inline void SetBlue(uint8_t b) { m_rgb[2] = b; }
	inline uint8_t GetRed() const { return m_rgb[0]; }
	inline uint8_t GetGreen() const { return m_rgb[1]; }
	inline uint8_t GetBlue() const { return m_rgb[2]; }

	inline float GetRedAsFloat() const;
	inline float GetGreenAsFloat() const;
	inline float GetBlueAsFloat() const;

	inline void ToRGBBuffer(uint8_t* targetBuffer) const;	// Assumes target has space for 3 consecutive bytes

	inline uint16_t ToR5G6B5() const;

	static inline ColourRGB Min(const ColourRGB& c0, const ColourRGB& c1);
	static inline ColourRGB Max(const ColourRGB& c0, const ColourRGB& c1);

	// Returns a float representing relative 'distance' from source to target
	static inline float Distance(const ColourRGB& source, const ColourRGB& target);

private:
	inline float ByteToFloatValue(uint8_t v) const;
	inline uint8_t FloatToByteValue(float v) const;
	union {
		uint32_t m_rgba;
		uint8_t m_rgb[4];
	};
};

#include "colour_rgb.inl"

#endif