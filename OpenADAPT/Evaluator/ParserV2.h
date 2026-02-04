#ifndef ADAPT_EVALUATOR_PARSER_V2_H
#define ADAPT_EVALUATOR_PARSER_V2_H

#include <string>
#include <string_view>
#include <map>
#include <vector>
#include <cctype>
#include <stdexcept>
#include <functional>
#include <OpenADAPT/Evaluator/FuncNode.h>
#include <OpenADAPT/Evaluator/Function.h>
#include <OpenADAPT/Evaluator/LayerFuncNode.h>
#include <OpenADAPT/Evaluator/Placeholder.h>

namespace adapt
{

namespace eval
{

namespace parser
{

// トークンの種類
enum class TokenType
{
	Number,        // 数値定数
	String,        // 文字列定数
	Identifier,    // プレースホルダ名または関数名
	Operator,      // 演算子
	LeftParen,     // (
	RightParen,    // )
	Comma,         // ,
	Dot,           // .
	End            // 終端
};

// トークン
struct Token
{
	TokenType type;
	std::string value;
	int precedence;  // 演算子の優先順位（C++準拠）
	
	Token() : type(TokenType::End), value(""), precedence(0) {}
	Token(TokenType t, std::string v = "", int prec = 0)
		: type(t), value(std::move(v)), precedence(prec) {}
};

// C++の演算子優先順位テーブル（高い順）
// https://en.cppreference.com/w/cpp/language/operator_precedence
inline int GetPrecedence(std::string_view op)
{
	// 優先順位15: 単項演算子は別途扱う
	// 優先順位14: 範囲外
	// 優先順位13: 乗除余
	if (op == "*" || op == "/" || op == "%") return 13;
	// 優先順位12: 加減
	if (op == "+" || op == "-") return 12;
	// 優先順位11: ビットシフト
	if (op == "<<" || op == ">>") return 11;
	// 優先順位10: 比較演算子
	if (op == "<" || op == "<=" || op == ">" || op == ">=") return 10;
	// 優先順位9: 等価演算子
	if (op == "==" || op == "!=") return 9;
	// 優先順位8: ビット積
	if (op == "&") return 8;
	// 優先順位7: ビット排他的論理和
	if (op == "^") return 7;
	// 優先順位6: ビット和
	if (op == "|") return 6;
	// 優先順位5: 論理積
	if (op == "&&") return 5;
	// 優先順位4: 論理和
	if (op == "||") return 4;
	// 優先順位3以下は未サポート
	return 0;
}

// 演算子・関数名の中央管理
namespace ops
{
	// 二項演算子
	constexpr const char* ADD = "+";
	constexpr const char* SUB = "-";
	constexpr const char* MUL = "*";
	constexpr const char* DIV = "/";
	constexpr const char* MOD = "%";
	constexpr const char* EQ = "==";
	constexpr const char* NE = "!=";
	constexpr const char* LT = "<";
	constexpr const char* LE = "<=";
	constexpr const char* GT = ">";
	constexpr const char* GE = ">=";
	constexpr const char* AND = "&&";
	constexpr const char* OR = "||";
	constexpr const char* BIT_AND = "&";
	constexpr const char* BIT_OR = "|";
	constexpr const char* BIT_XOR = "^";
	constexpr const char* SHL = "<<";
	constexpr const char* SHR = ">>";
	
	// 単項演算子
	constexpr const char* NEG = "-";  // 単項マイナス
	constexpr const char* NOT = "!";
	constexpr const char* BIT_NOT = "~";
	
	// プレースホルダメソッド
	constexpr const char* AT = "at";
	constexpr const char* OUTER = "outer";
	constexpr const char* O = "o";  // outerの省略形
	
