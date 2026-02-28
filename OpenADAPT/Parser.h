#ifndef ADAPT_PARSER_H
#define ADAPT_PARSER_H

#if defined(ADAPT_DLL_BUILD) && defined(_MSVC_VER)
#define ADAPT_DLL_EXPORT __declspec(dllexport)
#elif defined(ADAPT_DLL_IMPORT) && defined(_MSVC_VER)
#define ADAPT_DLL_EXPORT __declspec(dllimport)
#else
#define ADAPT_DLL_EXPORT
#endif

#include <OpenADAPT/ADAPT.h>
#include <OpenADAPT/Parser/Parser.h>

namespace adapt
{

ADAPT_EXPORT
template <class Container>
ADAPT_DLL_EXPORT eval::RttiFuncNode<Container> Parse(const Container& container, std::string_view expr);

}

#endif