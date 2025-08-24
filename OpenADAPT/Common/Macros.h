#ifndef ADAPT_COMMON_MACROS_H
#define ADAPT_COMMON_MACROS_H

#include <OpenADAPT/Utility/Macros.h>

//----------階層構造定義のヘルパー----------

#define ADAPT_DETAIL_CONV_D_ADD_LAYER(name, type) {#name, adapt::FieldType::type}


// DTree tに対して、ADAPT_D_ADD_LAYER(t, foo, I32, bar F64, baz Str)とすると、
// t.AddLayer({ {"foo", I32}, {"bar", F64}, {"baz", Str} })と展開される。
#define ADAPT_D_ADD_LAYER(c, ...)\
c.AddLayer({ ADAPT_DETAIL_EXPAND_CONV_PAIR(ADAPT_DETAIL_CONV_D_ADD_LAYER, __VA_ARGS__) })

// DTree/DTable tに対して、ADAPT_D_SET_TOP_LAYER(t, foo, I32, bar F64, baz Str)とすると、
// t.SetTopLayer({ {"foo", I32}, {"bar", F64}, {"baz", Str} })と展開される。
#define ADAPT_D_SET_TOP_LAYER(c, ...)\
c.SetTopLayer({ ADAPT_DETAIL_EXPAND_CONV_PAIR(ADAPT_DETAIL_CONV_D_ADD_LAYER, __VA_ARGS__) })

// DTable tに対して、ADAPT_D_SET_LAYER(t, l, foo, I32, bar F64, baz Str)とすると、
// t.SetTopLayer(l, { {"foo", I32}, {"bar", F64}, {"baz", Str} })と展開される。
#define ADAPT_D_SET_LAYER(c, l, ...)\
c.SetLayer(l, { ADAPT_DETAIL_EXPAND_CONV_PAIR(ADAPT_DETAIL_CONV_D_ADD_LAYER, __VA_ARGS__) })

#define ADAPT_DETAIL_CONV_S_DEFINE_LAYER(name, type) adapt::Named<#name, type>

// 例えば、ADAPT_S_DEFINE_LAYER(foo int32_t, bar double, baz std::string)とすると、
// adapt::NamedTuple<adapt::Named<"foo", int32_t>, adapt::Named<"bar", double>, adapt::Named<"baz", std::string>>と展開される。
// using Layer0 = ADAPT_S_DEFINE_LAYER(foo int32_t, bar double, baz std::string);のように使う。
#define ADAPT_S_DEFINE_LAYER(...)\
adapt::NamedTuple<ADAPT_DETAIL_EXPAND_CONV_PAIR(ADAPT_DETAIL_CONV_S_DEFINE_LAYER, __VA_ARGS__)>

#ifdef ADAPT_OMIT_MACRO_PREFIX
#define D_ADD_LAYER ADAPT_D_ADD_LAYER
#define D_SET_TOP_LAYER ADAPT_D_SET_TOP_LAYER
#define D_SET_LAYER ADAPT_D_SET_LAYER
#define S_DEFINE_LAYER ADAPT_S_DEFINE_LAYER
#endif

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

#ifdef ADAPT_OMIT_MACRO_PREFIX
#define GET_PLACEHOLDERS ADAPT_GET_PLACEHOLDERS
#define GET_RANKED_PLACEHOLDERS ADAPT_GET_RANKED_PLACEHOLDERS
#endif

//----------named関数のヘルパー----------

#define ADAPT_DETAIL_CONV_NAMED_FLD(x) adapt::detail::AddNameIfNP(x, #x##_fld)

//Extract実行時に、c | Filter(...) | Extract(ADAPT_NAMED_FIELDS(a, b, c, d, e));とすると、
//Extract(a.named("a"), b.named("b"), c.named("c"), d.named("d"), e.named("e"));と展開され、
//Extractの各引数a～eに変数名と同様の名前が割り当てられる。
#define ADAPT_NAMED_FIELDS(...) ADAPT_DETAIL_EXPAND_CONV(ADAPT_DETAIL_CONV_NAMED_FLD, __VA_ARGS__)

#define ADAPT_EXTRACT(...) adapt::Extract(ADAPT_NAMED_FIELDS(__VA_ARGS__))

#define ADAPT_HIST(...) adapt::Hist(ADAPT_NAMED_FIELDS(__VA_ARGS__))

#ifdef ADAPT_OMIT_MACRO_PREFIX
#define NAMED_AXES ADAPT_NAMED_AXES
#define NAMED_VARS ADAPT_NAMED_VARS
#define NAMED_FIELDS ADAPT_NAMED_FIELDS
#define EXTRACT ADAPT_EXTRACT
#define HASH ADAPT_HASH
#endif

#endif