	// 階層関数名
	constexpr const char* SIZE = "size";
	constexpr const char* EXIST = "exist";
	constexpr const char* COUNT = "count";
	constexpr const char* SUM = "sum";
	constexpr const char* MEAN = "mean";
	constexpr const char* DEV = "dev";
	constexpr const char* GREATEST = "greatest";
	constexpr const char* LEAST = "least";
	constexpr const char* ISGREATEST = "isgreatest";
	constexpr const char* ISLEAST = "isleast";
	constexpr const char* BPOS_GREATEST = "bpos_greatest";
	constexpr const char* BPOS_LEAST = "bpos_least";
	
	// 階層関数（条件付き）
	constexpr const char* SUM_IF = "sum_if";
	constexpr const char* MEAN_IF = "mean_if";
	constexpr const char* DEV_IF = "dev_if";
	constexpr const char* GREATEST_IF = "greatest_if";
	constexpr const char* LEAST_IF = "least_if";
	constexpr const char* ISGREATEST_IF = "isgreatest_if";
	constexpr const char* ISLEAST_IF = "isleast_if";
	constexpr const char* BPOS_GREATEST_IF = "bpos_greatest_if";
	constexpr const char* BPOS_LEAST_IF = "bpos_least_if";
	
	// ユーティリティ関数
	constexpr const char* CAST_I08 = "cast_i08";
	constexpr const char* CAST_I16 = "cast_i16";
	constexpr const char* CAST_I32 = "cast_i32";
	constexpr const char* CAST_I64 = "cast_i64";
	constexpr const char* CAST_F32 = "cast_f32";
	constexpr const char* CAST_F64 = "cast_f64";
	constexpr const char* TOSTR = "tostr";
	
	// 通常の数学関数
	constexpr const char* ABS = "abs";
	constexpr const char* SQRT = "sqrt";
	constexpr const char* POW = "pow";
	constexpr const char* EXP = "exp";
	constexpr const char* LOG = "log";
	constexpr const char* SIN = "sin";
	constexpr const char* COS = "cos";
	constexpr const char* TAN = "tan";
}

// 字句解析器（トークナイザー）
class Tokenizer
{
public:
	explicit Tokenizer(std::string_view expr) : m_expr(expr), m_pos(0) {}
	
	Token NextToken()
	{
		SkipWhitespace();
		
		if (m_pos >= m_expr.size())
			return Token(TokenType::End);
		
		char c = m_expr[m_pos];
		
		// 括弧
		if (c == '(')
		{
			m_pos++;
			return Token(TokenType::LeftParen, "(");
		}
		if (c == ')')
		{
			m_pos++;
			return Token(TokenType::RightParen, ")");
		}
		
		// カンマ
		if (c == ',')
		{
			m_pos++;
			return Token(TokenType::Comma, ",");
		}
		
		// ドット
		if (c == '.')
		{
			m_pos++;
			return Token(TokenType::Dot, ".");
		}
		
		// 文字列リテラル（将来の拡張用）
		if (c == '"' || c == '\'')
		{
			return ParseString();
		}
		
		// 数値（将来の拡張用）
		if (std::isdigit(c) || (c == '.' && m_pos + 1 < m_expr.size() && std::isdigit(m_expr[m_pos + 1])))
		{
			return ParseNumber();
		}
		
		// 演算子
		if (IsOperatorChar(c))
		{
			return ParseOperator();
		}
		
		// 識別子（プレースホルダ名または関数名）
		if (std::isalpha(c) || c == '_')
		{
			return ParseIdentifier();
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
			else
			{
				break;
			}
		}
		
		return Token(TokenType::Number, std::string(m_expr.substr(start, m_pos - start)));
	}
	
	Token ParseString()
	{
		char quote = m_expr[m_pos];
		m_pos++; // skip opening quote
		size_t start = m_pos;
		
		while (m_pos < m_expr.size() && m_expr[m_pos] != quote)
		{
			if (m_expr[m_pos] == '\\' && m_pos + 1 < m_expr.size())
				m_pos += 2; // skip escape sequence
			else
				m_pos++;
		}
		
		if (m_pos >= m_expr.size())
			throw std::runtime_error("Unterminated string literal");
		
		std::string value(m_expr.substr(start, m_pos - start));
		m_pos++; // skip closing quote
		return Token(TokenType::String, value);
	}
	
