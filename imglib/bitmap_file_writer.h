#ifndef IMGCONVERTER_BITMAP_FILE_WRITER_INCLUDED
#define IMGCONVERTER_BITMAP_FILE_WRITER_INCLUDED

class Image;
class RawFileBuffer;
struct BitmapFileHeader;

// This class takes an image source object and outputs 24bpp bitmap data
// to the RawFileBuffer target
class BitmapFileWriter
{
public:
	BitmapFileWriter();
	~BitmapFileWriter();

	bool WriteFile(const Image& sourceData, RawFileBuffer& targetBuffer);

private:
	void GenerateHeader(const Image& sourceData, BitmapFileHeader& header);
};

#endif