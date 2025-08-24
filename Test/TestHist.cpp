#include <Test/Aggregator.h>
#include <ranges>
#include <iterator>

using namespace adapt;
using namespace adapt::lit;

template <any_hist Hist>
void TestHist(Hist& hist)
{
	constexpr bool IsDHist = std::same_as<Hist, DHist>;
	constexpr size_t dim = 2;
	double min0 = 0.0, max0 = 100.0;
	double min1 = -9.0, max1 = 11.0;
	double wbin0 = 10.;
	double wbin1 = 2.0, cbin1 = 1.0;

	if constexpr (IsDHist)
	{
		std::vector<AxisAttr> attr(dim);
		attr[0] = { "x", wbin0 };
		attr[1] = { "y", wbin1, cbin1 };
		std::vector<std::pair<std::string, FieldType>> extr(1);
		extr[0] = { "xy", DFieldInfo::ValueTypeToTagType<double>() };
		hist.SetAxesAttr(attr, extr);
	}
	else
	{
		std::vector<AxisAttr> attr(dim);
		attr[0] = { wbin0 };
		attr[1] = { wbin1, cbin1 };
		hist.SetAxesAttr(attr);
	}

	std::mt19937_64 mt(0);
	std::uniform_real_distribution<double> dist0(min0, max0);
	std::uniform_real_distribution<double> dist1(min1, max1);

	// 10000個のランダムな値を生成してヒストグラムに追加する。
	for (size_t i = 0; i < 10000; ++i)
	{
		double x = dist0(mt);
		double y = dist1(mt);
		hist.Append(std::forward_as_tuple(x, y), std::forward_as_tuple(x * y));
	}
	// 超低確率でエラーになりうるけどまあさすがに無視でよかろう。
	std::array<BinBaseType, dim> tmp{ 0, 9 };
	EXPECT_EQ(hist.GetAxisRange(0), tmp);
	tmp = { -5, 4 };
	EXPECT_EQ(hist.GetAxisRange(1), tmp);
	EXPECT_EQ(hist.GetSize(0_layer), 100);
	EXPECT_EQ(hist.GetSize(1_layer), 10000);

	ADAPT_GET_PLACEHOLDERS(hist, ibin0, ibin1, x, y, xy);
	for (const auto& trav : hist.GetRange(1_layer))
	{
		auto [ix, iy, x_val, y_val, xy_val] = [&]()
		{
			if constexpr (IsDHist)
				return std::tuple{ trav[ibin0].i32(), trav[ibin1].i32(), trav[x].f64(), trav[y].f64(), trav[xy].f64() };
			else
				return std::tuple{ trav[ibin0], trav[ibin1], trav[x], trav[y], trav[xy] };
		}();
		EXPECT_EQ(ix, (BinBaseType)std::floor(x_val / wbin0));
		EXPECT_EQ(iy, (BinBaseType)std::floor((y_val - cbin1) / wbin1));
		EXPECT_EQ(xy_val, x_val * y_val);
	}
}

TEST_F(Aggregator, DHist)
{
	DHist hist;
	TestHist(hist);
}
TEST_F(Aggregator, SHist)
{
	using HistLayer1 = ADAPT_S_DEFINE_LAYER(x, double, y, double, xy, double);
	SHist2D<HistLayer1> hist;
	TestHist(hist);
}

template <any_tree Tree>
void TestHistFromTree(const Tree& tree, const std::vector<Class>& cls)
{
	//SetNumOfThreads(1);
	//SetGranularity(1);
	auto hist = [&tree]()
	{
		ADAPT_GET_PLACEHOLDERS(tree, english, math, name, exam);
		auto fmath = cast_f64(math);
		auto feng = cast_f64(english);
		auto h = tree | ADAPT_HIST(fmath, 5., 0., feng, 5., 0., name, exam);
		EXPECT_EQ(h.GetSize(0_layer), 441);
		EXPECT_EQ(h.GetSize(1_layer), 480);
		return h;
	} ();

	std::map<std::pair<std::string, int32_t>, std::pair<int32_t, int32_t>> name_map;
	{
		for (const auto& c : cls)
		{
			for (const auto& s : c.m_students)
			{
				for (const auto& r : s.m_records)
				{
					name_map[{s.m_name, r.m_exam}] = { r.m_math, r.m_english };
				}
			}
		}
	}
	{
		ADAPT_GET_PLACEHOLDERS(hist, ibin0, ibin1, fmath, feng, name, exam);
		for (const auto& trav : hist.GetRange(1_layer))
		{
			auto [ix, iy, x_val, y_val, name_val, exam_val] = [&]()
			{
				if constexpr (d_hist<decltype(hist)>)
					return std::tuple{ trav[ibin0].i32(), trav[ibin1].i32(), trav[fmath].f64(), trav[feng].f64(), trav[name].str(), trav[exam].i08() };
				else
					return std::tuple{ trav[ibin0], trav[ibin1], trav[fmath], trav[feng], trav[name], trav[exam] };
			}();
			EXPECT_EQ(ix, (BinBaseType)std::floor(x_val / 5.));
			EXPECT_EQ(iy, (BinBaseType)std::floor(y_val / 5.));
			auto it = name_map.find({ name_val, exam_val });
			EXPECT_EQ(it != name_map.end(), true) << "Name: " << name_val << ", Exam: " << (int)exam_val;
			if (it != name_map.end())
			{
				EXPECT_EQ(it->second.first, (int32_t)x_val) << "Name: " << name_val << ", Exam: " << (int)exam_val;
				EXPECT_EQ(it->second.second, (int32_t)y_val) << "Name: " << name_val << ", Exam: " << (int)exam_val;
			}
		}
	}
}
TEST_F(Aggregator, DHistFromDTree)
{
	TestHistFromTree(m_dtree, m_class);
}
TEST_F(Aggregator, SHistFromSTree)
{
	TestHistFromTree(m_stree, m_class);
}