#ifndef ADAPT_EVALUATOR_PARSER_V4_H
#define ADAPT_EVALUATOR_PARSER_V4_H

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <variant>
#include <stdexcept>
#include <cctype>
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
// マクロによる演算子/関数名の一元管理
// ========================================

// 二項演算子名
#define PARSER_BINARY_OPS \
	X(ADD, "+") \
	X(SUB, "-") \
	X(MUL, "*") \
	X(DIV, "/") \
	X(MOD, "%") \
	X(EQ, "==") \
	X(NE, "!=") \
	X(LT, "<") \
	X(LE, "<=") \
	X(GT, ">") \
	X(GE, ">=") \
	X(AND, "&&") \
	X(OR, "||") \
	X(BIT_AND, "&") \
	X(BIT_OR, "|") \
	X(BIT_XOR, "^") \
	X(SHL, "<<") \
	X(SHR, ">>")

// 単項演算子名
#define PARSER_UNARY_OPS \
	X(NEG, "-") \
	X(NOT, "!") \
	X(BIT_NOT, "~")

// 階層関数名
#define PARSER_LAYER_FUNCS \
	X(SIZE, "size") \
	X(EXIST, "exist") \
	X(COUNT, "count") \
	X(SUM, "sum") \
	X(MEAN, "mean") \
	X(DEV, "dev") \
	X(GREATEST, "greatest") \
	X(LEAST, "least")

// 通常関数名
#define PARSER_REGULAR_FUNCS \
	X(ABS, "abs") \
	X(SQRT, "sqrt") \
	X(POW, "pow") \
	X(SIN, "sin") \
	X(COS, "cos") \
	X(TAN, "tan") \
	X(EXP, "exp") \
	X(LOG, "log") \
	X(LOG10, "log10") \
	X(MIN, "min") \
	X(MAX, "max")

// メンバ関数名
#define PARSER_MEMBER_FUNCS \
	X(AT, "at") \
	X(OUTER, "outer") \
	X(O, "o")

// 演算子名定義の生成
namespace ops
{
	#define X(NAME, STR) inline constexpr const char* NAME = STR;
	PARSER_BINARY_OPS
	PARSER_UNARY_OPS
	#undef X
}

// 関数名定義の生成
namespace funcs
{
	#define X(NAME, STR) inline constexpr const char* NAME = STR;
	PARSER_LAYER_FUNCS
	PARSER_REGULAR_FUNCS
	PARSER_MEMBER_FUNCS
	#undef X
}

// ========================================
// トークナイザー
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
		
		// 数値
		if (std::isdigit(c) || (c == '.' && m_pos + 1 < m_expr.size() && std::isdigit(m_expr[m_pos + 1])))
		{
			return ParseNumber();
		}
		
		// 文字列
		if (c == '"')
		{
			return ParseString();
		}
		
		// 識別子
		if (std::isalpha(c) || c == '_')
		{
			return ParseIdentifier();
		}
		
		// 括弧・カンマ・ドット
		if (c == '(') { m_pos++; return {TokenType::LeftParen, "("}; }
		if (c == ')') { m_pos++; return {TokenType::RightParen, ")"}; }
		if (c == ',') { m_pos++; return {TokenType::Comma, ","}; }
		if (c == '.') { m_pos++; return {TokenType::Dot, "."}; }
		
		// 演算子
		if (IsOperatorChar(c))
		{
			return ParseOperator();
		}
		
		throw std::runtime_error("Unexpected character: " + std::string(1, c));
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
		while (m_pos < m_expr.size() && (std::isdigit(m_expr[m_pos]) || m_expr[m_pos] == '.'))
			m_pos++;
		return {TokenType::Number, std::string(m_expr.substr(start, m_pos - start))};
	}
	
	Token ParseString()
	{
		m_pos++; // skip opening quote
		size_t start = m_pos;
		while (m_pos < m_expr.size() && m_expr[m_pos] != '"')
		{
			if (m_expr[m_pos] == '\\' && m_pos + 1 < m_expr.size())
				m_pos += 2;
			else
				m_pos++;
		}
		if (m_pos >= m_expr.size())
			throw std::runtime_error("Unterminated string literal");
		std::string value(m_expr.substr(start, m_pos - start));
		m_pos++; // skip closing quote
		return {TokenType::String, value};
	}
	
	Token ParseIdentifier()
	{
		size_t start = m_pos;
		while (m_pos < m_expr.size() && (std::isalnum(m_expr[m_pos]) || m_expr[m_pos] == '_'))
			m_pos++;
		return {TokenType::Identifier, std::string(m_expr.substr(start, m_pos - start))};
	}
	
	Token ParseOperator()
	{
		// 2文字演算子のチェック
		if (m_pos + 1 < m_expr.size())
		{
			std::string two_char = std::string(m_expr.substr(m_pos, 2));
			if (two_char == "==" || two_char == "!=" || two_char == "<=" ||
				two_char == ">=" || two_char == "&&" || two_char == "||" ||
				two_char == "<<" || two_char == ">>")
			{
				m_pos += 2;
				return {TokenType::Operator, two_char};
			}
		}
		
		// 1文字演算子
		std::string op(1, m_expr[m_pos]);
		m_pos++;
		return {TokenType::Operator, op};
	}
	
	bool IsOperatorChar(char c)
	{
		return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
			   c == '=' || c == '!' || c == '<' || c == '>' ||
			   c == '&' || c == '|' || c == '^' || c == '~';
	}
	
	std::string_view m_expr;
	size_t m_pos;
};

