#pragma once 
#include <assert.h>
#include <mutex>
#include <vector>
#include "tbb/spin_mutex.h"

template <size_t FixedSizeBytes>
struct tmp_vector_pool
{
	tmp_vector_pool()
		: 
		m_alloc_one_calls(0),
		m_free_one_calls(0)
	{}

	~tmp_vector_pool()
	{
		for (auto ptr : string_pool)
		{
			free(ptr);
		}
	}

	char *AllocOne()
	{
		char *ret;
		pool_mutex.lock();
		m_alloc_one_calls++;

		if (string_pool.empty())
		{
			ret = (char *)malloc(FixedSizeBytes);
			assert(ret);
		}
		else
		{
			ret = string_pool.back();
			string_pool.pop_back();
		}
		pool_mutex.unlock();

		return ret;
	}

	void FreeOne(char *s)
	{
		pool_mutex.lock();
		m_free_one_calls++;
#ifdef _DEBUG
		for (int i = 0; i < size_as_int(string_pool.size()); i++)
		{
			// make sure duplicates don't appear back in the string pool
			assert(string_pool[i] != (char *)s);
		}
#endif
		string_pool.push_back((char *)s);
		pool_mutex.unlock();
	}

	int get_num_alloc_one_calls() const {
		return m_alloc_one_calls;
	}
	int get_num_free_one_calls() const {
		return m_free_one_calls;
	}
private:
	int m_alloc_one_calls;
	int m_free_one_calls;
	tbb::spin_mutex pool_mutex;
	std::vector<char *> string_pool;
};

template <typename T, size_t FixedSizeBytes>
struct tmp_vector2
{
	typedef T value_type;
	typedef size_t size_type;
private:
	T *m_s;
	tmp_vector_pool<FixedSizeBytes> *m_pool;
	size_type m_count;

public:
	tmp_vector2()
		: m_pool(nullptr),
		m_count(0),
		m_s(0)
	{}

	explicit tmp_vector2(tmp_vector_pool<FixedSizeBytes> *pool)
		:
		m_pool(pool),
		m_count(0)
	{
		m_s = (T*)m_pool->AllocOne();
	}
	~tmp_vector2()
	{
		if (m_s)
		{
			m_pool->FreeOne((char *)m_s);
		}
	}

	tmp_vector2(tmp_vector2 &&rhs)
		:
		m_s(rhs.m_s),
		m_pool(rhs.m_pool),
		m_count(rhs.m_count)
	{
		rhs.m_s = nullptr;
	}

	tmp_vector2 & operator = (tmp_vector2 &&rhs)
	{
		m_s = rhs.m_s;
		m_pool = rhs.m_pool;
		m_count = rhs.m_count;
		rhs.m_s = nullptr;
		return *this;
	}

	tmp_vector2(const tmp_vector2 &rhs) = delete;// dangerous - do I want to have copies of tempoaries
	tmp_vector2& operator=(const tmp_vector2&) = delete;

	T * data() { return m_s; }
	const T * data() const { return m_s; }

	const T * begin() const { return m_s; }
	const T * end() const { return m_s + m_count; }

	T * first() { return m_s; }
	T * last() { return m_s + m_count; }

	const T * first() const { return m_s; }
	const T * last() const { return m_s + m_count; }

	size_type max_size() const { return FixedSizeBytes / sizeof(T); }
	size_type size() const { return m_count; }
	void resize(size_type count) { m_count = count; }
	void clear() { m_count = 0; }

	T & operator[] (int pos) { return m_s[pos]; }
	const T & operator[] (int pos) const { return m_s[pos]; }

	void push_back(const T&ref)
	{
		m_s[m_count++] = ref;
	}
};


