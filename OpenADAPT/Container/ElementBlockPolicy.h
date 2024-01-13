#ifndef ADAPT_CONTAINER_ELEMENTBLOCK_POLICY_H
#define ADAPT_CONTAINER_ELEMENTBLOCK_POLICY_H

#include <OpenADAPT/Utility/Utility.h>
#include <OpenADAPT/Common/Definition.h>
#include <OpenADAPT/Evaluator/Placeholder.h>

namespace adapt
{

namespace detail
{

class SElementBlockPolicy
{
private:
	template <size_t N, class ...Placeholders, class ...Args>
	static void Create_rec(const std::tuple<Placeholders...>& phs, char* ptr, std::tuple<Args...> args)
	{
		if constexpr (N < sizeof...(Placeholders))
		{
			auto& ph = std::get<N>(phs);
			using ValueType = std::remove_cvref_t<decltype(ph)>::RetType;
			static_assert(std::convertible_to<GetType_t<N, Args...>, ValueType>);
			char* pos = ptr + ph.GetPtrOffset();
			new (pos) ValueType(std::get<N>(args));
			Create_rec<N + 1>(phs, ptr, std::move(args));
		}
	}
public:
	template <class ...Placeholders, class ...Args>
	static void Create(const std::tuple<Placeholders...>& phs, char* ptr, Args&& ...args)
	{
		static_assert(sizeof...(Placeholders) == sizeof...(Args));
		Create_rec<0>(phs, ptr, std::forward_as_tuple(std::forward<Args>(args)...));
	}
	template <class ...Placeholders, class ...Args>
	static void Create_static(const std::tuple<Placeholders...>& phs, char* ptr, Args&& ...args)
	{
		//SElementの場合、Create_staticとCreateは全く同等である。
		Create(phs, ptr, std::forward<Args>(args)...);
	}

private:
	template <size_t N, class ...Placeholders>
	static void Create_default_rec(const std::tuple<Placeholders...>& phs, char* ptr)
	{
		if constexpr (N < sizeof...(Placeholders))
		{
			auto& ph = std::get<N>(phs);
			using ValueType = std::remove_cvref_t<decltype(ph)>::RetType;
			char* pos = ptr + ph.GetPtrOffset();
			new (pos) ValueType();
			Create_default_rec<N + 1>(phs, ptr);
		}
	}
public:
	template <class ...Placeholders>
	static void Create_default(const std::tuple<Placeholders...>& phs, char* ptr)
	{
		Create_default_rec<0>(phs, ptr);
	}

private:
	template <size_t N, class ...Placeholders>
	static void Move_impl(const std::tuple<Placeholders...>& phs, char* from, char* to)
	{
		if constexpr (N < sizeof...(Placeholders))
		{
			ptrdiff_t offset = std::get<N>(phs).GetPtrOffset();
			using ValueType = GetType_t<N, Placeholders...>::RetType;
			ValueType* t_from = std::launder(reinterpret_cast<ValueType*>(from + offset));
			ValueType* t_to = std::launder(reinterpret_cast<ValueType*>(to + offset));
			*t_to = std::move(*t_from);
			Move_impl<N + 1>(phs, from, to);
		}
	}
public:
	template <class ...Placeholders>
	static void Move(const std::tuple<Placeholders...>& phs, char* from, char* to)
	{
		Move_impl<0>(phs, from, to);
	}

private:
	template <size_t N, class ...Placeholders>
	static void MoveAndDestroy_rec(const std::tuple<Placeholders...>& phs, char* from, char* to)
	{
		if constexpr (N < sizeof...(Placeholders))
		{
			ptrdiff_t offset = std::get<N>(phs).GetPtrOffset();
			using ValueType = std::remove_cvref_t<GetType_t<N, Placeholders...>>::RetType;
			ValueType* t_from = std::launder(reinterpret_cast<ValueType*>(from + offset));
			new (to + offset) ValueType(std::move(*t_from));
			t_from->~ValueType();
			MoveAndDestroy_rec<N + 1>(phs, from, to);
		}
	}
public:
	//ある要素fromの各フィールドを要素toへと移動した後、fromのフィールドのデストラクタを呼ぶ。
	//移動先のメモリ領域は未初期化、つまりコンストラクタの呼び出しが起きていないと仮定している。
	template <class ...Placeholders>
	static void MoveAndDestroy(const std::tuple<Placeholders...>& phs, char* from, char* to)
	{
		MoveAndDestroy_rec<0>(phs, from, to);
	}

private:
	template <size_t N, class ...Placeholders>
	static void Destroy_rec(const std::tuple<Placeholders...>& phs, char* ptr)
	{
		if constexpr (N < sizeof...(Placeholders))
		{
			ptrdiff_t offset = std::get<N>(phs).GetPtrOffset();
			using ValueType = std::remove_cvref_t<GetType_t<N, Placeholders...>>::RetType;
			ValueType* t_ptr = std::launder(reinterpret_cast<ValueType*>(ptr + offset));
			t_ptr->~ValueType();
			Destroy_rec<N + 1>(phs, ptr);
		}
	}
public:
	template <class ...Placeholders>
	static void Destroy(const std::tuple<Placeholders...>& phs, char* ptr)
	{
		Destroy_rec<0>(phs, ptr);
	}
};

class DElementBlockPolicy
{
private:
	template <class ValueType, class Arg>
		requires std::convertible_to<Arg, ValueType>
	static void CreateValue_impl(Arg&& arg, char* pos, int)
	{
		assert((intptr_t)(pos) % alignof(ValueType) == 0);
		//CreateValueが作る総当り呼び出しの中での余計な型変換による警告を避けるため、
		//数値型に関しては明示的に変換する。
		if constexpr (std::is_arithmetic_v<ValueType>)
			new (pos) ValueType(static_cast<ValueType>(arg));
		else if constexpr (IsComplex_v<ValueType> && std::is_arithmetic_v<std::decay_t<Arg>>)
			new (pos) ValueType(static_cast<ValueType::value_type>(arg));
		else
			new (pos) ValueType(std::forward<Arg>(arg));
	}
	template <class ValueType, class Arg>
	static void CreateValue_impl(Arg&&, char*, long)
	{
		throw MismatchType(std::format("cannot convert argument type \"{}\" to \"{}\"",
									   typeid(Arg).name(), typeid(ValueType).name()));
	}
	template <FieldType Type, class Arg>
	static void CreateValue(Arg&& arg, char* pos)
	{
		CreateValue_impl<DFieldInfo::TagTypeToValueType<Type>>(std::forward<Arg>(arg), pos, 0);
	}

