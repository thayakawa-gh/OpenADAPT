#ifndef OPENADAPT_CONTAINER_HIST_H
#define OPENADAPT_CONTAINER_HIST_H

#include <OpenADAPT/Container/Tree.h>
#include <OpenADAPT/Common/Macros.h>

namespace adapt
{

struct AxisAttr
{
	AxisAttr() = default;

	// "name" is only required for DHist. 
	AxisAttr(const std::string& name_, double w, double c = 0.) : name(name_), wbin(w), cbin(c) {}
	AxisAttr(std::string&& name_, double w, double c = 0.) : name(std::move(name_)), wbin(w), cbin(c) {}
	AxisAttr(double w, double c = 0.) : wbin(w), cbin(c) {}

	AxisAttr(const AxisAttr&) = default;
	AxisAttr(AxisAttr&&) = default;
	AxisAttr& operator=(const AxisAttr&) = default;
	AxisAttr& operator=(AxisAttr&&) = default;

	std::string name;
	double wbin = 0.;
	double cbin = 0.;
};

namespace detail
{

template <class DimType, class Container_, class Hierarchy_, class ElementBlockPolicy_>
class Hist_base : public Tree_base<Container_, Hierarchy_, ElementBlockPolicy_>
{
public:
	using Base = Tree_base<Container_, Hierarchy_, ElementBlockPolicy_>;
	using TopElementRef = typename Base::TopElementRef;
	using TopConstElementRef = typename Base::TopConstElementRef;
	using ElementRef_0 = typename Base::ElementRef_0;
	using ConstElementRef_0 = typename Base::ConstElementRef_0;
	using ElementRef_1 = typename Base::template ElementRefF<1>;
	using ConstElementRef_1 = typename Base::template ConstElementRefF<1>;

	static constexpr bool RunTimeDim = std::same_as<size_t, DimType>;

#define HIST_ASSERT(EQ)\
	if constexpr (RunTimeDim) assert(EQ == m_dim);\
	else static_assert(EQ == DimType{});

	//ヒストグラムのように要素をビンごとに区切って格納するコンテナ。
	//0層要素は固定である1個のビンに相当し、常にそのビンのインデックス（ibin[0], ibin[1], ..., ibin[dim]）をフィールドとして持つ。
	//1層要素は指定可能であるが、1層要素の最初のdim個がdim次元ヒストグラムの軸の値となる。
	//そのため、最初のdim個はdouble型であることが求められる。

	//各軸の値が{ ax[0], ax[1], ..., ax[dim] }のように与えられたとき、
	//ビンのインデックス{ ibin[0], ibin[1], ..., ibin[dim] }は以下の式に従って求められる。
	//ここで、cbinはヒストグラムの中心値、wbinはビンの幅であり、どちらもコンストラクタでユーザーが任意に指定するもの。
	// ibin[a] = static_cast<BinBaseType>(std::floor((ax[a] - cbin[a]) / wbin[a]));


private:
	template <class Attr>
	void SetAttr_impl(const std::vector<Attr>& attr)
	{
		TopElementRef top = Base::GetTopElement();
		const auto& top_phs = Base::GetPlaceholdersIn(-1);
		for (size_t i = 0; i < m_dim; ++i)
		{
			if (attr[i].wbin <= 0.)
				throw InvalidArg(std::format("The width of the bin for axis {} must be greater than 0.", i));
			auto& min_ph = top_phs[i];
			auto& max_ph = top_phs[i + m_dim];
			auto& wbin_ph = top_phs[i + m_dim * 2];
			auto& cbin_ph = top_phs[i + m_dim * 3];
			top.template ConstructFieldFrom<FieldType::I32>(min_ph, std::numeric_limits<BinBaseType>::max());
			top.template ConstructFieldFrom<FieldType::I32>(max_ph, std::numeric_limits<BinBaseType>::min());
			top.template ConstructFieldFrom<FieldType::F64>(wbin_ph, attr[i].wbin);
			top.template ConstructFieldFrom<FieldType::F64>(cbin_ph, attr[i].cbin);
		}
	}
public:
	void SetAxesAttr(const std::vector<AxisAttr>& attr)
		requires (!RunTimeDim)
	{
		if (attr.size() != m_dim)
			throw InvalidArg("The number of axis attributes does not match the dimension of the histogram.");
		SetAttr_impl(attr);
	}
	void SetAxesAttr(const std::vector<AxisAttr>& attr, const std::vector<std::pair<std::string, FieldType>>& extr = {})
		requires RunTimeDim
	{
		m_dim = attr.size();
		std::vector<std::pair<std::string, FieldType>> phs(m_dim * 4);
		{
			// top layer
			for (size_t i = 0; i < m_dim; ++i)
			{
				phs[i] = { std::format("min{}", i), FieldType::I32 };
				phs[i + m_dim] = { std::format("max{}", i), FieldType::I32 };
				phs[i + m_dim * 2] = { std::format("wbin{}", i), FieldType::F64 };
				phs[i + m_dim * 3] = { std::format("cbin{}", i), FieldType::F64 };
			}
			Base::SetTopLayer(phs);
		}
		{
			// 0層
			phs.resize(m_dim);
			for (size_t i = 0; i < m_dim; ++i)
				phs[i] = { std::format("ibin{}", i), FieldType::I32 };
			Base::SetLayer(0_layer, phs);
		}
		{
			// 1層
			for (size_t i = 0; i < m_dim; ++i)
				phs[i] = { attr[i].name, FieldType::F64 };
			phs.insert(phs.end(), extr.begin(), extr.end());
			Base::SetLayer(1_layer, phs);
		}
		Base::VerifyStructure();
		SetAttr_impl(attr);
	}

