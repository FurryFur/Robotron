#pragma once

#include <vector>
#include <string>
#include <algorithm>

class OutBufferStream {
public:
	OutBufferStream();

	OutBufferStream& write(std::uint64_t) noexcept;
	OutBufferStream& write(std::uint32_t) noexcept;
	OutBufferStream& write(std::uint16_t) noexcept;
	OutBufferStream& write(std::uint8_t) noexcept;
	OutBufferStream& write(std::int64_t) noexcept;
	OutBufferStream& write(std::int32_t) noexcept;
	OutBufferStream& write(std::int16_t) noexcept;
	OutBufferStream& write(std::int8_t) noexcept;
	OutBufferStream& write(std::float_t) noexcept;
	OutBufferStream& write(std::double_t) noexcept;
	OutBufferStream& write(const std::string&) noexcept;
	template<typename T>
	OutBufferStream& write(const T* _array, size_t length) noexcept;

	void reset() noexcept;
	const char* getData();

	// Returns the size of the buffer in bytes
	size_t getBytesWritten();

	OutBufferStream& operator<<(std::uint64_t) noexcept;
	OutBufferStream& operator<<(std::uint32_t) noexcept;
	OutBufferStream& operator<<(std::uint16_t) noexcept;
	OutBufferStream& operator<<(std::uint8_t) noexcept;
	OutBufferStream& operator<<(std::int64_t) noexcept;
	OutBufferStream& operator<<(std::int32_t) noexcept;
	OutBufferStream& operator<<(std::int16_t) noexcept;
	OutBufferStream& operator<<(std::int8_t) noexcept;
	OutBufferStream& operator<<(std::float_t) noexcept;
	OutBufferStream& operator<<(std::double_t) noexcept;
	OutBufferStream& operator<<(const std::string&) noexcept;

private:
	std::vector<char> m_data;
	size_t m_writeHeadIdx;
};

template<typename T>
inline OutBufferStream& OutBufferStream::write(const T* _array, size_t length) noexcept
{
	size_t chunkSize = length * sizeof(T);
	if (m_writeHeadIdx + chunkSize > m_data.size())
		m_data.resize(m_writeHeadIdx + chunkSize);

	for (size_t i = 0; i < length; ++i)
		write(_array[i]);

	return *this;
}

enum IBSError {
	IBS_ERROR_NONE,
	IBS_ERROR_CORRUPT_DATA,
	IBS_ERROR_WOULD_READ_PAST_END
};

class InBufferStream {
public:
	InBufferStream(const std::vector<char>& buffer) noexcept;

	IBSError checkBufferOverrun(size_t nextReadHeadIdx);
	IBSError checkBufferOverrunStr();
	IBSError getError();
	void setError(IBSError);

	InBufferStream& read(std::uint64_t&) noexcept;
	InBufferStream& read(std::uint32_t&) noexcept;
	InBufferStream& read(std::uint16_t&) noexcept;
	InBufferStream& read(std::uint8_t&) noexcept;
	InBufferStream& read(std::int64_t&) noexcept;
	InBufferStream& read(std::int32_t&) noexcept;
	InBufferStream& read(std::int16_t&) noexcept;
	InBufferStream& read(std::int8_t&) noexcept;
	InBufferStream& read(std::float_t&) noexcept;
	InBufferStream& read(std::double_t&) noexcept;
	InBufferStream& read(std::string&) noexcept;
	template<typename T>
	InBufferStream& read(T* dstArray, size_t length) noexcept;

	void reset() noexcept;

	// Returns the size of the buffer in bytes
	size_t getBytesRead();

	InBufferStream& operator>>(std::uint64_t&) noexcept;
	InBufferStream& operator>>(std::uint32_t&) noexcept;
	InBufferStream& operator>>(std::uint16_t&) noexcept;
	InBufferStream& operator>>(std::uint8_t&) noexcept;
	InBufferStream& operator>>(std::int64_t&) noexcept;
	InBufferStream& operator>>(std::int32_t&) noexcept;
	InBufferStream& operator>>(std::int16_t&) noexcept;
	InBufferStream& operator>>(std::int8_t&) noexcept;
	InBufferStream& operator>>(std::float_t&) noexcept;
	InBufferStream& operator>>(std::double_t&) noexcept;
	InBufferStream& operator>>(std::string&) noexcept;

private:
	const std::vector<char>& m_data;
	size_t m_readHeadIdx;
	IBSError m_error;
};

template<typename T>
inline InBufferStream& InBufferStream::read(T* dstArray, size_t length) noexcept
{
	for (size_t i = 0; i < length; ++i)
		read(dstArray[i]);

	return *this;
}
