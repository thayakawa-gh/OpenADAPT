#ifndef ADAPT_PARSER_PARSER_IMPL_H
#define ADAPT_PARSER_PARSER_IMPL_H

#include <OpenADAPT/Parser/Parser.h>

namespace adapt
{

namespace eval
{

namespace parser
{

// ----------Parserの実装----------

#define ADAPT_DETAIL_RETURN_2ARGS_OPS(SYM, LEFT, RIGHT)\
	if (left_is_##LEFT && right_is_##RIGHT)\
		return NodeType(std::move(std::get<LEFT##NodeType>(left)) SYM std::move(std::get<RIGHT##NodeType>(right)));

#define ADAPT_DETAIL_RETURN_2ARGS_FUNC(SYM, LEFT, RIGHT)\
	if (left_is_##LEFT && right_is_##RIGHT)\
		return NodeType(eval::SYM(std::move(std::get<LEFT##NodeType>(left)), std::move(std::get<RIGHT##NodeType>(right))));

#define ADAPT_DETAIL_RETURN_3ARGS_FUNC(SYM, LEFT, MIDDLE, RIGHT)\
	if (left_is_##LEFT && middle_is_##MIDDLE && right_is_##RIGHT)\
		return NodeType(eval::SYM(std::move(std::get<LEFT##NodeType>(left)), std::move(std::get<MIDDLE##NodeType>(middle_)), std::move(std::get<RIGHT##NodeType>(right_))));

#define ADAPT_DETAIL_RETURN_2ARGS_METHOD(SYM, LEFT, RIGHT)\
	if (left_is_##LEFT && right_is_##RIGHT)\
		return NodeType(field.GetPlaceholder().SYM(std::move(std::get<LEFT##NodeType>(left)), std::move(std::get<RIGHT##NodeType>(right))));

#define ADAPT_DETAIL_RETURN_3ARGS_METHOD(SYM, LEFT, MIDDLE, RIGHT)\
	if (left_is_##LEFT && middle_is_##MIDDLE && right_is_##RIGHT)\
		return NodeType(field.GetPlaceholder().SYM(std::move(std::get<LEFT##NodeType>(left)), std::move(std::get<MIDDLE##NodeType>(middle_)), std::move(std::get<RIGHT##NodeType>(right_))));


#define X(NAME, SYM)\
template <class Container>\
Parser<Container>::NodeType Parser<Container>::ApplyUnary##NAME(Parser<Container>::NodeType operand)\
{\
	bool is_field = std::holds_alternative<FieldNodeType>(operand);\
	bool is_const = std::holds_alternative<ConstNodeType>(operand);\
	bool is_func = std::holds_alternative<FuncNodeType>(operand);\
	if (is_field) return NodeType(SYM std::move(std::get<FieldNodeType>(operand)));\
	if (is_const) return NodeType(SYM ConvertConstNodeToRttiFuncNode<Container>(std::move(std::get<ConstNodeType>(operand))));\
	if (is_func) return NodeType(SYM std::move(std::get<FuncNodeType>(operand)));\
	throw ParseError("Unknown unary operator: " + std::string(#SYM));\
}
PARSER_UNARY_OPS
#undef X

#define X(NAME, SYM, PREC)\
template <class Container>\
Parser<Container>::NodeType Parser<Container>::ApplyBinary##NAME(Parser<Container>::NodeType left, Parser<Container>::NodeType right)\
{\
	bool left_is_Field = std::holds_alternative<FieldNodeType>(left);\
	bool left_is_Const = std::holds_alternative<ConstNodeType>(left);\
	bool left_is_Func = std::holds_alternative<FuncNodeType>(left);\
	bool right_is_Field = std::holds_alternative<FieldNodeType>(right);\
	bool right_is_Const = std::holds_alternative<ConstNodeType>(right);\
	bool right_is_Func = std::holds_alternative<FuncNodeType>(right);\
	ADAPT_DETAIL_RETURN_2ARGS_OPS(SYM, Field, Field)\
	ADAPT_DETAIL_RETURN_2ARGS_OPS(SYM, Field, Const)\
	ADAPT_DETAIL_RETURN_2ARGS_OPS(SYM, Field, Func)\
	ADAPT_DETAIL_RETURN_2ARGS_OPS(SYM, Const, Field)\
	if (left_is_Const && right_is_Const)\
		return NodeType(ConvertConstNodeToRttiFuncNode<Container>(std::move(std::get<ConstNodeType>(left))) SYM \
						std::move(std::get<ConstNodeType>(right)));\
	ADAPT_DETAIL_RETURN_2ARGS_OPS(SYM, Const, Func)\
	ADAPT_DETAIL_RETURN_2ARGS_OPS(SYM, Func, Field)\
	ADAPT_DETAIL_RETURN_2ARGS_OPS(SYM, Func, Const)\
	ADAPT_DETAIL_RETURN_2ARGS_OPS(SYM, Func, Func)\
	throw ParseError("Unknown binary operator: " + std::string(#SYM));\
}
PARSER_BINARY_OPS
#undef X


#define X(NAME, SYM)\
template <class Container>\
Parser<Container>::NodeType Parser<Container>::ApplyLayerFunc##NAME(Parser<Container>::NodeType arg)\
{\
	bool is_field = std::holds_alternative<FieldNodeType>(arg);\
	bool is_func = std::holds_alternative<FuncNodeType>(arg);\
	if (!is_field && !is_func)\
		throw ParseError(std::string(#SYM) + " requires a field or expression argument (not a constant)");\
	if (is_field) return NodeType(eval::SYM(std::move(std::get<FieldNodeType>(arg))));\
	if (is_func) return NodeType(eval::SYM(std::move(std::get<FuncNodeType>(arg))));\
	throw ParseError("Unknown layer function: " + std::string(#SYM));\
}
PARSER_LAYER_FUNCS
#undef X


#define X(NAME, SYM)\
template <class Container>\
template <LayerType Up>\
Parser<Container>::NodeType Parser<Container>::ApplyLayerFuncN##NAME(Parser<Container>::NodeType arg)\
{\
	bool is_field = std::holds_alternative<FieldNodeType>(arg);\
	bool is_func = std::holds_alternative<FuncNodeType>(arg);\
	if (!is_field && !is_func)\
		throw ParseError(#SYM + std::to_string(Up) + " requires a field or expression argument");\
	if (is_field) return NodeType(eval::SYM<Up>(std::get<FieldNodeType>(arg)));\
	if (is_func) return NodeType(eval::SYM<Up>(std::get<FuncNodeType>(arg)));\
	throw ParseError(std::format("Unknown layer function: {}{}", #SYM, Up));\
}
PARSER_LAYER_FUNCS
#undef X


#define X(NAME, SYM)\
template <class Container>\
Parser<Container>::NodeType Parser<Container>::ApplyLayerFuncIf##NAME(Parser<Container>::NodeType left, Parser<Container>::NodeType right)\
{\
	bool left_is_Field = std::holds_alternative<FieldNodeType>(left);\
	bool left_is_Const = std::holds_alternative<ConstNodeType>(left);\
	bool left_is_Func = std::holds_alternative<FuncNodeType>(left);\
	bool right_is_Field = std::holds_alternative<FieldNodeType>(right);\
	bool right_is_Const = std::holds_alternative<ConstNodeType>(right);\
	bool right_is_Func = std::holds_alternative<FuncNodeType>(right);\
	if (left_is_Const || right_is_Const)\
		throw ParseError(#SYM " requires a field or expression argument");\
	ADAPT_DETAIL_RETURN_2ARGS_FUNC(SYM, Field, Field)\
	ADAPT_DETAIL_RETURN_2ARGS_FUNC(SYM, Field, Func)\
	ADAPT_DETAIL_RETURN_2ARGS_FUNC(SYM, Func, Field)\
	ADAPT_DETAIL_RETURN_2ARGS_FUNC(SYM, Func, Func)\
	throw ParseError(std::format("Unknown layer function: {}", #SYM));\
}
PARSER_LAYER_FUNCS_IF
#undef X


#define X(NAME, SYM)\
template <class Container>\
template <LayerType Up>\
Parser<Container>::NodeType Parser<Container>::ApplyLayerFuncIfN##NAME(Parser<Container>::NodeType left, Parser<Container>::NodeType right)\
{\
	bool left_is_Field = std::holds_alternative<FieldNodeType>(left);\
	bool left_is_Const = std::holds_alternative<ConstNodeType>(left);\
	bool left_is_Func = std::holds_alternative<FuncNodeType>(left);\
	bool right_is_Field = std::holds_alternative<FieldNodeType>(right);\
	bool right_is_Const = std::holds_alternative<ConstNodeType>(right);\
	bool right_is_Func = std::holds_alternative<FuncNodeType>(right);\
	if (left_is_Const || right_is_Const)\
		throw ParseError(#SYM + std::to_string(Up) + " requires a field or expression argument");\
	ADAPT_DETAIL_RETURN_2ARGS_FUNC(SYM <Up>, Field, Field)\
	ADAPT_DETAIL_RETURN_2ARGS_FUNC(SYM <Up>, Field, Func)\
	ADAPT_DETAIL_RETURN_2ARGS_FUNC(SYM <Up>, Func, Field)\
	ADAPT_DETAIL_RETURN_2ARGS_FUNC(SYM <Up>, Func, Func)\
	throw ParseError(std::format("Unknown layer function: {}{}", #SYM, Up));\
}
PARSER_LAYER_FUNCS_IF
#undef X


#define X(NAME, SYM)\
template <class Container>\
Parser<Container>::NodeType Parser<Container>::ApplyRegularFunc##NAME(Parser<Container>::NodeType arg)\
{\
	bool is_field = std::holds_alternative<FieldNodeType>(arg);\
	bool is_const = std::holds_alternative<ConstNodeType>(arg);\
	bool is_func = std::holds_alternative<FuncNodeType>(arg);\
	if (is_const) return NodeType(eval::SYM(ConvertConstNodeToRttiFuncNode<Container>(std::move(std::get<ConstNodeType>(arg)))));\
	if (is_field) return NodeType(eval::SYM(std::get<FieldNodeType>(std::move(arg))));\
	if (is_func) return NodeType(eval::SYM(std::get<FuncNodeType>(std::move(arg))));\
	throw ParseError("Invalid function call: " #SYM);\
}
PARSER_REGULAR_FUNCS_1ARG
#undef X


#define X(NAME, SYM)\
template <class Container>\
Parser<Container>::NodeType Parser<Container>::ApplyRegularFunc##NAME(Parser<Container>::NodeType left, Parser<Container>::NodeType right)\
{\
	bool left_is_Field = std::holds_alternative<FieldNodeType>(left);\
	bool left_is_Const = std::holds_alternative<ConstNodeType>(left);\
	bool left_is_Func = std::holds_alternative<FuncNodeType>(left);\
	bool right_is_Field = std::holds_alternative<FieldNodeType>(right);\
	bool right_is_Const = std::holds_alternative<ConstNodeType>(right);\
	bool right_is_Func = std::holds_alternative<FuncNodeType>(right);\
	ADAPT_DETAIL_RETURN_2ARGS_FUNC(SYM, Field, Field)\
	ADAPT_DETAIL_RETURN_2ARGS_FUNC(SYM, Field, Const)\
	ADAPT_DETAIL_RETURN_2ARGS_FUNC(SYM, Field, Func)\
	ADAPT_DETAIL_RETURN_2ARGS_FUNC(SYM, Const, Field)\
	if (left_is_Const && right_is_Const)\
		return NodeType(SYM(ConvertConstNodeToRttiFuncNode<Container>(std::move(std::get<ConstNodeType>(left))),\
							std::move(std::get<ConstNodeType>(right))));\
	ADAPT_DETAIL_RETURN_2ARGS_FUNC(SYM, Const, Func)\
	ADAPT_DETAIL_RETURN_2ARGS_FUNC(SYM, Func, Field)\
	ADAPT_DETAIL_RETURN_2ARGS_FUNC(SYM, Func, Const)\
	ADAPT_DETAIL_RETURN_2ARGS_FUNC(SYM, Func, Func)\
	throw ParseError("Invalid function call: " #SYM);\
}
PARSER_REGULAR_FUNCS_2ARG
#undef X


#define X(NAME, SYM)\
template <class Container>\
Parser<Container>::NodeType Parser<Container>::ApplyRegularFunc##NAME(Parser<Container>::NodeType left, Parser<Container>::NodeType middle, Parser<Container>::NodeType right)\
{\
	bool left_is_Field = std::holds_alternative<FieldNodeType>(left);\
	bool left_is_Const = std::holds_alternative<ConstNodeType>(left);\
	bool left_is_Func = std::holds_alternative<FuncNodeType>(left);\
	bool middle_is_Field = std::holds_alternative<FieldNodeType>(middle);\
	bool middle_is_Const = std::holds_alternative<ConstNodeType>(middle);\
	bool middle_is_Func = std::holds_alternative<FuncNodeType>(middle);\
	bool right_is_Field = std::holds_alternative<FieldNodeType>(right);\
	bool right_is_Const = std::holds_alternative<ConstNodeType>(right);\
	bool right_is_Func = std::holds_alternative<FuncNodeType>(right);\
	Parser<Container>::NodeType middle_, right_;\
	if (middle_is_Const) middle_ = ConvertConstNodeToRttiFuncNode<Container>(std::move(std::get<ConstNodeType>(middle)));\
	else if (middle_is_Field) middle_ = fwd(std::get<FieldNodeType>(middle));\
	else if (middle_is_Func) middle_ = std::move(std::get<FuncNodeType>(middle));\
	else throw ParseError("Invalid function call: " #SYM);\
	if (right_is_Const) right_ = ConvertConstNodeToRttiFuncNode<Container>(std::move(std::get<ConstNodeType>(right)));\
	else if (right_is_Field) right_ = fwd(std::get<FieldNodeType>(right));\
	else if (right_is_Func) right_ = std::move(std::get<FuncNodeType>(right));\
	else throw ParseError("Invalid function call: " #SYM);\
	ADAPT_DETAIL_RETURN_3ARGS_FUNC(SYM, Field, Func, Func)\
	ADAPT_DETAIL_RETURN_3ARGS_FUNC(SYM, Const, Func, Func)\
	ADAPT_DETAIL_RETURN_3ARGS_FUNC(SYM, Func, Func, Func)\
	throw ParseError("Invalid function call: " #SYM);\
}
PARSER_REGULAR_FUNCS_3ARG
#undef X


#define X(NAME, SYM)\
template <class Container>\
Parser<Container>::NodeType Parser<Container>::ApplyFieldMethod##NAME(Parser<Container>::FieldNodeType field, Parser<Container>::NodeType arg)\
{\
	bool is_field = std::holds_alternative<FieldNodeType>(arg);\
	bool is_const = std::holds_alternative<ConstNodeType>(arg);\
	bool is_func = std::holds_alternative<FuncNodeType>(arg);\
	if (is_const) return NodeType(field.GetPlaceholder().SYM(std::move(std::get<ConstNodeType>(arg))));\
	if (is_field) return NodeType(field.GetPlaceholder().SYM(std::move(std::get<FieldNodeType>(arg))));\
	if (is_func) return NodeType(field.GetPlaceholder().SYM(std::move(std::get<FuncNodeType>(arg))));\
	throw ParseError("Unknown field method: " + std::string(#SYM)); \
}
PARSER_FIELD_METHODS_1ARG
#undef X


#define X(NAME, SYM)\
template <class Container>\
Parser<Container>::NodeType Parser<Container>::ApplyFieldMethod##NAME(Parser<Container>::FieldNodeType field, Parser<Container>::NodeType left, Parser<Container>::NodeType right)\
{\
	if constexpr (any_tree<Container>)\
	{\
		bool left_is_Field = std::holds_alternative<FieldNodeType>(left);\
		bool left_is_Const = std::holds_alternative<ConstNodeType>(left);\
		bool left_is_Func = std::holds_alternative<FuncNodeType>(left);\
		bool right_is_Field = std::holds_alternative<FieldNodeType>(right);\
		bool right_is_Const = std::holds_alternative<ConstNodeType>(right);\
		bool right_is_Func = std::holds_alternative<FuncNodeType>(right);\
		ADAPT_DETAIL_RETURN_2ARGS_METHOD(SYM, Const, Const)\
		ADAPT_DETAIL_RETURN_2ARGS_METHOD(SYM, Const, Field)\
		ADAPT_DETAIL_RETURN_2ARGS_METHOD(SYM, Const, Func)\
		ADAPT_DETAIL_RETURN_2ARGS_METHOD(SYM, Field, Const)\
		ADAPT_DETAIL_RETURN_2ARGS_METHOD(SYM, Field, Field)\
		ADAPT_DETAIL_RETURN_2ARGS_METHOD(SYM, Field, Func)\
		ADAPT_DETAIL_RETURN_2ARGS_METHOD(SYM, Func, Const)\
		ADAPT_DETAIL_RETURN_2ARGS_METHOD(SYM, Func, Field)\
		ADAPT_DETAIL_RETURN_2ARGS_METHOD(SYM, Func, Func)\
	}\
	throw ParseError("Unknown field method: " + std::string(#SYM)); \
}
PARSER_FIELD_METHODS_2ARG
#undef X


#define X(NAME, SYM)\
template <class Container>\
Parser<Container>::NodeType Parser<Container>::ApplyFieldMethod##NAME(Parser<Container>::FieldNodeType field, Parser<Container>::NodeType left, Parser<Container>::NodeType middle, Parser<Container>::NodeType right)\
{\
	if constexpr (any_tree<Container>)\
	{\
		bool left_is_Field = std::holds_alternative<FieldNodeType>(left);\
		bool left_is_Const = std::holds_alternative<ConstNodeType>(left);\
		bool left_is_Func = std::holds_alternative<FuncNodeType>(left);\
		bool middle_is_Field = std::holds_alternative<FieldNodeType>(middle);\
		bool middle_is_Const = std::holds_alternative<ConstNodeType>(middle);\
		bool middle_is_Func = std::holds_alternative<FuncNodeType>(middle);\
		bool right_is_Field = std::holds_alternative<FieldNodeType>(right);\
		bool right_is_Const = std::holds_alternative<ConstNodeType>(right);\
		bool right_is_Func = std::holds_alternative<FuncNodeType>(right);\
		Parser<Container>::NodeType middle_, right_;\
		if (middle_is_Const) middle_ = ConvertConstNodeToRttiFuncNode<Container>(std::move(std::get<ConstNodeType>(middle)));\
		else if (middle_is_Field) middle_ = fwd(std::get<FieldNodeType>(middle));\
		else if (middle_is_Func) middle_ = std::move(std::get<FuncNodeType>(middle));\
		else throw ParseError("Invalid function call: " #SYM);\
		if (right_is_Const) right_ = ConvertConstNodeToRttiFuncNode<Container>(std::move(std::get<ConstNodeType>(right)));\
		else if (right_is_Field) right_ = fwd(std::get<FieldNodeType>(right));\
		else if (right_is_Func) right_ = std::move(std::get<FuncNodeType>(right));\
		else throw ParseError("Invalid function call: " #SYM);\
		ADAPT_DETAIL_RETURN_3ARGS_METHOD(SYM, Const, Func, Func)\
		ADAPT_DETAIL_RETURN_3ARGS_METHOD(SYM, Field, Func, Func)\
		ADAPT_DETAIL_RETURN_3ARGS_METHOD(SYM, Func, Func, Func)\
	}\
	throw ParseError("Unknown field method: " + std::string(#SYM)); \
}
PARSER_FIELD_METHODS_3ARG
#undef X


// LayerTypeを引数に取る関数などが警告を発してしまうので、ここだけ抑制する。
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4244)
#endif
#define X(NAME, SYM)\
template <class Container>\
Parser<Container>::NodeType Parser<Container>::ApplyContainerMethod##NAME(Parser<Container>::ConstNodeType arg)\
{\
	auto f = [](const Container& c, std::integral auto v) { return c.SYM(v); };\
	return NodeType(ConvertToContainerMethod(f, m_container, arg));\
}
PARSER_CONTAINER_METHODS_1ARG
#undef X

#define X(NAME, SYM)\
template <class Container>\
Parser<Container>::NodeType Parser<Container>::ApplyContainerMethod##NAME(Parser<Container>::ConstNodeType left, Parser<Container>::ConstNodeType right)\
{\
	auto f = [](const Container& c, std::integral auto l, std::integral auto r) { return c.SYM(l, r); };\
	return NodeType(ConvertToContainerMethod(f, m_container, left, right));\
}
PARSER_CONTAINER_METHODS_2ARG
#undef X
#ifdef _MSC_VER
#pragma warning(pop)
#endif


}

}

}

#endif