	template <class Container>
	static void Create_rec(std::vector<eval::RttiPlaceholder<Container>>::const_iterator, char*) {}
	template <class Container, class Arg, class ...Args>
	static void Create_rec(std::vector<eval::RttiPlaceholder<Container>>::const_iterator it, char* ptr, Arg&& arg, Args&& ...args)
	{
		using enum FieldType;
		char* pos = ptr + it->GetPtrOffset();
		switch (it->GetType())
		{
		case I08: CreateValue<I08>(std::forward<Arg>(arg), pos); break;
		case I16: CreateValue<I16>(std::forward<Arg>(arg), pos); break;
		case I32: CreateValue<I32>(std::forward<Arg>(arg), pos); break;
		case I64: CreateValue<I64>(std::forward<Arg>(arg), pos); break;
		case F32: CreateValue<F32>(std::forward<Arg>(arg), pos); break;
		case F64: CreateValue<F64>(std::forward<Arg>(arg), pos); break;
		case C32: CreateValue<C32>(std::forward<Arg>(arg), pos); break;
		case C64: CreateValue<C64>(std::forward<Arg>(arg), pos); break;
		case Str: CreateValue<Str>(std::forward<Arg>(arg), pos); break;
		case Jbp: CreateValue<Jbp>(std::forward<Arg>(arg), pos); break;
		default: throw MismatchType("");
		}
		Create_rec<Container>(++it, ptr, std::forward<Args>(args)...);
	}
public:
	template <class Container, class ...Args>
	static void Create(const std::vector<eval::RttiPlaceholder<Container>>& ms, char* ptr, Args&& ...args)
	{
		Create_rec<Container>(ms.begin(), ptr, std::forward<Args>(args)...);
	}

private:
	template <class Container>
	static void Create_static_rec(std::vector<eval::RttiPlaceholder<Container>>::const_iterator, char*) {}
	template <class Container, class Arg, class ...Args>
	static void Create_static_rec(std::vector<eval::RttiPlaceholder<Container>>::const_iterator it, char* ptr, Arg&& arg, Args&& ...args)
	{
		using Type = std::remove_cvref_t<Arg>;
		assert(it->GetType() == DFieldInfo::ValueTypeToTagType<Type>());
		assert((intptr_t)(ptr + it->GetPtrOffset()) % alignof(Type) == 0);
		new (ptr + it->GetPtrOffset()) Type(std::forward<Arg>(arg));
		Create_static_rec(++it, ptr, std::forward<Args>(args)...);
	}
public:
	template <class Container, class ...Args>
	static void Create_static(const std::vector<eval::RttiPlaceholder<Container>>& ms, char* ptr, Args&& ...args)
	{
		Create_static_rec(ms.begin(), ptr, std::forward<Args>(args)...);
	}