	Token ParseIdentifier()
	{
		size_t start = m_pos;
		while (m_pos < m_expr.size() &&
			   (std::isalnum(m_expr[m_pos]) || m_expr[m_pos] == '_'))
		{
			m_pos++;
		}
		return Token(TokenType::Identifier, std::string(m_expr.substr(start, m_pos - start)));
	}
	
	Token ParseOperator()
	{
		size_t start = m_pos;
		
		// 2文字演算子のチェック
		if (m_pos + 1 < m_expr.size())
		{
			std::string two_char = std::string(m_expr.substr(m_pos, 2));
			if (two_char == "==" || two_char == "!=" || two_char == "<=" ||
				two_char == ">=" || two_char == "&&" || two_char == "||" ||
				two_char == "<<" || two_char == ">>")
			{
				m_pos += 2;
				return Token(TokenType::Operator, two_char, GetPrecedence(two_char));
			}
		}
		
		// 1文字演算子
		char c = m_expr[m_pos++];
		std::string op(1, c);
		return Token(TokenType::Operator, op, GetPrecedence(op));
	}
	
	bool IsOperatorChar(char c) const
	{
		return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
			   c == '=' || c == '!' || c == '<' || c == '>' ||
			   c == '&' || c == '|' || c == '^' || c == '~';
	}
	
	std::string_view m_expr;
	size_t m_pos;
};

// パーサー（構文解析器）
template <class Container>
class Parser
{
public:
	using PlaceholderType = typename Container::RttiPlaceholder;
	using NodeType = eval::RttiFuncNode<Container>;
	
	Parser(const Container& container, std::string_view expr)
		: m_container(container), m_tokenizer(expr)
	{
		m_current_token = m_tokenizer.NextToken();
		InitPlaceholders();
	}
	
	// 式をパースしてRttiFuncNodeを生成
	NodeType Parse()
	{
		auto result = ParseExpression(0);
		if (m_current_token.type != TokenType::End)
			throw std::runtime_error("Unexpected token after expression");
		return result;
	}
	
private:
	void InitPlaceholders()
	{
		// コンテナから全フィールド名を取得してプレースホルダマップを構築
		if constexpr (requires { Container::GetFieldNames(); })
		{
			// SHierarchyの場合（静的）
			constexpr auto names = Container::GetFieldNames();
			InitPlaceholdersFromTuple(names, std::make_index_sequence<std::tuple_size_v<decltype(names)>>{});
		}
		else if constexpr (requires { m_container.GetFieldNamesIn(LayerType{}); })
		{
			// DHierarchyの場合（動的）
			// すべての層からフィールド名を取得
			LayerType max_layer = m_container.GetMaxLayer();
			for (LayerType layer = -1; layer <= max_layer; ++layer)
			{
				auto names = m_container.GetFieldNamesIn(layer);
				auto phs = m_container.GetPlaceholdersIn(layer);
				for (size_t i = 0; i < names.size(); ++i)
				{
					m_placeholders[names[i]] = phs[i];
				}
			}
		}
		else
		{
			throw std::runtime_error("Container does not support field name retrieval");
		}
	}
	
	template <class Tuple, size_t... Is>
	void InitPlaceholdersFromTuple(const Tuple& names, std::index_sequence<Is...>)
	{
		([&]
		{
			std::string name(std::get<Is>(names).GetString());
			auto ph = Container::template GetPlaceholder<std::get<Is>(names)>();
			m_placeholders[name] = ph;
		}(), ...);
	}
	
	// 式をパース（演算子の優先順位を考慮）
	NodeType ParseExpression(int min_precedence)
	{
		auto left = ParseUnary();
		
		while (m_current_token.type == TokenType::Operator &&
			   m_current_token.precedence >= min_precedence)
		{
			Token op = m_current_token;
			Advance();
			
			auto right = ParseExpression(op.precedence + 1);
			left = ApplyBinaryOperator(op.value, std::move(left), std::move(right));
		}
		
		return left;
	}
	
