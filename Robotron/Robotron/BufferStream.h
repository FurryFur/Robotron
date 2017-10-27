#pragma once

#include <vector>
#include <string>
#include <algorithm>

class OutBufferStream {
public:
	OutBufferStream();

	template<typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
	OutBufferStream& write(T) noexcept;
	OutBufferStream& write(const std::string&) noexcept;
	template<typename T>
	OutBufferStream& write(const T* _array, size_t length) noexcept;

	void reset() noexcept;
	const char* getData();

	// Returns the size of the buffer in bytes
	size_t getBytesWritten();

	template<typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
	OutBufferStream& operator<<(T) noexcept;
	OutBufferStream& operator<<(const std::string&) noexcept;

private:
	std::vector<char> m_data;
	size_t m_writeHeadIdx;
};

template<typename T, typename>
inline OutBufferStream& OutBufferStream::write(T val) noexcept
{
	if (m_writeHeadIdx + sizeof(T) > m_data.size())
		m_data.resize(m_writeHeadIdx + sizeof(T));

	*reinterpret_cast<T*>(&m_data[m_writeHeadIdx]) = val;

	m_writeHeadIdx += sizeof(T);
	return *this;
}

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

template<typename T, typename>
inline OutBufferStream & OutBufferStream::operator<<(T val) noexcept
{
	return write(val);
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

	template<typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
	InBufferStream& read(T&) noexcept;
	InBufferStream& read(std::string&) noexcept;
	template<typename T>
	InBufferStream& read(T* dstArray, size_t length) noexcept;

	void reset() noexcept;

	// Returns the size of the buffer in bytes
	size_t getBytesRead();

	template<typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
	InBufferStream& operator>>(T&) noexcept;
	InBufferStream& operator>>(std::string&) noexcept;

private:
	const std::vector<char>& m_data;
	size_t m_readHeadIdx;
	IBSError m_error;
};

template<typename T, typename>
inline InBufferStream & InBufferStream::read(T& outVal) noexcept
{
	size_t nextReadHeadIdx = m_readHeadIdx + sizeof(T);
	if (checkBufferOverrun(nextReadHeadIdx) != IBS_ERROR_NONE)
		return *this;

	outVal = *reinterpret_cast<const T*>(&m_data[m_readHeadIdx]);
	m_readHeadIdx = nextReadHeadIdx;
	return *this;
}

template<typename T>
inline InBufferStream& InBufferStream::read(T* dstArray, size_t length) noexcept
{
	for (size_t i = 0; i < length; ++i)
		read(dstArray[i]);

	return *this;
}

template<typename T, typename>
inline InBufferStream& InBufferStream::operator>>(T& outVal) noexcept
{
	return read(outVal);
}
