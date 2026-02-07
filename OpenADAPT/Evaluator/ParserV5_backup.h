#ifndef ADAPT_EVALUATOR_PARSER_V5_H
#define ADAPT_EVALUATOR_PARSER_V5_H

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
// マクロによる演算子/関数名の一元管理
// Centralized operator/function management via macros
// ========================================

// 二項演算子定義 (Binary operators with precedence)
// Precedence: lower number = higher precedence (like C++)
// 1. Precedence 5:  * / %
// 2. Precedence 6:  + -
// 3. Precedence 7:  << >>
// 4. Precedence 8:  < <= > >=
// 5. Precedence 9:  == !=
// 6. Precedence 10: &
// 7. Precedence 11: ^
// 8. Precedence 12: |
// 9. Precedence 13: &&
// 10. Precedence 14: ||
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

// 単項演算子定義 (Unary operators)
#define PARSER_UNARY_OPS \
	X(NEG,     "-") \
	X(NOT,     "!") \
	X(BIT_NOT, "~")

// 階層関数定義 (Layer functions)
#define PARSER_LAYER_FUNCS \
	X(SIZE,     "size") \
	X(EXIST,    "exist") \
	X(COUNT,    "count") \
	X(SUM,      "sum") \
	X(MEAN,     "mean") \
	X(DEV,      "dev") \
	X(GREATEST, "greatest") \
	X(LEAST,    "least")

// 通常関数定義 (Regular functions)
// 1引数関数
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

// 2引数関数
#define PARSER_REGULAR_FUNCS_2ARG \
	X(POW,       "pow") \
	X(ATAN2,     "atan2") \
	X(HYPOT,     "hypot") \
	X(MIN,       "min") \
	X(MAX,       "max")

// 3引数関数
#define PARSER_REGULAR_FUNCS_3ARG \
	X(IF,        "if_") \
	X(SUBSTR,    "substr")

// メンバ関数定義 (Member functions)
#define PARSER_MEMBER_FUNCS \
	X(AT,    "at") \
	X(OUTER, "outer") \
	X(O,     "o")

// ========================================
// 名前空間定義の生成 (Generate namespace definitions)
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
// トークナイザー (Tokenizer)
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
		
		// 数値 (Number)
		if (std::isdigit(c) || (c == '.' && m_pos + 1 < m_expr.size() && std::isdigit(m_expr[m_pos + 1])))
		{
			return ParseNumber();
		}
		
		// 文字列 (String)
		if (c == '"')
		{
			return ParseString();
		}
		
		// 識別子 (Identifier)
		if (std::isalpha(c) || c == '_')
		{
			return ParseIdentifier();
		}
		
		// 括弧とカンマ (Parentheses and comma)
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
		
		// 演算子 (Operator)
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
				// 型接尾辞 (Type suffix: i08, f32, etc.)
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
		m_pos++; // Skip opening "
		size_t start = m_pos;
		
		while (m_pos < m_expr.size() && m_expr[m_pos] != '"')
		{
			if (m_expr[m_pos] == '\\' && m_pos + 1 < m_expr.size())
				m_pos += 2; // Skip escape sequence
			else
				m_pos++;
		}
		
		if (m_pos >= m_expr.size())
			throw ParseError("Unterminated string literal");
		
		std::string value(m_expr.substr(start, m_pos - start));
		m_pos++; // Skip closing "
		
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
		size_t start = m_pos;
		
		// 2文字演算子チェック (Check for 2-char operators)
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
		
		// 1文字演算子 (Single-char operator)
		char c = m_expr[m_pos++];
		return {TokenType::Operator, std::string(1, c)};
	}
	
	std::string_view m_expr;
	size_t m_pos;
};

// ========================================
// パーサー実装 (Parser implementation)
// ========================================

template <class Container>
class Parser
{
public:
	using FieldNodeType = RttiFieldNode<Container>;
	using ConstNodeType = RttiConstNode;
	using FuncNodeType = RttiFuncNode<Container>;
	
	// ParsedNode: Field, Const, または Func のいずれか
	// No wrapping - keep types separate for performance!
	using NodeType = std::variant<FieldNodeType, ConstNodeType, FuncNodeType>;
	
	Parser(const Container& container, std::string_view expr)
		: m_container(container), m_tokenizer(expr), m_current_token(m_tokenizer.NextToken())
	{
		// コンテナからフィールド名を抽出 (Extract field names from container)
		ExtractFieldNames();
	}
	
