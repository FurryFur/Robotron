#pragma once

#include <vector>

class OutBufferStream;

// A vector with a fixed size that returns false when trying to add past 
// the end of the vector. The vector is streamable to an OutBufferStream for 
// easy serialization.
// T must overload the stream operator for OutBufferStream types.
// i.e. An overload must exist for operator<<(OutBufferStream&, const T&)
template<typename T, size_t maxSize>
class StreamableVector {
public:
	// Pushes a value onto the end of the vector.
	// Returns false if the adding the element fails due to the vector
	// being full.
	bool push_back(const T& val);

	// Builds the object in-place, at the end of the vector
	// using the supplied arguments.
	// Returns false if adding the element fails due to the vector being full.
	template<typename ...Args>
	bool emplace_back(Args&&...);

	// Returns the item at the end of the vector
	T& back();

	// Returns a referece to the item at position i
	T& at(size_t i);

	// Clears the vector
	void clear();

	// Returns the current size of the vector. This is garanteed
	// to be less than the max size.
	size_t size();

	// Retuns the maximum size of the vector
	size_t getMaxSize();

	// Serializes the vector to an OutBufferStream object
	OutBufferStream& serialize(OutBufferStream&) const;
private:
	std::vector<T> m_vector;
};

template<typename T, size_t maxSize>
inline bool StreamableVector<T, maxSize>::push_back(const T& val)
{
	if (m_vector.size() >= maxSize)
		return false;

	m_vector.push_back(val);
	return true;
}

template<typename T, size_t maxSize>
template<typename ...Args>
inline bool StreamableVector<T, maxSize>::emplace_back(Args&&... args)
{
	if (m_vector.size() >= maxSize)
		return false;

	m_vector.emplace_back(std::forward<Args>(args)...);
	return true;
}


template<typename T, size_t maxSize>
inline T& StreamableVector<T, maxSize>::back()
{
	return m_vector.back();
}

template<typename T, size_t maxSize>
inline T& StreamableVector<T, maxSize>::at(size_t i)
{
	return m_vector.at(i);
}

template<typename T, size_t maxSize>
inline void StreamableVector<T, maxSize>::clear()
{
	m_vector.clear();
}

template<typename T, size_t maxSize>
inline size_t StreamableVector<T, maxSize>::size()
{
	return m_vector.size();
}

template<typename T, size_t maxSize>
inline size_t StreamableVector<T, maxSize>::getMaxSize()
{
	return maxSize;
}

template<typename T, size_t maxSize>
inline OutBufferStream& StreamableVector<T, maxSize>::serialize(OutBufferStream& obs) const
{
	for (const T& val : m_vector)
		obs << val;

	return obs;
}

template<typename T, size_t maxSize>
inline OutBufferStream& operator<<(OutBufferStream& obs,
	const StreamableVector<T, maxSize>& vector)
{
	return vector.serialize(obs);
}