#ifndef ADAPT_COMMON_MACROS_H
#define ADAPT_COMMON_MACROS_H

//----------GetPlaceholderのヘルパー----------

#define ADAPT_DETAIL_CONV_FLD(x) #x##_fld

//例えばadapt::DTreeなどのコンテナcに対して、GET_PLACEHOLDERS(c, x, y, z)とすると、
//これはauto[x, y, z] = c.GetPlaceholders("x"_fld, "y"_fld, "z"_fld);と展開される。
#define ADAPT_GET_PLACEHOLDERS(c, ...)\
auto[__VA_ARGS__] =\
	c.GetPlaceholders(ADAPT_DETAIL_EXPAND_CONV(ADAPT_DETAIL_CONV_FLD, __VA_ARGS__))

//Rank指定版。auto[x, y, z] = c.GetPlaceholders<Rank>("x"_fld, "y"_fld, "z"_fld);のように展開される。
#define ADAPT_GET_RANKED_PLACEHOLDERS(c, Rank, ...)\
auto[__VA_ARGS__] =\
	c.GetPlaceholders<Rank>(ADAPT_DETAIL_EXPAND_CONV(ADAPT_DETAIL_CONV_FLD, __VA_ARGS__))


//----------named関数のヘルパー----------

#define ADAPT_DETAIL_CONV_NAMED_FLD(x) x.named(#x)

//Extract実行時に、c | Filter(...) | Extract(ADAPT_NAMED_FIELDS(a, b, c, d, e));とすると、
//Extract(a.named("a"), b.named("b"), c.named("c"), d.named("d"), e.named("e"));と展開され、
//Extractの各引数a～eに変数名と同様の名前が割り当てられる。
#define ADAPT_NAMED_FIELDS(...) ADAPT_DETAIL_EXPAND_CONV(ADAPT_DETAIL_CONV_NAMED_FLD, __VA_ARGS__)

#endif
