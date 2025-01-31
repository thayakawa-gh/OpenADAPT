#ifndef ADAPT_VERBOSE_H
#define ADAPT_VERBOSE_H

#include <format>
#include <cassert>
#include <iostream>
#include <string_view>

namespace adapt
{

inline void SetVerbosityLevel(int l);
inline int GetVerbosityLevel();

namespace detail
{
#ifdef NDEBUG
inline int g_verbosity_level = 2;
#else
inline int g_verbosity_level = 4;
#endif

inline void PrintMessageWithVerbosityLevel(int level, std::string_view message)
{
	if (GetVerbosityLevel() >= level)
		std::cerr << message << std::endl;
}
template <class ...Args>
	requires (sizeof...(Args) > 0)
inline void PrintMessageWithVerbosityLevel(int level, std::format_string<Args...> fmt, Args&& ...args)
{
	if (GetVerbosityLevel() >= level)
		std::cerr << std::format(fmt, std::forward<Args>(args)...) << std::endl;
}

}

//パラメータ設定などの情報出力レベル。
//0: 何も出力しない。
//1: エラー出力。
//2: 危険、警告を出力。
//3: 好ましくない使い方のときに出力。
//3: 詳細情報を出力。
//4: デバッグ情報を出力。
inline void SetVerbosityLevel(int l)
{
	assert(0 <= l && l <= 4);
	detail::g_verbosity_level = l;
}
inline int GetVerbosityLevel()
{
	return detail::g_verbosity_level;
}
inline void PrintError(std::string_view message)
{
	detail::PrintMessageWithVerbosityLevel(1, message);
}
template <class ...Args>
	requires (sizeof...(Args) > 0)
inline void PrintError(std::format_string<Args...> fmt, Args&& ...args)
{
	detail::PrintMessageWithVerbosityLevel(1, fmt, std::forward<Args>(args)...);
}

inline void PrintWarning(std::string_view message)
{
	detail::PrintMessageWithVerbosityLevel(2, message);
}
template <class ...Args>
	requires (sizeof...(Args) > 0)
inline void PrintWarning(std::format_string<Args...> fmt, Args&& ...args)
{
	detail::PrintMessageWithVerbosityLevel(2, fmt, std::forward<Args>(args)...);
}

inline void PrintNotation(std::string_view message)
{
	detail::PrintMessageWithVerbosityLevel(3, message);
}
template <class ...Args>
	requires (sizeof...(Args) > 0)
inline void PrintNotation(std::format_string<Args...> fmt, Args&& ...args)
{
	detail::PrintMessageWithVerbosityLevel(3, fmt, std::forward<Args>(args)...);
}

inline void PrintInfo(std::string_view message)
{
	detail::PrintMessageWithVerbosityLevel(4, message);
}
template <class ...Args>
	requires (sizeof...(Args) > 0)
inline void PrintInfo(std::format_string<Args...> fmt, Args&& ...args)
{
	detail::PrintMessageWithVerbosityLevel(4, fmt, std::forward<Args>(args)...);
}

inline void PrintDebug(std::string_view message)
{
	detail::PrintMessageWithVerbosityLevel(5, message);
}
template <class ...Args>
	requires (sizeof...(Args) > 0)
inline void PrintDebug(std::format_string<Args...> fmt, Args&& ...args)
{
	detail::PrintMessageWithVerbosityLevel(5, fmt, std::forward<Args>(args)...);
}

}

#endif