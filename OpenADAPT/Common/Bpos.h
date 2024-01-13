#ifndef ADAPT_COMMON_BPOS_H
#define ADAPT_COMMON_BPOS_H

#include <cassert>
#include <cstring>
#include <vector>
#include <array>
#include <OpenADAPT/Common/Definition.h>

namespace adapt
{

struct Bpos final
{
	static constexpr int msMaxSmallLayer = 2;

	Bpos() : mLayer(-1), mRow(0), mTposPtr(nullptr) {}
	explicit Bpos(LayerType l)
		: mLayer(l), mRow(0), mTposPtr(nullptr)
	{
		Init(l);
	}
	/*template <std::integral ...Indices>
	explicit Bpos(Indices ...i)
		: mLayer((LayerType)sizeof...(Indices) - 1), mTposPtr(nullptr)
	{
		Init(mLayer);
		Assign(i...);
	}*/
	Bpos(std::initializer_list<BindexType> init)
		: mLayer((LayerType)init.size() - 1), mTposPtr(nullptr)
	{
		auto it = init.begin();
		mRow = *it;
		++it;
		if (mLayer > msMaxSmallLayer)
		{
			mTposPtr = new BindexType[init.size() - 1];
			BindexType* t = mTposPtr;
			for (; it != init.end(); ++it, ++t) *t = *it;
		}
		else
		{
			BindexType* t = mTposArr.data();
			for (; it != init.end(); ++it, ++t) *t = *it;
		}
	}

	Bpos(const Bpos& b)
		: mLayer(-1), mRow(0), mTposPtr(nullptr)
	{
		//operator=を直接呼び出すとdelete[]で無効なポインタを消そうとしてバグる。nullptrを代入しておくのも手だが。
		mLayer = b.mLayer;
		if (mLayer > msMaxSmallLayer)
		{
			mTposPtr = new BindexType[b.mLayer];
			std::memcpy(mTposPtr, b.mTposPtr, mLayer * sizeof(BindexType));
		}
		else
		{
			for (int i = 0; i < mLayer; ++i) mTposArr[i] = b.mTposArr[i];
		}
		mRow = b.mRow;
	}
	Bpos(Bpos&& b) noexcept
		: mLayer(-1), mRow(0), mTposPtr(nullptr)
	{
		//operator=を直接呼び出すとdelete[]で無効なポインタを消そうとしてバグる。nullptrを代入しておくのも手だが。
		mRow = b.mRow;
		mLayer = b.mLayer;
		if (mLayer > msMaxSmallLayer)
			mTposPtr = b.mTposPtr;
		else
			mTposArr = b.mTposArr;
		b.mTposPtr = nullptr;
	}
	~Bpos()
	{
		if (mLayer > msMaxSmallLayer) delete[] mTposPtr;
	}

	inline Bpos& operator=(const Bpos& b)
	{
		mRow = b.mRow;
		if (b.mLayer <= msMaxSmallLayer)
		{
			if (mLayer > msMaxSmallLayer)
			{
				delete[] mTposPtr;
				mTposPtr = nullptr;
			}
			mLayer = b.mLayer;
			mTposArr = b.mTposArr;
		}
		else
		{
			if (mLayer <= msMaxSmallLayer) mTposPtr = new BindexType[b.mLayer];
			mLayer = b.mLayer;
			for (int i = 0; i < mLayer; ++i) mTposPtr[i] = b.mTposPtr[i];
		}
		return *this;
	}
	inline Bpos& operator=(Bpos&& b) noexcept
	{
		mRow = b.mRow;
		mLayer = b.mLayer;
		if (mLayer > msMaxSmallLayer) delete[] mTposPtr;
		if (b.mLayer > msMaxSmallLayer)
		{
			mTposPtr = b.mTposPtr;
			b.mTposPtr = nullptr;
		}
		else
		{
			mTposArr = b.mTposArr;
		}
		return *this;
	}

