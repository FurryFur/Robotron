#include "BufferStream.h"

#include <cstring>
#include <iostream>

OutBufferStream::OutBufferStream()
	: m_writeHeadIdx{ 0 }
{
}

OutBufferStream& OutBufferStream::write(const std::string& str) noexcept
{
	size_t chunkSize = str.length() + 1;
	if (m_writeHeadIdx + chunkSize > m_data.size())
		m_data.resize(m_writeHeadIdx + chunkSize);

	std::memcpy(&m_data[m_writeHeadIdx], str.c_str(), chunkSize);

	m_writeHeadIdx += chunkSize;
	return *this;
}

void OutBufferStream::reset() noexcept
{
	m_writeHeadIdx = 0;
}

const char* OutBufferStream::getData()
{
	return m_data.data();
}

size_t OutBufferStream::getBytesWritten()
{
	return m_writeHeadIdx;
}

OutBufferStream& OutBufferStream::operator<<(const std::string& str) noexcept
{
	return write(str);
}

InBufferStream::InBufferStream(const std::vector<char>& buffer) noexcept
	: m_data{ buffer }
	, m_readHeadIdx{ 0 }
	, m_error{ IBS_ERROR_NONE }
{
}

IBSError InBufferStream::checkBufferOverrun(size_t nextReadHeadIdx)
{
	if (m_error)
		return m_error;

	if (nextReadHeadIdx > m_data.size()) {
		setError(IBS_ERROR_WOULD_READ_PAST_END);
	}

	return m_error;
}

IBSError InBufferStream::checkBufferOverrunStr()
{
	if (m_error)
		return m_error;

	if (std::memchr(&m_data[m_readHeadIdx], 0, m_data.size() - m_readHeadIdx)) {
		setError(IBS_ERROR_WOULD_READ_PAST_END);
	}

	return m_error;
}

IBSError InBufferStream::getError()
{
	return m_error;
}

void InBufferStream::setError(IBSError error)
{
	if (error != IBS_ERROR_NONE) {
		std::cout << "Error occured reading data from input buffer stream" << std::endl;
		switch (error)
		{
		case IBS_ERROR_CORRUPT_DATA:
			std::cout << "Data was corrupted" << std::endl;
			break;
		case IBS_ERROR_WOULD_READ_PAST_END:
			std::cout << "Detected would overrun buffer" << std::endl;
			break;
		default:
			break;
		}
		m_error = error;
	}
}

InBufferStream& InBufferStream::read(std::string& outString) noexcept
{
	if (checkBufferOverrunStr() != IBS_ERROR_NONE)
		return *this;

	outString = m_data[m_readHeadIdx];
	m_readHeadIdx += outString.length();
	return *this;
}

void InBufferStream::reset() noexcept
{
	m_readHeadIdx = 0;
}

size_t InBufferStream::getBytesRead()
{
	return m_readHeadIdx;
}

InBufferStream& InBufferStream::operator>>(std::string& outString) noexcept
{
	return read(outString);
}
