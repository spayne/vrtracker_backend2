#pragma once
#include "platform.h"
#include <algorithm>
//
// Utility functions to deal with ranges
//

template <typename I, typename ValType>
I last_item_less_than_or_equal_to(I begin, I end, ValType val)
{
	I it = upper_bound(begin, end, val);

	if (it != begin) {
		it--; // not at end of array so rewind to previous item
	}
	else {
		it = end; // no items before this point, so return end()
	}
	return it;
}

template <typename I, typename Ty, class _Pr>
I last_item_less_than_or_equal_to(I begin, I end, const Ty &val, _Pr pr)
{
	I it = std::upper_bound(begin, end, val, pr);

	if (it != begin) {
		it--; // not at end of array so rewind to previous item
	}
	else {
		it = end; // no items before this point, so return end()
	}
	return it;
}

template <typename I, typename Ty, class _Pr>
I first_item_greater_than_or_equal_to(I begin, I end, const Ty& val, _Pr pr)
{
	I it = upper_bound(begin, end, val, pr);

	// upper_bound returns the first item greater than val.
	// so since we want greater than or equal to, check the previous
	// value to see if it's equal to.
	if (it != begin) {
		I prev(it);
		prev--;
		time_index_t found = prev->get_time_index();
		if (found == val.get_time_index())
			it = prev;
	}
	return it;
}

// r points to a range of time indexed values
template <typename R, typename Ty, class _Pr>
R range_intersect(const R &r, const Ty& min_val, const Ty& max_val, _Pr pr)  // [min_val and max_val) are not inclusive
{
	if (r.empty() || min_val.get_time_index() >= max_val.get_time_index())		// check 1: is the range empty
		return R(r.end(), r.end());
	if (min_val.get_time_index() > r.back().get_time_index())
		return R(r.end(), r.end()); // check 2: minimum is outside the values of the range
	if (max_val.get_time_index() < r.front().get_time_index())
		return R(r.end(), r.end()); // check 3: maximum is outside the values of the range

	auto first = first_item_greater_than_or_equal_to(r.begin(), r.end(), min_val, pr);
	assert(first != r.end());	// can't happen because of checks 2 and 3
	if ((*first).get_time_index() >= max_val.get_time_index())
		return R(r.end(), r.end());

	auto second = upper_bound(first, r.end(), max_val, pr);
	return R(first, second);
}

template <typename R>
struct range_intersect_result
{
private:
	int query_min_val;
	int query_max_val;
	R query_range;
	R result_range;
public:
	range_intersect_result()
		: query_max_val(-1)
	{}

	bool different_query(const R &r_in, int min_val_in, int max_val_in)
	{
		if (query_min_val != min_val_in)
			return true;
		if (query_max_val != max_val_in)
			return true;
		if (query_range.begin() != r_in.begin())
			return true;
		if (query_range.end() != r_in.end())
			return true;
		return false;
	}
	template <class _Pr>
	R intersect(const R &r, int min_val, int max_val, _Pr pr)
	{
		if (different_query(r, min_val, max_val))
		{
			query_min_val = min_val;
			query_max_val = max_val;
			query_range = r;
			result_range = range_intersect(r, min_val, max_val, pr);
		}
		return result_range;
	}
};

