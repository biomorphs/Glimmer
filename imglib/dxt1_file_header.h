#ifndef IMGCONVERTER_DXT1_FILE_HEADER_INCLUDED
#define IMGCONVERTER_DXT1_FILE_HEADER_INCLUDED

#include <stdint.h>

// All flags / struct formats are from https://msdn.microsoft.com/en-us/library/bb943992%28v=vs.85%29.aspx
// Note that not all flags are dealt with, only the ones we care about are listed here

enum DXT1HeaderFlags
{
	DDSD_CAPS = 0x1,						//Required in every.dds file.	
	DDSD_HEIGHT = 0x2,						//Required in every.dds file.	
	DDSD_WIDTH = 0x4,						//Required in every.dds file.
	DDSD_PIXELFORMAT = 0x1000,				//Required in every.dds file.
	DDSD_MIPMAPCOUNT = 0x20000,				//Required in a mipmapped texture.
	DDSD_LINEARSIZE = 0x80000,				//Required when pitch is provided for a compressed texture.
};

enum DXT1HeaderCapsFlags
{
	DDSCAPS_TEXTURE = 0x1000				//Required for all textures, the only flag we care about
};

enum DXT1PixelFormatFlags
{
	DDPF_FOURCC = 0x4						//Texture contains compressed RGB data; 
};

struct DXT1FileHeader
{
	uint32_t m_ddsFileToken;				// Should always be 'DDS'
	uint32_t m_headerSize;					// Should always be 124
	uint32_t m_flags;						// See DXT1HeaderFlags
	uint32_t m_heightPx;					// Image height
	uint32_t m_widthPx;						// Image Width
	uint32_t m_pitchOrLinearSize;			// Pitch of uncompressed texture or total size of highest mip in compressed one
	uint32_t m_volumeDepth;					// How many slices in volume texture
	uint32_t m_mipCount;					// Mip count
	uint32_t m_reserved1[11];
	uint32_t m_pixelFormatSize;				// Should always be c_dxt1PixelFormatSize
	uint32_t m_pixelFormatFlags;			// See DXT1PixelFormatFlags
	uint32_t m_pixelFormatFourCC;			// FourCC of the pixel format. 
	uint32_t m_pixelFormatRGBBitCount;		// Num. bits in RGB data, we don't use it
	uint32_t m_pixelFormatRBitMask;			// Bit mask of red channel in uncompressed textures
	uint32_t m_pixelFormatGBitMask;			// Bit mask of green channel in uncompressed textures
	uint32_t m_pixelFormatBBitMask;			// Bit mask of blue channel in uncompressed textures
	uint32_t m_pixelFormatABitMask;			// Bit mask of alpha channel in uncompressed textures
	uint32_t m_capsFlags;					// See DXT1HeaderCapsFlags
	uint32_t m_cubeVolumeFlags;				// Cubemap / volume texture flags
	uint32_t m_unusedFlags3;
	uint32_t m_unusedFlags4;
	uint32_t m_reserved2;
};

static const uint32_t c_ddsFileToken = ' SDD';		// Token to mark a dds file
static const uint32_t c_dxt1HeaderSize = 124;		// Size of header struct, should always be 124
static const uint32_t c_dxt1PixelFormatSize = 32;	// Size of pixel format struct, should always be 32
static const uint32_t c_dxt1FourCC = '1TXD';		// DXT1 FourCC

#endif