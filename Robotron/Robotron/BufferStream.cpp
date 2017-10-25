#include "BufferStream.h"

OutBufferStream& OutBufferStream::write(std::uint64_t dword) noexcept
{
	if (m_writeHeadIdx + sizeof(dword) > m_data.size())
		m_data.resize(m_writeHeadIdx + sizeof(dword));

	*reinterpret_cast<std::uint64_t*>(&m_data[m_writeHeadIdx]) = dword;

	m_writeHeadIdx += sizeof(dword);
	return *this;
}

OutBufferStream& OutBufferStream::write(std::uint32_t word) noexcept
{
	if (m_writeHeadIdx + sizeof(word) > m_data.size())
		m_data.resize(m_writeHeadIdx + sizeof(word));

	*reinterpret_cast<std::uint32_t*>(&m_data[m_writeHeadIdx]) = word;

	m_writeHeadIdx += sizeof(word);
	return *this;
}

OutBufferStream& OutBufferStream::write(std::uint16_t _short) noexcept
{
	if (m_writeHeadIdx + sizeof(_short) > m_data.size())
		m_data.resize(m_writeHeadIdx + sizeof(_short));

	*reinterpret_cast<std::uint16_t*>(&m_data[m_writeHeadIdx]) = _short;

	m_writeHeadIdx += sizeof(_short);
	return *this;
}

OutBufferStream& OutBufferStream::write(std::uint8_t byte) noexcept
{
	if (m_writeHeadIdx + sizeof(byte) > m_data.size())
		m_data.resize(m_writeHeadIdx + sizeof(byte));

	*reinterpret_cast<std::uint8_t*>(&m_data[m_writeHeadIdx]) = byte;

	m_writeHeadIdx += sizeof(byte);
	return *this;
}

OutBufferStream& OutBufferStream::write(std::int64_t dword) noexcept
{
	return write(*reinterpret_cast<std::uint64_t*>(&dword));
}

OutBufferStream& OutBufferStream::write(std::int32_t word) noexcept
{
	return write(*reinterpret_cast<std::uint32_t*>(&word));
}

OutBufferStream& OutBufferStream::write(std::int16_t _short) noexcept
{
	return write(*reinterpret_cast<std::uint16_t*>(&_short));
}

OutBufferStream& OutBufferStream::write(std::int8_t byte) noexcept
{
	return write(*reinterpret_cast<std::uint8_t*>(&byte));
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

InBufferStream::InBufferStream(const std::vector<char>& data) noexcept
{

}

InBufferStream& InBufferStream::read(std::uint64_t& outDword) noexcept
{
	outDword = *reinterpret_cast<const std::uint64_t*>(&m_data[m_readHeadIdx]);
	m_readHeadIdx += sizeof(outDword);
	return *this;
}

InBufferStream& InBufferStream::read(std::uint32_t& outWord) noexcept
{
	outWord = *reinterpret_cast<const std::uint32_t*>(&m_data[m_readHeadIdx]);
	m_readHeadIdx += sizeof(outWord);
	return *this;
}

InBufferStream& InBufferStream::read(std::uint16_t& outShort) noexcept
{
	outShort = *reinterpret_cast<const std::uint16_t*>(&m_data[m_readHeadIdx]);
	m_readHeadIdx += sizeof(outShort);
	return *this;
}

InBufferStream& InBufferStream::read(std::uint8_t& outByte) noexcept
{
	outByte = *reinterpret_cast<const std::uint8_t*>(&m_data[m_readHeadIdx]);
	m_readHeadIdx += sizeof(outByte);
	return *this;
}

InBufferStream& InBufferStream::read(std::int64_t& outDword) noexcept
{
	outDword = *reinterpret_cast<const std::int64_t*>(&m_data[m_readHeadIdx]);
	m_readHeadIdx += sizeof(outDword);
	return *this;
}

InBufferStream& InBufferStream::read(std::int32_t& outWord) noexcept
{
	outWord = *reinterpret_cast<const std::int32_t*>(&m_data[m_readHeadIdx]);
	m_readHeadIdx += sizeof(outWord);
	return *this;
}

InBufferStream& InBufferStream::read(std::int16_t& outShort) noexcept
{
	outShort = *reinterpret_cast<const std::int16_t*>(&m_data[m_readHeadIdx]);
	m_readHeadIdx += sizeof(outShort);
	return *this;
}

InBufferStream& InBufferStream::read(std::int8_t& outByte) noexcept
{
	outByte = *reinterpret_cast<const std::int8_t*>(&m_data[m_readHeadIdx]);
	m_readHeadIdx += sizeof(outByte);
	return *this;
}

InBufferStream& InBufferStream::read(std::float_t& outFloat) noexcept
{
	outFloat = *reinterpret_cast<const std::float_t*>(&m_data[m_readHeadIdx]);
	m_readHeadIdx += sizeof(outFloat);
	return *this;
}

InBufferStream& InBufferStream::read(std::double_t& outDouble) noexcept
{
	outDouble = *reinterpret_cast<const std::double_t*>(&m_data[m_readHeadIdx]);
	m_readHeadIdx += sizeof(outDouble);
	return *this;
}

InBufferStream& InBufferStream::read(std::string& outString) noexcept
{
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
