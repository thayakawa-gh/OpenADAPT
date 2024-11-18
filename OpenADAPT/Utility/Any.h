#ifndef ADAPT_UTILITY_ANY_H
#define ADAPT_UTILITY_ANY_H

#include <new>
#include <memory>
#include <cassert>
#include <typeindex>
#include <OpenADAPT/Utility/TypeTraits.h>
#include <OpenADAPT/Utility/Exception.h>

namespace adapt
{

namespace detail
{

//AllowBigObjがfalseの場合、StrgSizeより大きなオブジェクトは格納できなくなる。
template <size_t StrgSize, bool AllowBigObj>
class Any_impl
{
	template <class T>
	static constexpr bool IsAny()
	{
		return std::is_same_v<std::decay_t<T>, Any_impl>;
	}

	template <class T>
	static constexpr bool IsSmall()
	{
		return
			sizeof(T) <= StrgSize &&
			std::is_nothrow_move_constructible_v<T>;
	}

	struct RTFuncs
	{
		template <class T, bool Small>
		static void Copy(Any_impl& to, const Any_impl& from)
		{
			if constexpr (Small)
			{
				if constexpr (!std::is_copy_constructible_v<T>) throw MismatchType("T is not copy constructible");
				else
				{
					new (&to.m_storage.m_small.m_data) T(from.Get_unsafe<T>());
				}
			}
			else if constexpr (AllowBigObj)
			{
				if constexpr (!std::is_copy_constructible_v<T>) throw MismatchType("T is not copy constructible");
				else to.m_storage.m_big.m_ptr = new T(from.Get_unsafe<T>());
			}
			else
				static_assert([]() { return false; }(), "size of the template argument is greater than Any's storage.");
		}
		template <class T, bool Small>
		static void Move(Any_impl& to, Any_impl&& from) noexcept
		{
			if constexpr (Small)
			{
				auto& t = *std::launder(reinterpret_cast<T*>(&to.m_storage.m_small.m_data));
				auto& f = *std::launder(reinterpret_cast<T*>(&from.m_storage.m_small.m_data));
				new (&t) T(std::move(f));
				f.~T();
			}
			else if constexpr (AllowBigObj)
			{
				to.m_storage.m_big.m_ptr = from.m_storage.m_big.m_ptr;
				from.m_storage.m_big.m_ptr = nullptr;
			}
			else
				static_assert([]() { return false; }(), "size of the template argument is greater than Any's storage.");
		}
		template <class T, bool Small>
		static void Destroy(Any_impl& to) noexcept
		{
			if constexpr (Small)
			{
				auto& t = *std::launder(reinterpret_cast<T*>(&to.m_storage.m_small.m_data));
				t.~T();
				to.m_storage.m_small = SmallStorage{};
			}
			else if constexpr (AllowBigObj)
			{
				auto* t = static_cast<T*>(to.m_storage.m_big.m_ptr);
				delete t;
				to.m_storage.m_big = BigStorage{};
			}
			else
				static_assert([]() { return false; }(), "size of the template argument is greater than Any's storage.");
		}
		template <class T>
		static const std::type_info& TypeInfo()
		{
			return typeid(T);
		}
		using CopyF = void(*)(Any_impl&, const Any_impl&);
		using MoveF = void(*)(Any_impl&, Any_impl&&);
		using DestroyF = void(*)(Any_impl&);
		using TypeInfoF = const std::type_info& (*)(void);

		CopyF m_copy = nullptr;
		MoveF m_move = nullptr;
		DestroyF m_destroy = nullptr;
		TypeInfoF m_type_info = nullptr;
	};
	template <class T, bool Small = IsSmall<T>()>
	inline static constexpr RTFuncs RTFuncs_value =
	{ &RTFuncs::template Copy<T, Small>, &RTFuncs::template Move<T, Small>,
	  &RTFuncs::template Destroy<T, Small>, &RTFuncs::template TypeInfo<T> };

public:

	Any_impl() : m_storage{}, m_rt_funcs(nullptr) {}
	template <class T, std::enable_if_t<!std::is_same_v<std::decay_t<T>, Any_impl>, std::nullptr_t> = nullptr>
	Any_impl(T&& v)
		: m_rt_funcs(nullptr)
	{
		Emplace_impl<std::decay_t<T>>(std::forward<T>(v));
	}
	Any_impl(const Any_impl& a)
		: m_rt_funcs(nullptr)
	{
		if (!a.IsEmpty()) Copy(a);
	}
	Any_impl(Any_impl&& a) noexcept
		: m_rt_funcs(nullptr)
	{
		if (!a.IsEmpty()) Move(std::move(a));
	}