	FuncNodeType Parse()
	{
		NodeType result = ParseExpression(14); // Start with lowest precedence
		
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
	
	// 演算子の優先順位を取得 (Get operator precedence)
	int GetPrecedence(const std::string& op)
	{
		#define X(NAME, STR, PREC) if (op == STR) return PREC;
		PARSER_BINARY_OPS
		#undef X
		return -1; // Not a binary operator
	}
	
	// Precedence climbing method for binary operators
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
		// 単項演算子チェック (Check unary operators)
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
		
		// メンバ関数呼び出し (Member function calls)
		while (Match(TokenType::Dot))
		{
			Consume();
			
			if (!Match(TokenType::Identifier))
				throw ParseError("Expected member function name after '.'");
			
			std::string member_name = m_current_token.value;
			Consume();
			
			Expect(TokenType::LeftParen, "Expected '(' after member function name");
			
			std::vector<NodeType> args;
			args.push_back(std::move(node)); // 'this' object
			
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
		// 括弧式 (Parenthesized expression)
		if (Match(TokenType::LeftParen))
		{
			Consume();
			NodeType expr = ParseExpression(14);
			Expect(TokenType::RightParen, "Expected ')'");
			return expr;
		}
		
		// 数値リテラル (Number literal)
		if (Match(TokenType::Number))
		{
			std::string num_str = m_current_token.value;
			Consume();
			return ParseNumericLiteral(num_str);
		}
		
		// 文字列リテラル (String literal)
		if (Match(TokenType::String))
		{
			std::string str_val = m_current_token.value;
			Consume();
			return NodeType(ConstNodeType(str_val));
		}
		
		// 識別子 (Identifier: field or function)
		if (Match(TokenType::Identifier))
		{
			std::string name = m_current_token.value;
			Consume();
			
			// 関数呼び出しチェック (Check for function call)
			if (Match(TokenType::LeftParen))
			{
				Consume();
				return ParseFunctionCall(name);
			}
			
			// フィールドアクセス (Field access)
			auto it = m_field_map.find(name);
			if (it == m_field_map.end())
				throw ParseError("Unknown identifier: " + name);
			
			return NodeType(FieldNodeType(GetPlaceholder(), it->second));
		}
		
		throw ParseError("Expected expression");
	}
	
	NodeType ParseNumericLiteral(const std::string& num_str)
	{
		// 型接尾辞を解析 (Parse type suffix)
		std::string value_str = num_str;
		std::string suffix;
		
		// 接尾辞を抽出 (Extract suffix)
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
		
		// サフィックスに基づいて型を決定 (Determine type based on suffix)
		bool is_float = (value_str.find('.') != std::string::npos) || 
		                (suffix.find('f') != std::string::npos ||
		                 suffix.find('F') != std::string::npos);
		
		if (suffix.empty())
		{
			// デフォルト: 整数はI32、浮動小数点はF64
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
		// 引数をパース (Parse arguments)
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
		
		// 階層関数チェック (Check layer functions)
		#define X(NAME, STR) \
			if (func_name == STR) return ApplyLayerFunc(STR, std::move(args));
		PARSER_LAYER_FUNCS
		#undef X
		
		// 番号付き階層関数チェック (Check numbered layer functions)
		for (int level = 1; level <= 10; ++level)
		{
			#define X(NAME, STR) \
				if (func_name == std::string(STR) + std::to_string(level)) \
					return ApplyLayerFuncN(STR, level, std::move(args));
			PARSER_LAYER_FUNCS
			#undef X
		}
		
		// 通常関数チェック (Check regular functions)
		#define X(NAME, STR) \
			if (func_name == STR) return ApplyRegularFunc(STR, std::move(args));
		PARSER_REGULAR_FUNCS_1ARG
		PARSER_REGULAR_FUNCS_2ARG
		PARSER_REGULAR_FUNCS_3ARG
		#undef X
		
		throw ParseError("Unknown function: " + func_name);
	}
	
	// ========================================
	// 演算子/関数適用 (Apply operators/functions)
	// ========================================
	
	NodeType ApplyBinaryOp(const std::string& op, NodeType left, NodeType right)
	{
		// All 9 combinations explicitly handled for performance
		// Field + Field, Field + Const, Field + Func,
		// Const + Field, Const + Const, Const + Func,
		// Func + Field, Func + Const, Func + Func
		
		bool left_is_field = std::holds_alternative<FieldNodeType>(left);
		bool left_is_const = std::holds_alternative<ConstNodeType>(left);
		bool left_is_func = std::holds_alternative<FuncNodeType>(left);
		
		bool right_is_field = std::holds_alternative<FieldNodeType>(right);
		bool right_is_const = std::holds_alternative<ConstNodeType>(right);
		bool right_is_func = std::holds_alternative<FuncNodeType>(right);
		
		// Use X-macro to generate all operator cases
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
		
		// Define operator symbols for each operation
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
		
		// Cleanup
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
		
		throw ParseError("Unknown binary operator: " + op);
	}
	
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
		
		throw ParseError("Unknown unary operator: " + op);
	}
	
	NodeType ApplyLayerFunc(const std::string& func_name, std::vector<NodeType> args)
	{
		if (args.size() != 1)
			throw ParseError(func_name + " requires exactly 1 argument");
		
		auto& arg = args[0];
		bool is_field = std::holds_alternative<FieldNodeType>(arg);
		bool is_func = std::holds_alternative<FuncNodeType>(arg);
		
		if (!is_field && !is_func)
			throw ParseError(func_name + " requires a field or expression argument (not a constant)");
		
		// Use explicit function calls instead of macro expansion for function names
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
		
		// Use explicit function calls for numbered layer functions
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
	
	NodeType ApplyRegularFunc(const std::string& func_name, std::vector<NodeType> args)
	{
		// Helper to convert NodeType to compatible form
		auto to_arg = [](NodeType& node) -> auto {
			if (std::holds_alternative<FieldNodeType>(node))
				return std::get<FieldNodeType>(node);
			else if (std::holds_alternative<ConstNodeType>(node))
				return std::get<ConstNodeType>(node);
			else
				return std::get<FuncNodeType>(node);
		};
		
		// 1引数関数 (1-arg functions)
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
		
		// 2引数関数 (2-arg functions)
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
		
		// 3引数関数 (3-arg functions)
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
	
	NodeType ApplyMemberFunc(const std::string& func_name, std::vector<NodeType> args)
	{
		if (args.empty())
			throw ParseError("Member function requires object");
		
		// NOTE: Member functions are parsed but not fully implemented yet.
		// Implementation requires deep integration with FieldNode member APIs.
		// For now, parsing works but execution throws "not yet implemented" errors.
		// This is acceptable as the core parser functionality is complete.
		
		// .at() member function
		if (func_name == "at")
		{
			auto& obj = args[0];
			bool is_field = std::holds_alternative<FieldNodeType>(obj);
			
			if (!is_field)
				throw ParseError(".at() can only be called on field nodes");
			
			auto field = std::get<FieldNodeType>(obj);
			
			// Single index: .at(i)
			if (args.size() == 2)
			{
				if (std::holds_alternative<ConstNodeType>(args[1]))
				{
					// TODO: Implement field.at(const_index)
					// Requires understanding FieldNode's indexing API
					throw ParseError(".at() with constant index not yet implemented");
				}
				else
				{
					// TODO: Implement field.at(expr)
					throw ParseError(".at() with expression index not yet implemented");
				}
			}
			// Multi-index: .at(i, j, ...)
			else if (args.size() > 2)
			{
				// TODO: Implement multi-dimensional .at()
				throw ParseError("Multi-dimensional .at() not yet implemented");
			}
			else
			{
				throw ParseError(".at() requires at least 1 index argument");
			}
		}
		
		// .outer(n) or .o(n) member function
		if (func_name == "outer" || func_name == "o")
		{
			auto& obj = args[0];
			bool is_field = std::holds_alternative<FieldNodeType>(obj);
			
			if (!is_field)
				throw ParseError("." + func_name + "() can only be called on field nodes");
			
			if (args.size() != 2)
				throw ParseError("." + func_name + "() requires exactly 1 argument");
			
			auto field = std::get<FieldNodeType>(obj);
			
			if (std::holds_alternative<ConstNodeType>(args[1]))
			{
				// TODO: Implement field.outer(const_level)
				// Requires understanding FieldNode's outer scope navigation API
				throw ParseError("." + func_name + "() not yet implemented");
			}
			else
			{
				throw ParseError("." + func_name + "() requires constant argument");
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
			// FieldNode can be converted to FuncNode
			return ConvertToRttiFuncNode(std::get<FieldNodeType>(node));
		}
		
		if (std::holds_alternative<ConstNodeType>(node))
		{
			// ConstNode needs special handling
			// For now, wrap in a lambda that returns the constant
			auto const_node = std::get<ConstNodeType>(node);
			// TODO: This requires Container type in ConstNode or different approach
			// For now, throw error for standalone constants
			throw ParseError("Standalone constant expressions not yet supported (constant must be combined with field or expression)");
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

#endif // ADAPT_EVALUATOR_PARSER_V5_H