// ========================================
// パーサーで扱うノードの型
// ========================================

template <class Container>
struct ParsedNode
{
	using FieldNodeType = eval::RttiFieldNode<typename Container::RttiPlaceholder>;
	using ConstNodeType = eval::RttiConstNode;
	using FuncNodeType = eval::RttiFuncNode<Container>;
	
	// バリアント：FieldNode、ConstNode、またはFuncNode
	// 重要：ConstNodeもFieldNodeと同様、仮想関数を回避するため直接保持する
	std::variant<FieldNodeType, ConstNodeType, FuncNodeType> node;
	
	// コンストラクタ
	ParsedNode(FieldNodeType fn) : node(std::move(fn)) {}
	ParsedNode(ConstNodeType cn) : node(std::move(cn)) {}
	ParsedNode(FuncNodeType fn) : node(std::move(fn)) {}
	
	// 型チェック
	bool IsFieldNode() const { return std::holds_alternative<FieldNodeType>(node); }
	bool IsConstNode() const { return std::holds_alternative<ConstNodeType>(node); }
	bool IsFuncNode() const { return std::holds_alternative<FuncNodeType>(node); }
	
	// 取得
	FieldNodeType& AsFieldNode() { return std::get<FieldNodeType>(node); }
	const FieldNodeType& AsFieldNode() const { return std::get<FieldNodeType>(node); }
	ConstNodeType& AsConstNode() { return std::get<ConstNodeType>(node); }
	const ConstNodeType& AsConstNode() const { return std::get<ConstNodeType>(node); }
	FuncNodeType& AsFuncNode() { return std::get<FuncNodeType>(node); }
	const FuncNodeType& AsFuncNode() const { return std::get<FuncNodeType>(node); }
	
	// 最終的な結果としてはRttiFuncNodeを返す
	// FieldNodeとConstNodeはここで初めてFuncNodeに変換される
	FuncNodeType ToFuncNode() &&
	{
		if (IsFuncNode())
		{
			return std::move(AsFuncNode());
		}
		else if (IsFieldNode())
		{
			return ConvertToRttiFuncNode(std::move(AsFieldNode()));
		}
		else // IsConstNode()
		{
			// ConstNodeをFuncNodeに変換
			// ConstNodeはContainerを持たないので、ラムダでラップする
			auto cn = std::move(AsConstNode());
			return FuncNodeType([cn = std::move(cn)](const Container&, const Bpos&) mutable
			{
				return std::move(cn);
			});
		}
	}
};

// ========================================
// パーサー本体
// ========================================

template <class Container>
class Parser
{
public:
	using NodeType = ParsedNode<Container>;
	using FieldNodeType = typename NodeType::FieldNodeType;
	using ConstNodeType = typename NodeType::ConstNodeType;
	using FuncNodeType = typename NodeType::FuncNodeType;
	using PlaceholderType = typename Container::RttiPlaceholder;
	
