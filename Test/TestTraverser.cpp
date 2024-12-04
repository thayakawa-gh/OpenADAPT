#include <Test/Aggregator.h>
#include <ranges>
#include <iterator>


using namespace adapt;
using namespace adapt::lit;

template <class Container, class Layer0, class Layer1, class Layer2>
void TestTraverser(Container& tree, const std::vector<Class>& cls,
				 const Layer0& l0, const Layer1& l1, const Layer2& l2)
{
	using enum FieldType;
	//constexpr bool is_joined = joined_container<Container>;
	
	auto [class_] = l0;
	auto [name] = l1;
	auto [math] = l2;

	auto get_field = []<FieldType Type, class Trav, class PH>(Number<Type>, const Trav & t, PH& ph)
	{
		if constexpr (s_container<Container>) return t[ph];
		else return t[ph].template as<Type>();
	};

	auto range = tree.GetRange(2);
	auto trav = range.begin();
	Bpos cur(2);
	Bpos buf(2);

	//正順
	size_t clssize = cls.size();
	for (BindexType i = 0; i < clssize; ++i)
	{
		cur[0] = i;
		const Class& c = cls[i];
		size_t stusize = c.m_students.size();
		for (BindexType j = 0; j < stusize; ++j)
		{
			cur[1] = j;
			const Student& s = c.m_students[j];
			size_t recsize = s.m_records.size();
			for (BindexType k = 0; k < recsize; ++k)
			{
				cur[2] = k;
				const Record& r = s.m_records[k];
				EXPECT_EQ(i, trav.GetPos(0));
				EXPECT_EQ(j, trav.GetPos(1));
				EXPECT_EQ(k, trav.GetPos(2));
				trav.GetBpos(buf);
				EXPECT_TRUE(cur.MatchPerfectly(buf));

				EXPECT_EQ(get_field(Number<I08>{}, trav, class_), c.m_class);
				EXPECT_EQ(get_field(Number<Str>{}, trav, name), s.m_name);
				EXPECT_EQ(get_field(Number<I32>{}, trav, math), r.m_math);

				++trav;
			}
		}
	}
	EXPECT_EQ(trav, range.end());

	//逆順
	for (int32_t i = (int32_t)clssize - 1; i >= 0; --i)
	{
		//forループをBindexTypeで回してはいけない。unsignedなのでi >= 0の判定が意味をなさない。
		cur[0] = i;
		const Class& c = cls[i];
		size_t stusize = c.m_students.size();
		for (int32_t j = (int32_t)stusize - 1; j >= 0; --j)
		{
			cur[1] = j;
			const Student& s = c.m_students[j];
			size_t recsize = s.m_records.size();
			for (int32_t k = (int32_t)recsize - 1; k >= 0; --k)
			{
				--trav;

				cur[2] = k;
				const Record& r = s.m_records[k];
				//joined containerの場合、特にdelayed jointでは、
				//backward移動中の最初のアクセスのときにはBackwardFlagを与えないと、最初の要素に連結してしまう。
				//そこで、この時点で連結を強要する。
				if constexpr (joined_container<Container>)
				{
					constexpr RankType MaxRank = std::decay_t<decltype(tree)>::MaxRank;
					trav.template TryJoin<MaxRank>(BackwardFlag{});
				}

				EXPECT_EQ(i, trav.GetPos(0));
				EXPECT_EQ(j, trav.GetPos(1));
				EXPECT_EQ(k, trav.GetPos(2));
				trav.GetBpos(buf);
				EXPECT_TRUE(cur.MatchPerfectly(buf));

				EXPECT_EQ(get_field(Number<I08>{}, trav, class_), c.m_class);
				EXPECT_EQ(get_field(Number<Str>{}, trav, name), s.m_name);
				EXPECT_EQ(get_field(Number<I32>{}, trav, math), r.m_math);
			}
		}
	}
	EXPECT_EQ(trav, range.begin());

	//MoveForwardによる移動テスト
	for (BindexType i = 0; i < clssize; ++i)
	{
		cur[0] = i;
		const Class& c = cls[i];
		size_t stusize = c.m_students.size();
		for (BindexType j = 0; j < stusize; ++j)
		{
			cur[1] = j;
			const Student& s = c.m_students[j];
			size_t recsize = s.m_records.size();
			for (BindexType k = 0; k < recsize; ++k)
			{
				cur[2] = k;
				const Record& r = s.m_records[k];
				EXPECT_EQ(i, trav.GetPos(0));
				EXPECT_EQ(j, trav.GetPos(1));
				EXPECT_EQ(k, trav.GetPos(2));
				trav.GetBpos(buf);
				EXPECT_TRUE(cur.MatchPerfectly(buf));

				EXPECT_EQ(get_field(Number<I08>{}, trav, class_), c.m_class);
				EXPECT_EQ(get_field(Number<Str>{}, trav, name), s.m_name);
				EXPECT_EQ(get_field(Number<I32>{}, trav, math), r.m_math);

				trav.MoveForward(2);
			}
			trav.MoveForward(1);
		}
		trav.MoveForward(0);
	}
	//MoveForwardのあとはend状態にならない。末尾の有効要素を指したままとなる。
	EXPECT_EQ(trav.GetPos(0), clssize - 1);
	EXPECT_EQ(trav.GetPos(1), 29);
	EXPECT_EQ(trav.GetPos(2), 3);

	//MoveBackward
	for (int32_t i = (int32_t)clssize - 1; i >= 0; --i)
	{
		//forループをBindexTypeで回してはいけない。unsignedなのでi >= 0の判定が意味をなさない。
		cur[0] = i;
		const Class& c = cls[i];
		size_t stusize = c.m_students.size();
		for (int32_t j = (int32_t)stusize - 1; j >= 0; --j)
		{
			cur[1] = j;
			const Student& s = c.m_students[j];
			size_t recsize = s.m_records.size();
			for (int32_t k = (int32_t)recsize - 1; k >= 0; --k)
			{
				cur[2] = k;
				const Record& r = s.m_records[k];
				//joined containerの場合、特にdelayed jointでは、
				//backward移動中の最初のアクセスのときにはBackwardFlagを与えないと、最初の要素に連結してしまう。
				//そこで、この時点で連結を強要する。
				if constexpr (joined_container<Container>)
				{
					constexpr RankType MaxRank = std::decay_t<decltype(tree)>::MaxRank;
					trav.template TryJoin<MaxRank>(BackwardFlag{});
				}
				EXPECT_EQ(i, trav.GetPos(0));
				EXPECT_EQ(j, trav.GetPos(1));
				EXPECT_EQ(k, trav.GetPos(2));
				trav.GetBpos(buf);
				EXPECT_TRUE(cur.MatchPerfectly(buf));

				EXPECT_EQ(get_field(Number<I08>{}, trav, class_), c.m_class);
				EXPECT_EQ(get_field(Number<Str>{}, trav, name), s.m_name);
				EXPECT_EQ(get_field(Number<I32>{}, trav, math), r.m_math);

				trav.MoveBackward(2);
			}
			trav.MoveBackward(1);
		}
		trav.MoveBackward(0);
	}
	EXPECT_EQ(trav.GetPos(0), 0);
	EXPECT_EQ(trav.GetPos(1), 0);
	EXPECT_EQ(trav.GetPos(2), 0);
}

