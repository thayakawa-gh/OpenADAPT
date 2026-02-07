#ifndef ADAPT_EVALUATOR_PARSER_V6_H
#define ADAPT_EVALUATOR_PARSER_V6_H

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <variant>
#include <cctype>
#include <OpenADAPT/Utility/Exception.h>
#include <OpenADAPT/Evaluator/Placeholder.h>
#include <OpenADAPT/Evaluator/FieldNode.h>
#include <OpenADAPT/Evaluator/ConstNode.h>
#include <OpenADAPT/Evaluator/FuncNode.h>
#include <OpenADAPT/Evaluator/Function.h>
#include <OpenADAPT/Evaluator/LayerFuncNode.h>

namespace adapt
{

namespace eval
{

namespace parser
{

// ========================================
// Parser Exception Class
// ========================================

class ParseError : public adapt::InvalidArg
{
public:
	ParseError(std::string_view mes)
		: adapt::InvalidArg(std::string("PARSE_ERROR : ") + std::string(mes))
	{}
};

// ========================================
// X-Macro Definitions for Operators/Functions
// ========================================

// Binary operators (name, string, precedence)
#define PARSER_BINARY_OPS \
	X(MUL,     "*",  5) \
	X(DIV,     "/",  5) \
	X(MOD,     "%",  5) \
	X(ADD,     "+",  6) \
	X(SUB,     "-",  6) \
	X(SHL,     "<<", 7) \
	X(SHR,     ">>", 7) \
	X(LT,      "<",  8) \
	X(LE,      "<=", 8) \
	X(GT,      ">",  8) \
	X(GE,      ">=", 8) \
	X(EQ,      "==", 9) \
	X(NE,      "!=", 9) \
	X(BIT_AND, "&",  10) \
	X(BIT_XOR, "^",  11) \
	X(BIT_OR,  "|",  12) \
	X(AND,     "&&", 13) \
	X(OR,      "||", 14)

// Unary operators
#define PARSER_UNARY_OPS \
	X(NEG,     "-") \
	X(NOT,     "!") \
	X(BIT_NOT, "~")

// Layer functions
#define PARSER_LAYER_FUNCS \
	X(SIZE,     "size") \
	X(EXIST,    "exist") \
	X(COUNT,    "count") \
	X(SUM,      "sum") \
	X(MEAN,     "mean") \
	X(DEV,      "dev") \
	X(GREATEST, "greatest") \
	X(LEAST,    "least")

// Regular functions - 1 arg
#define PARSER_REGULAR_FUNCS_1ARG \
	X(ABS,       "abs") \
	X(SQRT,      "sqrt") \
	X(CBRT,      "cbrt") \
	X(SQUARE,    "square") \
	X(CUBE,      "cube") \
	X(SIN,       "sin") \
	X(COS,       "cos") \
	X(TAN,       "tan") \
	X(ASIN,      "asin") \
	X(ACOS,      "acos") \
	X(ATAN,      "atan") \
	X(SINH,      "sinh") \
	X(COSH,      "cosh") \
	X(TANH,      "tanh") \
	X(ASINH,     "asinh") \
	X(ACOSH,     "acosh") \
	X(ATANH,     "atanh") \
	X(EXP,       "exp") \
	X(EXP2,      "exp2") \
	X(LOG,       "log") \
	X(LOG10,     "log10") \
	X(LOG2,      "log2") \
	X(CEIL,      "ceil") \
	X(FLOOR,     "floor") \
	X(ISFINITE,  "isfinite") \
	X(ISINF,     "isinf") \
	X(ISNAN,     "isnan") \
	X(ISNORMAL,  "isnormal") \
	X(LEN,       "len") \
	X(TOSTR,     "tostr")

// Regular functions - 2 args
#define PARSER_REGULAR_FUNCS_2ARG \
	X(POW,       "pow") \
	X(ATAN2,     "atan2") \
	X(HYPOT,     "hypot") \
	X(MIN,       "min") \
	X(MAX,       "max")

// Regular functions - 3 args
#define PARSER_REGULAR_FUNCS_3ARG \
	X(IF,        "if_") \
	X(SUBSTR,    "substr")

// Member functions
#define PARSER_MEMBER_FUNCS \
	X(AT,    "at") \
	X(OUTER, "outer") \
	X(O,     "o")

// ========================================
// Namespace for operator/function names
// ========================================

namespace ops
{
	#define X(NAME, STR, PREC) inline constexpr const char* NAME = STR; inline constexpr int NAME##_PREC = PREC;
	PARSER_BINARY_OPS
	#undef X
	
