#include <Test/Aggregator.h>
#include <ranges>
#include <iterator>

using namespace adapt;
using namespace adapt::lit;

template <class Container, class Layer0, class Layer1, class Layer2>
void TestContainer(Container& tree, const std::vector<Class>& cls,
				 const Layer0& l0, const Layer1& l1, const Layer2& l2)
{
	tree.ShowHierarchy();
	//0層要素。学年とクラス。
	auto [grade, class_] = l0;
	//1層要素。出席番号、名前、生年月日。
	auto [number, name] = l1;
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, jpn, eng, sci, soc] = l2;

	EXPECT_EQ(tree.GetSize(0_layer), 4);
	EXPECT_EQ(tree.GetSize(1_layer), 120);
	EXPECT_EQ(tree.GetSize(2_layer), 480);

	size_t clssize = cls.size();
	for (BindexType i = 0; i < clssize; ++i)
	{
		const auto& tc = tree[i];
		const Class& c = cls[i];

		EXPECT_EQ(tc.GetSize(1_layer), 30);
		EXPECT_EQ(tc.GetSize(2_layer), 120);

		if constexpr (d_container<Container>)
		{
			EXPECT_EQ(tc[grade].i08(), c.m_grade);
			EXPECT_EQ(tc[class_].i08(), c.m_class);
		}
		else
		{
			EXPECT_EQ(tc[grade], c.m_grade);
			EXPECT_EQ(tc[class_], c.m_class);
		}

		size_t stusize = c.m_students.size();
		for (BindexType j = 0; j < stusize; ++j)
		{
			const auto& ts = tc[j];
			const Student& s = c.m_students[j];

			EXPECT_EQ(ts.GetSize(2_layer), 4);

			if constexpr (d_container<Container>)
			{
				EXPECT_EQ(ts[number].i16(), s.m_number);
				EXPECT_EQ(ts[name].str(), s.m_name);
			}
			else
			{
				EXPECT_EQ(ts[number], s.m_number);
				EXPECT_EQ(ts[name], s.m_name);
			}

			size_t recsize = s.m_records.size();
			for (BindexType k = 0; k < recsize; ++k)
			{
				const auto& tr = ts[k];
				const Record& r = s.m_records[k];

				if constexpr (d_container<Container>)
				{
					EXPECT_EQ(tr[exam].i08(), r.m_exam);
					EXPECT_EQ(tr[math].i32(), r.m_math);
					EXPECT_EQ(tr[jpn].i32(), r.m_japanese);
					EXPECT_EQ(tr[eng].i32(), r.m_english);
					EXPECT_EQ(tr[sci].i32(), r.m_science);
					EXPECT_EQ(tr[soc].i32(), r.m_social);
				}
				else
				{
					EXPECT_EQ(tr[exam], r.m_exam);
					EXPECT_EQ(tr[math], r.m_math);
					EXPECT_EQ(tr[jpn], r.m_japanese);
					EXPECT_EQ(tr[eng], r.m_english);
					EXPECT_EQ(tr[sci], r.m_science);
					EXPECT_EQ(tr[soc], r.m_social);
				}
			}
		}
	}
}

TEST_F(Aggregator, DTree_Container)
{
	//0層要素。学年とクラス。
	auto [grade, class_] = m_dtree.GetPlaceholders("grade", "class_");
	//1層要素。出席番号、名前、生年月日。
	auto [number, name] = m_dtree.GetPlaceholders("number", "name");
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, jpn, eng, sci, soc] = m_dtree.GetPlaceholders("exam", "math", "japanese", "english", "science", "social");

	TestContainer(m_dtree, m_class,
				 std::make_tuple(grade, class_),
				 std::make_tuple(number, name),
				 std::make_tuple(exam, math, jpn, eng, sci, soc));
}
TEST_F(Aggregator, STree_Container)
{
	//0層要素。学年とクラス。
	auto [grade, class_] = m_stree.GetPlaceholders<"grade", "class_">();
	//1層要素。出席番号、名前、生年月日。
	auto [number, name] = m_stree.GetPlaceholders<"number", "name">();
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, jpn, eng, sci, soc] = m_stree.GetPlaceholders<"exam", "math", "japanese", "english", "science", "social">();

	TestContainer(m_stree, m_class,
				std::make_tuple(grade, class_),
				std::make_tuple(number, name),
				std::make_tuple(exam, math, jpn, eng, sci, soc));
}

