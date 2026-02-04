#ifndef ADAPT_EVALUATOR_PARSER_H
#define ADAPT_EVALUATOR_PARSER_H

#include <string>
#include <string_view>
#include <map>
#include <vector>
#include <cctype>
#include <stdexcept>
#include <OpenADAPT/Evaluator/FuncNode.h>
#include <OpenADAPT/Evaluator/Function.h>
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
	Identifier,    // プレースホルダ名
	Operator,      // 演算子
	LeftParen,     // (
	RightParen,    // )
	End            // 終端
};

// トークン
struct Token
{
	TokenType type;
	std::string value;
	int precedence;  // 演算子の優先順位
	
	Token() : type(TokenType::End), value(""), precedence(0) {}
	Token(TokenType t, std::string v = "", int prec = 0)
		: type(t), value(std::move(v)), precedence(prec) {}
};

// 演算子の優先順位を返す
inline int GetPrecedence(std::string_view op)
{
	if (op == "||") return 1;
	if (op == "&&") return 2;
	if (op == "|") return 3;
	if (op == "^") return 4;
	if (op == "&") return 5;
	if (op == "==" || op == "!=") return 6;
	if (op == "<" || op == "<=" || op == ">" || op == ">=") return 7;
	if (op == "<<" || op == ">>") return 8;
	if (op == "+" || op == "-") return 9;
	if (op == "*" || op == "/" || op == "%") return 10;
	if (op == "!") return 11;  // 単項演算子
	return 0;
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
		
		// 文字列リテラル
		if (c == '"' || c == '\'')
		{
			return ParseString();
		}
		
		// 数値
		if (std::isdigit(c) || (c == '.' && m_pos + 1 < m_expr.size() && std::isdigit(m_expr[m_pos + 1])))
		{
			return ParseNumber();
		}
		
		// 演算子
		if (IsOperatorChar(c))
		{
			return ParseOperator();
		}
		
		// 識別子（プレースホルダ名）
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
	using PlaceholderMap = std::map<std::string, PlaceholderType>;
	using NodeType = eval::RttiFuncNode<Container>;
	
	Parser(std::string_view expr, const PlaceholderMap& placeholders)
		: m_tokenizer(expr), m_placeholders(placeholders)
	{
		m_current_token = m_tokenizer.NextToken();
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
	// 式をパース（演算子の優先順位を考慮）
	NodeType ParseExpression(int min_precedence)
	{
		auto left = ParsePrimary();
		
		while (m_current_token.type == TokenType::Operator &&
			   m_current_token.precedence >= min_precedence)
		{
			Token op = m_current_token;
			Advance();
			
			auto right = ParseExpression(op.precedence + 1);
			left = ApplyOperator(op.value, std::move(left), std::move(right));
		}
		
		return left;
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
		
		// 単項演算子
		if (m_current_token.type == TokenType::Operator &&
			(m_current_token.value == "-" || m_current_token.value == "!" || m_current_token.value == "~"))
		{
			Token op = m_current_token;
			Advance();
			auto operand = ParsePrimary();
			return ApplyUnaryOperator(op.value, std::move(operand));
		}
		
		// 数値・文字列リテラルは未サポート（将来の拡張のため構文チェックのみ）
		if (m_current_token.type == TokenType::Number)
		{
			throw std::runtime_error("Number literals are not yet supported");
		}
		
		if (m_current_token.type == TokenType::String)
		{
			throw std::runtime_error("String literals are not yet supported");
		}
		
		// プレースホルダ
		if (m_current_token.type == TokenType::Identifier)
		{
			std::string name = m_current_token.value;
			Advance();
			
			auto it = m_placeholders.find(name);
			if (it == m_placeholders.end())
				throw std::runtime_error("Unknown placeholder: " + name);
			
			return ConvertToRttiFuncNode(eval::RttiFieldNode{ it->second });
		}
		
		throw std::runtime_error("Unexpected token in primary expression");
	}
	
	// 二項演算子を適用
	NodeType ApplyOperator(const std::string& op, NodeType left, NodeType right)
	{
		if (op == "+") return left + right;
		if (op == "-") return left - right;
		if (op == "*") return left * right;
		if (op == "/") return left / right;
		if (op == "%") return left % right;
		if (op == "==") return left == right;
		if (op == "!=") return left != right;
		if (op == "<") return left < right;
		if (op == "<=") return left <= right;
		if (op == ">") return left > right;
		if (op == ">=") return left >= right;
		if (op == "&&") return left && right;
		if (op == "||") return left || right;
		if (op == "&") return left & right;
		if (op == "|") return left | right;
		if (op == "^") return left ^ right;
		if (op == "<<") return left << right;
		if (op == ">>") return left >> right;
		
		throw std::runtime_error("Unknown operator: " + op);
	}
	
	// 単項演算子を適用
	NodeType ApplyUnaryOperator(const std::string& op, NodeType operand)
	{
		if (op == "-") return -operand;
		if (op == "!") return !operand;
		if (op == "~") return ~operand;
		
		throw std::runtime_error("Unknown unary operator: " + op);
	}
	
	void Advance()
	{
		m_current_token = m_tokenizer.NextToken();
	}
	
	Tokenizer m_tokenizer;
	Token m_current_token;
	const PlaceholderMap& m_placeholders;
};

// ユーティリティ関数：文字列からRttiFuncNodeを生成
template <class Container>
auto ParseRttiFuncNode(std::string_view expr,
					   const std::map<std::string, typename Container::RttiPlaceholder>& placeholders)
{
	Parser<Container> parser(expr, placeholders);
	return parser.Parse();
}

// ユーティリティ関数：プレースホルダの配列から名前マップを生成
template <class Container, size_t N>
auto MakePlaceholderMap(const Container& container,
						const std::array<std::string, N>& names)
{
	auto phs = container.GetPlaceholders(names);
	std::map<std::string, typename Container::RttiPlaceholder> map;
	for (size_t i = 0; i < N; ++i)
	{
		map[names[i]] = phs[i];
	}
	return map;
}

} // namespace parser

} // namespace eval

} // namespace adapt

#endif
