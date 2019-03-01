#ifndef IMGCONVERTER_BITMAP_FILE_HEADER_INCLUDED
#define IMGCONVERTER_BITMAP_FILE_HEADER_INCLUDED

#include <stdint.h>
#include <stddef.h>

#pragma pack(1)								// Assume compiler supports this, static assert below to test
struct BitmapFileHeader
{
	uint16_t m_fileTypeToken;				// Should always be 'BM' / 0x4d42
	uint32_t m_totalFileSize;				// May be unreliable
	uint32_t m_reserved;
	uint32_t m_dataOffset;					// Offset into the data buffer where the image is stored
	uint32_t m_infoHeaderSize;				// Size of the next header part; should be 40 bytes for standard bmp
	uint32_t m_imgWidthPx;
	uint32_t m_imgHeightPx;
	uint16_t m_planeCount;					// Must be 1
	uint16_t m_colourBitsPerPx;				// Bits per pixel, should be 24 for uncompressed 24 bit bmp
	uint32_t m_compressionType;				// We only support '0' (no compression)
	uint32_t m_imageSizeBytes;				// May be 0 for uncompressed bitmaps
	uint32_t m_pixelsPerMeterX;				// Pixels per meter horizontal
	uint32_t m_pixelsPerMeterY;				// Pixels per meter vertical
	uint32_t m_colourIndicesUsed;			// How many colours in the palette are used, 0 = all colours. Unused
	uint32_t m_colourIndicesImportant;		// How many colours in the palette are required to display the image. Unused
};
#pragma pack()
static_assert(offsetof(BitmapFileHeader, m_totalFileSize) == 2, "Bitmap header struct not packed correctly, check packing");

const uint16_t c_bitmapHeaderFileToken = 0x4d42;

#endif