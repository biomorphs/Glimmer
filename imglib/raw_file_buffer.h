#ifndef IMGCONVERTER_RAW_FILE_BUFFER_INCLUDED
#define IMGCONVERTER_RAW_FILE_BUFFER_INCLUDED

#include <stdint.h>
#include <vector>

// File buffer wrapper. Used to store the contents of raw files from disk
class RawFileBuffer
{
public:
	RawFileBuffer();
	explicit RawFileBuffer(size_t initialSize);
	RawFileBuffer(RawFileBuffer&& other);
	~RawFileBuffer();

	RawFileBuffer(const RawFileBuffer& other) = delete;
	RawFileBuffer& operator=(const RawFileBuffer& other) = delete;

	inline size_t GetBufferSize() const { return m_rawData.size(); }
	inline uint8_t* GetContents() { return m_rawData.data(); }
	inline const uint8_t* GetContents() const { return m_rawData.data(); }	

	void AppendBytes(const uint8_t* sourceData, size_t sourceBufferSize);
	void Resize(size_t newSize);

private:
	std::vector<uint8_t> m_rawData;
};

#endif