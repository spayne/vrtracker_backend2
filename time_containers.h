#pragma once

#include "url_named.h"
#include "range.h"
#include "range_algorithm.h"
#include "base_serialization.h"

template <typename T>
struct time_indexed
{
	time_indexed() {}

	template<typename... Args>
	explicit time_indexed(time_index_t t, Args&&... args)
		: time_index(t),
		value(std::forward<Args>(args)...)
	{}

	bool operator == (const time_indexed &rhs) const
	{
		if (this == &rhs)
			return true;
		return (time_index == rhs.time_index) && (value == rhs.value);
	}

	bool operator != (const time_indexed &rhs) const
	{
		return !(*this == rhs);
	}

	const T& get_value() const { return value; }
	time_index_t get_time_index() const { return time_index; }

	void set_value(const T&v) { value = v; }
	void set_time_index(time_index_t t) { time_index = t; }

	// write just the value out to the stream
	void encode(BaseStream &e) const
	{
		e.write_to_stream(&time_index, sizeof(time_index));
		value.encode(e);
	}

	void decode(BaseStream &e) 
	{
		e.read_from_stream(&time_index, sizeof(time_index));
		value.decode(e);
	}

private:
	time_index_t time_index;
	T value;
};




// time_indexed_vector:  a container of items wrapped in time_indexed<T>
//                       a pretty thin layer
template <typename T,
	template <typename, typename> class Container,
	template <typename> class A>
	struct time_indexed_vector : public base::url_named
{
	typedef T									value_type;
	typedef time_indexed<T>						time_indexed_type;
	typedef Container<time_indexed_type, A<time_indexed_type>>	container_type_t;
	typedef typename container_type_t::iterator iterator;

	time_indexed_vector()
		: container(A<time_indexed_type>())
	{}

	time_indexed_vector(const time_indexed_vector &rhs)
		: 
		base::url_named(rhs),
		container(rhs.container)
	{}


	template<typename... Args>
	explicit time_indexed_vector(const base::URL &url, Args&&... args)
		:
		url_named(url),
		container(std::forward<Args>(args)...)
	{}

	bool empty() const { return container.empty(); }
	size_t size() const { return container.size(); }

	const T& operator()() const { return container.back().get_value(); }
	const time_indexed<T>& latest() const { return container.back(); }
	const time_indexed<T>& earliest() const { return container.front(); }

	// [start and end)  (half open range)
	std::range<iterator> get_range(time_index_t a, time_index_t b)
	{
		return range_intersect(get_range(), time_indexed_type(a), time_indexed_type(b),
			[](const time_indexed_type &a, const time_indexed_type &b) { return a.get_time_index() < b.get_time_index(); });
	}

	std::range<iterator> get_range()
	{
		return std::range<iterator>(container.begin(), container.end());
	}

	iterator end()
	{
		return container.end();
	}

	iterator last_item_less_than_or_equal_to_time(time_index_t a)
	{
		return last_item_less_than_or_equal_to(container.begin(), container.end(), time_indexed_type(a),
			[](const time_indexed_type &a, const time_indexed_type &b) { return a.get_time_index() < b.get_time_index(); });
	}

	// check the hint iterator first before searching for it
	iterator last_item_less_than_or_equal_to_time(time_index_t a, iterator &hint_iterator)
	{
		if (hint_iterator != end())
		{
			time_index_t hint_index = hint_iterator->get_time_index();
			if (hint_index == a)
			{
				return hint_iterator;
			}
			else if (hint_index < a)
			{
				return last_item_less_than_or_equal_to(hint_iterator, container.end(), time_indexed_type(a),
					[](const time_indexed_type &a, const time_indexed_type &b) { return a.get_time_index() < b.get_time_index(); });
			}
			else // hint index > a
			{
				return last_item_less_than_or_equal_to(container.begin(), hint_iterator, time_indexed_type(a),
					[](const time_indexed_type &a, const time_indexed_type &b) { return a.get_time_index() < b.get_time_index(); });
			}
		}

		return last_item_less_than_or_equal_to(container.begin(), container.end(), time_indexed_type(a),
			[](const time_indexed_type &a, const time_indexed_type &b) { return a.get_time_index() < b.get_time_index(); });
	}


	template<typename... Args>
	void emplace_back(time_index_t time_index, Args&&... args)
	{
		container.emplace_back(time_index, std::forward<Args>(args)...);
	}

	void push_back(const time_indexed_type &val)
	{
		container.push_back(val);
	}

	// write just the value out to the stream
	void encode(BaseStream &e) const 
	{
		base::url_named::encode(e);
		int size = size_as_int(container.size());
		e.write_to_stream(&size, sizeof(size)); // write the container size
		
		for (const auto &timeval: container)
		{
			timeval.encode(e);
		}
	}

	// read the value from the stream
	void decode(BaseStream &e) 
	{
		base::url_named::decode(e);

		container.clear();
		int size;
		e.read_from_stream(&size, sizeof(size));
		assert(size >= 0);
		container.reserve(size);
		for (int i = 0; i < size; i++)
		{
			time_indexed_type time_val;
			time_val.decode(e);
			container.push_back(time_val);
		}
	}

	bool operator==(const time_indexed_vector &rhs) const
	{
		if (!base::url_named::operator==(rhs))
			return false;
		return container == rhs.container;
	}

	bool operator!=(const time_indexed_vector &rhs) const
	{
		return !(*this == rhs);
	}

	container_type_t container;
};