	Parser(const Container& container, std::string_view expr)
		: m_container(container), m_tokenizer(expr), m_current_token(m_tokenizer.NextToken())
	{
		// コンテナからフィールド名を抽出してプレースホルダーマップを構築
		ExtractFieldNames();
	}
	
	FuncNodeType Parse()
	{
		auto result = ParseExpression(0);
		if (m_current_token.type != TokenType::End)
		{
			throw std::runtime_error("Unexpected token after expression: " + m_current_token.value);
		}
		return std::move(result).ToFuncNode();
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
		else
		{
			// フィールド名の自動抽出ができない場合は空のマップ
			// 手動でフィールドを追加する必要がある
		}
	}
	
	void Advance()
	{
		m_current_token = m_tokenizer.NextToken();
	}
	
	// 演算子の優先順位（C++準拠）
	int GetPrecedence(const std::string& op)
	{
		// 優先順位13: 乗算・除算・剰余
		if (op == ops::MUL || op == ops::DIV || op == ops::MOD) return 13;
		// 優先順位12: 加算・減算
		if (op == ops::ADD || op == ops::SUB) return 12;
		// 優先順位11: ビットシフト
		if (op == ops::SHL || op == ops::SHR) return 11;
		// 優先順位9: 比較演算子
		if (op == ops::LT || op == ops::LE || op == ops::GT || op == ops::GE) return 9;
		// 優先順位8: 等価演算子
		if (op == ops::EQ || op == ops::NE) return 8;
		// 優先順位7: ビットAND
		if (op == ops::BIT_AND) return 7;
		// 優先順位6: ビットXOR
		if (op == ops::BIT_XOR) return 6;
		// 優先順位5: ビットOR
		if (op == ops::BIT_OR) return 5;
		// 優先順位4: 論理AND
		if (op == ops::AND) return 4;
		// 優先順位3: 論理OR
		if (op == ops::OR) return 3;
		
		return -1;
	}
	
	// 式のパース（演算子優先順位クライミング法）
	NodeType ParseExpression(int min_precedence)
	{
		auto left = ParseUnary();
		
		while (m_current_token.type == TokenType::Operator)
		{
			std::string op = m_current_token.value;
			int precedence = GetPrecedence(op);
			
			if (precedence < min_precedence)
				break;
			
			Advance();
			auto right = ParseExpression(precedence + 1);
			left = ApplyBinaryOperator(op, std::move(left), std::move(right));
		}
		
		return left;
	}
	
	// 単項演算子のパース
	NodeType ParseUnary()
	{
		if (m_current_token.type == TokenType::Operator)
		{
			std::string op = m_current_token.value;
			if (op == ops::SUB || op == ops::NOT || op == ops::BIT_NOT)
			{
				Advance();
				auto operand = ParseUnary();
				return ApplyUnaryOperator(op, std::move(operand));
			}
		}
		
		return ParsePostfix();
	}
	
	// 後置式のパース（メンバアクセス、関数呼び出しなど）
	NodeType ParsePostfix()
	{
		auto node = ParsePrimary();
		
		while (true)
		{
			if (m_current_token.type == TokenType::Dot)
			{
				// メンバ関数呼び出し: node.func(args...)
				Advance();
				if (m_current_token.type != TokenType::Identifier)
					throw std::runtime_error("Expected member function name after '.'");
				
				std::string member_name = m_current_token.value;
				Advance();
				
				if (m_current_token.type == TokenType::LeftParen)
				{
					auto args = ParseFunctionArgs();
					node = ApplyMemberFunction(member_name, std::move(node), std::move(args));
				}
				else
				{
					throw std::runtime_error("Expected '(' after member function name");
				}
			}
			else
			{
				break;
			}
		}
		
		return node;
	}
	
