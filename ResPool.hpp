#pragma once
#include <queue>
#include <mutex>
#include <memory>

template<typename T>
class ResPool
{
public:
	ResPool(const ResPool&) = delete;
	ResPool& operator= (const ResPool&) = delete;

	ResPool(ResPool&&) = default;
	ResPool& operator= (ResPool&&) = default;

	~ResPool() = default;

public:
	ResPool(std::function<T*()> factoryfunc, size_t size) {
		m_factory = factoryfunc;
		m_size = size;
	}

	std::unique_ptr<T> Acquire() {
		{
			std::lock_guard<std::mutex> lock(m_mtx);
			if (m_resources.size()) {
				std::unique_ptr<T> r = std::move(m_resources.front());
				m_resources.pop();
				return r;
			}
		}
		return std::unique_ptr<T>(m_factory());
	}

	void Release(std::unique_ptr<T>& r) {
		{
			std::lock_guard<std::mutex> lock(m_mtx);
			if (m_resources.size() < m_size) {
				m_resources.push(std::move(r));
				return;
			}
		}
		r.reset();
	}

private:
	std::queue<std::unique_ptr<T>> m_resources;
	std::mutex m_mtx;
	std::function<T*()> m_factory;
	size_t m_size;
};