	inline BindexType& operator[](int layer)
	{
		assert(0 <= layer && layer <= mLayer);
		if (layer == 0) return mRow;
		if (mLayer > msMaxSmallLayer) return mTposPtr[layer - 1];
		else return mTposArr[layer - 1];
	}
	inline BindexType operator[](int layer) const
	{
		assert(0 <= layer && layer <= mLayer);
		if (layer == 0) return mRow;
		if (mLayer > msMaxSmallLayer) return mTposPtr[layer - 1];
		else return mTposArr[layer - 1];
	}
	inline const BindexType* GetTpos() const
	{
		//return mTpos;
		return mLayer > msMaxSmallLayer ? mTposPtr : mTposArr.data();
	}
	inline BindexType GetTpos(int layer) const
	{
		//return mTpos[layer - 1];
		return mLayer > msMaxSmallLayer ? mTposPtr[layer - 1] : mTposArr[layer - 1];
	}
	inline BindexType& GetTpos(int layer)
	{
		return mLayer > msMaxSmallLayer ? mTposPtr[layer - 1] : mTposArr[layer - 1];
	}
	/*inline const BindexType& GetLine() const
	{
		return mRow;
	}
	inline BindexType& GetLine()
	{
		return mRow;
	}*/
	inline const BindexType& GetRow() const
	{
		return mRow;
	}
	inline BindexType& GetRow()
	{
		return mRow;
	}
	inline LayerType GetLayer() const
	{
		return (LayerType)mLayer;
	}

	//cposとcをより階層が小さい方のclayerまで比較し、一致すればtrue、不一致ならfalseを返却する。
	//clayerの異なるbpos比較用関数。
	/*inline bool MatchPartially(const Bpos& b) const
	{
		LayerType layer = std::min(mLayer, b.mLayer);
		return MatchPartially(b, layer);
	}*/
	inline bool MatchPartially(const Bpos& b, LayerType maxlayer) const
	{
		assert(mLayer >= maxlayer && b.mLayer >= maxlayer);
		if (mRow != b.mRow) return false;
		return Eq(GetTpos(), b.GetTpos(), maxlayer);
	}
	//layerまでが一致するかどうかを調べる。layerより下は無視される。
	//これ自身とb、双方がlayer以上の層数を持つ必要がある。
	bool MatchPerfectly(const Bpos& b, int layer) const
	{
		assert(mLayer >= layer && b.mLayer >= layer);
		if (mRow != b.mRow) return false;
		return Eq(GetTpos(), b.GetTpos(), layer);
	}

	//rowは当然として、cposの値も階層も全てが一致した場合にtrueを返却する。==演算子と使い分けるように。
	inline bool MatchPerfectly(const Bpos& b) const
	{
		if (mLayer != b.mLayer) return false;
		if (mRow != b.mRow) return false;
		if (mLayer > msMaxSmallLayer)
		{
			for (int i = 0; i < mLayer; ++i)
				if (mTposPtr[i] != b.mTposPtr[i]) return false;
		}
		else
		{
			for (int i = 0; i < mLayer; ++i)
				if (mTposArr[i] != b.mTposArr[i]) return false;
		}
		return true;
	}

	inline bool operator<(const Bpos& b) const
	{
		if (mRow != b.mRow) return mRow < b.GetRow();
		return Less(GetTpos(), mLayer, b.GetTpos(), b.mLayer);
	}
	inline bool operator<=(const Bpos& b) const
	{
		if (mRow != b.mRow) return mRow < b.GetRow();
		return LessEq(GetTpos(), mLayer, b.GetTpos(), b.mLayer);
	}

	//階層をlayerに変更し、全階層の位置を0で初期化する。
	inline void Init(LayerType layer)
	{
		Init(layer, 0);
	}
	//階層をlayerに変更し、全階層の位置をindexで初期化する。
	inline void Init(LayerType layer, BindexType index)
	{		
		mRow = index;
		mLayer = layer;
		BindexType* tpos = nullptr;
		if (layer != mLayer)
		{
			if (mLayer > msMaxSmallLayer) delete[] mTposPtr;
			if (layer > msMaxSmallLayer)
			{
				mTposPtr = new BindexType[layer];
				tpos = mTposPtr;
			}
			else
			{
				tpos = mTposArr.data();
			}
		}
		else
		{
			tpos = mLayer > msMaxSmallLayer ? mTposPtr : mTposArr.data();
		}
		Init(tpos, layer, index);
		mLayer = layer;
	}