TEST_F(Aggregator, DTree_Traverser)
{
	auto class_ = m_dtree.GetPlaceholder("class_");
	auto name = m_dtree.GetPlaceholder("name");
	auto math = m_dtree.GetPlaceholder("math");

	TestTraverser(m_dtree, m_class,
				  std::make_tuple(class_),
				  std::make_tuple(name),
				  std::make_tuple(math));
}
TEST_F(Aggregator, STree_Traverser)
{
	auto class_ = m_stree.GetPlaceholder("class_"_fld);
	auto name = m_stree.GetPlaceholder("name"_fld);
	auto math = m_stree.GetPlaceholder("math"_fld);
	static_assert(ctti_placeholder<decltype(class_)>);
	static_assert(ctti_placeholder<decltype(name)>);
	static_assert(ctti_placeholder<decltype(math)>);

	TestTraverser(m_stree, m_class,
				  std::make_tuple(class_),
				  std::make_tuple(name),
				  std::make_tuple(math));
}
TEST_F(Aggregator, DJoinedContainer_Traverser)
{
	auto a = m_dtree.GetPlaceholder("number").i16();//MakeHashmapはキーの型を特定する必要があるため、
	auto b = m_dtree.GetPlaceholder("name").str();//これらのPlaceholderは予め型情報を与えなければならない。
	auto hash = m_dtree | MakeHashtable(a, b);

	auto jtree = Join(m_dtree, 1_layer, 1_layer, m_dtree);
	//0層要素。学年とクラス。
	auto [nu, na] = jtree.GetPlaceholders<0>("number"_fld, "name"_fld);//検索用
	auto class_ = jtree.GetPlaceholder<1>("class_"_fld);//値取得用。
	//1層要素。出席番号、名前、生年月日。
	auto name = jtree.GetPlaceholder<1>("name"_fld);
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto math = jtree.GetPlaceholder<1>("math");

	jtree.SetKeyJoint<1>(std::move(hash), nu, na);

	TestTraverser(jtree, m_class,
				  std::make_tuple(class_),
				  std::make_tuple(name),
				  std::make_tuple(math));
}