template <container_simplex Tree>
void TestTreeHandling(Tree& tree)
{
	auto [i00, i01, i10, i11] = tree.GetPlaceholders("i00", "i01", "i10", "i11");
	{
		tree.Resize(10);
		for (BindexType i = 0; i < 10; ++i)
			tree[i].Assign(i, i);
		EXPECT_EQ(tree.GetSize(), 10);
		EXPECT_EQ(tree.GetCapacity(), 10);
		for (BindexType i = 0; i < 10; ++i)
		{
			auto x = tree[i];
			EXPECT_EQ(x[i00].i32(), i);
			EXPECT_EQ(x[i01].i32(), i);
			x.Resize(5);
			for (BindexType j = 0; j < 5; ++j)
				x[j].Assign(i, j);
			for (BindexType j = 0; j < 5; ++j)
			{
				auto y = x[j];
				EXPECT_EQ(y[i10].i32(), i);
				EXPECT_EQ(y[i11].i32(), j);
			}
			EXPECT_EQ(x.GetSize(), 5);
			EXPECT_EQ(x.GetCapacity(), 5);
		}
		for (BindexType i = 0; i < 10; ++i)
			tree.Push(i + 10, i + 10);
		EXPECT_EQ(tree.GetSize(), 20);
		EXPECT_EQ(tree.GetCapacity(), 21);
		for (BindexType i = 0; i < 20; ++i)
		{
			auto x = tree[i];
			EXPECT_EQ(x[i00].i32(), i);
			EXPECT_EQ(x[i01].i32(), i);
			if (i < 10)
			{
				EXPECT_EQ(x.GetSize(), 5);
				for (BindexType j = 0; j < 5; ++j)
				{
					auto y = x[j];
					EXPECT_EQ(y[i10].i32(), i);
					EXPECT_EQ(y[i11].i32(), j);
				}
			}
			else
			{
				EXPECT_EQ(x.GetSize(), 0);
			}
		}
		tree.Erase(2, 3);
		EXPECT_EQ(tree.GetSize(), 17);
		EXPECT_EQ(tree.GetCapacity(), 21);
		for (BindexType i = 0; i < 17; ++i)
		{
			auto x = tree[i];
			if (i < 2)
			{
				EXPECT_EQ(x[i00].i32(), i);
				EXPECT_EQ(x[i01].i32(), i);
				EXPECT_EQ(x.GetSize(), 5);
				for (BindexType j = 0; j < 5; ++j)
				{
					auto y = x[j];
					EXPECT_EQ(y[i10].i32(), i);
					EXPECT_EQ(y[i11].i32(), j);
				}
			}
			else if (i < 7)
			{
				EXPECT_EQ(x[i00].i32(), i + 3);
				EXPECT_EQ(x[i01].i32(), i + 3);
				EXPECT_EQ(x.GetSize(), 5);
				for (BindexType j = 0; j < 5; ++j)
				{
					auto y = x[j];
					EXPECT_EQ(y[i10].i32(), i + 3);
					EXPECT_EQ(y[i11].i32(), j);
				}
			}
			else
			{
				EXPECT_EQ(x.GetSize(), 0);
			}
		}
		tree.Insert(2, 2, 2);
		tree.Insert(3, 3, 3);
		tree.Insert(4, 4, 4);
		for (BindexType i = 2; i < 5; ++i)
		{
			auto x = tree[i];
			for (BindexType j = 0; j < 5; ++j)
				x.Push(i, j);
			EXPECT_EQ(x.GetSize(), 5);
			EXPECT_EQ(x.GetCapacity(), 7);
		}
		EXPECT_EQ(tree.GetSize(), 20);
		EXPECT_EQ(tree.GetCapacity(), 21);
		for (BindexType i = 0; i < 15; ++i) tree.Pop();
		EXPECT_EQ(tree.GetSize(), 5);
		EXPECT_EQ(tree.GetCapacity(), 21);
		for (BindexType i = 0; i < 5; ++i)
		{
			auto x = tree[i];
			EXPECT_EQ(x[i00].i32(), i);
			EXPECT_EQ(x[i01].i32(), i);
			EXPECT_EQ(x.GetSize(), 5);
			for (BindexType j = 0; j < 5; ++j)
			{
				auto y = x[j];
				EXPECT_EQ(y[i10].i32(), i);
				EXPECT_EQ(y[i11].i32(), j);
			}
		}
	}
}


TEST_F(Aggregator, DTree_TreeHandling)
{
	using enum FieldType;
	//通常のテストでは中身の確認を行うだけだが、こちらはPushとPop、Insert、Eraseもテストする。
	DTree tree;
	tree.AddLayer({ {"i00",I32 }, { "i01", I32 } });
	tree.AddLayer({ {"i10",I32 }, { "i11", I32 } });
	tree.VerifyStructure();
	TestTreeHandling(tree);
}
TEST_F(Aggregator, STree_TreeHandling)
{
	using enum FieldType;
	//通常のテストでは中身の確認を行うだけだが、こちらはPushとPop、Insert、Eraseもテストする。
	using TopLayerTest = NamedTuple<>;
	using Layer0Test = NamedTuple<Named<"i00", int32_t>, Named<"i01", int32_t>>;
	using Layer1Test = NamedTuple<Named<"i10", int32_t>, Named<"i11", int32_t>>;
	STree<TopLayerTest, Layer0Test, Layer1Test> tree;
	TestTreeHandling(tree);
}