	template <class T, std::enable_if_t<!std::is_same_v<std::decay_t<T>, Any_impl>, std::nullptr_t> = nullptr>
	Any_impl& operator=(T&& v)
	{
		Emplace<std::decay_t<T>>(std::forward<T>(v));
		return *this;
	}

	Any_impl& operator=(const Any_impl& a)
	{
		if (!IsEmpty()) Destroy();
		if (!a.IsEmpty()) Copy(a);
		return *this;
	}
	Any_impl& operator=(Any_impl&& a) noexcept
	{
		if (!IsEmpty()) Destroy();
		if (!a.IsEmpty()) Move(std::move(a));
		return *this;
	}
	~Any_impl()
	{
		if (!IsEmpty()) Destroy();
	}

	template <class T, class ...Args>
	void Emplace(Args&& ...args)
	{
		if (!IsEmpty()) Destroy();
		Emplace_impl<T>(std::forward<Args>(args)...);
	}

	bool IsEmpty() const { return m_rt_funcs == nullptr; }
	operator bool() const { return !IsEmpty(); }
	template <class T>
	bool Is() const
	{
		return !IsEmpty() && typeid(T) == TypeInfo();
	}
	const std::type_info& GetType() const
	{
		if (IsEmpty()) return typeid(EmptyClass);
		return TypeInfo();
	}

	template <class T>
	const T& Get() const&
	{
		if (!Is<T>()) throw MismatchType("bad cast of Any");
		return Get_unsafe<T>();
	}
	template <class T>
	T& Get()&
	{
		if (!Is<T>()) throw MismatchType("bad cast of Any");
		return Get_unsafe<T>();
	}
	template <class T>
	T&& Get()&&
	{
		if (!Is<T>()) throw MismatchType("bad cast of Any");
		return Get_unsafe<T>();
	}

	template <class T>
	const T& Get_unsafe() const&
	{
		assert(!IsEmpty());
		if constexpr (IsSmall<T>())
			return *std::launder(reinterpret_cast<const T*>(&m_storage.m_small.m_data));
		else if constexpr (AllowBigObj)
			return *static_cast<const T*>(m_storage.m_big.m_ptr);
		else
			static_assert([]() { return false; }(), "size of the template argument is greater than Any's storage.");
	}
	template <class T>
	T& Get_unsafe()&
	{
		assert(!IsEmpty());
		if constexpr (IsSmall<T>())
			return *std::launder(reinterpret_cast<T*>(&m_storage.m_small.m_data));
		else if constexpr (AllowBigObj)
			return *static_cast<T*>(m_storage.m_big.m_ptr);
		else
			static_assert([]() { return false; }(), "size of the template argument is greater than Any's storage.");
	}
	template <class T>
	T&& Get_unsafe()&&
	{
		assert(!IsEmpty());
		if constexpr (IsSmall<T>())
			return std::move(*reinterpret_cast<T*>(&m_storage.m_small.m_data));
		else if constexpr (AllowBigObj)
			return std::move(*static_cast<T*>(m_storage.m_big.m_ptr));
		else
			static_assert([]() { return false; }(), "size of the template argument is greater than Any's storage.");
	}

	//Get_unsafeと違い、型チェックなどを一切行わない。
	template <class T, bool B = AllowBigObj, std::enable_if_t<!B, std::nullptr_t> = nullptr>
	T& Cast()&
	{
		return reinterpret_cast<T&>(m_storage.m_small.m_data);
	}
	template <class T, bool B = AllowBigObj, std::enable_if_t<!B, std::nullptr_t> = nullptr>
	const T& Cast() const&
	{
		return reinterpret_cast<const T&>(m_storage.m_small.m_data);
	}

private:

	void Copy(const Any_impl& from)
	{
		assert(IsEmpty());
		m_rt_funcs = from.m_rt_funcs;
		m_rt_funcs->m_copy(*this, from);
	}
	void Move(Any_impl&& from) noexcept
	{
		assert(IsEmpty());
		m_rt_funcs = from.m_rt_funcs;
		m_rt_funcs->m_move(*this, std::move(from));
		from.m_rt_funcs = nullptr;
	}
	void Destroy() noexcept
	{
		assert(!IsEmpty());
		auto* d = m_rt_funcs->m_destroy;
		m_rt_funcs = nullptr;
		d(*this);
	}
	const std::type_info& TypeInfo() const
	{
		assert(!IsEmpty());
		return m_rt_funcs->m_type_info();
	}

	//Tはdecayされているものとする。
	template <class T, class ...Args>
	void Emplace_impl(Args&& ...args)
	{
		assert(IsEmpty());
		if constexpr (IsSmall<T>())
		{
			//small
			new (&m_storage.m_small.m_data) T(std::forward<Args>(args)...);
			m_rt_funcs = &RTFuncs_value<T>;
		}
		else if constexpr (AllowBigObj)
		{
			//big
			m_storage.m_big.m_ptr = new T(std::forward<Args>(args)...);
			m_rt_funcs = &RTFuncs_value<T>;
		}
		else
			static_assert([]() { return false; }, "size of the template argument is greater than Any's storage.");
	}

	struct BigStorage
	{
		char Padding[StrgSize - sizeof(void*)];
		void* m_ptr;
	};
	struct SmallStorage
	{
		std::aligned_storage_t<StrgSize, alignof(std::max_align_t)> m_data;
	};
	template <bool, class = void>
	union Storage
	{
		BigStorage m_big;
		SmallStorage m_small;
	};
	template <class Dummy>
	union Storage<false, Dummy>
	{
		SmallStorage m_small;
	};

	Storage<AllowBigObj> m_storage;
	const RTFuncs* m_rt_funcs;
};

template <>
class Any_impl<0, true>
{
	class PlaceHolder
	{
	public:
		virtual ~PlaceHolder() {}
		virtual PlaceHolder* Clone() const = 0;
	};

	template <class Type>
	class Holder : public PlaceHolder
	{
	public:
		template <class ...Args>
		Holder(Args&& ...args)
			: m_value(std::forward<Args>(args)...)
		{}
	private:
		template <class Dummy = void, bool B = std::is_copy_constructible<Type>::value>
		struct Clone_impl;
		template <class Dummy, bool B>
		struct Clone_impl
		{
			static PlaceHolder* f(const Holder<Type>& h) { return new Holder<Type>(h.m_value); }
		};
		template <class Dummy>
		struct Clone_impl<Dummy, false>
		{
			static PlaceHolder* f(const Holder<Type>&) { throw MismatchType("Assigned class is not copyable."); }
		};
	public:
		virtual PlaceHolder* Clone() const override
		{
			return Clone_impl<>::f(*this);
		}

		Type m_value;
	};

public:

	Any_impl(EmptyClass = EmptyClass()) : m_content(nullptr) {}

	template <class Type, std::enable_if_t<!std::is_same<std::decay_t<Type>, Any_impl>::value, std::nullptr_t> = nullptr>
	Any_impl(Type&& t)
		: m_content(new Holder<std::remove_cvref_t<Type>>(std::forward<Type>(t)))
	{}

	Any_impl(const Any_impl& any)
		: m_content(any.m_content->Clone())
	{}
	Any_impl(Any_impl&& any) noexcept
		: m_content(std::move(any.m_content))
	{}

	Any_impl& operator=(const Any_impl& any)
	{
		m_content.reset(any.m_content->Clone());
		return *this;
	}
	Any_impl& operator=(Any_impl&& any) noexcept
	{
		m_content = std::move(any.m_content);
		return *this;
	}

	template <class Type, std::enable_if_t<!std::is_same<std::decay_t<Type>, Any_impl>::value, std::nullptr_t> = nullptr>
	Any_impl& operator=(Type&& v)
	{
		m_content = std::make_unique<Holder<std::remove_cvref_t<Type>>>(std::forward<Type>(v));
		return *this;
	}

	template <class Type, class ...Args>
	void Emplace(Args&& ...args)
	{
		m_content = std::make_unique<Holder<Type>>(std::forward<Args>(args)...);
	}

	template <class Type>
	Type& Get()&
	{
		if (!Is<Type>()) throw MismatchType("bad Any cast");
		return Get<Type>();
	}
	template <class Type>
	const Type& Get() const&
	{
		if (!Is<Type>()) throw MismatchType("bad Any cast");
		return Get<Type>();
	}
	template <class Type>
	Type&& Get()&&
	{
		if (!Is<Type>()) throw MismatchType("bad Any cast");
		return Get<Type>();
	}