	//lineはそのまま与えられたb.lineで初期化、cposの階層数は変化せず、
	//引数に与えられたlayerまでをb.cposで初期化、b.cposより深い階層値は0で初期化される。
	inline void Assign(const Bpos& b, int layer)
	{
		mRow = b.mRow;
		assert(mLayer >= layer && b.mLayer >= layer);
		BindexType* tpos = mLayer > msMaxSmallLayer ? mTposPtr : mTposArr.data();
		Init(tpos, b.GetTpos(), mLayer, layer);
	}
	//lineはそのまま与えられたb.lineで初期化、cposの階層数は変化せず、
	//自身か与えられたbのより浅い（階層数の小さい）方の階層数までをb.cposで初期化、b.cposより深い階層値は0で初期化される。
	inline void Assign(const Bpos& b)
	{
		mRow = b.mRow;
		int min = std::min(mLayer, b.mLayer);
		Assign(b, min);
	}
private:
	template <int N, class ...Args>
	inline void AssignPtr_impl(BindexType a, Args ...args)
	{
		mTposPtr[N] = a;
		AssignPtr_impl<N + 1>(args...);
	}
	template <int N>
	inline void AssignPtr_impl() {}
	template <int N, class ...Args>
	inline void AssignArr_impl(BindexType a, Args ...args)
	{
		mTposArr[N] = a;
		AssignArr_impl<N + 1>(args...);
	}
	template <int N>
	inline void AssignArr_impl() {}
public:
	template <std::integral ...Args>
	inline void Assign(BindexType row, Args ...args)
	{
		assert(mLayer == sizeof...(Args));
		mRow = row;
		if (mLayer > msMaxSmallLayer) AssignPtr_impl<0>(args...);
		else AssignArr_impl<0>(args...);
	}

	//全ての層の位置をposにする。
	inline void AssignAll(BindexType pos)
	{
		for (LayerType l = 0; l <= GetLayer(); ++l) (*this)[l] = pos;
	}

protected:

	template <class Dst>
	static void Init(Dst& dst, int mylayer, BindexType pos)
	{
		for (int i = 0; i < mylayer; ++i) dst[i] = pos;
	}
	template <class Dst, class Src>
	static void Init(Dst& dst, const Src& src, int mylayer, int min)
	{
		for (int i = 0; i < min; ++i) dst[i] = src[i];
		for (int i = min; i < mylayer; ++i) dst[i] = 0;
	};
	template <class A, class B>
	static bool Eq(const A& a, const B& b, int layer)
	{
		for (int i = 0; i < layer; ++i)
			if (a[i] != b[i]) return false;
		return true;
	}
	template <class A, class B>
	static bool Less(const A& a, int al, const B& b, int bl)
	{
		int size = std::min(al, bl);
		for (int i = 0; i < size; ++i)
		{
			if (a[i] == b[i]) continue;
			return a[i] < b[i];
		}
		return al < bl;
	}
	template <class A, class B>
	static bool LessEq(const A& a, int al, const B& b, int bl)
	{
		int size = std::min(al, bl);
		for (int i = 0; i < size; ++i)
		{
			if (a[i] == b[i]) continue;
			return a[i] < b[i];
		}
		return al <= bl;
	}

	LayerType mLayer;
	BindexType mRow;
	union
	{
		BindexType* mTposPtr;
		std::array<BindexType, 2> mTposArr;
	};
};

class JBpos
{
	template <RankType Rank>
	void Construct()
	{
		assert(Rank == m_max_rank + 1);
	}
	template <RankType Rank, class BPOS, class ...BPOSES>
	void Construct(BPOS&& b, BPOSES&& ...bs)
	{
		m_bpos[Rank] = std::forward<BPOS>(b);
		Construct<Rank + 1>(std::forward<BPOSES>(bs)...);
	}

public:

	JBpos() : m_max_rank(-1), m_bpos(nullptr) {}
	explicit JBpos(RankType rank)
		: m_max_rank(rank), m_bpos(new Bpos[rank + 1])
	{}
	template <class ...BPOSES>
	requires (std::is_convertible_v<BPOSES, Bpos> && ...)
	explicit JBpos(BPOSES&& ...bs)
		: m_max_rank((RankType)sizeof...(BPOSES) - 1), m_bpos(new Bpos[sizeof...(BPOSES)])
	{
		Construct<0>(std::forward<BPOSES>(bs)...);
	}
	~JBpos()
	{
		if (m_bpos) delete[] m_bpos;
	}

