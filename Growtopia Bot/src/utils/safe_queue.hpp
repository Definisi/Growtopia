#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>

template<typename T>
class SafeQueue {
private:
	std::queue<T> m_queue;
	std::mutex m_mutex;
	std::condition_variable m_condition;
public:
	void clear() {
		std::unique_lock<std::mutex> lock(m_mutex);

		while (!m_queue.empty()) {
			m_queue.pop();
		}
	}

	void push(T item) {
		std::unique_lock<std::mutex> lock(m_mutex);

		m_queue.push(item);
		m_condition.notify_one();
	}

	T pop() {
		std::unique_lock<std::mutex> lock(m_mutex);

		m_condition.wait(lock, [this]() {
				return !m_queue.empty();
			});

		T item = m_queue.front();
		m_queue.pop();

		return item;
	}

	const bool& empty() {
		std::unique_lock<std::mutex> lock(m_mutex);

		return m_queue.empty();
	}
};