	// 単項演算子をパース
	NodeType ParseUnary()
	{
		// 単項演算子
		if (m_current_token.type == TokenType::Operator &&
			(m_current_token.value == ops::NEG || m_current_token.value == ops::NOT || 
			 m_current_token.value == ops::BIT_NOT))
		{
			Token op = m_current_token;
			Advance();
			auto operand = ParseUnary();
			return ApplyUnaryOperator(op.value, std::move(operand));
		}
		
		return ParsePostfix();
	}
	
	// 後置演算子をパース（関数呼び出し、メンバアクセスなど）
	NodeType ParsePostfix()
	{
		auto node = ParsePrimary();
		
		while (true)
		{
			if (m_current_token.type == TokenType::Dot)
			{
				// メンバ関数呼び出し: node.method(args...)
				Advance();
				if (m_current_token.type != TokenType::Identifier)
					throw std::runtime_error("Expected method name after '.'");
				
				std::string method_name = m_current_token.value;
				Advance();
				
				if (m_current_token.type != TokenType::LeftParen)
					throw std::runtime_error("Expected '(' after method name");
				
				node = ParseMemberFunction(std::move(node), method_name);
			}
			else
			{
				break;
			}
		}
		
		return node;
	}
	
	// 一次式をパース
	NodeType ParsePrimary()
	{
		// 括弧式
		if (m_current_token.type == TokenType::LeftParen)
		{
			Advance();
			auto result = ParseExpression(0);
			if (m_current_token.type != TokenType::RightParen)
				throw std::runtime_error("Expected ')'");
			Advance();
			return result;
		}
		
		// 識別子（プレースホルダまたは関数）
		if (m_current_token.type == TokenType::Identifier)
		{
			std::string name = m_current_token.value;
			Advance();
			
			// 関数呼び出しかチェック
			if (m_current_token.type == TokenType::LeftParen)
			{
				return ParseFunctionCall(name);
			}
			
			// プレースホルダ
			auto it = m_placeholders.find(name);
			if (it == m_placeholders.end())
				throw std::runtime_error("Unknown identifier: " + name);
			
			return ConvertToRttiFuncNode(eval::RttiFieldNode{ it->second });
		}
		
		// 数値・文字列リテラルは将来の拡張用
		if (m_current_token.type == TokenType::Number)
		{
			throw std::runtime_error("Number literals are not yet supported");
		}
		
		if (m_current_token.type == TokenType::String)
		{
			throw std::runtime_error("String literals are not yet supported");
		}
		
		throw std::runtime_error("Unexpected token in primary expression");
	}
	
	// 関数呼び出しをパース
	NodeType ParseFunctionCall(const std::string& func_name)
	{
		Advance(); // skip '('
		
		std::vector<NodeType> args;
		
		// 引数リストをパース
		if (m_current_token.type != TokenType::RightParen)
		{
			while (true)
			{
				args.push_back(ParseExpression(0));
				
				if (m_current_token.type == TokenType::Comma)
				{
					Advance();
				}
				else if (m_current_token.type == TokenType::RightParen)
				{
					break;
				}
				else
				{
					throw std::runtime_error("Expected ',' or ')' in function call");
				}
			}
		}
		
		Advance(); // skip ')'
		
		return ApplyFunction(func_name, std::move(args));
	}
	
	// メンバ関数呼び出しをパース
	NodeType ParseMemberFunction(NodeType node, const std::string& method_name)
	{
		Advance(); // skip '('
		
		std::vector<NodeType> args;
		
		// 引数リストをパース
		if (m_current_token.type != TokenType::RightParen)
		{
			while (true)
			{
				args.push_back(ParseExpression(0));
				
				if (m_current_token.type == TokenType::Comma)
				{
					Advance();
				}
				else if (m_current_token.type == TokenType::RightParen)
				{
					break;
				}
				else
				{
					throw std::runtime_error("Expected ',' or ')' in method call");
				}
			}
		}
		
		Advance(); // skip ')'
		
		return ApplyMemberFunction(std::move(node), method_name, std::move(args));
	}
	
