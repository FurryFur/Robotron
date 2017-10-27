#include "BufferStream.h"

#include <cstring>
#include <iostream>

OutBufferStream::OutBufferStream()
	: m_writeHeadIdx{ 0 }
{
}

OutBufferStream& OutBufferStream::write(std::float_t _float) noexcept
{
	return write(*reinterpret_cast<std::uint32_t*>(&_float));
}

OutBufferStream& OutBufferStream::write(std::double_t _double) noexcept
{
	return write(*reinterpret_cast<std::uint64_t*>(&_double));
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

OutBufferStream& OutBufferStream::operator<<(std::uint64_t dword) noexcept
{
	return write(dword);
}

OutBufferStream& OutBufferStream::operator<<(std::uint32_t word) noexcept
{
	return write(word);
}

OutBufferStream& OutBufferStream::operator<<(std::uint16_t _short) noexcept
{
	return write(_short);
}

OutBufferStream& OutBufferStream::operator<<(std::uint8_t byte) noexcept
{
	return write(byte);
}

OutBufferStream& OutBufferStream::operator<<(std::int64_t dword) noexcept
{
	return write(dword);
}

OutBufferStream& OutBufferStream::operator<<(std::int32_t word) noexcept
{
	return write(word);
}

OutBufferStream& OutBufferStream::operator<<(std::int16_t _short) noexcept
{
	return write(_short);
}

OutBufferStream& OutBufferStream::operator<<(std::int8_t byte) noexcept
{
	return write(byte);
}

OutBufferStream& OutBufferStream::operator<<(std::float_t _float) noexcept
{
	return write(_float);
}

OutBufferStream& OutBufferStream::operator<<(std::double_t _double) noexcept
{
	return write(_double);
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

InBufferStream& InBufferStream::read(std::uint64_t& outDword) noexcept
{
	size_t nextReadHeadIdx = m_readHeadIdx + sizeof(outDword);
	if (checkBufferOverrun(nextReadHeadIdx) != IBS_ERROR_NONE)
		return *this;

	outDword = *reinterpret_cast<const std::uint64_t*>(&m_data[m_readHeadIdx]);
	m_readHeadIdx = nextReadHeadIdx;
	return *this;
}

InBufferStream& InBufferStream::read(std::uint32_t& outWord) noexcept
{
	size_t nextReadHeadIdx = m_readHeadIdx + sizeof(outWord);
	if (checkBufferOverrun(nextReadHeadIdx) != IBS_ERROR_NONE)
		return *this;

	outWord = *reinterpret_cast<const std::uint32_t*>(&m_data[m_readHeadIdx]);
	m_readHeadIdx = nextReadHeadIdx;
	return *this;
}

InBufferStream& InBufferStream::read(std::uint16_t& outShort) noexcept
{
	size_t nextReadHeadIdx = m_readHeadIdx + sizeof(outShort);
	if (checkBufferOverrun(nextReadHeadIdx) != IBS_ERROR_NONE)
		return *this;

	outShort = *reinterpret_cast<const std::uint16_t*>(&m_data[m_readHeadIdx]);
	m_readHeadIdx = nextReadHeadIdx;
	return *this;
}

InBufferStream& InBufferStream::read(std::uint8_t& outByte) noexcept
{
	size_t nextReadHeadIdx = m_readHeadIdx + sizeof(outByte);
	if (checkBufferOverrun(nextReadHeadIdx) != IBS_ERROR_NONE)
		return *this;

	outByte = *reinterpret_cast<const std::uint8_t*>(&m_data[m_readHeadIdx]);
	m_readHeadIdx = nextReadHeadIdx;
	return *this;
}

InBufferStream& InBufferStream::read(std::int64_t& outDword) noexcept
{
	size_t nextReadHeadIdx = m_readHeadIdx + sizeof(outDword);
	if (checkBufferOverrun(nextReadHeadIdx) != IBS_ERROR_NONE)
		return *this;

	outDword = *reinterpret_cast<const std::int64_t*>(&m_data[m_readHeadIdx]);
	m_readHeadIdx = nextReadHeadIdx;
	return *this;
}

InBufferStream& InBufferStream::read(std::int32_t& outWord) noexcept
{
	size_t nextReadHeadIdx = m_readHeadIdx + sizeof(outWord);
	if (checkBufferOverrun(nextReadHeadIdx) != IBS_ERROR_NONE)
		return *this;

	outWord = *reinterpret_cast<const std::int32_t*>(&m_data[m_readHeadIdx]);
	m_readHeadIdx = nextReadHeadIdx;
	return *this;
}

InBufferStream& InBufferStream::read(std::int16_t& outShort) noexcept
{
	size_t nextReadHeadIdx = m_readHeadIdx + sizeof(outShort);
	if (checkBufferOverrun(nextReadHeadIdx) != IBS_ERROR_NONE)
		return *this;

	outShort = *reinterpret_cast<const std::int16_t*>(&m_data[m_readHeadIdx]);
	m_readHeadIdx = nextReadHeadIdx;
	return *this;
}

InBufferStream& InBufferStream::read(std::int8_t& outByte) noexcept
{
	size_t nextReadHeadIdx = m_readHeadIdx + sizeof(outByte);
	if (checkBufferOverrun(nextReadHeadIdx) != IBS_ERROR_NONE)
		return *this;

	outByte = *reinterpret_cast<const std::int8_t*>(&m_data[m_readHeadIdx]);
	m_readHeadIdx = nextReadHeadIdx;
	return *this;
}

InBufferStream& InBufferStream::read(std::float_t& outFloat) noexcept
{
	size_t nextReadHeadIdx = m_readHeadIdx + sizeof(outFloat);
	if (checkBufferOverrun(nextReadHeadIdx) != IBS_ERROR_NONE)
		return *this;

	outFloat = *reinterpret_cast<const std::float_t*>(&m_data[m_readHeadIdx]);
	m_readHeadIdx = nextReadHeadIdx;
	return *this;
}

InBufferStream& InBufferStream::read(std::double_t& outDouble) noexcept
{
	size_t nextReadHeadIdx = m_readHeadIdx + sizeof(outDouble);
	if (checkBufferOverrun(nextReadHeadIdx) != IBS_ERROR_NONE)
		return *this;

	outDouble = *reinterpret_cast<const std::double_t*>(&m_data[m_readHeadIdx]);
	m_readHeadIdx = nextReadHeadIdx;
	return *this;
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

InBufferStream& InBufferStream::operator>>(std::uint64_t& outDword) noexcept
{
	return read(outDword);
}

InBufferStream& InBufferStream::operator>>(std::uint32_t& outWord) noexcept
{
	return read(outWord);
}

InBufferStream& InBufferStream::operator>>(std::uint16_t& outShort) noexcept
{
	return read(outShort);
}

InBufferStream& InBufferStream::operator>>(std::uint8_t& outByte) noexcept
{
	return read(outByte);
}

InBufferStream& InBufferStream::operator>>(std::int64_t& outDword) noexcept
{
	return read(outDword);
}

InBufferStream& InBufferStream::operator>>(std::int32_t& outWord) noexcept
{
	return read(outWord);
}

InBufferStream& InBufferStream::operator>>(std::int16_t& outShort) noexcept
{
	return read(outShort);
}

InBufferStream& InBufferStream::operator>>(std::int8_t& outByte) noexcept
{
	return read(outByte);
}

InBufferStream& InBufferStream::operator>>(std::float_t& outFloat) noexcept
{
	return read(outFloat);
}

InBufferStream& InBufferStream::operator>>(std::double_t& outDouble) noexcept
{
	return read(outDouble);
}

InBufferStream& InBufferStream::operator>>(std::string& outString) noexcept
{
	return read(outString);
}
