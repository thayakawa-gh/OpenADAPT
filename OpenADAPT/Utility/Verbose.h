#ifndef ADAPT_VERBOSE_H
#define ADAPT_VERBOSE_H

#include <cassert>
#include <iostream>
#include <string_view>

namespace adapt
{

namespace detail
{
#ifdef NDEBUG
inline int g_verbosity_level = 2;
#else
inline int g_verbosity_level = 4;
#endif
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
	assert(0 <= l && l <= 2);
	detail::g_verbosity_level = l;
}
inline int GetVerbosityLevel()
{
	return detail::g_verbosity_level;
}
inline void PrintError(std::string_view message)
{
	if (GetVerbosityLevel() >= 1)
		std::cout << message;
}
inline void PrintWarning(std::string_view message)
{
	if (GetVerbosityLevel() >= 2)
		std::cout << message;
}
inline void PrintNotation(std::string_view message)
{
	if (GetVerbosityLevel() >= 3)
		std::cout << message;
}
inline void PrintInfo(std::string_view message)
{
	if (GetVerbosityLevel() >= 4)
		std::cout << message;
}
inline void PrintDebug(std::string_view message)
{
	if (GetVerbosityLevel() >= 5)
		std::cout << message;
}

}

#endif