	template <FieldType Type>
	static void CreateDefaultValue(char* pos)
	{
		using ValueType = DFieldInfo::TagTypeToValueType<Type>;
		assert((intptr_t)(pos) % alignof(ValueType) == 0);
		new (pos) ValueType{};
	}
	template <class Container>
	static void Create_default(const std::vector<eval::RttiPlaceholder<Container>>& phs, char* ptr)
	{
		using enum FieldType;
		for (const auto& ph : phs)
		{
			char* pos = ptr + ph.GetPtrOffset();
			switch (ph.GetType())
			{
			case I08: CreateDefaultValue<I08>(pos); break;
			case I16: CreateDefaultValue<I16>(pos); break;
			case I32: CreateDefaultValue<I32>(pos); break;
			case I64: CreateDefaultValue<I64>(pos); break;
			case F32: CreateDefaultValue<F32>(pos); break;
			case F64: CreateDefaultValue<F64>(pos); break;
			case C32: CreateDefaultValue<C32>(pos); break;
			case C64: CreateDefaultValue<C64>(pos); break;
			case Str: CreateDefaultValue<Str>(pos); break;
			case Jbp: CreateDefaultValue<Jbp>(pos); break;
			default: throw MismatchType("");
			}
		}
	}

private:
	template <class Type>
	static void Move_impl(char* from, char* to)
	{
		Type* t_from = std::launder(reinterpret_cast<Type*>(from));
		Type* t_to = std::launder(reinterpret_cast<Type*>(to));
		*t_to = std::move(*t_from);
	}
public:
	//ある要素fromの各フィールドを要素toへと移動する。
	//こちらはfromのフィールドのデストラクタを呼ばないので、
	//fromのフィールドはムーブコンストラクタで破壊された後の（初期）状態となる。
	template <class Container>
	static void Move(const std::vector<eval::RttiPlaceholder<Container>>& ms, char* from, char* to)
	{
		for (const auto& m : ms)
		{
			FieldType tag = m.GetType();
			auto offset = m.GetPtrOffset();
			if (DFieldInfo::IsTrivial(tag)) std::memcpy(to + offset, from + offset, m.GetSize());
			else if (DFieldInfo::IsStr(tag)) Move_impl<std::string>(from + offset, to + offset);
			else if (DFieldInfo::IsJbp(tag)) Move_impl<JBpos>(from + offset, to + offset);
		}
	}

private:
	template <class Type>
	static void MoveAndDestroy_impl(char* from, char* to)
	{
		Type* t_from = std::launder(reinterpret_cast<Type*>(from));
		new (to) Type(std::move(*t_from));
		t_from->~Type();
	}
public:
	//ある要素fromの各フィールドを要素toへと移動した後、fromのフィールドのデストラクタを呼ぶ。
	//移動先のメモリ領域は未初期化、つまりコンストラクタの呼び出しが起きていないと仮定している。
	template <class Container>
	static void MoveAndDestroy(const std::vector<eval::RttiPlaceholder<Container>>& ms, char* from, char* to)
	{
		for (const auto& m : ms)
		{
			FieldType tag = m.GetType();
			auto offset = m.GetPtrOffset();
			if (DFieldInfo::IsTrivial(tag)) std::memcpy(to + offset, from + offset, m.GetSize());
			else if (DFieldInfo::IsStr(tag)) MoveAndDestroy_impl<std::string>(from + offset, to + offset);
			else if (DFieldInfo::IsJbp(tag)) MoveAndDestroy_impl<JBpos>(from + offset, to + offset);
		}
	}

	template <class Container>
	static void Destroy(const std::vector<eval::RttiPlaceholder<Container>>& ms, char* ptr)
	{
		//フィールドを全て削除する。
		//ただし下層のElementBlockはDestroyElementBlockで別途削除すること。
		for (const auto& m : ms) Destroy(m, ptr);
	}
	template <class Container>
	static void Destroy(const eval::RttiPlaceholder<Container>& ph, char* ptr)
	{
		//指定されたフィールドだけを削除する。
		//ptrは要素先頭のポインタ。削除するフィールドへのポインタではない。
		Destroy(ph.GetType(), ptr + ph.GetPtrOffset());
	}
	static void Destroy(FieldType type, char* ptr)
	{
		//指定されたフィールドだけを削除する。
		using enum FieldType;
		//INT、FLT,、CPXに属すものはデストラクタを呼ぶ必要はない。
		if (DFieldInfo::IsTrivial(type)) return;
		if (DFieldInfo::IsStr(type)) Destroy_impl<DFieldInfo::TagTypeToValueType<Str>>(ptr);
		else if (DFieldInfo::IsJbp(type)) Destroy_impl<DFieldInfo::TagTypeToValueType<Jbp>>(ptr);
	}
private:
	template <class Type>
	static void Destroy_impl(char* ptr)
	{
		Type* t_ptr = std::launder(reinterpret_cast<Type*>(ptr));
		t_ptr->~Type();
		//ここで例外が投げられる場合、DTreeに対してSetTopLayerを呼んでいない可能性がある。
	}
public:
};


}

}

#endif
