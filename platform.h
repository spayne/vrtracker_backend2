//
// system wide definitions and typedefs
//
#pragma once 
#include <thread>
#include <chrono>
#include <cassert>
#include <cstddef>
#include <limits>
#include <intrin.h>

namespace plat
{
	static void sleep_ms(unsigned long ms)
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1ms);
	}
}

typedef uint64_t time_stamp_t;
typedef int time_index_t;

inline uint64_t rdtsc() {
	return __rdtsc();
}

#define TBL_SIZE(t) (sizeof(t)/sizeof(t[0]))

inline int CLAMP(int min, int max, int sample)
{
	if (sample < min)
		return min;
	if (sample > max)
		return max;
	return sample;
}

template <typename T>
constexpr int size_as_int(const T &size_in) {
	assert(size_in <= static_cast<std::size_t>(std::numeric_limits<int>::max()));
	return static_cast<int>(size_in);
}