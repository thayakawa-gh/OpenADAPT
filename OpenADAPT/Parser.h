#ifndef ADAPT_PARSER_H
#define ADAPT_PARSER_H

#ifdef ADAPT_DLL_BUILD
#define ADAPT_DLL_EXPORT __declspec(dllexport)
#elif defined(ADAPT_DLL_IMPORT)
#define ADAPT_DLL_EXPORT __declspec(dllimport)
#else
#define ADAPT_DLL_EXPORT
#endif

namespace adapt
{

ADAPT_EXPORT
template <class Container>
ADAPT_DLL_EXPORT eval::RttiFuncNode<Container> Parse(const Container& container, std::string_view expr);

}

#endif