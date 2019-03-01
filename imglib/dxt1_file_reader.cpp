#include "dxt1_file_reader.h"
#include "dxt1_file_header.h"
#include "block_compressed_image.h"
#include "raw_file_buffer.h"

// Header flags that should always be set in DXT1 textures
static const uint32_t c_expectedHeaderFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_LINEARSIZE;

DXT1FileReader::DXT1FileReader()
{

}

DXT1FileReader::~DXT1FileReader()
{

}

bool DXT1FileReader::ExtractHeader(const RawFileBuffer& data, DXT1FileHeader& targetHeader)
{
	if (data.GetContents() == nullptr || data.GetBufferSize() < sizeof(targetHeader))
	{
		return false;
	}

	memcpy(&targetHeader, data.GetContents(), sizeof(targetHeader));

	// Ensure its a dds file
	if (targetHeader.m_ddsFileToken != c_ddsFileToken)
	{
		return false;
	}

	// Ensure the header looks valid by testing the embedded struct sizes
	if (targetHeader.m_headerSize != c_dxt1HeaderSize || targetHeader.m_pixelFormatSize != c_dxt1PixelFormatSize)
	{
		return false;
	}

	return true;
}

bool DXT1FileReader::IsFormatSupported(const DXT1FileHeader& header)
{
	if ((header.m_flags & c_expectedHeaderFlags) != c_expectedHeaderFlags)
	{
		return false;	// Unexpected header flags
	}
	if (header.m_heightPx == 0 || header.m_widthPx == 0)
	{
		return false;
	}
	if (header.m_pitchOrLinearSize == 0)
	{
		return false;	// This must be set to pull out the top mip level
	}
	if ((header.m_pixelFormatFlags & DDPF_FOURCC) == 0)
	{
		return false;	// DXT1 only
	}
	if ((header.m_capsFlags & DDSCAPS_TEXTURE) == 0)
	{
		return false;
	}
	if (header.m_pixelFormatFourCC != c_dxt1FourCC)
	{
		return false;	// DXT1 only
	}

	return true;
}

std::unique_ptr<BlockCompressedImage> DXT1FileReader::ExtractImage(const RawFileBuffer& data)
{
	DXT1FileHeader header;
	if (!ExtractHeader(data, header))
	{
		return nullptr;
	}

	if (!IsFormatSupported(header))
	{
		return nullptr;
	}

	// The data should be immediately after the header and aligned to 16 bytes
	const uintptr_t dataOffset = sizeof(DXT1FileHeader);
	Assert(data.GetBufferSize() - dataOffset >= header.m_pitchOrLinearSize);

	// Create the new block compressed image and copy the data directly into its buffer
	auto newImage = std::make_unique<BlockCompressedImage>(header.m_widthPx, header.m_heightPx);
	Assert(newImage.get() != nullptr && newImage->BlockData() != nullptr);
	memcpy(newImage->BlockData(), data.GetContents() + dataOffset, header.m_pitchOrLinearSize);

	return newImage;
}