#ifndef ADAPT_EVALUATOR_PARSER_V3_H
#define ADAPT_EVALUATOR_PARSER_V3_H

#include <string>
#include <string_view>
#include <map>
#include <vector>
#include <variant>
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
inline int GetPrecedence(std::string_view op)
{
	if (op == "*" || op == "/" || op == "%") return 13;
	if (op == "+" || op == "-") return 12;
	if (op == "<<" || op == ">>") return 11;
	if (op == "<" || op == "<=" || op == ">" || op == ">=") return 10;
	if (op == "==" || op == "!=") return 9;
	if (op == "&") return 8;
	if (op == "^") return 7;
	if (op == "|") return 6;
	if (op == "&&") return 5;
	if (op == "||") return 4;
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
	constexpr const char* NEG = "-";
	constexpr const char* NOT = "!";
	constexpr const char* BIT_NOT = "~";
	
	// プレースホルダメソッド
	constexpr const char* AT = "at";
	constexpr const char* OUTER = "outer";
	constexpr const char* O = "o";
	
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

// パーサーで扱うノードの型
// RttiFieldNode, RttiConstNode, RttiFuncNodeを区別して保持する（パフォーマンス最適化のため）
template <class Container>
struct ParsedNode
{
	using FieldNodeType = eval::RttiFieldNode<typename Container::RttiPlaceholder>;
	using ConstNodeType = eval::RttiConstNode;
	using FuncNodeType = eval::RttiFuncNode<Container>;
	
	// バリアント：FieldNode、ConstNode、またはFuncNode
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
	FuncNodeType ToFuncNode() &&
	{
		if (IsFuncNode())
		{
			return std::move(AsFuncNode());
		}
		else if (IsFieldNode())
		{
			// FieldNodeをFuncNodeでラップ（必要な場合のみ）
			return ConvertToRttiFuncNode(std::move(AsFieldNode()));
		}
		else // IsConstNode()
		{
			// ConstNodeをFuncNodeでラップ
			// Container型を指定する必要があるため、ConstNodeをラムダでキャプチャして
			// MakeRttiFuncNodeを使う
			auto cn = std::move(AsConstNode());
			auto f = [](const auto& a) { return a; };
			return eval::detail::MakeRttiFuncNode(f, ValueList<>(), std::tuple<>(),
												  eval::detail::ConvertToNode(std::move(cn), std::true_type{}));
		}
	}
};

// パーサー（構文解析器）
template <class Container>
class Parser
{
public:
	using PlaceholderType = typename Container::RttiPlaceholder;
	using NodeType = ParsedNode<Container>;
	using FuncNodeType = eval::RttiFuncNode<Container>;
	using FieldNodeType = eval::RttiFieldNode<PlaceholderType>;
	using ConstNodeType = eval::RttiConstNode;
	
	Parser(const Container& container, std::string_view expr)
		: m_container(container), m_tokenizer(expr)
	{
		m_current_token = m_tokenizer.NextToken();
		InitPlaceholders();
	}
	
	// 式をパースしてRttiFuncNodeを生成
	FuncNodeType Parse()
	{
		auto result = ParseExpression(0);
		if (m_current_token.type != TokenType::End)
			throw std::runtime_error("Unexpected token after expression");
		return std::move(result).ToFuncNode();
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
			
			// プレースホルダ - RttiFieldNodeとして返す（パフォーマンス最適化）
			auto it = m_placeholders.find(name);
			if (it == m_placeholders.end())
				throw std::runtime_error("Unknown identifier: " + name);
			
			return NodeType(FieldNodeType{ it->second });
		}
		
		throw std::runtime_error("Unexpected token in primary expression");
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
			
			// 型接尾辞を判定
			if (suffix == "i08") type = FieldType::I08;
			else if (suffix == "i16") type = FieldType::I16;
			else if (suffix == "i32") type = FieldType::I32;
			else if (suffix == "i64") type = FieldType::I64;
			else if (suffix == "f32") type = FieldType::F32;
			else if (suffix == "f64") type = FieldType::F64;
			
			if (type != FieldType::Emp)
			{
				Advance(); // 接尾辞を消費
			}
		}
		
		// 浮動小数点か整数かを判定
		bool is_float = value.find('.') != std::string::npos;
		
		// デフォルト型の決定
		if (type == FieldType::Emp)
		{
			type = is_float ? FieldType::F64 : FieldType::I32;
		}
		
		// 型に応じてConstNodeを生成し、直接FuncNodeでラップ
		// ConstNodeを保持するのではなく、直接ラムダとして扱う
		try
		{
			switch (type)
			{
			case FieldType::I08:
			{
				int8_t val = static_cast<int8_t>(std::stoi(value));
				auto lambda = [val](const Container&, const Bpos&) { return ConstNodeType(val); };
				return NodeType(FuncNodeType(std::move(lambda)));
			}
			case FieldType::I16:
			{
				int16_t val = static_cast<int16_t>(std::stoi(value));
				auto lambda = [val](const Container&, const Bpos&) { return ConstNodeType(val); };
				return NodeType(FuncNodeType(std::move(lambda)));
			}
			case FieldType::I32:
			{
				int32_t val = static_cast<int32_t>(std::stoi(value));
				auto lambda = [val](const Container&, const Bpos&) { return ConstNodeType(val); };
				return NodeType(FuncNodeType(std::move(lambda)));
			}
			case FieldType::I64:
			{
				int64_t val = static_cast<int64_t>(std::stoll(value));
				auto lambda = [val](const Container&, const Bpos&) { return ConstNodeType(val); };
				return NodeType(FuncNodeType(std::move(lambda)));
			}
			case FieldType::F32:
			{
				float val = static_cast<float>(std::stof(value));
				auto lambda = [val](const Container&, const Bpos&) { return ConstNodeType(val); };
				return NodeType(FuncNodeType(std::move(lambda)));
			}
			case FieldType::F64:
			{
				double val = std::stod(value);
				auto lambda = [val](const Container&, const Bpos&) { return ConstNodeType(val); };
				return NodeType(FuncNodeType(std::move(lambda)));
			}
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
		
		// 文字列のFuncNodeを直接生成
		auto lambda = [value](const Container&, const Bpos&) { return ConstNodeType(value); };
		return NodeType(FuncNodeType(std::move(lambda)));
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
	// RttiFieldNodeは直接渡す（パフォーマンス最適化）
	// ConstNodeは一度FuncNodeに変換する必要がある
	NodeType ApplyBinaryOperator(const std::string& op, NodeType left, NodeType right)
	{
		// ConstNodeを先にFuncNodeに変換
		if (left.IsConstNode())
		{
			auto func_node = ConvertToRttiFuncNode(std::move(left.AsConstNode()));
			left = NodeType(std::move(func_node));
		}
		if (right.IsConstNode())
		{
			auto func_node = ConvertToRttiFuncNode(std::move(right.AsConstNode()));
			right = NodeType(std::move(func_node));
		}
		
		// この時点でleftとrightはFieldNodeまたはFuncNode（ConstNodeではない）
		// 各ケースを直接処理して、ConstNodeのパスを避ける
		FuncNodeType result;
		
		if (left.IsFieldNode() && right.IsFieldNode())
		{
			auto& l = left.AsFieldNode();
			auto& r = right.AsFieldNode();
			if (op == ops::ADD) result = l + r;
			else if (op == ops::SUB) result = l - r;
			else if (op == ops::MUL) result = l * r;
			else if (op == ops::DIV) result = l / r;
			else if (op == ops::MOD) result = l % r;
			else if (op == ops::EQ) result = l == r;
			else if (op == ops::NE) result = l != r;
			else if (op == ops::LT) result = l < r;
			else if (op == ops::LE) result = l <= r;
			else if (op == ops::GT) result = l > r;
			else if (op == ops::GE) result = l >= r;
			else if (op == ops::AND) result = l && r;
			else if (op == ops::OR) result = l || r;
			else if (op == ops::BIT_AND) result = l & r;
			else if (op == ops::BIT_OR) result = l | r;
			else if (op == ops::BIT_XOR) result = l ^ r;
			else if (op == ops::SHL) result = l << r;
			else if (op == ops::SHR) result = l >> r;
			else throw std::runtime_error("Unknown binary operator: " + op);
		}
		else if (left.IsFieldNode() && right.IsFuncNode())
		{
			auto& l = left.AsFieldNode();
			auto& r = right.AsFuncNode();
			if (op == ops::ADD) result = l + r;
			else if (op == ops::SUB) result = l - r;
			else if (op == ops::MUL) result = l * r;
			else if (op == ops::DIV) result = l / r;
			else if (op == ops::MOD) result = l % r;
			else if (op == ops::EQ) result = l == r;
			else if (op == ops::NE) result = l != r;
			else if (op == ops::LT) result = l < r;
			else if (op == ops::LE) result = l <= r;
			else if (op == ops::GT) result = l > r;
			else if (op == ops::GE) result = l >= r;
			else if (op == ops::AND) result = l && r;
			else if (op == ops::OR) result = l || r;
			else if (op == ops::BIT_AND) result = l & r;
			else if (op == ops::BIT_OR) result = l | r;
			else if (op == ops::BIT_XOR) result = l ^ r;
			else if (op == ops::SHL) result = l << r;
			else if (op == ops::SHR) result = l >> r;
			else throw std::runtime_error("Unknown binary operator: " + op);
		}
		else if (left.IsFuncNode() && right.IsFieldNode())
		{
			auto& l = left.AsFuncNode();
			auto& r = right.AsFieldNode();
			if (op == ops::ADD) result = l + r;
			else if (op == ops::SUB) result = l - r;
			else if (op == ops::MUL) result = l * r;
			else if (op == ops::DIV) result = l / r;
			else if (op == ops::MOD) result = l % r;
			else if (op == ops::EQ) result = l == r;
			else if (op == ops::NE) result = l != r;
			else if (op == ops::LT) result = l < r;
			else if (op == ops::LE) result = l <= r;
			else if (op == ops::GT) result = l > r;
			else if (op == ops::GE) result = l >= r;
			else if (op == ops::AND) result = l && r;
			else if (op == ops::OR) result = l || r;
			else if (op == ops::BIT_AND) result = l & r;
			else if (op == ops::BIT_OR) result = l | r;
			else if (op == ops::BIT_XOR) result = l ^ r;
			else if (op == ops::SHL) result = l << r;
			else if (op == ops::SHR) result = l >> r;
			else throw std::runtime_error("Unknown binary operator: " + op);
		}
		else // both FuncNode
		{
			auto& l = left.AsFuncNode();
			auto& r = right.AsFuncNode();
			if (op == ops::ADD) result = l + r;
			else if (op == ops::SUB) result = l - r;
			else if (op == ops::MUL) result = l * r;
			else if (op == ops::DIV) result = l / r;
			else if (op == ops::MOD) result = l % r;
			else if (op == ops::EQ) result = l == r;
			else if (op == ops::NE) result = l != r;
			else if (op == ops::LT) result = l < r;
			else if (op == ops::LE) result = l <= r;
			else if (op == ops::GT) result = l > r;
			else if (op == ops::GE) result = l >= r;
			else if (op == ops::AND) result = l && r;
			else if (op == ops::OR) result = l || r;
			else if (op == ops::BIT_AND) result = l & r;
			else if (op == ops::BIT_OR) result = l | r;
			else if (op == ops::BIT_XOR) result = l ^ r;
			else if (op == ops::SHL) result = l << r;
			else if (op == ops::SHR) result = l >> r;
			else throw std::runtime_error("Unknown binary operator: " + op);
		}
		
		return NodeType(std::move(result));
	}
	
	// 単項演算子を適用
	NodeType ApplyUnaryOperator(const std::string& op, NodeType operand)
	{
		// ConstNodeを先にFuncNodeに変換
		if (operand.IsConstNode())
		{
			auto func_node = ConvertToRttiFuncNode(std::move(operand.AsConstNode()));
			operand = NodeType(std::move(func_node));
		}
		
		// この時点でoperandはFieldNodeまたはFuncNode（ConstNodeではない）
		FuncNodeType result;
		
		if (operand.IsFieldNode())
		{
			auto& o = operand.AsFieldNode();
			if (op == ops::NEG) result = -o;
			else if (op == ops::NOT) result = !o;
			else if (op == ops::BIT_NOT) result = ~o;
			else throw std::runtime_error("Unknown unary operator: " + op);
		}
		else // FuncNode
		{
			auto& o = operand.AsFuncNode();
			if (op == ops::NEG) result = -o;
			else if (op == ops::NOT) result = !o;
			else if (op == ops::BIT_NOT) result = ~o;
			else throw std::runtime_error("Unknown unary operator: " + op);
		}
		
		return NodeType(std::move(result));
	}
	
	// 関数を適用
	NodeType ApplyFunction(const std::string& func_name, std::vector<NodeType> args)
	{
		// 階層関数のチェック（例: mean, sum, etc.）
		if (args.size() == 1)
		{
			// 数字付き関数名をチェック（例: mean2, sum3）
			if (func_name.size() > 1 && std::isdigit(func_name.back()))
			{
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
		
		throw std::runtime_error("Unknown or unsupported function: " + func_name);
	}
	
	// 階層関数を適用
	NodeType ApplyLayerFunction(const std::string& func_name, NodeType arg)
	{
		return ApplyLayerFunctionWithLevel(func_name, std::move(arg), 1);
	}
	
	// レベル付き階層関数を適用
	// RttiFieldNodeは直接渡す（パフォーマンス最適化）
	// ConstNodeは階層関数に使用できない
	NodeType ApplyLayerFunctionWithLevel(const std::string& func_name, NodeType arg, int level)
	{
		// ConstNodeは階層関数では使えない（階層を持たないため）
		if (arg.IsConstNode())
		{
			throw std::runtime_error("Cannot apply layer function to constant literal");
		}
		
		#define APPLY_LAYER_FUNC_IMPL(FIELD_OR_FUNC, CONST_NAME, FUNC_NAME, LEVEL) \
			if (func_name == ops::CONST_NAME && level == LEVEL) { \
				return NodeType(eval::FUNC_NAME##LEVEL(std::forward<decltype(FIELD_OR_FUNC)>(FIELD_OR_FUNC))); \
			}
		
		// FieldNodeの場合
		if (arg.IsFieldNode())
		{
			auto& a = arg.AsFieldNode();
			
			// size
			APPLY_LAYER_FUNC_IMPL(a, SIZE, size, 1)
			APPLY_LAYER_FUNC_IMPL(a, SIZE, size, 2)
			APPLY_LAYER_FUNC_IMPL(a, SIZE, size, 3)
			APPLY_LAYER_FUNC_IMPL(a, SIZE, size, 4)
			APPLY_LAYER_FUNC_IMPL(a, SIZE, size, 5)
			APPLY_LAYER_FUNC_IMPL(a, SIZE, size, 6)
			APPLY_LAYER_FUNC_IMPL(a, SIZE, size, 7)
			APPLY_LAYER_FUNC_IMPL(a, SIZE, size, 8)
			APPLY_LAYER_FUNC_IMPL(a, SIZE, size, 9)
			APPLY_LAYER_FUNC_IMPL(a, SIZE, size, 10)
			
			// exist
			APPLY_LAYER_FUNC_IMPL(a, EXIST, exist, 1)
			APPLY_LAYER_FUNC_IMPL(a, EXIST, exist, 2)
			APPLY_LAYER_FUNC_IMPL(a, EXIST, exist, 3)
			APPLY_LAYER_FUNC_IMPL(a, EXIST, exist, 4)
			APPLY_LAYER_FUNC_IMPL(a, EXIST, exist, 5)
			APPLY_LAYER_FUNC_IMPL(a, EXIST, exist, 6)
			APPLY_LAYER_FUNC_IMPL(a, EXIST, exist, 7)
			APPLY_LAYER_FUNC_IMPL(a, EXIST, exist, 8)
			APPLY_LAYER_FUNC_IMPL(a, EXIST, exist, 9)
			APPLY_LAYER_FUNC_IMPL(a, EXIST, exist, 10)
			
			// count
			APPLY_LAYER_FUNC_IMPL(a, COUNT, count, 1)
			APPLY_LAYER_FUNC_IMPL(a, COUNT, count, 2)
			APPLY_LAYER_FUNC_IMPL(a, COUNT, count, 3)
			APPLY_LAYER_FUNC_IMPL(a, COUNT, count, 4)
			APPLY_LAYER_FUNC_IMPL(a, COUNT, count, 5)
			APPLY_LAYER_FUNC_IMPL(a, COUNT, count, 6)
			APPLY_LAYER_FUNC_IMPL(a, COUNT, count, 7)
			APPLY_LAYER_FUNC_IMPL(a, COUNT, count, 8)
			APPLY_LAYER_FUNC_IMPL(a, COUNT, count, 9)
			APPLY_LAYER_FUNC_IMPL(a, COUNT, count, 10)
			
			// sum
			APPLY_LAYER_FUNC_IMPL(a, SUM, sum, 1)
			APPLY_LAYER_FUNC_IMPL(a, SUM, sum, 2)
			APPLY_LAYER_FUNC_IMPL(a, SUM, sum, 3)
			APPLY_LAYER_FUNC_IMPL(a, SUM, sum, 4)
			APPLY_LAYER_FUNC_IMPL(a, SUM, sum, 5)
			APPLY_LAYER_FUNC_IMPL(a, SUM, sum, 6)
			APPLY_LAYER_FUNC_IMPL(a, SUM, sum, 7)
			APPLY_LAYER_FUNC_IMPL(a, SUM, sum, 8)
			APPLY_LAYER_FUNC_IMPL(a, SUM, sum, 9)
			APPLY_LAYER_FUNC_IMPL(a, SUM, sum, 10)
			
			// mean
			APPLY_LAYER_FUNC_IMPL(a, MEAN, mean, 1)
			APPLY_LAYER_FUNC_IMPL(a, MEAN, mean, 2)
			APPLY_LAYER_FUNC_IMPL(a, MEAN, mean, 3)
			APPLY_LAYER_FUNC_IMPL(a, MEAN, mean, 4)
			APPLY_LAYER_FUNC_IMPL(a, MEAN, mean, 5)
			APPLY_LAYER_FUNC_IMPL(a, MEAN, mean, 6)
			APPLY_LAYER_FUNC_IMPL(a, MEAN, mean, 7)
			APPLY_LAYER_FUNC_IMPL(a, MEAN, mean, 8)
			APPLY_LAYER_FUNC_IMPL(a, MEAN, mean, 9)
			APPLY_LAYER_FUNC_IMPL(a, MEAN, mean, 10)
			
			// dev
			APPLY_LAYER_FUNC_IMPL(a, DEV, dev, 1)
			APPLY_LAYER_FUNC_IMPL(a, DEV, dev, 2)
			APPLY_LAYER_FUNC_IMPL(a, DEV, dev, 3)
			APPLY_LAYER_FUNC_IMPL(a, DEV, dev, 4)
			APPLY_LAYER_FUNC_IMPL(a, DEV, dev, 5)
			APPLY_LAYER_FUNC_IMPL(a, DEV, dev, 6)
			APPLY_LAYER_FUNC_IMPL(a, DEV, dev, 7)
			APPLY_LAYER_FUNC_IMPL(a, DEV, dev, 8)
			APPLY_LAYER_FUNC_IMPL(a, DEV, dev, 9)
			APPLY_LAYER_FUNC_IMPL(a, DEV, dev, 10)
			
			// greatest
			APPLY_LAYER_FUNC_IMPL(a, GREATEST, greatest, 1)
			APPLY_LAYER_FUNC_IMPL(a, GREATEST, greatest, 2)
			APPLY_LAYER_FUNC_IMPL(a, GREATEST, greatest, 3)
			APPLY_LAYER_FUNC_IMPL(a, GREATEST, greatest, 4)
			APPLY_LAYER_FUNC_IMPL(a, GREATEST, greatest, 5)
			APPLY_LAYER_FUNC_IMPL(a, GREATEST, greatest, 6)
			APPLY_LAYER_FUNC_IMPL(a, GREATEST, greatest, 7)
			APPLY_LAYER_FUNC_IMPL(a, GREATEST, greatest, 8)
			APPLY_LAYER_FUNC_IMPL(a, GREATEST, greatest, 9)
			APPLY_LAYER_FUNC_IMPL(a, GREATEST, greatest, 10)
			
			// least
			APPLY_LAYER_FUNC_IMPL(a, LEAST, least, 1)
			APPLY_LAYER_FUNC_IMPL(a, LEAST, least, 2)
			APPLY_LAYER_FUNC_IMPL(a, LEAST, least, 3)
			APPLY_LAYER_FUNC_IMPL(a, LEAST, least, 4)
			APPLY_LAYER_FUNC_IMPL(a, LEAST, least, 5)
			APPLY_LAYER_FUNC_IMPL(a, LEAST, least, 6)
			APPLY_LAYER_FUNC_IMPL(a, LEAST, least, 7)
			APPLY_LAYER_FUNC_IMPL(a, LEAST, least, 8)
			APPLY_LAYER_FUNC_IMPL(a, LEAST, least, 9)
			APPLY_LAYER_FUNC_IMPL(a, LEAST, least, 10)
		}
		else // FuncNode
		{
			auto& a = arg.AsFuncNode();
			
			// size
			APPLY_LAYER_FUNC_IMPL(a, SIZE, size, 1)
			APPLY_LAYER_FUNC_IMPL(a, SIZE, size, 2)
			APPLY_LAYER_FUNC_IMPL(a, SIZE, size, 3)
			APPLY_LAYER_FUNC_IMPL(a, SIZE, size, 4)
			APPLY_LAYER_FUNC_IMPL(a, SIZE, size, 5)
			APPLY_LAYER_FUNC_IMPL(a, SIZE, size, 6)
			APPLY_LAYER_FUNC_IMPL(a, SIZE, size, 7)
			APPLY_LAYER_FUNC_IMPL(a, SIZE, size, 8)
			APPLY_LAYER_FUNC_IMPL(a, SIZE, size, 9)
			APPLY_LAYER_FUNC_IMPL(a, SIZE, size, 10)
			
			// exist
			APPLY_LAYER_FUNC_IMPL(a, EXIST, exist, 1)
			APPLY_LAYER_FUNC_IMPL(a, EXIST, exist, 2)
			APPLY_LAYER_FUNC_IMPL(a, EXIST, exist, 3)
			APPLY_LAYER_FUNC_IMPL(a, EXIST, exist, 4)
			APPLY_LAYER_FUNC_IMPL(a, EXIST, exist, 5)
			APPLY_LAYER_FUNC_IMPL(a, EXIST, exist, 6)
			APPLY_LAYER_FUNC_IMPL(a, EXIST, exist, 7)
			APPLY_LAYER_FUNC_IMPL(a, EXIST, exist, 8)
			APPLY_LAYER_FUNC_IMPL(a, EXIST, exist, 9)
			APPLY_LAYER_FUNC_IMPL(a, EXIST, exist, 10)
			
			// count
			APPLY_LAYER_FUNC_IMPL(a, COUNT, count, 1)
			APPLY_LAYER_FUNC_IMPL(a, COUNT, count, 2)
			APPLY_LAYER_FUNC_IMPL(a, COUNT, count, 3)
			APPLY_LAYER_FUNC_IMPL(a, COUNT, count, 4)
			APPLY_LAYER_FUNC_IMPL(a, COUNT, count, 5)
			APPLY_LAYER_FUNC_IMPL(a, COUNT, count, 6)
			APPLY_LAYER_FUNC_IMPL(a, COUNT, count, 7)
			APPLY_LAYER_FUNC_IMPL(a, COUNT, count, 8)
			APPLY_LAYER_FUNC_IMPL(a, COUNT, count, 9)
			APPLY_LAYER_FUNC_IMPL(a, COUNT, count, 10)
			
			// sum
			APPLY_LAYER_FUNC_IMPL(a, SUM, sum, 1)
			APPLY_LAYER_FUNC_IMPL(a, SUM, sum, 2)
			APPLY_LAYER_FUNC_IMPL(a, SUM, sum, 3)
			APPLY_LAYER_FUNC_IMPL(a, SUM, sum, 4)
			APPLY_LAYER_FUNC_IMPL(a, SUM, sum, 5)
			APPLY_LAYER_FUNC_IMPL(a, SUM, sum, 6)
			APPLY_LAYER_FUNC_IMPL(a, SUM, sum, 7)
			APPLY_LAYER_FUNC_IMPL(a, SUM, sum, 8)
			APPLY_LAYER_FUNC_IMPL(a, SUM, sum, 9)
			APPLY_LAYER_FUNC_IMPL(a, SUM, sum, 10)
			
			// mean
			APPLY_LAYER_FUNC_IMPL(a, MEAN, mean, 1)
			APPLY_LAYER_FUNC_IMPL(a, MEAN, mean, 2)
			APPLY_LAYER_FUNC_IMPL(a, MEAN, mean, 3)
			APPLY_LAYER_FUNC_IMPL(a, MEAN, mean, 4)
			APPLY_LAYER_FUNC_IMPL(a, MEAN, mean, 5)
			APPLY_LAYER_FUNC_IMPL(a, MEAN, mean, 6)
			APPLY_LAYER_FUNC_IMPL(a, MEAN, mean, 7)
			APPLY_LAYER_FUNC_IMPL(a, MEAN, mean, 8)
			APPLY_LAYER_FUNC_IMPL(a, MEAN, mean, 9)
			APPLY_LAYER_FUNC_IMPL(a, MEAN, mean, 10)
			
			// dev
			APPLY_LAYER_FUNC_IMPL(a, DEV, dev, 1)
			APPLY_LAYER_FUNC_IMPL(a, DEV, dev, 2)
			APPLY_LAYER_FUNC_IMPL(a, DEV, dev, 3)
			APPLY_LAYER_FUNC_IMPL(a, DEV, dev, 4)
			APPLY_LAYER_FUNC_IMPL(a, DEV, dev, 5)
			APPLY_LAYER_FUNC_IMPL(a, DEV, dev, 6)
			APPLY_LAYER_FUNC_IMPL(a, DEV, dev, 7)
			APPLY_LAYER_FUNC_IMPL(a, DEV, dev, 8)
			APPLY_LAYER_FUNC_IMPL(a, DEV, dev, 9)
			APPLY_LAYER_FUNC_IMPL(a, DEV, dev, 10)
			
			// greatest
			APPLY_LAYER_FUNC_IMPL(a, GREATEST, greatest, 1)
			APPLY_LAYER_FUNC_IMPL(a, GREATEST, greatest, 2)
			APPLY_LAYER_FUNC_IMPL(a, GREATEST, greatest, 3)
			APPLY_LAYER_FUNC_IMPL(a, GREATEST, greatest, 4)
			APPLY_LAYER_FUNC_IMPL(a, GREATEST, greatest, 5)
			APPLY_LAYER_FUNC_IMPL(a, GREATEST, greatest, 6)
			APPLY_LAYER_FUNC_IMPL(a, GREATEST, greatest, 7)
			APPLY_LAYER_FUNC_IMPL(a, GREATEST, greatest, 8)
			APPLY_LAYER_FUNC_IMPL(a, GREATEST, greatest, 9)
			APPLY_LAYER_FUNC_IMPL(a, GREATEST, greatest, 10)
			
			// least
			APPLY_LAYER_FUNC_IMPL(a, LEAST, least, 1)
			APPLY_LAYER_FUNC_IMPL(a, LEAST, least, 2)
			APPLY_LAYER_FUNC_IMPL(a, LEAST, least, 3)
			APPLY_LAYER_FUNC_IMPL(a, LEAST, least, 4)
			APPLY_LAYER_FUNC_IMPL(a, LEAST, least, 5)
			APPLY_LAYER_FUNC_IMPL(a, LEAST, least, 6)
			APPLY_LAYER_FUNC_IMPL(a, LEAST, least, 7)
			APPLY_LAYER_FUNC_IMPL(a, LEAST, least, 8)
			APPLY_LAYER_FUNC_IMPL(a, LEAST, least, 9)
			APPLY_LAYER_FUNC_IMPL(a, LEAST, least, 10)
		}
		
		#undef APPLY_LAYER_FUNC_IMPL
		
		throw std::runtime_error("Unknown or unsupported layer function: " + func_name);
	}
	
	// メンバ関数を適用
	NodeType ApplyMemberFunction(NodeType node, const std::string& method_name, std::vector<NodeType> args)
	{
		// at() メソッド: node.at(indices...)
		if (method_name == ops::AT)
		{
			throw std::runtime_error("Member function 'at()' is not yet supported in parser");
		}
		
		// outer() / o() メソッド
		if (method_name == ops::OUTER || method_name == ops::O)
		{
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