	template <class Type>
	Type& Get_unsafe()&
	{
		return static_cast<Holder<Type>&>(*m_content).m_value;
	}
	template <class Type>
	const Type& Get_unsafe() const&
	{
		return static_cast<const Holder<Type>&>(*m_content).m_value;
	}
	template <class Type>
	Type&& Get_unsafe()&&
	{
		return static_cast<Holder<Type>&&>(*m_content).m_value;
	}

	bool IsEmpty() const { return !m_content; }

	template <class Type>
	bool Is() const
	{
		return dynamic_cast<const Holder<Type>*>(m_content.get()) != nullptr;
	}

private:
	std::unique_ptr<PlaceHolder> m_content;
};

}

using Any = detail::Any_impl<24, true>;
using DynamicAny = detail::Any_impl<0, true>;
template <size_t Size>
using StaticAny = detail::Any_impl<Size, false>;

//shared_ptr+anyみたいなもの。機能的にはshared_ptr<void>と大差ない気がする。
//任意の型の値を格納することができ、なおかつreference countを持ち共有することができる。
//通常のコピーはshared_ptrと同じようにshallow copyするだけで、
//完全に複製するにはClone関数を呼ぶ必要がある。
//循環参照対策をしていないので注意。
class ShareableAny
{
	class PlaceHolder
	{
	public:
		PlaceHolder() : m_count(1) {}
		virtual ~PlaceHolder() {}
		virtual PlaceHolder* Clone() const = 0;

		int Increase() { return ++m_count; }
		int Decrease() { return --m_count; }
		int GetCount() const { return m_count; }
	private:
		int m_count;
	};

	template <class Type>
	class Holder : public PlaceHolder
	{
	public:
		template <class ...Args>
		Holder(Args&& ...args)
			: m_value(std::forward<Args>(args)...)
		{}
	private:
		template <class Dummy = void, bool B = std::is_copy_constructible<Type>::value>
		struct Clone_impl;
		template <class Dummy, bool B>
		struct Clone_impl
		{
			static PlaceHolder* f(const Holder<Type>& h) { return new Holder<Type>(h.m_value); }
		};
		template <class Dummy>
		struct Clone_impl<Dummy, false>
		{
			static PlaceHolder* f(const Holder<Type>&) { throw MismatchType("Assigned class is not copyable."); }
		};
	public:
		virtual PlaceHolder* Clone() const override
		{
			return Clone_impl<>::f(*this);
		}

		Type m_value;
	};

public:

	ShareableAny(EmptyClass = EmptyClass()) : m_content(nullptr) {}

	template <class Type,
		std::enable_if_t<!std::is_same<std::remove_cvref_t<Type>, ShareableAny>::value, std::nullptr_t> = nullptr>
		ShareableAny(Type&& type)
		: m_content(new Holder<Type>(std::forward<Type>(type)))
	{}
	/*template <class Type, class ...Args>
	ShareableAny(Args&& ...args)
		: mContent(new Holder<Type>(std::forward<Args>(args)...))
	{}*/

	//ShareableAnyのコピーはShallowCopyである。
	ShareableAny(const ShareableAny& any)
		: m_content(any.m_content)
	{
		m_content->Increase();
	}
	ShareableAny(ShareableAny&& any) noexcept
		: m_content(any.m_content)
	{
		any.m_content = nullptr;
	}

	ShareableAny& operator=(const ShareableAny& any)
	{
		Delete();
		m_content = any.m_content;
		m_content->Increase();
		return *this;
	}
	ShareableAny& operator=(ShareableAny&& any) noexcept
	{
		Delete();
		m_content = any.m_content;
		any.m_content = nullptr;
		return *this;
	}

	~ShareableAny()
	{
		Delete();
	}

	template <class Type, class ...Args>
	void Emplace(Args&& ...args)
	{
		Delete();
		m_content = new Holder<Type>(std::forward<Args>(args)...);
	}

	bool IsEmpty() const { return !m_content; }
	template <class T>
	bool Is() const
	{
		return dynamic_cast<const Holder<T>*>(m_content) != nullptr;
	}