	// 二項演算子を適用
	NodeType ApplyBinaryOperator(const std::string& op, NodeType left, NodeType right)
	{
		if (op == ops::ADD) return left + right;
		if (op == ops::SUB) return left - right;
		if (op == ops::MUL) return left * right;
		if (op == ops::DIV) return left / right;
		if (op == ops::MOD) return left % right;
		if (op == ops::EQ) return left == right;
		if (op == ops::NE) return left != right;
		if (op == ops::LT) return left < right;
		if (op == ops::LE) return left <= right;
		if (op == ops::GT) return left > right;
		if (op == ops::GE) return left >= right;
		if (op == ops::AND) return left && right;
		if (op == ops::OR) return left || right;
		if (op == ops::BIT_AND) return left & right;
		if (op == ops::BIT_OR) return left | right;
		if (op == ops::BIT_XOR) return left ^ right;
		if (op == ops::SHL) return left << right;
		if (op == ops::SHR) return left >> right;
		
		throw std::runtime_error("Unknown binary operator: " + op);
	}
	
	// 単項演算子を適用
	NodeType ApplyUnaryOperator(const std::string& op, NodeType operand)
	{
		if (op == ops::NEG) return -operand;
		if (op == ops::NOT) return !operand;
		if (op == ops::BIT_NOT) return ~operand;
		
		throw std::runtime_error("Unknown unary operator: " + op);
	}
	
	// 関数を適用
	NodeType ApplyFunction(const std::string& func_name, std::vector<NodeType> args)
	{
		// 階層関数のチェック（例: mean, sum, etc.）
		// 階層関数は通常1個の引数を取る
		if (args.size() == 1)
		{
			// 数字付き関数名をチェック（例: mean2, sum3）
			if (func_name.size() > 1 && std::isdigit(func_name.back()))
			{
				// 基本名と数字を分離
				size_t num_start = func_name.size() - 1;
				while (num_start > 0 && std::isdigit(func_name[num_start - 1]))
					num_start--;
				
				std::string base_name = func_name.substr(0, num_start);
				int level = std::stoi(func_name.substr(num_start));
				
				return ApplyLayerFunctionWithLevel(base_name, std::move(args[0]), level);
			}
			
			// 通常の階層関数
			return ApplyLayerFunction(func_name, std::move(args[0]));
		}
		
		// TODO: 通常の数学関数など
		
		throw std::runtime_error("Unknown or unsupported function: " + func_name);
	}
	
	// 階層関数を適用
	NodeType ApplyLayerFunction(const std::string& func_name, NodeType arg)
	{
		// デフォルトは level 1
		return ApplyLayerFunctionWithLevel(func_name, std::move(arg), 1);
	}
	
