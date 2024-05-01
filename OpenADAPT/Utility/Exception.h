#ifndef ADAPT_EXCEPTION_H
#define ADAPT_EXCEPTION_H

#include <exception>
#include <string>
#include <string_view>
#include <format>

namespace adapt
{

class Exception
{
public:

};

class ExceptionWithMessage : public Exception
{
public:
	ExceptionWithMessage(std::string_view mes) : m_message(mes) {}
	const std::string& GetMessage() const { return m_message; }
	const char* what() const noexcept { return m_message.c_str(); }
private:
	std::string m_message;
};

class NoElements : public Exception
{
	//コンテナの要素が見つからない、範囲外などの場合に投げられる。
public:

};

class JointError : public Exception
{
public:

};

/*class Terminate : public ExceptionWithMessage
{
	//処理を強制終了させるもの。Extractなどが投げる。
public:
	Terminate(std::string_view mes)
		: ExceptionWithMessage(std::format("TERMINATE : {}", mes))
	{}
};

class SkipOver : public Exception
{
	//処理を飛ばすためのもの。
public:

};*/

class BadAlloc : public ExceptionWithMessage
{
	//メモリ確保に失敗した場合。
public:
	BadAlloc(std::string_view mes)
		: ExceptionWithMessage(std::format("BAD_ALLOC : {}", mes))
	{}
};

class BadFile : public adapt::ExceptionWithMessage
{
public:
	BadFile(std::string_view mes)
		: adapt::ExceptionWithMessage(std::format("BAD_FILE : {}", mes))
	{}
};

class InvalidLayer : public ExceptionWithMessage
{
	//階層に問題がある場合。
public:
	InvalidLayer(std::string_view mes)
		: ExceptionWithMessage(std::format("INVALID_LAYER : {}", mes))
	{}
};

class InvalidArg : public ExceptionWithMessage
{
	//引数に問題がある場合。
public:
	InvalidArg(std::string_view mes)
		: ExceptionWithMessage(std::format("INVALID_ARG : {}", mes))
	{}
};

class MismatchType : public ExceptionWithMessage
{
	//主に動的型を扱う際に型の誤りがあった場合。
public:
	MismatchType(std::string_view mes)
		: ExceptionWithMessage(std::format("MISMATCH_TYPE : {}", mes))
	{}
};

class Forbidden : public ExceptionWithMessage
{
	//静的型を動的に処理する場合などで、
	//呼び出してはいけない関数などを呼び出した場合。
public:
	Forbidden(std::string_view mes)
		: ExceptionWithMessage(std::format("FORBIDDEN : {}", mes))
	{}
};

class NotInitialized : public ExceptionWithMessage
{
	//未初期化の状態でアクセスすると投げられる例外。
public:
	NotInitialized(std::string_view mes)
		: ExceptionWithMessage(std::format("NOT_INITIALIZED : {}", mes))
	{}
};

}

#endif