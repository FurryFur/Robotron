#include "BufferStream.h"

void OutBufferStream::write(std::uint64_t dword) noexcept
{
	if (m_writeHeadIdx + sizeof(dword) > m_data.size())
		m_data.resize(m_writeHeadIdx + sizeof(dword));

	*reinterpret_cast<std::uint64_t*>(&m_data[m_writeHeadIdx]) = dword;

	m_writeHeadIdx += sizeof(dword);
}

void OutBufferStream::write(std::uint32_t word) noexcept
{
	if (m_writeHeadIdx + sizeof(word) > m_data.size())
		m_data.resize(m_writeHeadIdx + sizeof(word));

	*reinterpret_cast<std::uint32_t*>(&m_data[m_writeHeadIdx]) = word;

	m_writeHeadIdx += sizeof(word);
}

void OutBufferStream::write(std::uint16_t _short) noexcept
{
	if (m_writeHeadIdx + sizeof(_short) > m_data.size())
		m_data.resize(m_writeHeadIdx + sizeof(_short));

	*reinterpret_cast<std::uint16_t*>(&m_data[m_writeHeadIdx]) = _short;

	m_writeHeadIdx += sizeof(_short);
}

void OutBufferStream::write(std::uint8_t byte) noexcept
{
	if (m_writeHeadIdx + sizeof(byte) > m_data.size())
		m_data.resize(m_writeHeadIdx + sizeof(byte));

	*reinterpret_cast<std::uint8_t*>(&m_data[m_writeHeadIdx]) = byte;

	m_writeHeadIdx += sizeof(byte);
}

void OutBufferStream::write(std::int64_t dword) noexcept
{
	write(*reinterpret_cast<std::uint64_t*>(&dword));
}

void OutBufferStream::write(std::int32_t word) noexcept
{
	write(*reinterpret_cast<std::uint32_t*>(&word));
}

void OutBufferStream::write(std::int16_t _short) noexcept
{
	write(*reinterpret_cast<std::uint16_t*>(&_short));
}

void OutBufferStream::write(std::int8_t byte) noexcept
{
	write(*reinterpret_cast<std::uint8_t*>(&byte));
}

void OutBufferStream::write(std::float_t _float) noexcept
{
	write(*reinterpret_cast<std::uint32_t*>(&_float));
}

void OutBufferStream::write(std::double_t _double) noexcept
{
	write(*reinterpret_cast<std::uint64_t*>(&_double));
}

void OutBufferStream::write(const std::string& str) noexcept
{
	size_t chunkSize = str.length() + 1;
	if (m_writeHeadIdx + chunkSize > m_data.size())
		m_data.resize(m_writeHeadIdx + chunkSize);

	std::memcpy(&m_data[m_writeHeadIdx], str.c_str(), chunkSize);

	m_writeHeadIdx += chunkSize;
}

void OutBufferStream::reset() noexcept
{
	m_data.clear();
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
	write(dword);
	return *this;
}

OutBufferStream& OutBufferStream::operator<<(std::uint32_t word) noexcept
{
	write(word);
	return *this;
}

OutBufferStream& OutBufferStream::operator<<(std::uint16_t _short) noexcept
{
	write(_short);
	return *this;
}

OutBufferStream& OutBufferStream::operator<<(std::uint8_t byte) noexcept
{
	write(byte);
	return *this;
}

OutBufferStream& OutBufferStream::operator<<(std::int64_t dword) noexcept
{
	write(dword);
	return *this;
}

OutBufferStream& OutBufferStream::operator<<(std::int32_t word) noexcept
{
	write(word);
	return *this;
}

OutBufferStream& OutBufferStream::operator<<(std::int16_t _short) noexcept
{
	write(_short);
	return *this;
}

OutBufferStream& OutBufferStream::operator<<(std::int8_t byte) noexcept
{
	write(byte);
	return *this;
}

OutBufferStream & OutBufferStream::operator<<(std::float_t _float) noexcept
{
	write(_float);
	return *this;
}

OutBufferStream & OutBufferStream::operator<<(std::double_t _double) noexcept
{
	write(_double);
	return *this;
}

OutBufferStream& OutBufferStream::operator<<(const std::string& str) noexcept
{
	write(str);
	return *this;
}