	// プライマリ式のパース
	NodeType ParsePrimary()
	{
		// 括弧
		if (m_current_token.type == TokenType::LeftParen)
		{
			Advance();
			auto node = ParseExpression(0);
			if (m_current_token.type != TokenType::RightParen)
				throw std::runtime_error("Expected ')'");
			Advance();
			return node;
		}
		
		// 数値リテラル
		if (m_current_token.type == TokenType::Number)
		{
			return ParseNumberLiteral();
		}
		
		// 文字列リテラル
		if (m_current_token.type == TokenType::String)
		{
			return ParseStringLiteral();
		}
		
		// 識別子（プレースホルダー、関数、etc.）
		if (m_current_token.type == TokenType::Identifier)
		{
			std::string name = m_current_token.value;
			Advance();
			
			// 関数呼び出し
			if (m_current_token.type == TokenType::LeftParen)
			{
				auto args = ParseFunctionArgs();
				return ApplyFunction(name, std::move(args));
			}
			
			// プレースホルダー
			auto it = m_field_map.find(name);
			if (it != m_field_map.end())
			{
				return NodeType(FieldNodeType(PlaceholderType::Get(it->second)));
			}
			
			throw std::runtime_error("Unknown identifier: " + name);
		}
		
		throw std::runtime_error("Unexpected token: " + m_current_token.value);
	}
	
	// 関数の引数をパース
	std::vector<NodeType> ParseFunctionArgs()
	{
		std::vector<NodeType> args;
		
		Advance(); // skip '('
		
		if (m_current_token.type == TokenType::RightParen)
		{
			Advance();
			return args;
		}
		
		args.push_back(ParseExpression(0));
		
		while (m_current_token.type == TokenType::Comma)
		{
			Advance();
			args.push_back(ParseExpression(0));
		}
		
		if (m_current_token.type != TokenType::RightParen)
			throw std::runtime_error("Expected ')' after function arguments");
		Advance();
		
		return args;
	}
	
	// 数値リテラルをパース
	NodeType ParseNumberLiteral()
	{
		std::string value = m_current_token.value;
		Advance();
		
		// 型接尾辞をチェック
		FieldType type = FieldType::Emp;
		if (m_current_token.type == TokenType::Identifier)
		{
			std::string suffix = m_current_token.value;
			
			if (suffix == "i08") type = FieldType::I08;
			else if (suffix == "i16") type = FieldType::I16;
			else if (suffix == "i32") type = FieldType::I32;
			else if (suffix == "i64") type = FieldType::I64;
			else if (suffix == "f32") type = FieldType::F32;
			else if (suffix == "f64") type = FieldType::F64;
			
			if (type != FieldType::Emp)
				Advance();
		}
		
		bool is_float = value.find('.') != std::string::npos;
		if (type == FieldType::Emp)
			type = is_float ? FieldType::F64 : FieldType::I32;
		
		try
		{
			switch (type)
			{
			case FieldType::I08:
				return NodeType(ConstNodeType(static_cast<int8_t>(std::stoi(value))));
			case FieldType::I16:
				return NodeType(ConstNodeType(static_cast<int16_t>(std::stoi(value))));
			case FieldType::I32:
				return NodeType(ConstNodeType(static_cast<int32_t>(std::stoi(value))));
			case FieldType::I64:
				return NodeType(ConstNodeType(static_cast<int64_t>(std::stoll(value))));
			case FieldType::F32:
				return NodeType(ConstNodeType(static_cast<float>(std::stof(value))));
			case FieldType::F64:
				return NodeType(ConstNodeType(std::stod(value)));
			default:
				throw std::runtime_error("Unsupported literal type");
			}
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error("Failed to parse number literal: " + value);
		}
	}
	
	// 文字列リテラルをパース
	NodeType ParseStringLiteral()
	{
		std::string value = m_current_token.value;
		Advance();
		return NodeType(ConstNodeType(std::move(value)));
	}
	
	// ========================================
	// 演算子適用（マクロで一元管理）
	// ========================================
	
