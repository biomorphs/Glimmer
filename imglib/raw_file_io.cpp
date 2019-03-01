#include "raw_file_io.h"
#include "raw_file_buffer.h"
#include "utils.h"
#include <fstream>

RawFileBufferReader::RawFileBufferReader()
{
}

RawFileBufferReader::~RawFileBufferReader()
{
}

bool RawFileBufferReader::LoadFromFile(const std::string& path, RawFileBuffer& target)
{
	if (path.length() == 0)
	{
		return false;
	}

	if (!LoadInternal(path, target))
	{
		return false;
	}

	return true;
}

bool RawFileBufferReader::LoadInternal(const std::string& path, RawFileBuffer& target)
{
	std::ifstream fileStream(path.c_str(), std::ios::binary);
	if (fileStream.is_open() == false)
	{
		return false;
	}

	fileStream.seekg(0, std::ios_base::end);
	size_t totalFileSize = static_cast<size_t>( fileStream.tellg() );

	if (totalFileSize > 0)
	{
		target.Resize(totalFileSize);
		Assert(target.GetBufferSize() == totalFileSize && target.GetContents() != nullptr, "Failed to allocate file buffer");

		fileStream.seekg(0, std::ios_base::beg);
		fileStream.read(reinterpret_cast<char*>( target.GetContents() ), totalFileSize);
	}

	return true;
}

RawFileBufferWriter::RawFileBufferWriter()
{
}

RawFileBufferWriter::~RawFileBufferWriter()
{
}

bool RawFileBufferWriter::WriteTofile(const std::string& path, const RawFileBuffer& sourceData)
{
	if (path.length() == 0)
	{
		return false;
	}

	if (!WriteInternal(path, sourceData))
	{
		return false;
	}

	return true;
}

bool RawFileBufferWriter::WriteInternal(const std::string& path, const RawFileBuffer& sourceData)
{
	std::ofstream fileStream(path.c_str(), std::ios::binary);
	if (!fileStream.is_open())
	{
		return false;
	}

	fileStream.write(reinterpret_cast<const char*>( sourceData.GetContents() ), sourceData.GetBufferSize());

	return true;
}