	#define X(NAME, STR) inline constexpr const char* NAME = STR;
	PARSER_UNARY_OPS
	#undef X
}

namespace funcs
{
	#define X(NAME, STR) inline constexpr const char* NAME = STR;
	PARSER_LAYER_FUNCS
	PARSER_REGULAR_FUNCS_1ARG
	PARSER_REGULAR_FUNCS_2ARG
	PARSER_REGULAR_FUNCS_3ARG
	PARSER_MEMBER_FUNCS
	#undef X
}

// ========================================
// Tokenizer
// ========================================

enum class TokenType
{
	End,
	Number,
	String,
	Identifier,
	Operator,
	LeftParen,
	RightParen,
	Comma,
	Dot
};

struct Token
{
	TokenType type;
	std::string value;
};

class Tokenizer
{
public:
	Tokenizer(std::string_view expr) : m_expr(expr), m_pos(0) {}
	
	Token NextToken()
	{
		SkipWhitespace();
		
		if (m_pos >= m_expr.size())
			return {TokenType::End, ""};
		
		char c = m_expr[m_pos];
		
		if (std::isdigit(c) || (c == '.' && m_pos + 1 < m_expr.size() && std::isdigit(m_expr[m_pos + 1])))
			return ParseNumber();
		
		if (c == '"')
			return ParseString();
		
		if (std::isalpha(c) || c == '_')
			return ParseIdentifier();
		
		if (c == '(')
		{
			m_pos++;
			return {TokenType::LeftParen, "("};
		}
		if (c == ')')
		{
			m_pos++;
			return {TokenType::RightParen, ")"};
		}
		if (c == ',')
		{
			m_pos++;
			return {TokenType::Comma, ","};
		}
		if (c == '.')
		{
			m_pos++;
			return {TokenType::Dot, "."};
		}
		
		return ParseOperator();
	}
	
private:
	void SkipWhitespace()
	{
		while (m_pos < m_expr.size() && std::isspace(m_expr[m_pos]))
			m_pos++;
	}
	
	Token ParseNumber()
	{
		size_t start = m_pos;
		bool has_dot = false;
		
		while (m_pos < m_expr.size())
		{
			char c = m_expr[m_pos];
			if (std::isdigit(c))
			{
				m_pos++;
			}
			else if (c == '.' && !has_dot)
			{
				has_dot = true;
				m_pos++;
			}
			else if (std::isalpha(c) || c == '_')
			{
				m_pos++;
			}
			else
			{
				break;
			}
		}
		
		return {TokenType::Number, std::string(m_expr.substr(start, m_pos - start))};
	}
	
	Token ParseString()
	{
		m_pos++;
		size_t start = m_pos;
		
		while (m_pos < m_expr.size() && m_expr[m_pos] != '"')
		{
			if (m_expr[m_pos] == '\\' && m_pos + 1 < m_expr.size())
				m_pos += 2;
			else
				m_pos++;
		}
		
		if (m_pos >= m_expr.size())
			throw ParseError("Unterminated string literal");
		
		std::string value(m_expr.substr(start, m_pos - start));
		m_pos++;
		
		return {TokenType::String, value};
	}
	
	Token ParseIdentifier()
	{
		size_t start = m_pos;
		
		while (m_pos < m_expr.size() &&
			   (std::isalnum(m_expr[m_pos]) || m_expr[m_pos] == '_'))
		{
			m_pos++;
		}
		
		return {TokenType::Identifier, std::string(m_expr.substr(start, m_pos - start))};
	}
	
	Token ParseOperator()
	{
		if (m_pos + 1 < m_expr.size())
		{
			std::string two_char = std::string(m_expr.substr(m_pos, 2));
			if (two_char == "==" || two_char == "!=" ||
				two_char == "<=" || two_char == ">=" ||
				two_char == "&&" || two_char == "||" ||
				two_char == "<<" || two_char == ">>")
			{
				m_pos += 2;
				return {TokenType::Operator, two_char};
			}
		}
		
		char c = m_expr[m_pos++];
		return {TokenType::Operator, std::string(1, c)};
	}
	
	std::string_view m_expr;
	size_t m_pos;
};

// ========================================
// Parser Implementation
// ========================================

template <class Container>
class Parser
{
public:
	using FieldNodeType = RttiFieldNode<Container>;
	using ConstNodeType = RttiConstNode;
	using FuncNodeType = RttiFuncNode<Container>;
	
