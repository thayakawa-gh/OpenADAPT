#include <Test/Aggregator.h>
#include <OpenADAPT/Utility/KeywordArgs.h>
#include <OpenADAPT/Plot/Canvas.h>

namespace args
{

ADAPT_DEFINE_KEYWORD_OPTION(non_type)
ADAPT_DEFINE_KEYWORD_OPTION_WITH_VALUE(floating_point, float)
ADAPT_DEFINE_KEYWORD_OPTION_WITH_VALUE(rvector, std::vector<double>&&)
ADAPT_DEFINE_KEYWORD_OPTION_WITH_VALUE(lvector, std::vector<double>&)

}

namespace tagged_args
{

struct Tag {};
struct Tag2;

ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(non_type, Tag)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(floating_point, float, Tag2)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(rvector, std::vector<double>&&, Tag)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(lvector, std::vector<double>&, Tag2)

}

template <keyword_arg ...Args>
void func(Args&& ...args)
{
	//キーワードの有無のみを調べる。下のmain関数では与えられているので、trueとなる。
	std::cout << "Non_type       : " << KeywordExists(args::non_type, args...) << std::endl;

	//引数を指定されなかったため末尾のデフォルト値が返る。
	std::cout << "Floating point : " << GetKeywordArg(args::floating_point, 3.0, args...) << std::endl;

	auto rv = GetKeywordArg(args::rvector, args...);
	std::cout << "RVector        :";
	for (auto& vv : rv) std::cout << " " << vv;
	std::cout << std::endl;

	auto& lv = GetKeywordArg(args::lvector, args...);
	std::cout << "LVector        :";
	for (auto& vv : lv) std::cout << " " << vv;
	std::cout << std::endl;
	for (int i = 0; i < (int)lv.size(); ++i) lv[i] = i * 3;
}

TEST_F(Aggregator, KeywordArg)
{
	std::vector<double> r = { 5, 5, 5 };
	std::vector<double> l = { 5, 5, 5 };
	//non_typeのようにbool値の場合、args::non_typeのように値指定なしの名前のみで与えると、
	//args::non_type = trueと同等とみなされる。
	//bool型以外では名前のみで与えるとエラーになる。
	func(args::non_type, args::rvector = std::move(r), args::lvector = l);
	std::cout << "RVector        :";
	for (auto& v : r) std::cout << " " << v;//moveしたので空に。
	std::cout << std::endl;
	std::cout << "LVector        :";
	for (auto& v : l) std::cout << " " << v;//左辺値参照なので中身が書き換えられる。
	std::cout << std::endl;
}

template <keyword_arg_tagged_with<tagged_args::Tag, tagged_args::Tag2> ...Args>
void func2(Args&& ...args)
{
	//キーワードの有無のみを調べる。下のmain関数では与えられているので、trueとなる。
	std::cout << "Non_type       : " << KeywordExists(tagged_args::non_type, args...) << std::endl;

	//引数を指定されなかったため末尾のデフォルト値が返る。
	std::cout << "Floating point : " << GetKeywordArg(tagged_args::floating_point, 3.0, args...) << std::endl;

	auto rv = GetKeywordArg(tagged_args::rvector, args...);
	std::cout << "RVector        :";
	for (auto& vv : rv) std::cout << " " << vv;
	std::cout << std::endl;

	auto& lv = GetKeywordArg(tagged_args::lvector, args...);
	std::cout << "LVector        :";
	for (auto& vv : lv) std::cout << " " << vv;
	std::cout << std::endl;
	for (int i = 0; i < (int)lv.size(); ++i) lv[i] = i * 3;
}
TEST_F(Aggregator, KeywordArgWithTag)
{
	std::vector<double> r = { 5, 5, 5 };
	std::vector<double> l = { 5, 5, 5 };
	func2(tagged_args::non_type, tagged_args::rvector = std::move(r), tagged_args::lvector = l);
	std::cout << "RVector        :";
	for (auto& v : r) std::cout << " " << v;//moveしたので空に。
	std::cout << std::endl;
	std::cout << "LVector        :";
	for (auto& v : l) std::cout << " " << v;//左辺値参照なので中身が書き換えられる。
	std::cout << std::endl;
}

TEST_F(Aggregator, Plot_xyerrorbars)
{
	std::vector<double> x = { 1, 2, 3, 4, 5 };
	std::vector<double> y = { 1, 2, 3, 4, 5 };
	std::vector<double> xerr = { 0.1, 0.1, 0.1, 0.1, 0.1 };
	std::vector<double> yerr = { 0.1, 0.2, 0.3, 0.4, 0.5 };
	std::vector<double> vc = { 1, 2, 3, 4, 5 };

	Canvas2D g("Plot_xyerrorbars.png");
	g.SetTitle("Plot\\_xyerrorbars");
	g.SetXLabel("X");
	g.SetYLabel("Y");
	g.PlotPoints(x, y, plot::xerrorbar = xerr, plot::yerrorbar = yerr, plot::variable_color = vc);
}