#pragma once

#include <deque>

class OutBufferStream;

// T must be a streamable type.
// i.e. An overload must exist for operator<<(OutBufferStream&, const T&)
template<typename T, size_t maxSize>
class StreamableQueue {
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

	T& front();
	T& at(size_t i);
	size_t size();
	size_t getMaxSize();

	OutBufferStream& serialize(OutBufferStream&) const;

	friend OutBufferStream& operator<<(OutBufferStream&,
		const RemoteProcedureCall&);
private:
	std::deque<T> m_queue;
};

template<typename T, size_t maxSize>
inline void StreamableQueue<T, maxSize>::push(const T& val)
{
	m_queue.push_front(val);
	if (m_queue.size() > maxSize)
		m_queue.pop_back();
}

template<typename T, size_t maxSize>
inline T& StreamableQueue<T, maxSize>::front()
{
	return m_queue.front();
}

template<typename T, size_t maxSize>
inline T& StreamableQueue<T, maxSize>::at(size_t i)
{
	return m_queue.at(i);
}

template<typename T, size_t maxSize>
inline size_t StreamableQueue<T, maxSize>::size()
{
	return m_queue.size();
}

template<typename T, size_t maxSize>
inline size_t StreamableQueue<T, maxSize>::getMaxSize()
{
	return maxSize;
}

template<typename T, size_t maxSize>
inline OutBufferStream& StreamableQueue<T, maxSize>::serialize(OutBufferStream& obs) const
{
	for (const T& val : m_queue)
		obs << val;

	return obs;
}

template<typename T, size_t maxSize>
inline OutBufferStream& operator<<(OutBufferStream& obs, 
	const StreamableQueue<T, maxSize>& queue)
{
	return queue.serialize(obs);
}

template<typename T, size_t maxSize>
template<typename ...Args>
inline void StreamableQueue<T, maxSize>::emplace(Args&&... args)
{
	m_queue.emplace_front(std::forward<Args>(args)...);
	if (m_queue.size() > maxSize)
		m_queue.pop_back();
}