template <class Container, class Layer0>
void TestTraverser(Container& tree, const std::vector<Class>& cls,
	const Layer0& l0)
{
	using enum FieldType;
	//constexpr bool is_joined = joined_container<Container>;

	auto [class_, name, math] = l0;

	auto get_field = []<FieldType Type, class Trav, class PH>(Number<Type>, const Trav & t, PH & ph)
	{
		if constexpr (s_container<Container>) return t[ph];
		else return t[ph].template as<Type>();
	};

	auto range = tree.GetRange(0_layer);
	auto trav = range.begin();
	Bpos cur(2);
	Bpos buf(2);

	//正順
	size_t clssize = cls.size();
	for (BindexType i = 0; i < clssize; ++i)
	{
		cur[0] = i;
		const Class& c = cls[i];
		size_t stusize = c.m_students.size();
		for (BindexType j = 0; j < stusize; ++j)
		{
			cur[1] = j;
			const Student& s = c.m_students[j];
			size_t recsize = s.m_records.size();
			for (BindexType k = 0; k < recsize; ++k)
			{
				cur[2] = k;
				const Record& r = s.m_records[k];

				EXPECT_EQ(get_field(Number<I08>{}, trav, class_), c.m_class);
				EXPECT_EQ(get_field(Number<Str>{}, trav, name), s.m_name);
				EXPECT_EQ(get_field(Number<I32>{}, trav, math), r.m_math);

				++trav;
			}
		}
	}
	EXPECT_EQ(trav, range.end());

	//逆順
	for (int32_t i = (int32_t)clssize - 1; i >= 0; --i)
	{
		//forループをBindexTypeで回してはいけない。unsignedなのでi >= 0の判定が意味をなさない。
		cur[0] = i;
		const Class& c = cls[i];
		size_t stusize = c.m_students.size();
		for (int32_t j = (int32_t)stusize - 1; j >= 0; --j)
		{
			cur[1] = j;
			const Student& s = c.m_students[j];
			size_t recsize = s.m_records.size();
			for (int32_t k = (int32_t)recsize - 1; k >= 0; --k)
			{
				--trav;

				cur[2] = k;
				const Record& r = s.m_records[k];

				EXPECT_EQ(get_field(Number<I08>{}, trav, class_), c.m_class);
				EXPECT_EQ(get_field(Number<Str>{}, trav, name), s.m_name);
				EXPECT_EQ(get_field(Number<I32>{}, trav, math), r.m_math);
			}
		}
	}
	EXPECT_EQ(trav, range.begin());
}
TEST_F(Aggregator, Traverse_dtable)
{
	auto& t = m_dtable;

	auto [class_, name, math] = t.GetPlaceholders("class_", "name", "math");
	static_assert(adapt::container_simplex<adapt::DTable>);
	TestTraverser(t, m_class,
		std::make_tuple(class_, name, math));
}
TEST_F(Aggregator, Traverse_stable)
{
	auto& t = m_stable;

	auto [class_, name, math] = t.GetPlaceholders("class_"_fld, "name"_fld, "math"_fld);
	TestTraverser(t, m_class,
		std::make_tuple(class_, name, math));
}