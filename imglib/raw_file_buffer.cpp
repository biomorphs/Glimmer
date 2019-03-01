#include "raw_file_buffer.h"
#include "utils.h"

RawFileBuffer::RawFileBuffer()
{
}

RawFileBuffer::RawFileBuffer(size_t initialSize)
{
	m_rawData.reserve(initialSize);
}

RawFileBuffer::RawFileBuffer(RawFileBuffer&& other)
{
	m_rawData = std::move(other.m_rawData);
}

RawFileBuffer::~RawFileBuffer()
{
}

void RawFileBuffer::AppendBytes(const uint8_t* sourceData, size_t sourceBufferSize)
{
	if (sourceData != nullptr && sourceBufferSize > 0)
	{
		m_rawData.insert(m_rawData.end(), sourceData, sourceData + sourceBufferSize);
	}
}

void RawFileBuffer::Resize(size_t newSize)
{
	m_rawData.resize(newSize);
}