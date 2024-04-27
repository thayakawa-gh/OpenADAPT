#ifndef ADAPT_COMMON_GLOBAL_H
#define ADAPT_COMMON_GLOBAL_H

#include <thread>

namespace adapt
{

inline int32_t g_num_of_threads = -1;
inline int32_t g_granularity = 128;

inline int32_t GetNumOfThreads()
{
	if (g_num_of_threads == -1)
		return std::thread::hardware_concurrency();
	return g_num_of_threads;
}
inline void SetNumOfThreads(int32_t num_of_threads = -1)
{
	g_num_of_threads = num_of_threads;
}

inline int32_t GetGranularity()
{
	return g_granularity;
}
inline void SetGranularity(int32_t granularity)
{
	g_granularity = granularity;
}

}

#endif