	template <class Type>
	const Type& Get() const&
	{
		if (!Is<Type>()) throw MismatchType("bad ShareableAny cast");
		return Get_unsafe<Type>();
	}
	template <class Type>
	Type& Get()&
	{
		if (!Is<Type>()) throw MismatchType("bad ShareableAny cast");
		return Get_unsafe<Type>();
	}
	template <class Type>
	Type&& Get()&&
	{
		if (!Is<Type>()) throw MismatchType("bad ShareableAny cast");
		return Get_unsafe<Type>();
	}
	template <class Type>
	Type& Get_unsafe()&
	{
		return static_cast<Holder<Type>&>(*m_content).m_value;
	}
	template <class Type>
	const Type& Get_unsafe() const&
	{
		return static_cast<const Holder<Type>&>(*m_content).m_value;
	}
	template <class Type>
	Type&& Get_unsafe()&&
	{
		return static_cast<const Holder<Type>&>(*m_content).m_value;
	}

	//ShareableAny Clone() const { return ShareableAny(mContent->Clone()); }

	void Delete()
	{
		if (m_content)
		{
			m_content->Decrease();
			if (m_content->GetCount() == 0) delete m_content;
			m_content = nullptr;
		}
	}
	int GetCount() const { return m_content->GetCount(); }

private:
	PlaceHolder* m_content;
};

class AnyURef
{
protected:

	class HolderBase
	{
	public:
		virtual void CopyTo(void* b) const = 0;
		virtual std::type_index GetTypeIndex() const = 0;
	};

	template <class T>
	class Holder : public HolderBase
	{
	public:
		Holder() = default;
		Holder(T v) : m_value(static_cast<T>(v)) {}
		Holder(const Holder& h) : m_value(static_cast<T>(h.m_value)) {}
		virtual void CopyTo(void* ptr) const
		{
			new (ptr) Holder<T>(*this);
		}
		virtual std::type_index GetTypeIndex() const { return typeid(T); }
		T m_value;
	};

	template <class Type>
	void Construct(Type&& v)
	{
		static_assert(sizeof(Holder<Type&&>) >= sizeof(m_storage), "the size of storage is insufficient.");
		new ((void*)&m_storage) Holder<Type&&>(static_cast<Type&&>(v));
	}

	template <class Type>
	using NotAnyURefOrNull = std::enable_if_t<!std::is_same_v<std::remove_cvref_t<Type>, AnyURef> &&
		!std::is_same_v<std::remove_cvref_t<Type>, EmptyClass>, std::nullptr_t>;

public:

	AnyURef(EmptyClass = EmptyClass())
	{
		//こちらだとGCC10.1以降、-std=c++17を有効にしたとき何故かコンパイルエラーになる。
		//new ((void*)&m_storage) Holder<EmptyClass>;

		//一時オブジェクトなので寿命が尽きるが、中身にアクセスすることはないので一時的な対処として。
		Construct(EmptyClass());
	}

	template <class Type, NotAnyURefOrNull<Type> = nullptr>
	AnyURef(Type&& v)
	{
		Construct(std::forward<Type>(v));
	}
	AnyURef(const AnyURef& a)
	{
		auto* p = a.GetHolderBase();
		p->CopyTo(&m_storage);
	}
	template <class Type, NotAnyURefOrNull<Type> = nullptr>
	AnyURef& operator=(Type&& v)
	{
		Construct(std::forward<Type>(v));
		return *this;
	}
	AnyURef& operator=(const AnyURef& a)
	{
		//copy
		const HolderBase* q = a.GetHolderBase();
		q->CopyTo(&m_storage);
		return *this;
	}

	template <class Type>
	Type Get() const
	{
		if (!Is<Type>()) throw MismatchType("bad AnyRef cast");
		return Get_unsafe<Type>();
	}
	template <class Type>
	Type Get_unsafe() const
	{
		const Holder<Type>* p = GetHolder_unsafe<Type>();
		return static_cast<Type>(p->m_value);
	}

	template <class Type>
	bool Is() const
	{
		return GetHolder<Type>() != nullptr;
	}

	bool IsEmpty() const
	{
		return Is<EmptyClass&&>();
	}

	std::type_index GetTypeIndex() const
	{
		return GetHolderBase()->GetTypeIndex();
	}

private:

