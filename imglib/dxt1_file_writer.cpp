#include "dxt1_file_writer.h"
#include "dxt1_file_header.h"
#include "block_compressed_image.h"
#include "raw_file_buffer.h"

DXT1FileWriter::DXT1FileWriter()
{
}

DXT1FileWriter::~DXT1FileWriter()
{

}

bool DXT1FileWriter::WriteFile(const BlockCompressedImage& sourceImage, RawFileBuffer& targetBuffer)
{
	if (sourceImage.GetWidthPixels() == 0 || sourceImage.GetHeightPixels() == 0)
	{
		return false;
	}

	DXT1FileHeader fileHeader;
	GenerateHeader(sourceImage, fileHeader);
	targetBuffer.AppendBytes(reinterpret_cast<const uint8_t*>(&fileHeader), sizeof(fileHeader));

	// We can now dump the entire block data object straight to the buffer since it is already
	// in the correct format
	const auto dataSize = sourceImage.GetHeightBlocks() * sourceImage.GetWidthBlocks() * sizeof(BlockCompressedPixels);
	targetBuffer.AppendBytes(sourceImage.BlockData(), dataSize);

	return true;
}

void DXT1FileWriter::GenerateHeader(const BlockCompressedImage& sourceImage, DXT1FileHeader& header)
{
	header.m_ddsFileToken = c_ddsFileToken;
	header.m_headerSize = c_dxt1HeaderSize;
	header.m_flags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT | DDSD_LINEARSIZE;
	header.m_heightPx = sourceImage.GetHeightPixels();
	header.m_widthPx = sourceImage.GetWidthPixels();
	header.m_pitchOrLinearSize = sourceImage.GetHeightBlocks() * sourceImage.GetWidthBlocks() * sizeof(BlockCompressedPixels);
	header.m_volumeDepth = 0;
	header.m_mipCount = 0;
	header.m_pixelFormatSize = c_dxt1PixelFormatSize;
	header.m_pixelFormatFlags = DDPF_FOURCC;
	header.m_pixelFormatFourCC = c_dxt1FourCC;
	header.m_capsFlags = DDSCAPS_TEXTURE;
	header.m_cubeVolumeFlags = 0;
	header.m_unusedFlags3;
	header.m_unusedFlags4;
	header.m_reserved2;
}