	using NodeType = std::variant<FieldNodeType, ConstNodeType, FuncNodeType>;
	
	Parser(const Container& container, std::string_view expr)
		: m_container(container), m_tokenizer(expr), m_current_token(m_tokenizer.NextToken())
	{
		ExtractFieldNames();
	}
	
	FuncNodeType Parse()
	{
		NodeType result = ParseExpression(14);
		
		if (m_current_token.type != TokenType::End)
			throw ParseError("Unexpected tokens after expression");
		
		return ToFuncNode(std::move(result));
	}
	
private:
	void ExtractFieldNames()
	{
		if constexpr (requires { m_container.GetFieldNames(); })
		{
			auto names = m_container.GetFieldNames();
			for (size_t i = 0; i < names.size(); ++i)
			{
				m_field_map[names[i]] = i;
			}
		}
	}
	
	void Consume()
	{
		m_current_token = m_tokenizer.NextToken();
	}
	
	bool Match(TokenType type)
	{
		return m_current_token.type == type;
	}
	
	bool Match(TokenType type, const std::string& value)
	{
		return m_current_token.type == type && m_current_token.value == value;
	}
	
	void Expect(TokenType type, const std::string& msg)
	{
		if (!Match(type))
			throw ParseError(msg);
		Consume();
	}
	
	int GetPrecedence(const std::string& op)
	{
		#define X(NAME, STR, PREC) if (op == STR) return PREC;
		PARSER_BINARY_OPS
		#undef X
		return -1;
	}
	
	NodeType ParseExpression(int min_prec)
	{
		NodeType left = ParseUnary();
		
		while (Match(TokenType::Operator))
		{
			int prec = GetPrecedence(m_current_token.value);
			if (prec < 0 || prec < min_prec)
				break;
			
			std::string op = m_current_token.value;
			Consume();
			
			NodeType right = ParseExpression(prec + 1);
			left = ApplyBinaryOp(op, std::move(left), std::move(right));
		}
		
		return left;
	}
	
	NodeType ParseUnary()
	{
		if (Match(TokenType::Operator))
		{
			#define X(NAME, STR) \
				if (m_current_token.value == STR) { \
					Consume(); \
					NodeType operand = ParseUnary(); \
					return ApplyUnaryOp(STR, std::move(operand)); \
				}
			PARSER_UNARY_OPS
			#undef X
		}
		
		return ParsePostfix();
	}
	
	NodeType ParsePostfix()
	{
		NodeType node = ParsePrimary();
		
		while (Match(TokenType::Dot))
		{
			Consume();
			
			if (!Match(TokenType::Identifier))
				throw ParseError("Expected member function name after '.'");
			
			std::string member_name = m_current_token.value;
			Consume();
			
			Expect(TokenType::LeftParen, "Expected '(' after member function name");
			
			std::vector<NodeType> args;
			args.push_back(std::move(node));
			
			if (!Match(TokenType::RightParen))
			{
				do
				{
					if (Match(TokenType::Comma))
						Consume();
					args.push_back(ParseExpression(14));
				} while (Match(TokenType::Comma));
			}
			
			Expect(TokenType::RightParen, "Expected ')' after member function arguments");
			
			node = ApplyMemberFunc(member_name, std::move(args));
		}
		
		return node;
	}
	
	NodeType ParsePrimary()
	{
		if (Match(TokenType::LeftParen))
		{
			Consume();
			NodeType expr = ParseExpression(14);
			Expect(TokenType::RightParen, "Expected ')'");
			return expr;
		}
		
		if (Match(TokenType::Number))
		{
			std::string num_str = m_current_token.value;
			Consume();
			return ParseNumericLiteral(num_str);
		}
		
		if (Match(TokenType::String))
		{
			std::string str_val = m_current_token.value;
			Consume();
			return NodeType(ConstNodeType(str_val));
		}
		
		if (Match(TokenType::Identifier))
		{
			std::string name = m_current_token.value;
			Consume();
			
			if (Match(TokenType::LeftParen))
			{
				Consume();
				return ParseFunctionCall(name);
			}
			
			auto it = m_field_map.find(name);
			if (it == m_field_map.end())
				throw ParseError("Unknown identifier: " + name);
			
			return NodeType(FieldNodeType(GetPlaceholder(), it->second));
		}
		
		throw ParseError("Expected expression");
	}
	
