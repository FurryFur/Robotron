#pragma once

#include <deque>

class OutBufferStream;

// A queue with a fixed size that evicts its oldest elements when full
// to make room for new elements. The queue is streamable to an 
// OutBufferStream for easy serialization.
// T must overload the stream operator for OutBufferStream types.
// i.e. An overload must exist for operator<<(OutBufferStream&, const T&)
template<typename T, size_t maxSize>
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
	T& at(size_t i);

	// Clears the queue
	void clear();

	// Returns the current size of the queue. This is garanteed
	// to be less than the max size.
	size_t size();

	// Retuns the maximum size of the queue
	size_t getMaxSize();

	// Serializes the queue to an OutBufferStream object
	OutBufferStream& serialize(OutBufferStream&) const;
private:
	std::deque<T> m_queue;
};

template<typename T, size_t maxSize>
inline void StreamableEvictQueue<T, maxSize>::push(const T& val)
{
	m_queue.push_front(val);
	if (m_queue.size() > maxSize)
		m_queue.pop_back();
}

template<typename T, size_t maxSize>
inline T& StreamableEvictQueue<T, maxSize>::front()
{
	return m_queue.front();
}

template<typename T, size_t maxSize>
inline T& StreamableEvictQueue<T, maxSize>::at(size_t i)
{
	return m_queue.at(i);
}

template<typename T, size_t maxSize>
inline void StreamableEvictQueue<T, maxSize>::clear()
{
	m_queue.clear();
}

template<typename T, size_t maxSize>
inline size_t StreamableEvictQueue<T, maxSize>::size()
{
	return m_queue.size();
}

template<typename T, size_t maxSize>
inline size_t StreamableEvictQueue<T, maxSize>::getMaxSize()
{
	return maxSize;
}

template<typename T, size_t maxSize>
inline OutBufferStream& StreamableEvictQueue<T, maxSize>::serialize(OutBufferStream& obs) const
{
	for (const T& val : m_queue)
		obs << val;

	return obs;
}

template<typename T, size_t maxSize>
inline OutBufferStream& operator<<(OutBufferStream& obs, 
	const StreamableEvictQueue<T, maxSize>& queue)
{
	return queue.serialize(obs);
}

template<typename T, size_t maxSize>
template<typename ...Args>
inline void StreamableEvictQueue<T, maxSize>::emplace(Args&&... args)
{
	m_queue.emplace_front(std::forward<Args>(args)...);
	if (m_queue.size() > maxSize)
		m_queue.pop_back();
}