	JBpos(const JBpos& that)
		: m_max_rank(that.m_max_rank), m_bpos(new Bpos[that.m_max_rank + (RankType)1])
	{
		for (RankType r = 0; r <= m_max_rank; ++r) m_bpos[r] = that.m_bpos[r];
	}
	JBpos(JBpos&& that) noexcept
		: m_max_rank(that.m_max_rank), m_bpos(that.m_bpos)
	{
		that.m_max_rank = -1;
		that.m_bpos = nullptr;
	}
	JBpos& operator=(const JBpos& that)
	{
		if (m_max_rank != that.m_max_rank)
		{
			if (m_bpos) delete[] m_bpos;
			m_max_rank = that.m_max_rank;
			m_bpos = new Bpos[m_max_rank + 1];
		}
		for (RankType r = 0; r <= m_max_rank; ++r) m_bpos[r] = that.m_bpos[r];
		return *this;
	}
	JBpos& operator=(JBpos&& that) noexcept
	{
		delete[] m_bpos;
		m_max_rank = that.m_max_rank;
		m_bpos = that.m_bpos;
		that.m_max_rank = -1;
		that.m_bpos = nullptr;
		return *this;
	}

	void Init(RankType rank)
	{
		if (m_bpos) delete[] m_bpos;
		if (rank >= 0) m_bpos = new Bpos[rank + 1];
		m_max_rank = rank;
	}

	/*bool MatchPartially(const JBpos& b) const
	{
		RankType min_1 = std::min(GetMaxRank(), b.GetMaxRank()) - 1;
		for (RankType i = 0; i < min_1; ++i)
		{
			if (!m_bpos[i].MatchPerfectly(b.m_bpos[i])) return false;
		}
		return m_bpos[min_1].MatchPartially(b.m_bpos[min_1]);
	}*/
	//maxrank-1位まではPerfectMatch、
	//maxrank位についてはPartialMatch(bpos, maxlayer)
	//を判定する。
	//maxlayerにはmaxrank位のInternalLayer（連結前階層）を与える。
	bool MatchPartially(const JBpos& b, RankType maxrank, LayerType maxlayer) const
	{
		assert(maxrank <= m_max_rank && maxrank <= b.m_max_rank);
		for (RankType i = 0; i < maxrank; ++i)
		{
			if (!m_bpos[i].MatchPerfectly(b.m_bpos[i])) return false;
		}
		return m_bpos[maxrank].MatchPartially(b.m_bpos[maxrank], maxlayer);
	}
	//maxrank - 1位までは完全一致、
	//maxrank位はmaxlayerまでの部分一致。
	//maxlayerにはInternalLayerを与える。
	template <size_t R>
	bool MatchPartially(const Bpos& bpos, const std::array<std::pair<LayerType, LayerType>, R>& j, RankType maxrank, LayerType maxlayer)
	{
		assert(m_max_rank == (RankType)(R - 1));
		LayerType eupp = -1_layer;
		for (RankType r = 0_rank; r < maxrank; ++r)
		{
			const Bpos& b = m_bpos[r];
			auto [ujoint, ljoint] = j[r];
			for (LayerType l = ujoint + 1_layer; l <= ljoint; ++l)
			{
				if (b[l] != bpos[eupp + l - ujoint]) return false;
			}
			eupp = LayerType(eupp + ljoint - ujoint);
		}
		{
			const Bpos& b = m_bpos[maxrank];
			auto [ujoint, ljoint] = j[maxrank];
			for (LayerType l = ujoint + 1_layer; l <= maxlayer; ++l)
			{
				if (b[l] != bpos[eupp + l - ujoint]) return false;
			}
		}
		return true;
	}
	bool MatchPerfectly(const JBpos& b) const
	{
		RankType size = GetMaxRank();
		if (size != b.GetMaxRank()) return false;
		for (RankType i = 0; i < size; ++i)
		{
			if (!m_bpos[i].MatchPerfectly(b.m_bpos[i])) return false;
		}
		return true;
	}

	RankType GetMaxRank() const
	{
		return m_max_rank;
	}

	Bpos& operator[](RankType rank) { return m_bpos[rank]; }
	const Bpos& operator[](RankType rank) const { return m_bpos[rank]; }

private:

	RankType m_max_rank;
	Bpos* m_bpos;
};


inline std::ostream& operator<<(std::ostream& out, const Bpos& bpos)
{
	LayerType layer = bpos.GetLayer();
	if (layer == -1_layer) return out << "[]";
	else
	{
		out << "[" << bpos[0_layer];
		for (LayerType l = 1_layer; l <= layer; ++l) out << "," << bpos[l];
		return out << "]";
	}
}

inline std::ostream& operator<<(std::ostream& out, const JBpos& jbpos)
{
	RankType rank = jbpos.GetMaxRank();
	if (rank == -1_rank) return out << "[]";
	else
	{
		out << "[" << jbpos[0_rank];
		for (RankType r = 1_layer; r <= rank; ++r) out << "," << jbpos[r];
		return out << "]";
	}
}

}

#endif