	NodeType ParseNumericLiteral(const std::string& num_str)
	{
		std::string value_str = num_str;
		std::string suffix;
		
		size_t alpha_pos = value_str.size();
		for (size_t i = 0; i < value_str.size(); ++i)
		{
			if (std::isalpha(value_str[i]) || value_str[i] == '_')
			{
				alpha_pos = i;
				break;
			}
		}
		
		if (alpha_pos < value_str.size())
		{
			suffix = value_str.substr(alpha_pos);
			value_str = value_str.substr(0, alpha_pos);
		}
		
		bool is_float = (value_str.find('.') != std::string::npos) || 
		                (suffix.find('f') != std::string::npos ||
		                 suffix.find('F') != std::string::npos);
		
		if (suffix.empty())
		{
			if (is_float)
				return NodeType(ConstNodeType(std::stod(value_str)));
			else
				return NodeType(ConstNodeType(std::stoi(value_str)));
		}
		else if (suffix == "i08" || suffix == "i8")
			return NodeType(ConstNodeType(static_cast<int8_t>(std::stoi(value_str))));
		else if (suffix == "i16")
			return NodeType(ConstNodeType(static_cast<int16_t>(std::stoi(value_str))));
		else if (suffix == "i32")
			return NodeType(ConstNodeType(static_cast<int32_t>(std::stoi(value_str))));
		else if (suffix == "i64")
			return NodeType(ConstNodeType(static_cast<int64_t>(std::stoll(value_str))));
		else if (suffix == "f32")
			return NodeType(ConstNodeType(std::stof(value_str)));
		else if (suffix == "f64")
			return NodeType(ConstNodeType(std::stod(value_str)));
		else
			throw ParseError("Unknown type suffix: " + suffix);
	}
	
	NodeType ParseFunctionCall(const std::string& func_name)
	{
		std::vector<NodeType> args;
		
		if (!Match(TokenType::RightParen))
		{
			do
			{
				if (Match(TokenType::Comma))
					Consume();
				args.push_back(ParseExpression(14));
			} while (Match(TokenType::Comma));
		}
		
		Expect(TokenType::RightParen, "Expected ')' after function arguments");
		
		// Layer functions
		#define X(NAME, STR) \
			if (func_name == STR) return ApplyLayerFunc(STR, std::move(args));
		PARSER_LAYER_FUNCS
		#undef X
		
		// Numbered layer functions
		for (int level = 1; level <= 10; ++level)
		{
			#define X(NAME, STR) \
				if (func_name == std::string(STR) + std::to_string(level)) \
					return ApplyLayerFuncN(STR, level, std::move(args));
			PARSER_LAYER_FUNCS
			#undef X
		}
		
		// Regular functions
		#define X(NAME, STR) \
			if (func_name == STR) return ApplyRegularFunc(STR, std::move(args));
		PARSER_REGULAR_FUNCS_1ARG
		PARSER_REGULAR_FUNCS_2ARG
		PARSER_REGULAR_FUNCS_3ARG
		#undef X
		
		throw ParseError("Unknown function: " + func_name);
	}
	
	// ========================================
	// Apply Binary Operators (X-Macro Generated)
	// ========================================
	
