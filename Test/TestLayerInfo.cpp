#include <Test//Aggregator.h>
#include <ranges>
#include <iterator>

using namespace adapt;
using namespace lit;

constexpr LayerInfo<3> GetLayerInfo()
{
	/*
	  -1        0         1         2         3         4         5         6         7         8
	Global -- Layer0 -- Layer1 -- Layer2
	         (Global)--(Layer0)-- Layer1
	                              Global -- Layer0 -- Layer1 -- Layer2 -- Layer3
	                                                           (Global)-- Layer0 -- Layer1
	*/
	constexpr JointLayerArray<3> joint(-1_layer, 2_layer,
									   1_layer, 1_layer,
									   -1_layer, 3_layer,
									   0_layer, -1_layer);
	constexpr LayerInfo<3> e(joint);
	return e;
}

TEST_F(Aggregator, LayerInfo)
{
	{
		constexpr auto e = GetLayerInfo();
		EXPECT_EQ(e.GetTravLayer(), -1_layer);//全順位無効な場合の走査層は-1扱い。
	}
	{
		constexpr auto f = []()
		{
			auto e = GetLayerInfo();
			e.Enable<2>();
			e.SetTravLayer<2>(2);
			return e.GetTravLayer();
		};
		EXPECT_EQ(f(), 5_layer);
	}
	{
		//Raiseによる階層値上昇の確認。
		constexpr auto f = []()
		{
			auto e = GetLayerInfo();
			e.Enable<3>();
			e.SetTravLayer<3>(1);//これで7層。
			auto res1 = e;
			e.Raise(5);//これで2層。
			auto res2 = e;
			e.Raise(1);//これで1層。Rank0以外は無効になる。
			auto res3 = e;
			return std::make_tuple(res1, res2, res3);
		};
		constexpr auto tup = f();
		constexpr auto res1 = std::get<0>(tup);
		constexpr auto res2 = std::get<1>(tup);
		constexpr auto res3 = std::get<2>(tup);

		constexpr auto t1 = res1.GetTravLayer();
		EXPECT_EQ(t1, 7);
		for (RankType r = 0; r < 4; ++r) EXPECT_TRUE(res1.GetInternal(r).IsEnabled());

		//res2の場合は最下位のみ無効化されている。
		constexpr auto t2 = res2.GetTravLayer();
		EXPECT_EQ(t2, 2);
		for (RankType r = 0; r < 3; ++r) EXPECT_TRUE(res2.GetInternal(r).IsEnabled());
		EXPECT_TRUE(res2.GetInternal(3).IsDisabled());

		constexpr auto t3 = res3.GetTravLayer();
		EXPECT_EQ(t3, 1);
		EXPECT_TRUE(res3.GetInternal(0).IsEnabled());
		for (RankType r = 1; r < 4; ++r) EXPECT_TRUE(res3.GetInternal(r).IsDisabled());
	}
	{
		//Pseudo topのテスト。
		constexpr auto f = []()
		{
			auto e = GetLayerInfo();
			e.SetPseudoTop<1>();
			auto res1 = e;
			e.SetTravLayer<1>(1);
			auto res2 = e;
			e.Fix(1);
			auto res3 = e;
			e.Normalize();
			auto res4 = e;
			return std::make_tuple(res1, res2, res3, res4);
		};
		constexpr auto tup = f();

		constexpr auto res1 = std::get<0>(tup);
		EXPECT_TRUE(res1.GetInternal(0).IsIgnored());
		EXPECT_TRUE(res1.GetInternal(1).IsPseudoTop());

		constexpr auto res2 = std::get<1>(tup);
		constexpr auto t2 = res2.GetTravLayer();
		EXPECT_EQ(t2, -2);

		constexpr auto res3 = std::get<2>(tup);
		EXPECT_EQ(res3.GetInternal(1).GetFixedLayer(), 0);

		//正規化すると2になる。
		constexpr auto res4 = std::get<3>(tup);
		constexpr auto t4 = res4.GetTravLayer();
		EXPECT_EQ(t4, 2);
	}
}