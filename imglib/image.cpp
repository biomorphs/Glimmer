#include "image.h"

Image::Image(uint32_t widthPixels, uint32_t heightPixels)
	: m_widthPixels(widthPixels)
	, m_heightPixels(heightPixels)
{
	m_pixelData.resize(widthPixels * heightPixels);
}

Image::Image(Image&& other)
{
	*this = std::move(other);
}

Image::~Image()
{
}

Image& Image::operator=(Image&& other)
{
	m_pixelData = std::move(other.m_pixelData);
	m_widthPixels = other.m_widthPixels;
	m_heightPixels = other.m_heightPixels;
	other.m_widthPixels = 0;
	other.m_heightPixels = 0;
	return *this;
}