	NodeType ApplyBinaryOp(const std::string& op, NodeType left, NodeType right)
	{
		bool left_is_field = std::holds_alternative<FieldNodeType>(left);
		bool left_is_const = std::holds_alternative<ConstNodeType>(left);
		bool left_is_func = std::holds_alternative<FuncNodeType>(left);
		
		bool right_is_field = std::holds_alternative<FieldNodeType>(right);
		bool right_is_const = std::holds_alternative<ConstNodeType>(right);
		bool right_is_func = std::holds_alternative<FuncNodeType>(right);
		
		// X-macro generates ALL combinations
		#define X(NAME, STR, PREC) \
			if (op == STR) { \
				if (left_is_field && right_is_field) \
					return NodeType(std::get<FieldNodeType>(left) BINARY_OP_##NAME std::get<FieldNodeType>(right)); \
				if (left_is_field && right_is_const) \
					return NodeType(std::get<FieldNodeType>(left) BINARY_OP_##NAME std::get<ConstNodeType>(right)); \
				if (left_is_field && right_is_func) \
					return NodeType(std::get<FieldNodeType>(left) BINARY_OP_##NAME std::get<FuncNodeType>(right)); \
				if (left_is_const && right_is_field) \
					return NodeType(std::get<ConstNodeType>(left) BINARY_OP_##NAME std::get<FieldNodeType>(right)); \
				if (left_is_const && right_is_const) \
					return NodeType(std::get<ConstNodeType>(left) BINARY_OP_##NAME std::get<ConstNodeType>(right)); \
				if (left_is_const && right_is_func) \
					return NodeType(std::get<ConstNodeType>(left) BINARY_OP_##NAME std::get<FuncNodeType>(right)); \
				if (left_is_func && right_is_field) \
					return NodeType(std::get<FuncNodeType>(left) BINARY_OP_##NAME std::get<FieldNodeType>(right)); \
				if (left_is_func && right_is_const) \
					return NodeType(std::get<FuncNodeType>(left) BINARY_OP_##NAME std::get<ConstNodeType>(right)); \
				if (left_is_func && right_is_func) \
					return NodeType(std::get<FuncNodeType>(left) BINARY_OP_##NAME std::get<FuncNodeType>(right)); \
			}
		
		#define BINARY_OP_MUL *
		#define BINARY_OP_DIV /
		#define BINARY_OP_MOD %
		#define BINARY_OP_ADD +
		#define BINARY_OP_SUB -
		#define BINARY_OP_SHL <<
		#define BINARY_OP_SHR >>
		#define BINARY_OP_LT <
		#define BINARY_OP_LE <=
		#define BINARY_OP_GT >
		#define BINARY_OP_GE >=
		#define BINARY_OP_EQ ==
		#define BINARY_OP_NE !=
		#define BINARY_OP_BIT_AND &
		#define BINARY_OP_BIT_XOR ^
		#define BINARY_OP_BIT_OR |
		#define BINARY_OP_AND &&
		#define BINARY_OP_OR ||
		
		PARSER_BINARY_OPS
		#undef X
		
		#undef BINARY_OP_MUL
		#undef BINARY_OP_DIV
		#undef BINARY_OP_MOD
		#undef BINARY_OP_ADD
		#undef BINARY_OP_SUB
		#undef BINARY_OP_SHL
		#undef BINARY_OP_SHR
		#undef BINARY_OP_LT
		#undef BINARY_OP_LE
		#undef BINARY_OP_GT
		#undef BINARY_OP_GE
		#undef BINARY_OP_EQ
		#undef BINARY_OP_NE
		#undef BINARY_OP_BIT_AND
		#undef BINARY_OP_BIT_XOR
		#undef BINARY_OP_BIT_OR
		#undef BINARY_OP_AND
		#undef BINARY_OP_OR
		
		// If we reach here, the operator was matched in GetPrecedence() but not here
		// This should never happen - internal logic error
		throw ParseError("Internal error: operator matched in GetPrecedence but not in dispatch");
	}
	
	// ========================================
	// Apply Unary Operators (X-Macro Generated)
	// ========================================
	
	NodeType ApplyUnaryOp(const std::string& op, NodeType operand)
	{
		bool is_field = std::holds_alternative<FieldNodeType>(operand);
		bool is_const = std::holds_alternative<ConstNodeType>(operand);
		bool is_func = std::holds_alternative<FuncNodeType>(operand);
		
		#define X(NAME, STR) \
			if (op == STR) { \
				if (is_field) return NodeType(UNARY_OP_##NAME std::get<FieldNodeType>(operand)); \
				if (is_const) return NodeType(UNARY_OP_##NAME std::get<ConstNodeType>(operand)); \
				if (is_func) return NodeType(UNARY_OP_##NAME std::get<FuncNodeType>(operand)); \
			}
		
		#define UNARY_OP_NEG -
		#define UNARY_OP_NOT !
		#define UNARY_OP_BIT_NOT ~
		
		PARSER_UNARY_OPS
		#undef X
		
		#undef UNARY_OP_NEG
		#undef UNARY_OP_NOT
		#undef UNARY_OP_BIT_NOT
		
		// If we reach here, the operator was used in ParseUnary but not dispatched
		// This should never happen - internal logic error
		throw ParseError("Internal error: unary operator not handled in dispatch");
	}
	
	// ========================================
	// Apply Layer Functions (X-Macro Generated)
	// ========================================
	
	NodeType ApplyLayerFunc(const std::string& func_name, std::vector<NodeType> args)
	{
		if (args.size() != 1)
			throw ParseError(func_name + " requires exactly 1 argument");
		
		auto& arg = args[0];
		bool is_field = std::holds_alternative<FieldNodeType>(arg);
		bool is_func = std::holds_alternative<FuncNodeType>(arg);
		
		if (!is_field && !is_func)
			throw ParseError(func_name + " requires a field or expression argument");
		
		// Use explicit function names to avoid macro issues
		if (func_name == "size") {
			if (is_field) return NodeType(eval::size(std::get<FieldNodeType>(arg)));
			if (is_func) return NodeType(eval::size(std::get<FuncNodeType>(arg)));
		}
		if (func_name == "exist") {
			if (is_field) return NodeType(eval::exist(std::get<FieldNodeType>(arg)));
			if (is_func) return NodeType(eval::exist(std::get<FuncNodeType>(arg)));
		}
		if (func_name == "count") {
			if (is_field) return NodeType(eval::count(std::get<FieldNodeType>(arg)));
			if (is_func) return NodeType(eval::count(std::get<FuncNodeType>(arg)));
		}
		if (func_name == "sum") {
			if (is_field) return NodeType(eval::sum(std::get<FieldNodeType>(arg)));
			if (is_func) return NodeType(eval::sum(std::get<FuncNodeType>(arg)));
		}
		if (func_name == "mean") {
			if (is_field) return NodeType(eval::mean(std::get<FieldNodeType>(arg)));
			if (is_func) return NodeType(eval::mean(std::get<FuncNodeType>(arg)));
		}
		if (func_name == "dev") {
			if (is_field) return NodeType(eval::dev(std::get<FieldNodeType>(arg)));
			if (is_func) return NodeType(eval::dev(std::get<FuncNodeType>(arg)));
		}
		if (func_name == "greatest") {
			if (is_field) return NodeType(eval::greatest(std::get<FieldNodeType>(arg)));
			if (is_func) return NodeType(eval::greatest(std::get<FuncNodeType>(arg)));
		}
		if (func_name == "least") {
			if (is_field) return NodeType(eval::least(std::get<FieldNodeType>(arg)));
			if (is_func) return NodeType(eval::least(std::get<FuncNodeType>(arg)));
		}
		
		throw ParseError("Unknown layer function: " + func_name);
	}
	
	NodeType ApplyLayerFuncN(const std::string& func_name, int level, std::vector<NodeType> args)
	{
		if (args.size() != 1)
			throw ParseError(func_name + std::to_string(level) + " requires exactly 1 argument");
		
		auto& arg = args[0];
		bool is_field = std::holds_alternative<FieldNodeType>(arg);
		bool is_func = std::holds_alternative<FuncNodeType>(arg);
		
		if (!is_field && !is_func)
			throw ParseError(func_name + std::to_string(level) + " requires a field or expression argument");
		
		// Use explicit function names
		if (func_name == "size") {
			if (is_field) return NodeType(eval::size(std::get<FieldNodeType>(arg), level));
			if (is_func) return NodeType(eval::size(std::get<FuncNodeType>(arg), level));
		}
		if (func_name == "exist") {
			if (is_field) return NodeType(eval::exist(std::get<FieldNodeType>(arg), level));
			if (is_func) return NodeType(eval::exist(std::get<FuncNodeType>(arg), level));
		}
		if (func_name == "count") {
			if (is_field) return NodeType(eval::count(std::get<FieldNodeType>(arg), level));
			if (is_func) return NodeType(eval::count(std::get<FuncNodeType>(arg), level));
		}
		if (func_name == "sum") {
			if (is_field) return NodeType(eval::sum(std::get<FieldNodeType>(arg), level));
			if (is_func) return NodeType(eval::sum(std::get<FuncNodeType>(arg), level));
		}
		if (func_name == "mean") {
			if (is_field) return NodeType(eval::mean(std::get<FieldNodeType>(arg), level));
			if (is_func) return NodeType(eval::mean(std::get<FuncNodeType>(arg), level));
		}
		if (func_name == "dev") {
			if (is_field) return NodeType(eval::dev(std::get<FieldNodeType>(arg), level));
			if (is_func) return NodeType(eval::dev(std::get<FuncNodeType>(arg), level));
		}
		if (func_name == "greatest") {
			if (is_field) return NodeType(eval::greatest(std::get<FieldNodeType>(arg), level));
			if (is_func) return NodeType(eval::greatest(std::get<FuncNodeType>(arg), level));
		}
		if (func_name == "least") {
			if (is_field) return NodeType(eval::least(std::get<FieldNodeType>(arg), level));
			if (is_func) return NodeType(eval::least(std::get<FuncNodeType>(arg), level));
		}
		
		throw ParseError("Unknown layer function: " + func_name);
	}
	
	// ========================================
	// Apply Regular Functions (X-Macro Generated)
	// ========================================
	
	NodeType ApplyRegularFunc(const std::string& func_name, std::vector<NodeType> args)
	{
		auto to_arg = [](NodeType& node) -> auto {
			if (std::holds_alternative<FieldNodeType>(node))
				return std::get<FieldNodeType>(node);
			else if (std::holds_alternative<ConstNodeType>(node))
				return std::get<ConstNodeType>(node);
			else
				return std::get<FuncNodeType>(node);
		};
		
		// 1-arg functions
		if (args.size() == 1)
		{
			auto arg0 = to_arg(args[0]);
			if (func_name == "abs") return NodeType(eval::abs(arg0));
			if (func_name == "sqrt") return NodeType(eval::sqrt(arg0));
			if (func_name == "cbrt") return NodeType(eval::cbrt(arg0));
			if (func_name == "square") return NodeType(eval::square(arg0));
			if (func_name == "cube") return NodeType(eval::cube(arg0));
			if (func_name == "sin") return NodeType(eval::sin(arg0));
			if (func_name == "cos") return NodeType(eval::cos(arg0));
			if (func_name == "tan") return NodeType(eval::tan(arg0));
			if (func_name == "asin") return NodeType(eval::asin(arg0));
			if (func_name == "acos") return NodeType(eval::acos(arg0));
			if (func_name == "atan") return NodeType(eval::atan(arg0));
			if (func_name == "sinh") return NodeType(eval::sinh(arg0));
			if (func_name == "cosh") return NodeType(eval::cosh(arg0));
			if (func_name == "tanh") return NodeType(eval::tanh(arg0));
			if (func_name == "asinh") return NodeType(eval::asinh(arg0));
			if (func_name == "acosh") return NodeType(eval::acosh(arg0));
			if (func_name == "atanh") return NodeType(eval::atanh(arg0));
			if (func_name == "exp") return NodeType(eval::exp(arg0));
			if (func_name == "exp2") return NodeType(eval::exp2(arg0));
			if (func_name == "log") return NodeType(eval::log(arg0));
			if (func_name == "log10") return NodeType(eval::log10(arg0));
			if (func_name == "log2") return NodeType(eval::log2(arg0));
			if (func_name == "ceil") return NodeType(eval::ceil(arg0));
			if (func_name == "floor") return NodeType(eval::floor(arg0));
			if (func_name == "isfinite") return NodeType(eval::isfinite(arg0));
			if (func_name == "isinf") return NodeType(eval::isinf(arg0));
			if (func_name == "isnan") return NodeType(eval::isnan(arg0));
			if (func_name == "isnormal") return NodeType(eval::isnormal(arg0));
			if (func_name == "len") return NodeType(eval::len(arg0));
			if (func_name == "tostr") return NodeType(eval::tostr(arg0));
		}
		
		// 2-arg functions
		if (args.size() == 2)
		{
			auto arg0 = to_arg(args[0]);
			auto arg1 = to_arg(args[1]);
			if (func_name == "pow") return NodeType(eval::pow(arg0, arg1));
			if (func_name == "atan2") return NodeType(eval::atan2(arg0, arg1));
			if (func_name == "hypot") return NodeType(eval::hypot(arg0, arg1));
			if (func_name == "min") return NodeType(eval::min(arg0, arg1));
			if (func_name == "max") return NodeType(eval::max(arg0, arg1));
		}
		
		// 3-arg functions
		if (args.size() == 3)
		{
			auto arg0 = to_arg(args[0]);
			auto arg1 = to_arg(args[1]);
			auto arg2 = to_arg(args[2]);
			if (func_name == "if_") return NodeType(eval::if_(arg0, arg1, arg2));
			if (func_name == "substr") return NodeType(eval::substr(arg0, arg1, arg2));
		}
		
		throw ParseError("Invalid function call: " + func_name);
	}
	
	// ========================================
	// Apply Member Functions
	// ========================================
	
	NodeType ApplyMemberFunc(const std::string& func_name, std::vector<NodeType> args)
	{
		if (args.empty())
			throw ParseError("Member function requires object");
		
		// Helper to check if node is ConstNode with integer type
		auto is_int_const = [](const NodeType& node) -> bool {
			if (!std::holds_alternative<ConstNodeType>(node))
				return false;
			auto& cn = std::get<ConstNodeType>(node);
			auto type = cn.GetType();
			return type == FieldType::I08 || type == FieldType::I16 || 
			       type == FieldType::I32 || type == FieldType::I64;
		};
		
		// A. Container member functions (.size, .pos)
		// Called on Container object (not nodes), with 0-4 integer constant args
		if (func_name == "size" || func_name == "pos")
		{
			// For container members, the 'this' object should be a special identifier
			// But we don't have container as primary expression, so this is invalid
			throw ParseError("." + func_name + "() is a container method, not a field method. Use container." + func_name + "(...) or " + func_name + "(...) as function");
		}
		
		// B. Field member .at() - accepts ANY nodes
		if (func_name == "at")
		{
			auto& obj = args[0];
			bool is_field = std::holds_alternative<FieldNodeType>(obj);
			
			if (!is_field)
				throw ParseError(".at() can only be called on field nodes");
			
			auto field = std::get<FieldNodeType>(obj);
			
			if (args.size() == 1)
				throw ParseError(".at() requires at least 1 index argument");
			
			// Extract indices as variant of FieldNode/ConstNode/FuncNode
			auto get_index = [](NodeType& node) -> auto {
				if (std::holds_alternative<FieldNodeType>(node))
					return std::get<FieldNodeType>(node);
				else if (std::holds_alternative<ConstNodeType>(node))
					return std::get<ConstNodeType>(node);
				else
					return std::get<FuncNodeType>(node);
			};
			
			// Support 1-4 arguments
			if (args.size() == 2)
			{
				auto idx0 = get_index(args[1]);
				return NodeType(MakeIndexedFieldNode(field, idx0));
			}
			else if (args.size() == 3)
			{
				auto idx0 = get_index(args[1]);
				auto idx1 = get_index(args[2]);
				return NodeType(MakeIndexedFieldNode(field, idx0, idx1));
			}
			else if (args.size() == 4)
			{
				auto idx0 = get_index(args[1]);
				auto idx1 = get_index(args[2]);
				auto idx2 = get_index(args[3]);
				return NodeType(MakeIndexedFieldNode(field, idx0, idx1, idx2));
			}
			else if (args.size() == 5)
			{
				auto idx0 = get_index(args[1]);
				auto idx1 = get_index(args[2]);
				auto idx2 = get_index(args[3]);
				auto idx3 = get_index(args[4]);
				return NodeType(MakeIndexedFieldNode(field, idx0, idx1, idx2, idx3));
			}
			else
			{
				throw ParseError(".at() supports up to 4 index arguments");
			}
		}
		
		// C. Field members .outer()/.o() - only integer constants
		if (func_name == "outer" || func_name == "o")
		{
			auto& obj = args[0];
			bool is_field = std::holds_alternative<FieldNodeType>(obj);
			
			if (!is_field)
				throw ParseError("." + func_name + "() can only be called on field nodes");
			
			auto field = std::get<FieldNodeType>(obj);
			
			// No args: .outer() with depth 0
			if (args.size() == 1)
			{
				return NodeType(RttiFieldNode<Container>(field, 0));
			}
			
			// Validate argument is integer constant
			if (!is_int_const(args[1]))
				throw ParseError("." + func_name + "() requires integer constant argument");
			
			// Support only single depth argument
			if (args.size() == 2)
			{
				int depth = std::get<ConstNodeType>(args[1]).i32();
				return NodeType(RttiFieldNode<Container>(field, depth));
			}
			else
			{
				throw ParseError("." + func_name + "() supports only 0 or 1 argument");
			}
		}
		
		throw ParseError("Unknown member function: " + func_name);
	}
	
	FuncNodeType ToFuncNode(NodeType node)
	{
		if (std::holds_alternative<FuncNodeType>(node))
			return std::get<FuncNodeType>(node);
		
		if (std::holds_alternative<FieldNodeType>(node))
		{
			return ConvertToRttiFuncNode(std::get<FieldNodeType>(node));
		}
		
		if (std::holds_alternative<ConstNodeType>(node))
		{
			throw ParseError("Standalone constant expressions not yet supported");
		}
		
		throw ParseError("Internal error: invalid node type");
	}
	
	auto GetPlaceholder() const
	{
		return BindContainer(m_container);
	}
	
	const Container& m_container;
	Tokenizer m_tokenizer;
	Token m_current_token;
	std::unordered_map<std::string, size_t> m_field_map;
};

} // namespace parser

// ========================================
// Public API
// ========================================

template <class Container>
auto ParseRttiFuncNode(const Container& container, std::string_view expr)
{
	parser::Parser<Container> parser(container, expr);
	return parser.Parse();
}

} // namespace eval

} // namespace adapt

#endif // ADAPT_EVALUATOR_PARSER_V6_H
