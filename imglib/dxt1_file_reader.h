#ifndef IMGCONVERTER_DXT1_FILE_READER_INCLUDED
#define IMGCONVERTER_DXT1_FILE_READER_INCLUDED

#include <memory>

class RawFileBuffer;
class BlockCompressedImage;
struct DXT1FileHeader;

// This reads a DXT1 file from DDS data in the RawFileBuffer provided
// A block-compressed image is returned (i.e. no decompression happens here)
class DXT1FileReader
{
public:
	DXT1FileReader();
	~DXT1FileReader();

	bool ExtractHeader(const RawFileBuffer& data, DXT1FileHeader& targetHeader);
	bool IsFormatSupported(const DXT1FileHeader& header);
	std::unique_ptr<BlockCompressedImage> ExtractImage(const RawFileBuffer& data);
};

#endif