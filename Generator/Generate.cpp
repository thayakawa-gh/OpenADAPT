#include <format>
#include <fstream>
#include <OpenADAPT/Evaluator/Function.h>

using namespace adapt;

template <class Func, class ...ArgTypes>
constexpr auto IsCallable(int, Func f) -> decltype(f(std::declval<ArgTypes>()...), true) { return true; }
template <class Func, class ...ArgTypes>
constexpr bool IsCallable(long, Func) { return false; }

template <int N, class Prefix, FieldType Type, class GenType>
struct FieldTypeList;
template <int N, FieldType ...Prefix, FieldType Type, FieldType ...GenTypes>
struct FieldTypeList<N, ValueList<Prefix...>, Type, ValueList<GenTypes...>>
	: public FieldTypeList<N - 1, ValueList<Prefix...>, Type, ValueList<Type, GenTypes...>>
{};
template <FieldType Type_, FieldType ...Prefix, FieldType ...GenTypes>
struct FieldTypeList<0, ValueList<Prefix...>, Type_, ValueList<GenTypes...>> { using Type = ValueList<Prefix..., GenTypes...>; };


template <int NArgs, FieldType ...Types, class Func>
void MakeFuncCode_rec(Func f, std::string& result, ValueList<Types...>, ValueList<0>);
template <int NArgs, class Func, FieldType ...Types, int AllSame>
	requires (AllSame != 0)
void MakeFuncCode_rec(Func f, std::string& result, ValueList<Types...>, ValueList<AllSame>)
{
	if constexpr (NArgs == sizeof...(Types))
	{
		std::string fmt = "		if (";
		for (int i = 0; i < NArgs; ++i)
		{
			fmt += "arg" + std::to_string(i + 1) + ".GetType() == {" + std::to_string(i) + "}";
			if (i != NArgs - 1) fmt += " && ";
		}
		fmt += ")\n";
		fmt += "			return MakeRttiFuncNode_construct<Container, ";
		for (int i = 0; i < NArgs; ++i)
		{
			fmt += "DFieldInfo::TagTypeToValueType<{" + std::to_string(i) + "}>";
			if (i != NArgs - 1) fmt += ", ";
		}
		fmt += ">\n				(1, f, std::forward<Nodes>(nodes)...);\n";

		if (IsCallable<Func, typename DFieldInfo::TagTypeToValueType<Types>...>(0, f))
		{
			result += std::vformat(fmt, std::make_format_args(DFieldInfo::GetTagTypeString(Types)...));
		}
	}
	else
	{
#define RECURCE_MAKE_FUNC_CODE(TYPE)\
	MakeFuncCode_rec<NArgs>(std::forward<Func>(f), result, ValueList<Types..., TYPE>{}, ValueList<AllSame - 1>{});
		ADAPT_LOOP_FIELD_TYPE(RECURCE_MAKE_FUNC_CODE);
	}
}

template <int NArgs, FieldType ...Types, class Func>
void MakeFuncCode_rec(Func f, std::string& result, ValueList<Types...>, ValueList<0>)
{
#define DIRECTLY_CALL_MAKE_FUNC_CODE(TYPE)\
	MakeFuncCode_rec<NArgs>(f, result, typename FieldTypeList<NArgs - sizeof...(Types), ValueList<Types...>, TYPE, ValueList<>>::Type{}, ValueList<-1>{});
	ADAPT_LOOP_FIELD_TYPE(DIRECTLY_CALL_MAKE_FUNC_CODE);
}
template <class Func, int ...NArgs, int AllSameFlag>
std::string MakeFuncCode(Func f, const std::string& fname, ValueList<NArgs...>, ValueList<AllSameFlag> same)
{
	static const std::string fmt_head =
		"template <class Container, class Func, any_node ...Nodes>\n"
		"	requires std::same_as<std::decay_t<Func>, {}>\n"
		"eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)\n"
		"{{\n"
		"	using enum FieldType;\n";

	static const std::string fmt_nargs =
		"	if constexpr (sizeof...(Nodes) == {})\n"
		"	{{\n";

	std::string result = std::vformat(fmt_head, std::make_format_args(fname));

#define MAKE_FUNC_CODE_FOR_NARGS(NARGS)\
	if constexpr (((NArgs == NARGS) || ...))\
	{\
		result += std::vformat(fmt_nargs, std::make_format_args(NARGS));\
		result += "		const auto& [ ";\
		for (int i = 0; i < NARGS; i++)\
		{\
			result += "arg" + std::to_string(i + 1);\
			if (i != NARGS - 1) result += ", ";\
		}\
		result += " ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);\n";\
		MakeFuncCode_rec<NARGS>(f, result, ValueList<>{}, same);\
		result += "	}\n";\
	}
	MAKE_FUNC_CODE_FOR_NARGS(1);
	MAKE_FUNC_CODE_FOR_NARGS(2);
	MAKE_FUNC_CODE_FOR_NARGS(3);
	MAKE_FUNC_CODE_FOR_NARGS(4);
	MAKE_FUNC_CODE_FOR_NARGS(5);
	MAKE_FUNC_CODE_FOR_NARGS(6);
	MAKE_FUNC_CODE_FOR_NARGS(7);
	MAKE_FUNC_CODE_FOR_NARGS(8);
	MAKE_FUNC_CODE_FOR_NARGS(9);
	MAKE_FUNC_CODE_FOR_NARGS(10);

	return result + "	throw MismatchType(\"\");\n}\n";
}

