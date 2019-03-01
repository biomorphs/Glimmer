#ifndef IMGCONVERTER_BITMAP_FILE_READER_INCLUDED
#define IMGCONVERTER_BITMAP_FILE_READER_INCLUDED

#include <memory>

class RawFileBuffer;
class Image;
struct BitmapFileHeader;

// This class rakes a RawFileBuffer and returns an Image object containing the 
// image data
class BitmapFileReader
{
public:
	BitmapFileReader();
	~BitmapFileReader();

	bool ExtractHeader(const RawFileBuffer& data, BitmapFileHeader& targetHeader);
	bool IsFormatSupported(const BitmapFileHeader& header);
	std::unique_ptr<Image> ExtractImage(const RawFileBuffer& data);
};

#endif