#pragma once

#include <vector>
#include <string>
#include <algorithm>

class OutBufferStream {
public:
	void write(std::uint64_t) noexcept;
	void write(std::uint32_t) noexcept;
	void write(std::uint16_t) noexcept;
	void write(std::uint8_t) noexcept;
	void write(std::int64_t) noexcept;
	void write(std::int32_t) noexcept;
	void write(std::int16_t) noexcept;
	void write(std::int8_t) noexcept;
	void write(std::float_t) noexcept;
	void write(std::double_t) noexcept;
	void write(const std::string&) noexcept;

	template<typename T>
	void write(const T* _array, size_t length) noexcept;

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
inline void OutBufferStream::write(const T* _array, size_t length) noexcept
{
	size_t chunkSize = length * sizeof(T);
	if (m_writeHeadIdx + chunkSize > m_data.size())
		m_data.resize(m_writeHeadIdx + chunkSize);

	for (size_t i = 0; i < length; ++i)
		write(_array[i]);
}