	// 二項演算子を適用
	// 全ての組み合わせを明示的に処理（9通り: 3x3）
	NodeType ApplyBinaryOperator(const std::string& op, NodeType left, NodeType right)
	{
		// 特殊ケース: 両方がConstNodeの場合、最初のものをFuncNodeに変換
		// これによりExtractContainerがContainerを見つけられる
		if (left.IsConstNode() && right.IsConstNode())
		{
			auto cn = std::move(left.AsConstNode());
			left = NodeType(FuncNodeType([cn = std::move(cn)](const Container&, const Bpos&) mutable
			{
				return std::move(cn);
			}));
		}
		
		// マクロで演算子適用を定義
		#define APPLY_BINARY_OP(OP_CONST, OP_SYM) \
			if (op == ops::OP_CONST) { \
				if (left.IsFieldNode() && right.IsFieldNode()) { \
					return NodeType(left.AsFieldNode() OP_SYM right.AsFieldNode()); \
				} else if (left.IsFieldNode() && right.IsConstNode()) { \
					return NodeType(left.AsFieldNode() OP_SYM right.AsConstNode()); \
				} else if (left.IsFieldNode() && right.IsFuncNode()) { \
					return NodeType(left.AsFieldNode() OP_SYM right.AsFuncNode()); \
				} else if (left.IsConstNode() && right.IsFieldNode()) { \
					return NodeType(left.AsConstNode() OP_SYM right.AsFieldNode()); \
				} else if (left.IsConstNode() && right.IsFuncNode()) { \
					return NodeType(left.AsConstNode() OP_SYM right.AsFuncNode()); \
				} else if (left.IsFuncNode() && right.IsFieldNode()) { \
					return NodeType(left.AsFuncNode() OP_SYM right.AsFieldNode()); \
				} else if (left.IsFuncNode() && right.IsConstNode()) { \
					return NodeType(left.AsFuncNode() OP_SYM right.AsConstNode()); \
				} else { \
					return NodeType(left.AsFuncNode() OP_SYM right.AsFuncNode()); \
				} \
			}
		
		APPLY_BINARY_OP(ADD, +)
		APPLY_BINARY_OP(SUB, -)
		APPLY_BINARY_OP(MUL, *)
		APPLY_BINARY_OP(DIV, /)
		APPLY_BINARY_OP(MOD, %)
		APPLY_BINARY_OP(EQ, ==)
		APPLY_BINARY_OP(NE, !=)
		APPLY_BINARY_OP(LT, <)
		APPLY_BINARY_OP(LE, <=)
		APPLY_BINARY_OP(GT, >)
		APPLY_BINARY_OP(GE, >=)
		APPLY_BINARY_OP(AND, &&)
		APPLY_BINARY_OP(OR, ||)
		APPLY_BINARY_OP(BIT_AND, &)
		APPLY_BINARY_OP(BIT_OR, |)
		APPLY_BINARY_OP(BIT_XOR, ^)
		APPLY_BINARY_OP(SHL, <<)
		APPLY_BINARY_OP(SHR, >>)
		
		#undef APPLY_BINARY_OP
		
		throw std::runtime_error("Unknown binary operator: " + op);
	}
	
	// 単項演算子を適用
	NodeType ApplyUnaryOperator(const std::string& op, NodeType operand)
	{
		#define APPLY_UNARY_OP(OP_CONST, OP_SYM) \
			if (op == ops::OP_CONST) { \
				if (operand.IsFieldNode()) { \
					return NodeType(OP_SYM operand.AsFieldNode()); \
				} else if (operand.IsConstNode()) { \
					return NodeType(OP_SYM operand.AsConstNode()); \
				} else { \
					return NodeType(OP_SYM operand.AsFuncNode()); \
				} \
			}
		
		APPLY_UNARY_OP(NEG, -)
		APPLY_UNARY_OP(NOT, !)
		APPLY_UNARY_OP(BIT_NOT, ~)
		
		#undef APPLY_UNARY_OP
		
		throw std::runtime_error("Unknown unary operator: " + op);
	}
	
	// ========================================
	// 関数適用（マクロで一元管理）
	// ========================================
	
