#ifndef ADAPT_CUF_EXPECT_H
#define ADAPT_CUF_EXPECT_H

#include <variant>

namespace adapt
{


template <class Succeeded, class Error>
class Expected
{
public:



private:
	std::variant<Succeeded, Error> mResult;
};

}

#endif