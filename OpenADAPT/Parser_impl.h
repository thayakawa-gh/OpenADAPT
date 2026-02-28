#ifndef ADAPT_PARSER_IMPL_H
#define ADAPT_PARSER_IMPL_H

#include <OpenADAPT/Parser.h>
#include <OpenADAPT/Parser/Parser_impl.h>

namespace adapt
{

ADAPT_EXPORT
template <class Container>
eval::RttiFuncNode<Container> Parse(const Container& container, std::string_view expr)
{
	eval::parser::Parser<Container> parser(container, expr);
	return parser.Parse();
}

}

#endif