	using Base::operator[];
	template <size_t Dim>
	ElementRef_0 operator[](const Bin<Dim>& bin)
	{
		HIST_ASSERT(Dim)
		BindexType index = BinToIndex<Dim>{}(bin, GetMinList(), GetMaxList());
		return Base::operator[](index);
	}
	template <size_t Dim>
	ConstElementRef_0 operator[](const Bin<Dim>& bin) const
	{
		HIST_ASSERT(Dim)
		BindexType index = BinToIndex<Dim>{}(bin, GetMinList(), GetMaxList());
		return Base::operator[](index);
	}

	template <size_t Dim>
	void Resize(const Bin<Dim>& min, const Bin<Dim>& max)
	{
		HIST_ASSERT(Dim)
		Base::GetLowerElements().Resize(GetMinList(), GetMaxList(), min, max);
		UpdateMinMax(min, max);
	}
	template <size_t Dim>
	void Rebin(const std::array<double, Dim>&)
	{
		HIST_ASSERT(Dim)

	}

	std::array<BinBaseType, 2> GetAxisRange(size_t axis) const
	{
		assert(axis < m_dim);
		const BinBaseType* min_list = GetMinList();
		const BinBaseType* max_list = GetMaxList();
		return { min_list[axis], max_list[axis] };
	}
	template <size_t Dim>
	void GetMin(Bin<Dim>& min) const
	{
		HIST_ASSERT(Dim)
		const BinBaseType* min_list = GetMinList();
		for (size_t i = 0; i < Dim; ++i) min[i] = min_list[i];
	}
	auto GetMin() const requires (!RunTimeDim)
	{
		Bin<DimType::value> min;
		GetMin(min);
		return min;
	}
	template <size_t Dim>
	void GetMax(Bin<Dim>& max) const
	{
		HIST_ASSERT(Dim)
		const BinBaseType* max_list = GetMaxList();
		for (size_t i = 0; i < Dim; ++i) max[i] = max_list[i];
	}
	auto GetMax() const requires (!RunTimeDim)
	{
		Bin<DimType::value> max;
		GetMax(max);
		return max;
	}
	template <size_t Dim>
	bool IsInside(const Bin<Dim>& bin) const
	{
		const BinBaseType* min_list = GetMinList();
		const BinBaseType* max_list = GetMaxList();
		for (size_t i = 0; i < Dim; ++i)
		{
			if (bin[i] < min_list[i] || bin[i] > max_list[i]) return false;
		}
		return true;
	}