	// レベル付き階層関数を適用
	NodeType ApplyLayerFunctionWithLevel(const std::string& func_name, NodeType arg, int level)
	{
		// 階層関数の適用には、現在のRttiFuncNodeを対応する階層関数で包む必要がある
		// しかし、これはテンプレートベースのため、実行時に動的に生成するのは難しい
		// 
		// 解決策: すべてのレベル（1-10）について switch で分岐
		// 関数名は小文字（size, mean など）で、レベルが付くと size1, mean2 になる
		
		#define APPLY_LAYER_FUNC_CASE(CONST_NAME, FUNC_NAME, LEVEL) \
			if (func_name == ops::CONST_NAME && level == LEVEL) { \
				return eval::FUNC_NAME##LEVEL(std::move(arg)); \
			}
		
		// size
		APPLY_LAYER_FUNC_CASE(SIZE, size, 1)
		APPLY_LAYER_FUNC_CASE(SIZE, size, 2)
		APPLY_LAYER_FUNC_CASE(SIZE, size, 3)
		APPLY_LAYER_FUNC_CASE(SIZE, size, 4)
		APPLY_LAYER_FUNC_CASE(SIZE, size, 5)
		APPLY_LAYER_FUNC_CASE(SIZE, size, 6)
		APPLY_LAYER_FUNC_CASE(SIZE, size, 7)
		APPLY_LAYER_FUNC_CASE(SIZE, size, 8)
		APPLY_LAYER_FUNC_CASE(SIZE, size, 9)
		APPLY_LAYER_FUNC_CASE(SIZE, size, 10)
		
		// exist
		APPLY_LAYER_FUNC_CASE(EXIST, exist, 1)
		APPLY_LAYER_FUNC_CASE(EXIST, exist, 2)
		APPLY_LAYER_FUNC_CASE(EXIST, exist, 3)
		APPLY_LAYER_FUNC_CASE(EXIST, exist, 4)
		APPLY_LAYER_FUNC_CASE(EXIST, exist, 5)
		APPLY_LAYER_FUNC_CASE(EXIST, exist, 6)
		APPLY_LAYER_FUNC_CASE(EXIST, exist, 7)
		APPLY_LAYER_FUNC_CASE(EXIST, exist, 8)
		APPLY_LAYER_FUNC_CASE(EXIST, exist, 9)
		APPLY_LAYER_FUNC_CASE(EXIST, exist, 10)
		
		// count
		APPLY_LAYER_FUNC_CASE(COUNT, count, 1)
		APPLY_LAYER_FUNC_CASE(COUNT, count, 2)
		APPLY_LAYER_FUNC_CASE(COUNT, count, 3)
		APPLY_LAYER_FUNC_CASE(COUNT, count, 4)
		APPLY_LAYER_FUNC_CASE(COUNT, count, 5)
		APPLY_LAYER_FUNC_CASE(COUNT, count, 6)
		APPLY_LAYER_FUNC_CASE(COUNT, count, 7)
		APPLY_LAYER_FUNC_CASE(COUNT, count, 8)
		APPLY_LAYER_FUNC_CASE(COUNT, count, 9)
		APPLY_LAYER_FUNC_CASE(COUNT, count, 10)
		
		// sum
		APPLY_LAYER_FUNC_CASE(SUM, sum, 1)
		APPLY_LAYER_FUNC_CASE(SUM, sum, 2)
		APPLY_LAYER_FUNC_CASE(SUM, sum, 3)
		APPLY_LAYER_FUNC_CASE(SUM, sum, 4)
		APPLY_LAYER_FUNC_CASE(SUM, sum, 5)
		APPLY_LAYER_FUNC_CASE(SUM, sum, 6)
		APPLY_LAYER_FUNC_CASE(SUM, sum, 7)
		APPLY_LAYER_FUNC_CASE(SUM, sum, 8)
		APPLY_LAYER_FUNC_CASE(SUM, sum, 9)
		APPLY_LAYER_FUNC_CASE(SUM, sum, 10)
		
		// mean
		APPLY_LAYER_FUNC_CASE(MEAN, mean, 1)
		APPLY_LAYER_FUNC_CASE(MEAN, mean, 2)
		APPLY_LAYER_FUNC_CASE(MEAN, mean, 3)
		APPLY_LAYER_FUNC_CASE(MEAN, mean, 4)
		APPLY_LAYER_FUNC_CASE(MEAN, mean, 5)
		APPLY_LAYER_FUNC_CASE(MEAN, mean, 6)
		APPLY_LAYER_FUNC_CASE(MEAN, mean, 7)
		APPLY_LAYER_FUNC_CASE(MEAN, mean, 8)
		APPLY_LAYER_FUNC_CASE(MEAN, mean, 9)
		APPLY_LAYER_FUNC_CASE(MEAN, mean, 10)
		
		// dev
		APPLY_LAYER_FUNC_CASE(DEV, dev, 1)
		APPLY_LAYER_FUNC_CASE(DEV, dev, 2)
		APPLY_LAYER_FUNC_CASE(DEV, dev, 3)
		APPLY_LAYER_FUNC_CASE(DEV, dev, 4)
		APPLY_LAYER_FUNC_CASE(DEV, dev, 5)
		APPLY_LAYER_FUNC_CASE(DEV, dev, 6)
		APPLY_LAYER_FUNC_CASE(DEV, dev, 7)
		APPLY_LAYER_FUNC_CASE(DEV, dev, 8)
		APPLY_LAYER_FUNC_CASE(DEV, dev, 9)
		APPLY_LAYER_FUNC_CASE(DEV, dev, 10)
		
		// greatest
		APPLY_LAYER_FUNC_CASE(GREATEST, greatest, 1)
		APPLY_LAYER_FUNC_CASE(GREATEST, greatest, 2)
		APPLY_LAYER_FUNC_CASE(GREATEST, greatest, 3)
		APPLY_LAYER_FUNC_CASE(GREATEST, greatest, 4)
		APPLY_LAYER_FUNC_CASE(GREATEST, greatest, 5)
		APPLY_LAYER_FUNC_CASE(GREATEST, greatest, 6)
		APPLY_LAYER_FUNC_CASE(GREATEST, greatest, 7)
		APPLY_LAYER_FUNC_CASE(GREATEST, greatest, 8)
		APPLY_LAYER_FUNC_CASE(GREATEST, greatest, 9)
		APPLY_LAYER_FUNC_CASE(GREATEST, greatest, 10)
		
		// least
		APPLY_LAYER_FUNC_CASE(LEAST, least, 1)
		APPLY_LAYER_FUNC_CASE(LEAST, least, 2)
		APPLY_LAYER_FUNC_CASE(LEAST, least, 3)
		APPLY_LAYER_FUNC_CASE(LEAST, least, 4)
		APPLY_LAYER_FUNC_CASE(LEAST, least, 5)
		APPLY_LAYER_FUNC_CASE(LEAST, least, 6)
		APPLY_LAYER_FUNC_CASE(LEAST, least, 7)
		APPLY_LAYER_FUNC_CASE(LEAST, least, 8)
		APPLY_LAYER_FUNC_CASE(LEAST, least, 9)
		APPLY_LAYER_FUNC_CASE(LEAST, least, 10)
		
		#undef APPLY_LAYER_FUNC_CASE
		
		throw std::runtime_error("Unknown or unsupported layer function: " + func_name);
	}
	