	template <class Type>
	const Holder<Type>* GetHolder_unsafe() const
	{
		const HolderBase* p = std::launder(reinterpret_cast<const HolderBase*>(&m_storage));
		return static_cast<const Holder<Type>*>(p);
	}
	template <class Type>
	const Holder<Type>* GetHolder() const
	{
		const HolderBase* p = std::launder(reinterpret_cast<const HolderBase*>(&m_storage));
		return dynamic_cast<const Holder<Type>*>(p);
	}
	const HolderBase* GetHolderBase() const
	{
		return std::launder(reinterpret_cast<const HolderBase*>(&m_storage));
	}

	std::aligned_storage_t<16> m_storage;
};

class AnyRef : public AnyURef
{
	using Base = AnyURef;

	template <class Type>
	using NotURefOrNullOrConst = std::enable_if_t<!std::is_base_of_v<AnyURef, Type> &&
		!std::is_const_v<Type> &&
		!std::is_same_v<Type, EmptyClass>, std::nullptr_t>;

public:

	AnyRef(EmptyClass = EmptyClass()) : AnyURef() {}

	template <class Type, NotURefOrNullOrConst<Type> = nullptr>
	AnyRef(Type& v) : Base(v)
	{}
	AnyRef(const AnyRef& a) : Base(static_cast<const AnyURef&>(a))
	{}

	AnyRef& operator=(const AnyRef& a)
	{
		Base::operator=(static_cast<const AnyURef&>(a));
		return *this;
	}
	template <class Type, NotURefOrNullOrConst<Type> = nullptr>
	AnyRef& operator=(Type& a)
	{
		Base::operator=(a);
		return *this;
	}

	template <class Type>
	Type& Get() const { return Base::Get<Type&>(); }
	template <class Type>
	Type& Get_unsafe() const { return Base::Get_unsafe<Type&>(); }
	template <class Type>
	bool Is() const { return Base::Is<Type&>(); }
};
class AnyCRef : public AnyURef
{
	using Base = AnyURef;

	template <class Type>
	using NotAnyURefOrNull = std::enable_if_t<!std::is_base_of_v<AnyURef, Type> &&
		!std::is_same_v<Type, EmptyClass>, std::nullptr_t>;

public:

	AnyCRef(EmptyClass = EmptyClass{}) : AnyURef() {}

	template <class Type, NotAnyURefOrNull<Type> = nullptr>
	AnyCRef(const Type& v) : Base(v)
	{}
	AnyCRef(const AnyCRef& a) : Base(static_cast<const AnyURef&>(a))
	{}

	AnyCRef& operator=(const AnyCRef& a)
	{
		Base::operator=(static_cast<const AnyURef&>(a));
		return *this;
	}
	template <class Type, NotAnyURefOrNull<Type> = nullptr>
	AnyCRef& operator=(const Type& a)
	{
		Base::operator=(a);
		return *this;
	}

	template <class Type>
	const Type& Get() const { return Base::Get<const Type&>(); }
	template <class Type>
	const Type& Get_unsafe() const { return Base::Get_unsafe<const Type&>(); }
	template <class Type>
	bool Is() const { return Base::Is<const Type&>(); }
};
class AnyRRef : public AnyURef
{
	using Base = AnyURef;

	template <class Type>
	using NotURefNullLValue = std::enable_if_t<!std::is_base_of_v<AnyURef, Type>&&
		std::is_rvalue_reference_v<Type&&> &&
		!std::is_same_v<Type, EmptyClass>, std::nullptr_t>;
public:

	AnyRRef(EmptyClass = EmptyClass()) : AnyURef() {}

	template <class Type, NotURefNullLValue<Type> = nullptr>
	AnyRRef(Type&& v) : Base(std::move(v))
	{}
	AnyRRef(const AnyRRef& a) : Base(static_cast<const AnyURef&>(a))
	{}

	AnyRRef& operator=(const AnyRRef& a)
	{
		Base::operator=(static_cast<const AnyRRef&>(a));
		return *this;
	}
	template <class Type, NotURefNullLValue<Type> = nullptr>
	AnyRRef& operator=(Type&& a)
	{
		Base::operator=(a);
		return *this;
	}

	template <class Type>
	Type&& Get() const { return Base::Get<Type&&>(); }
	template <class Type>
	Type&& Get_unsafe() const { return Base::Get_unsafe<Type&&>(); }
	template <class Type>
	bool Is() const { return Base::Is<Type&&>(); }
};

}

#endif