	// 最初のm_dim個の要素はヒストグラムの軸の値に用いられる。
	template <class ...Axes, class ...Vars>
		requires (std::same_as<std::remove_cvref_t<Axes>, double> && ...)
	ElementRef_0 Append(std::tuple<Axes...> axes, std::tuple<Vars...> vars)
	{
		constexpr size_t Dim = sizeof...(Axes);
		HIST_ASSERT(Dim);
		const double* cbin_list = GetCBinList();
		const double* wbin_list = GetWBinList();
		Bin<Dim> bin;
		GetBin<0>(bin, axes, cbin_list, wbin_list);
		ResizeIfNeeded(bin);
		auto ref = (*this)[bin];
		std::apply([&ref](auto&&... args) { ref.Push(std::forward<decltype(args)>(args)...); }, std::tuple_cat(std::move(axes), std::move(vars)));
		return ref;
	}
	template <class ...Axes>
		requires (std::same_as<std::remove_cvref_t<Axes>, double> && ...)
	ElementRef_0 Append(Axes&& ...axes)
	{
		return Append(std::forward_as_tuple(std::forward<Axes>(axes)...), std::make_tuple());
	}

private:
	template <size_t N, class ...Axes>
		requires (std::same_as<std::remove_cvref_t<Axes>, double> && ...)
	void SetAxesFields(double* ptr, double a, Axes&& ...axes)
	{
		*ptr = a;
		++ptr;
		if constexpr (sizeof...(Axes) > 0) SetAxesFields<N + 1>(ptr, std::forward<Axes>(axes)...);
	}
public:
	// 軸の値以外がデフォルト値で初期化された要素を追加する。
	// Append(axes, vars)のうちvarsを省略し、その部分を代わりにデフォルト値で初期化するもの。
	template <class ...Axes>
		requires (std::same_as<std::remove_cvref_t<Axes>, double> && ...)
	ElementRef_0 AppendDefaultElement(Axes&& ...axes)
	{
		constexpr size_t Dim = sizeof...(Axes);
		HIST_ASSERT(Dim);
		const double* cbin_list = GetCBinList();
		const double* wbin_list = GetWBinList();
		Bin<Dim> bin;
		GetBin<0>(bin, std::make_tuple(axes...), cbin_list, wbin_list);
		ResizeIfNeeded(bin);
		auto ref = (*this)[bin];
		ref.PushDefaultElement();
		auto back = ref.Back();
		double* ptr = std::launder(reinterpret_cast<double*>(back.GetBlock()));
		SetAxesFields<0>(ptr, std::forward<Axes>(axes)...);
		return ref;
	}


private:

	template <size_t Dim, class ...Axes>
	void GetBin(Bin<sizeof...(Axes)>& bin, const std::tuple<Axes...>& axes, const double* cbin_list, const double* wbin_list)
	{
		if constexpr (Dim < sizeof...(Axes))
		{
			double ax = std::get<Dim>(axes);
			double wbin = wbin_list[Dim];
			double cbin = cbin_list[Dim];
			bin[Dim] = static_cast<BinBaseType>(std::floor((ax - cbin) / wbin));
			GetBin<Dim + 1>(bin, axes, cbin_list, wbin_list);
		}
	}
	template <size_t Dim>
	void ResizeIfNeeded(const Bin<Dim>& bin)
	{
		const BinBaseType* min_list = GetMinList();
		const BinBaseType* max_list = GetMaxList();
		bool flag = false;
		Bin<Dim> newmin{}, newmax{};
		for (size_t i = 0; i < Dim; ++i)
		{
			newmin[i] = bin[i] < min_list[i] ? (flag = true, bin[i]) : min_list[i];
			newmax[i] = bin[i] > max_list[i] ? (flag = true, bin[i]) : max_list[i];
		}
		if (flag)
			Resize(newmin, newmax);
	}
	template <size_t Dim>
	void UpdateMinMax(const Bin<Dim>& min, const Bin<Dim>& max)
	{
		BinBaseType* min_list = GetMinList();
		BinBaseType* max_list = GetMaxList();
		for (size_t i = 0; i < m_dim; ++i)
		{
			min_list[i] = min[i];
			max_list[i] = max[i];
		}
	}

	const BinBaseType* GetMinList() const
	{
		auto top_element = Base::GetTopElement();
		constexpr ptrdiff_t offset = 0;
		assert(Base::GetPlaceholder("min0").GetPtrOffset() == offset);
		return std::launder(reinterpret_cast<const BinBaseType*>(top_element.GetBlock() + offset));
	}
	BinBaseType* GetMinList()
	{
		auto top_element = Base::GetTopElement();
		constexpr ptrdiff_t offset = 0;
		assert(Base::GetPlaceholder("min0").GetPtrOffset() == offset);
		return std::launder(reinterpret_cast<BinBaseType*>(top_element.GetBlock() + offset));
	}
	const BinBaseType* GetMaxList() const
	{
		auto top_element = Base::GetTopElement();
		ptrdiff_t offset = sizeof(BinBaseType) * m_dim;
		assert(Base::GetPlaceholder("max0").GetPtrOffset() == offset);
		return std::launder(reinterpret_cast<const BinBaseType*>(top_element.GetBlock() + offset));
	}
	BinBaseType* GetMaxList()
	{
		auto top_element = Base::GetTopElement();
		ptrdiff_t offset = sizeof(BinBaseType) * m_dim;
		assert(Base::GetPlaceholder("max0").GetPtrOffset() == offset);
		return std::launder(reinterpret_cast<BinBaseType*>(top_element.GetBlock() + offset));
	}
	const double* GetWBinList() const
	{
		auto top_element = Base::GetTopElement();
		ptrdiff_t offset = sizeof(BinBaseType) * m_dim + sizeof(BinBaseType) * m_dim;
		assert(Base::GetPlaceholder("wbin0").GetPtrOffset() == offset);
		return std::launder(reinterpret_cast<const double*>(top_element.GetBlock() + offset));
	}
	const double* GetCBinList() const
	{
		auto top_element = Base::GetTopElement();
		ptrdiff_t offset = sizeof(BinBaseType) * m_dim + sizeof(BinBaseType) * m_dim + sizeof(double) * m_dim;
		assert(Base::GetPlaceholder("cbin0").GetPtrOffset() == offset);
		return std::launder(reinterpret_cast<const double*>(top_element.GetBlock() + offset));
	}