	// メンバ関数を適用
	NodeType ApplyMemberFunction(NodeType node, const std::string& method_name, std::vector<NodeType> args)
	{
		// at() メソッド: node.at(indices...)
		if (method_name == ops::AT)
		{
			// TODO: at() の実装は複雑
			// RttiFieldNodeに対してat()を呼ぶ必要があるが、
			// NodeTypeはRttiFuncNodeなので直接呼べない
			// 
			// 現時点では未サポート
			throw std::runtime_error("Member function 'at()' is not yet supported in parser");
		}
		
		// outer() / o() メソッド
		if (method_name == ops::OUTER || method_name == ops::O)
		{
			// TODO: outer() も同様に複雑
			throw std::runtime_error("Member function 'outer()' is not yet supported in parser");
		}
		
		throw std::runtime_error("Unknown member function: " + method_name);
	}
	
	void Advance()
	{
		m_current_token = m_tokenizer.NextToken();
	}
	
	const Container& m_container;
	Tokenizer m_tokenizer;
	Token m_current_token;
	std::map<std::string, PlaceholderType> m_placeholders;
};

// ユーティリティ関数：文字列からRttiFuncNodeを生成（改良版API）
template <class Container>
auto ParseRttiFuncNode(const Container& container, std::string_view expr)
{
	Parser<Container> parser(container, expr);
	return parser.Parse();
}

} // namespace parser

} // namespace eval

} // namespace adapt

#endif
