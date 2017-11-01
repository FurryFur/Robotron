#pragma once

#include "BufferStream.h"

#include <deque>

// A queue with a fixed size that evicts its oldest elements when full
// to make room for new elements. The queue is streamable to an 
// OutBufferStream for easy serialization.
// T must overload the stream operator for OutBufferStream types.
// i.e. An overload must exist for operator<<(OutBufferStream&, const T&)
template<typename T, std::uint16_t maxSize>
class StreamableEvictQueue {
public:
	// Pushes a value onto the front of the queue.
	// When the queue becomes full, the oldest elements
	// are poped off the back to make room.
	void push(const T& val);

	// Builds the object in-place, at the front of the queue
	// using the supplied arguments.
	// When the queue becomes full, the oldest elements
	// are poped off the back to make room.
	template<typename ...Args>
	void emplace(Args&&...);

	// Returns the newest item on the queue
	T& front();

	// Returns the item at position i
	T& at(std::uint16_t i);

	// Returns the item at position i
	const T& at(std::uint16_t i) const;

	// Clears the queue
	void clear();

	// Returns the current size of the queue. This is garanteed
	// to be less than the max size.
	std::uint16_t size() const;

	// Retuns the maximum size of the queue
	std::uint16_t getMaxSize() const;

	// Serializes the queue to an OutBufferStream object
	OutBufferStream& serialize(OutBufferStream&) const;
	InBufferStream& deserialize(InBufferStream&);
private:
	std::deque<T> m_queue;
};

template<typename T, std::uint16_t maxSize>
inline void StreamableEvictQueue<T, maxSize>::push(const T& val)
{
	m_queue.push_front(val);
	if (m_queue.size() > maxSize)
		m_queue.pop_back();
}

template<typename T, std::uint16_t maxSize>
inline T& StreamableEvictQueue<T, maxSize>::front()
{
	return m_queue.front();
}

template<typename T, std::uint16_t maxSize>
inline T& StreamableEvictQueue<T, maxSize>::at(std::uint16_t i)
{
	return m_queue.at(i);
}

template<typename T, std::uint16_t maxSize>
inline const T & StreamableEvictQueue<T, maxSize>::at(std::uint16_t i) const
{
	return m_queue.at(i);
}

template<typename T, std::uint16_t maxSize>
inline void StreamableEvictQueue<T, maxSize>::clear()
{
	m_queue.clear();
}

template<typename T, std::uint16_t maxSize>
inline std::uint16_t StreamableEvictQueue<T, maxSize>::size() const
{
	return static_cast<std::uint16_t>(m_queue.size());
}

template<typename T, std::uint16_t maxSize>
inline std::uint16_t StreamableEvictQueue<T, maxSize>::getMaxSize() const
{
	return maxSize;
}

template<typename T, std::uint16_t maxSize>
inline OutBufferStream& StreamableEvictQueue<T, maxSize>::serialize(OutBufferStream& obs) const
{
	obs << size();
	for (const T& val : m_queue)
		obs << val;

	return obs;
}

template<typename T, std::uint16_t maxSize>
inline InBufferStream& StreamableEvictQueue<T, maxSize>::deserialize(InBufferStream& ibs)
{
	if (ibs.getError())
		return ibs;

	std::uint16_t size;
	ibs >> size;
	if (size > maxSize) {
		ibs.setError(IBS_ERROR_CORRUPT_DATA);
		return ibs;
	}

	m_queue.resize(size);
	for (T& val : m_queue)
		ibs >> val;

	return ibs;
}

template<typename T, std::uint16_t maxSize>
inline OutBufferStream& operator<<(OutBufferStream& obs, const StreamableEvictQueue<T, maxSize>& queue)
{
	return queue.serialize(obs);
}

template<typename T, std::uint16_t maxSize>
inline InBufferStream& operator>>(InBufferStream& ibs, StreamableEvictQueue<T, maxSize>& queue)
{
	return queue.deserialize(ibs);
}

template<typename T, std::uint16_t maxSize>
template<typename ...Args>
inline void StreamableEvictQueue<T, maxSize>::emplace(Args&&... args)
{
	m_queue.emplace_front(std::forward<Args>(args)...);
	if (m_queue.size() > maxSize)
		m_queue.pop_back();
}