	DimType m_dim = {};// ヒストグラムの次元。SHistならSizeConstantだが、DHistならsize_t型で実行時に決まる。

#undef HIST_ASSERT
};

template <size_t Dim>
struct BinInfoSelector;
template <>
struct BinInfoSelector<1>
{
	using HistTopLayer = ADAPT_S_DEFINE_LAYER(min0, BinBaseType, max0, BinBaseType, wbin0, double, cbin0, double);
	using HistLayer0 = ADAPT_S_DEFINE_LAYER(ibin0, BinBaseType);
};
template <>
struct BinInfoSelector<2>
{
	using HistTopLayer = ADAPT_S_DEFINE_LAYER(min0, BinBaseType, min1, BinBaseType,
											  max0, BinBaseType, max1, BinBaseType,
											  wbin0, double, wbin1, double,
											  cbin0, double, cbin1, double);
	using HistLayer0 = ADAPT_S_DEFINE_LAYER(ibin0, BinBaseType, ibin1, BinBaseType);
};
template <>
struct BinInfoSelector<3>
{
	using HistTopLayer = ADAPT_S_DEFINE_LAYER(min0, BinBaseType, min1, BinBaseType, min2, BinBaseType,
											  max0, BinBaseType, max1, BinBaseType, max2, BinBaseType,
											  wbin0, double, wbin1, double, wbin2, double,
											  cbin0, double, cbin1, double, cbin2, double);
	using HistLayer0 = ADAPT_S_DEFINE_LAYER(ibin0, BinBaseType, ibin1, BinBaseType, ibin2, BinBaseType);
};
template <>
struct BinInfoSelector<4>
{
	using HistTopLayer = ADAPT_S_DEFINE_LAYER(min0, BinBaseType, min1, BinBaseType, min2, BinBaseType, min3, BinBaseType,
											  max0, BinBaseType, max1, BinBaseType, max2, BinBaseType, max3, BinBaseType,
											  wbin0, double, wbin1, double, wbin2, double, wbin3, double,
											  cbin0, double, cbin1, double, cbin2, double, cbin3, double);
	using HistLayer0 = ADAPT_S_DEFINE_LAYER(ibin0, BinBaseType, ibin1, BinBaseType, ibin2, BinBaseType, ibin3, BinBaseType);
};

template <size_t Dim>
using HistTopLayer = typename BinInfoSelector<Dim>::HistTopLayer;
template <size_t Dim>
using HistLayer0 = typename BinInfoSelector<Dim>::HistLayer0;

}

class DHist : public detail::Hist_base<size_t, DHist, FHierarchy<DHist, 1>, detail::DElementBlockPolicy>
{
public:
	using Base = detail::Hist_base<size_t, DHist, FHierarchy<DHist, 1>, detail::DElementBlockPolicy>;
	using Base::Base;
};
template <size_t Dim, named_tuple Layer1>
class SHist : public detail::Hist_base<SizeConstant<Dim>, SHist<Dim, Layer1>,
									   SHierarchy<SHist<Dim, Layer1>, detail::HistTopLayer<Dim>, detail::HistLayer0<Dim>, Layer1>,
									   detail::SElementBlockPolicy>
{
public:
	using Base = detail::Hist_base<SizeConstant<Dim>, SHist<Dim, Layer1>,
								   SHierarchy<SHist<Dim, Layer1>, detail::HistTopLayer<Dim>, detail::HistLayer0<Dim>, Layer1>,
								   detail::SElementBlockPolicy>;
	using Base::Base;
};

template <named_tuple Layer1>
using SHist1D = SHist<1, Layer1>;
template <named_tuple Layer1>
using SHist2D = SHist<2, Layer1>;
template <named_tuple Layer1>
using SHist3D = SHist<3, Layer1>;
template <named_tuple Layer1>
using SHist4D = SHist<4, Layer1>;

}


#endif