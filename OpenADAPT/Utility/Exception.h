#ifndef ADAPT_UTILITY_EXCEPTION_H
#define ADAPT_UTILITY_EXCEPTION_H

#include <exception>
#include <string>
#include <string_view>
#include <format>
#include <OpenADAPT/Utility/Common.h>

namespace adapt
{

ADAPT_EXPORT
class Exception
{
public:
	virtual ~Exception() = default;
	virtual const char* what() const noexcept { return "EXCEPTION"; }
};

ADAPT_EXPORT
class ExceptionWithMessage : public Exception
{
public:
	virtual ~ExceptionWithMessage() = default;
	ExceptionWithMessage(std::string_view mes) : m_message(mes) {}
	const std::string& GetMessage() const { return m_message; }
	virtual const char* what() const noexcept override { return m_message.c_str(); }
private:
	std::string m_message;
};

ADAPT_EXPORT
class NoElements : public Exception
{
	//コンテナの要素が見つからない、範囲外などの場合に投げられる。
public:
	virtual ~NoElements() = default;
	virtual const char* what() const noexcept override { return "NO_ELEMENTS"; }

};

ADAPT_EXPORT
class JointError : public Exception
{
public:
	virtual ~JointError() = default;
	virtual const char* what() const noexcept override { return "JOINT_ERROR"; }

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

ADAPT_EXPORT
class BadAlloc : public ExceptionWithMessage
{
	//メモリ確保に失敗した場合。
public:
	virtual ~BadAlloc() = default;
	BadAlloc(std::string_view mes)
		: ExceptionWithMessage(std::format("BAD_ALLOC : {}", mes))
	{}
};

ADAPT_EXPORT
class BadFile : public adapt::ExceptionWithMessage
{
public:
	virtual ~BadFile() = default;
	BadFile(std::string_view mes)
		: adapt::ExceptionWithMessage(std::format("BAD_FILE : {}", mes))
	{}
};

ADAPT_EXPORT
class InvalidLayer : public ExceptionWithMessage
{
	//階層に問題がある場合。
public:
	virtual ~InvalidLayer() = default;
	InvalidLayer(std::string_view mes)
		: ExceptionWithMessage(std::format("INVALID_LAYER : {}", mes))
	{}
};

ADAPT_EXPORT
class InvalidArg : public ExceptionWithMessage
{
	//引数に問題がある場合。
public:
	virtual ~InvalidArg() = default;
	InvalidArg(std::string_view mes)
		: ExceptionWithMessage(std::format("INVALID_ARG : {}", mes))
	{}
};

ADAPT_EXPORT
class MismatchType : public ExceptionWithMessage
{
	//主に動的型を扱う際に型の誤りがあった場合。
public:
	virtual ~MismatchType() = default;
	MismatchType(std::string_view mes)
		: ExceptionWithMessage(std::format("MISMATCH_TYPE : {}", mes))
	{}
};

ADAPT_EXPORT
class Forbidden : public ExceptionWithMessage
{
	//静的型を動的に処理する場合などで、
	//呼び出してはいけない関数などを呼び出した場合。
public:
	virtual ~Forbidden() = default;
	Forbidden(std::string_view mes)
		: ExceptionWithMessage(std::format("FORBIDDEN : {}", mes))
	{}
};

ADAPT_EXPORT
class NotInitialized : public ExceptionWithMessage
{
	//未初期化の状態でアクセスすると投げられる例外。
public:
	virtual ~NotInitialized() = default;
	NotInitialized(std::string_view mes)
		: ExceptionWithMessage(std::format("NOT_INITIALIZED : {}", mes))
	{}
};

}

#endif