	// 関数を適用
	NodeType ApplyFunction(const std::string& func_name, std::vector<NodeType> args)
	{
		// 階層関数のチェック
		if (IsLayerFunction(func_name))
		{
			if (args.size() != 1)
				throw std::runtime_error("Layer function requires exactly 1 argument: " + func_name);
			
			// 数値付き階層関数（例: mean2, sum3）のチェック
			if (func_name.size() > 1 && std::isdigit(func_name.back()))
			{
				size_t num_start = func_name.size() - 1;
				while (num_start > 0 && std::isdigit(func_name[num_start - 1]))
					num_start--;
				
				std::string base_name = func_name.substr(0, num_start);
				int level = std::stoi(func_name.substr(num_start));
				
				return ApplyLayerFunctionWithLevel(base_name, std::move(args[0]), level);
			}
			
			return ApplyLayerFunction(func_name, std::move(args[0]));
		}
		
		// 通常の関数（abs, sqrt, pow, etc.）
		if (IsRegularFunction(func_name))
		{
			return ApplyRegularFunction(func_name, std::move(args));
		}
		
		throw std::runtime_error("Unknown function: " + func_name);
	}
	
	// 階層関数かどうかをチェック
	bool IsLayerFunction(const std::string& name) const
	{
		// 数値付き関数名から基本名を抽出
		std::string base_name = name;
		if (!name.empty() && std::isdigit(name.back()))
		{
			size_t i = name.size() - 1;
			while (i > 0 && std::isdigit(name[i - 1]))
				i--;
			base_name = name.substr(0, i);
		}
		
		#define X(NAME, STR) if (base_name == funcs::NAME) return true;
		PARSER_LAYER_FUNCS
		#undef X
		return false;
	}
	
	// 通常の関数かどうかをチェック
	bool IsRegularFunction(const std::string& name) const
	{
		#define X(NAME, STR) if (name == funcs::NAME) return true;
		PARSER_REGULAR_FUNCS
		#undef X
		return false;
	}
	
	// 階層関数を適用
	NodeType ApplyLayerFunction(const std::string& func_name, NodeType arg)
	{
		return ApplyLayerFunctionWithLevel(func_name, std::move(arg), 1);
	}
	
