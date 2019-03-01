#include "bitmap_file_reader.h"
#include "bitmap_file_header.h"
#include "raw_file_buffer.h"
#include "image.h"

BitmapFileReader::BitmapFileReader()
{

}

BitmapFileReader::~BitmapFileReader()
{

}

bool BitmapFileReader::ExtractHeader(const RawFileBuffer& data, BitmapFileHeader& targetHeader)
{
	// Ensure the file buffer is big enough to pull out the header
	if (data.GetBufferSize() < sizeof(BitmapFileHeader) || data.GetContents() == nullptr)
	{
		return false;
	}

	// Copy the data directly from the buffer into the header.
	memcpy(static_cast<void*>(&targetHeader), static_cast<const void*>(data.GetContents()), sizeof(targetHeader));

	// Ensure the file type token is set
	if (targetHeader.m_fileTypeToken != c_bitmapHeaderFileToken)	// Is it a bitmap?
	{
		return false;
	}

	return true;
}

bool BitmapFileReader::IsFormatSupported(const BitmapFileHeader& header)
{
	if (header.m_dataOffset < sizeof(BitmapFileHeader))	// Data cannot overlap header
	{
		return false;
	}
	if (header.m_infoHeaderSize != 40)	// We only support 'basic' bitmap format with 40-byte header
	{
		return false;
	}
	if (header.m_imgWidthPx == 0 || header.m_imgHeightPx == 0)	// Must have at least 1 pixel width or height
	{
		return false;
	}
	if (header.m_planeCount != 1)	// Invalid bmp? Planes must always == 1
	{
		return false;
	}
	if (header.m_colourBitsPerPx != 24)	// We only care about uncompressed, unpaletted RGB8 bitmaps
	{
		return false;
	}
	if (header.m_compressionType != 0)	// don't support compressed bitmaps
	{
		return false;
	}

	return true;
}

std::unique_ptr<Image> BitmapFileReader::ExtractImage(const RawFileBuffer& data)
{
	BitmapFileHeader imageHeader;
	if (!ExtractHeader(data, imageHeader))
	{
		return nullptr;
	}

	if (!IsFormatSupported(imageHeader))
	{
		return nullptr;
	}

	// Ensure data buffer is big enough for the data (i.e. is the header lying?)
	if (data.GetBufferSize() < imageHeader.m_dataOffset + imageHeader.m_imageSizeBytes)
	{
		return nullptr;
	}

	auto theImage = std::make_unique<Image>(imageHeader.m_imgWidthPx, imageHeader.m_imgHeightPx);

	// Load the image data line-by-line
	const uint32_t c_width = imageHeader.m_imgWidthPx;
	const uint32_t c_height = imageHeader.m_imgHeightPx;
	const uint8_t* dataPtr = data.GetContents() + imageHeader.m_dataOffset;
	for (uint32_t pY = 0; pY < c_height; ++pY)
	{
		for (uint32_t pX = 0; pX < c_width; ++pX)
		{
			// Colours are stored in BGR format, swizzle them here
			ColourRGB c(*(dataPtr + 2), *(dataPtr + 1), *dataPtr);
			theImage->SetPixelColour(pX, pY, c);
			dataPtr += 3;
		}
	}

	return theImage;
}