int main()
{
	std::ofstream ofs("MakeRttiFuncNode_gen.h");
	ofs << "#ifndef ADAPT_EVALUATOR_MAKE_RTTI_FUNC_NODE_H\n";
	ofs << "#define ADAPT_EVALUATOR_MAKE_RTTI_FUNC_NODE_H\n\n";
	ofs << "#include <OpenADAPT/Evaluator/Function.h>\n\n";
	ofs << "namespace adapt\n{\n\n";
	ofs << "namespace eval\n{\n\n";
	ofs << "namespace detail\n{\n\n";

#define MAKE_FUNC_CODE(FUNC, ...) MakeFuncCode(FUNC{}, #FUNC, ValueList<__VA_ARGS__>{}, ValueList<-1>{})
#define MAKE_FUNC_CODE_SAME(N, FUNC, ...) MakeFuncCode(FUNC{}, #FUNC, ValueList<__VA_ARGS__>{}, ValueList<N>{})

	ofs << MAKE_FUNC_CODE(eval::Not, 1);
	ofs << MAKE_FUNC_CODE(eval::Negate, 1);
	ofs << MAKE_FUNC_CODE(eval::Plus, 2);
	ofs << MAKE_FUNC_CODE(eval::Minus, 2);
	ofs << MAKE_FUNC_CODE(eval::Multiply, 2);
	ofs << MAKE_FUNC_CODE(eval::Divide, 2);
	ofs << MAKE_FUNC_CODE(eval::Modulus, 2);
	ofs << MAKE_FUNC_CODE(eval::Power, 2);
	ofs << MAKE_FUNC_CODE(eval::Equal, 2);
	ofs << MAKE_FUNC_CODE(eval::NotEqual, 2);
	ofs << MAKE_FUNC_CODE(eval::Less, 2);
	ofs << MAKE_FUNC_CODE(eval::LessEqual, 2);
	ofs << MAKE_FUNC_CODE(eval::Greater, 2);
	ofs << MAKE_FUNC_CODE(eval::GreaterEqual, 2);
	ofs << MAKE_FUNC_CODE(eval::OperatorAnd, 2);
	ofs << MAKE_FUNC_CODE(eval::OperatorOr, 2);
	ofs << MAKE_FUNC_CODE(eval::Sin, 1);
	ofs << MAKE_FUNC_CODE(eval::Cos, 1);
	ofs << MAKE_FUNC_CODE(eval::Tan, 1);
	ofs << MAKE_FUNC_CODE(eval::ASin, 1);
	ofs << MAKE_FUNC_CODE(eval::ACos, 1);
	ofs << MAKE_FUNC_CODE(eval::ATan, 1);
	ofs << MAKE_FUNC_CODE(eval::Exponential, 1);
	ofs << MAKE_FUNC_CODE(eval::Square, 1);
	ofs << MAKE_FUNC_CODE(eval::Sqrt, 1);
	ofs << MAKE_FUNC_CODE(eval::Cube, 1);
	ofs << MAKE_FUNC_CODE(eval::Cbrt, 1);
	ofs << MAKE_FUNC_CODE(eval::Ceil, 1);
	ofs << MAKE_FUNC_CODE(eval::Floor, 1);
	ofs << MAKE_FUNC_CODE(eval::Round, 1);
	ofs << MAKE_FUNC_CODE(eval::Log, 1);
	ofs << MAKE_FUNC_CODE(eval::Log10, 1);
	ofs << MAKE_FUNC_CODE(eval::Abs, 1);
	ofs << MAKE_FUNC_CODE(eval::Len, 1);
	ofs << MAKE_FUNC_CODE(eval::NumToStr, 1);
	ofs << MAKE_FUNC_CODE(eval::ATan2, 2);
	ofs << MAKE_FUNC_CODE(eval::Log2, 2);
	ofs << MAKE_FUNC_CODE(eval::Hypot, 2, 3);
	ofs << MAKE_FUNC_CODE_SAME(0, eval::Max, 2, 3, 4, 5, 6, 7, 8, 9, 10);
	ofs << MAKE_FUNC_CODE_SAME(0, eval::Min, 2, 3, 4, 5, 6, 7, 8, 9, 10);
	ofs << MAKE_FUNC_CODE(eval::IfFunction, 3);
	ofs << MAKE_FUNC_CODE_SAME(1, eval::SwitchFunction, 2, 3, 4, 5, 6, 7, 8, 9, 10);

	ofs << "\n}\n\n";//detail
	ofs << "}\n\n";//eval
	ofs << "}\n\n";//adapt
	ofs << "#endif\n";
}