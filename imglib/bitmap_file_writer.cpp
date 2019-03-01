#include "bitmap_file_writer.h"
#include "bitmap_file_header.h"
#include "image.h"
#include "raw_file_buffer.h"

BitmapFileWriter::BitmapFileWriter()
{

}

BitmapFileWriter::~BitmapFileWriter()
{

}

bool BitmapFileWriter::WriteFile(const Image& sourceData, RawFileBuffer& targetBuffer)
{
	if (sourceData.GetWidthPixels() == 0 || sourceData.GetHeightPixels() == 0)
	{
		return false;
	}

	BitmapFileHeader fileHeader;
	GenerateHeader(sourceData, fileHeader);
	targetBuffer.AppendBytes(reinterpret_cast<const uint8_t*>(&fileHeader), sizeof(fileHeader));

	// Generate rows of 24-bit BGR data and write them to the buffer
	// Temporary buffer for one row of pixels (avoid too many appends to the file buffer for speed)
	std::vector<uint8_t> oneDataRow;	
	oneDataRow.resize(sourceData.GetWidthPixels() * 3);

	const uint32_t c_width = sourceData.GetWidthPixels();
	const uint32_t c_height = sourceData.GetHeightPixels();
	for (uint32_t pY = 0; pY < c_height; ++pY)
	{
		uint8_t* writePtr = oneDataRow.data();
		for (uint32_t pX = 0; pX < c_width; ++pX)
		{
			ColourRGB sourcePixel;
			sourceData.GetPixelColour(pX, pY, sourcePixel);
			*writePtr++ = sourcePixel.GetBlue();
			*writePtr++ = sourcePixel.GetGreen();
			*writePtr++ = sourcePixel.GetRed();
		}
		targetBuffer.AppendBytes(oneDataRow.data(), oneDataRow.size());
	}

	return true;
}

void BitmapFileWriter::GenerateHeader(const Image& sourceData, BitmapFileHeader& header)
{
	const uint32_t imgDataSizeBytes = static_cast< uint32_t >( sourceData.GetHeightPixels() * sourceData.GetWidthPixels() * 3 );
	const uint32_t imgFileTotalSize = static_cast< uint32_t >( imgDataSizeBytes + sizeof(BitmapFileHeader) );

	header.m_fileTypeToken = c_bitmapHeaderFileToken;
	header.m_totalFileSize = imgFileTotalSize;
	header.m_reserved = 0;
	header.m_dataOffset = sizeof(BitmapFileHeader);
	header.m_infoHeaderSize = 40;	// sizeof standard bitmapinfo header
	header.m_imgWidthPx = sourceData.GetWidthPixels();
	header.m_imgHeightPx = sourceData.GetHeightPixels();
	header.m_planeCount = 1;
	header.m_colourBitsPerPx = 24;
	header.m_compressionType = 0;	// no compression
	header.m_imageSizeBytes = imgDataSizeBytes;
	header.m_pixelsPerMeterX = 2835;	// 2835 pixels/meter = ~72dpi
	header.m_pixelsPerMeterY = 2835;	// 2835 pixels/meter = ~72dpi
	header.m_colourIndicesUsed = 0;	
	header.m_colourIndicesImportant = 0;
}