	// レベル付き階層関数を適用
	NodeType ApplyLayerFunctionWithLevel(const std::string& func_name, NodeType arg, int level)
	{
		// ConstNodeは階層関数では使えない
		if (arg.IsConstNode())
			throw std::runtime_error("Cannot apply layer function to constant literal");
		
		// マクロで階層関数適用を定義
		#define APPLY_LAYER_FUNC(FUNC_CONST, FUNC_NAME, LVL) \
			if (func_name == funcs::FUNC_CONST && level == LVL) { \
				if (arg.IsFieldNode()) { \
					return NodeType(eval::FUNC_NAME##LVL(arg.AsFieldNode())); \
				} else { \
					return NodeType(eval::FUNC_NAME##LVL(arg.AsFuncNode())); \
				} \
			}
		
		// 各階層関数をレベル1-10で展開
		#define APPLY_LAYER_FUNC_ALL_LEVELS(FUNC_CONST, FUNC_NAME) \
			APPLY_LAYER_FUNC(FUNC_CONST, FUNC_NAME, 1) \
			APPLY_LAYER_FUNC(FUNC_CONST, FUNC_NAME, 2) \
			APPLY_LAYER_FUNC(FUNC_CONST, FUNC_NAME, 3) \
			APPLY_LAYER_FUNC(FUNC_CONST, FUNC_NAME, 4) \
			APPLY_LAYER_FUNC(FUNC_CONST, FUNC_NAME, 5) \
			APPLY_LAYER_FUNC(FUNC_CONST, FUNC_NAME, 6) \
			APPLY_LAYER_FUNC(FUNC_CONST, FUNC_NAME, 7) \
			APPLY_LAYER_FUNC(FUNC_CONST, FUNC_NAME, 8) \
			APPLY_LAYER_FUNC(FUNC_CONST, FUNC_NAME, 9) \
			APPLY_LAYER_FUNC(FUNC_CONST, FUNC_NAME, 10)
		
		APPLY_LAYER_FUNC_ALL_LEVELS(SIZE, size)
		APPLY_LAYER_FUNC_ALL_LEVELS(EXIST, exist)
		APPLY_LAYER_FUNC_ALL_LEVELS(COUNT, count)
		APPLY_LAYER_FUNC_ALL_LEVELS(SUM, sum)
		APPLY_LAYER_FUNC_ALL_LEVELS(MEAN, mean)
		APPLY_LAYER_FUNC_ALL_LEVELS(DEV, dev)
		APPLY_LAYER_FUNC_ALL_LEVELS(GREATEST, greatest)
		APPLY_LAYER_FUNC_ALL_LEVELS(LEAST, least)
		
		#undef APPLY_LAYER_FUNC_ALL_LEVELS
		#undef APPLY_LAYER_FUNC
		
		throw std::runtime_error("Unknown or unsupported layer function: " + func_name);
	}
	
	// 通常の関数を適用（abs, sqrt, pow, etc.）
	NodeType ApplyRegularFunction(const std::string& func_name, std::vector<NodeType> args)
	{
		// 1引数関数
		if (args.size() == 1)
		{
			auto& arg = args[0];
			
			#define APPLY_REGULAR_FUNC_1ARG(FUNC_CONST, FUNC_NAME) \
				if (func_name == funcs::FUNC_CONST) { \
					if (arg.IsFieldNode()) { \
						return NodeType(eval::FUNC_NAME(arg.AsFieldNode())); \
					} else if (arg.IsConstNode()) { \
						return NodeType(eval::FUNC_NAME(arg.AsConstNode())); \
					} else { \
						return NodeType(eval::FUNC_NAME(arg.AsFuncNode())); \
					} \
				}
			
			APPLY_REGULAR_FUNC_1ARG(ABS, abs)
			APPLY_REGULAR_FUNC_1ARG(SQRT, sqrt)
			APPLY_REGULAR_FUNC_1ARG(SIN, sin)
			APPLY_REGULAR_FUNC_1ARG(COS, cos)
			APPLY_REGULAR_FUNC_1ARG(TAN, tan)
			APPLY_REGULAR_FUNC_1ARG(EXP, exp)
			APPLY_REGULAR_FUNC_1ARG(LOG, log)
			APPLY_REGULAR_FUNC_1ARG(LOG10, log10)
			
			#undef APPLY_REGULAR_FUNC_1ARG
		}
		
		// 2引数関数
		if (args.size() == 2)
		{
			auto& arg1 = args[0];
			auto& arg2 = args[1];
			
			// 特殊ケース: 両方がConstNodeの場合、最初のものをFuncNodeに変換
			if (arg1.IsConstNode() && arg2.IsConstNode())
			{
				auto cn = std::move(arg1.AsConstNode());
				arg1 = NodeType(FuncNodeType([cn = std::move(cn)](const Container&, const Bpos&) mutable
				{
					return std::move(cn);
				}));
			}
			
			#define APPLY_REGULAR_FUNC_2ARG(FUNC_CONST, FUNC_NAME) \
				if (func_name == funcs::FUNC_CONST) { \
					if (arg1.IsFieldNode() && arg2.IsFieldNode()) { \
						return NodeType(eval::FUNC_NAME(arg1.AsFieldNode(), arg2.AsFieldNode())); \
					} else if (arg1.IsFieldNode() && arg2.IsConstNode()) { \
						return NodeType(eval::FUNC_NAME(arg1.AsFieldNode(), arg2.AsConstNode())); \
					} else if (arg1.IsFieldNode() && arg2.IsFuncNode()) { \
						return NodeType(eval::FUNC_NAME(arg1.AsFieldNode(), arg2.AsFuncNode())); \
					} else if (arg1.IsConstNode() && arg2.IsFieldNode()) { \
						return NodeType(eval::FUNC_NAME(arg1.AsConstNode(), arg2.AsFieldNode())); \
					} else if (arg1.IsConstNode() && arg2.IsFuncNode()) { \
						return NodeType(eval::FUNC_NAME(arg1.AsConstNode(), arg2.AsFuncNode())); \
					} else if (arg1.IsFuncNode() && arg2.IsFieldNode()) { \
						return NodeType(eval::FUNC_NAME(arg1.AsFuncNode(), arg2.AsFieldNode())); \
					} else if (arg1.IsFuncNode() && arg2.IsConstNode()) { \
						return NodeType(eval::FUNC_NAME(arg1.AsFuncNode(), arg2.AsConstNode())); \
					} else { \
						return NodeType(eval::FUNC_NAME(arg1.AsFuncNode(), arg2.AsFuncNode())); \
					} \
				}
			
			APPLY_REGULAR_FUNC_2ARG(POW, pow)
			APPLY_REGULAR_FUNC_2ARG(MIN, min)
			APPLY_REGULAR_FUNC_2ARG(MAX, max)
			
			#undef APPLY_REGULAR_FUNC_2ARG
		}
		
		throw std::runtime_error("Unsupported function or argument count: " + func_name);
	}
	
	// メンバ関数を適用（.at(), .outer(), etc.）
	NodeType ApplyMemberFunction(const std::string& member_name, NodeType object, std::vector<NodeType> args)
	{
		// .at() メンバ関数
		if (member_name == funcs::AT)
		{
			// objectはFieldNodeまたはFuncNodeのはず
			if (object.IsConstNode())
				throw std::runtime_error("Cannot call .at() on constant");
			
			// 引数は整数のConstNodeまたは式
			if (args.size() == 1)
			{
				// 1次元アクセス: .at(i)
				auto& idx = args[0];
				if (object.IsFieldNode())
				{
					if (idx.IsConstNode())
						return NodeType(object.AsFieldNode().at(idx.AsConstNode()));
					else if (idx.IsFieldNode())
						return NodeType(object.AsFieldNode().at(idx.AsFieldNode()));
					else
						return NodeType(object.AsFieldNode().at(idx.AsFuncNode()));
				}
				else // FuncNode
				{
					if (idx.IsConstNode())
						return NodeType(object.AsFuncNode().at(idx.AsConstNode()));
					else if (idx.IsFieldNode())
						return NodeType(object.AsFuncNode().at(idx.AsFieldNode()));
					else
						return NodeType(object.AsFuncNode().at(idx.AsFuncNode()));
				}
			}
			else if (args.size() == 2)
			{
				// 2次元アクセス: .at(i, j)
				auto& idx1 = args[0];
				auto& idx2 = args[1];
				
				// 全ての組み合わせを処理（簡略化のため、ConstNodeの場合のみ実装）
				if (object.IsFieldNode() && idx1.IsConstNode() && idx2.IsConstNode())
				{
					return NodeType(object.AsFieldNode().at(idx1.AsConstNode(), idx2.AsConstNode()));
				}
				// その他の組み合わせも同様に実装可能だが、ここでは省略
				throw std::runtime_error(".at(i, j) with non-constant indices not yet supported");
			}
			
			throw std::runtime_error(".at() requires 1 or 2 arguments");
		}
		
		// .outer() または .o() メンバ関数
		if (member_name == funcs::OUTER || member_name == funcs::O)
		{
			if (object.IsConstNode())
				throw std::runtime_error("Cannot call .outer() on constant");
			
			if (args.size() != 1)
				throw std::runtime_error(".outer() requires exactly 1 argument");
			
			auto& depth = args[0];
			if (!depth.IsConstNode())
				throw std::runtime_error(".outer() argument must be a constant");
			
			// depth.AsConstNode()から整数値を取得
			// 簡略化のため、I32と仮定
			if (!depth.AsConstNode().IsI32())
				throw std::runtime_error(".outer() argument must be an integer");
			
			// RttiConstNodeからI32の値を取得する方法がないので、ここでは仮実装
			// 実際にはEvaluate()を使って値を取得する必要がある
			throw std::runtime_error(".outer() not yet fully implemented");
		}
		
		throw std::runtime_error("Unknown member function: " + member_name);
	}
	
	const Container& m_container;
	Tokenizer m_tokenizer;
	Token m_current_token;
	std::unordered_map<std::string, size_t> m_field_map;
};

// ========================================
// パーサーのエントリーポイント
// ========================================

template <class Container>
auto ParseRttiFuncNode(const Container& container, std::string_view expr)
{
	Parser<Container> parser(container, expr);
	return parser.Parse();
}

} // namespace parser

} // namespace eval

} // namespace adapt

#endif // ADAPT_EVALUATOR_PARSER_V4_H
