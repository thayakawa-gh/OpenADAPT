#include <ranges>
#include <iterator>
#include <Test/Common/Aggregator.h>

using namespace adapt;
using namespace adapt::lit;

template <FieldType Type, class Trav, class PH>
decltype(auto) get_field(Number<Type>, const Trav& t, PH& ph)
{
	if constexpr (statistically_typed<PH>) return t[ph];
	else return t[ph].template as<Type>();
};

template <any_tree Container, class Layer0, class Layer1, class Layer2>
void TestTraverserIncr(Container& tree, const std::vector<Class>& cls,
					   const Layer0& l0, const Layer1& l1, const Layer2& l2)
{
	using enum FieldType;
	//0層要素。学年とクラス。
	[[maybe_unused]] auto [grade, class_] = l0;
	//1層要素。出席番号、名前、生年月日。
	[[maybe_unused]] auto [number, name] = l1;
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	[[maybe_unused]] auto [exam, math, jpn, eng, sci, soc] = l2;

	auto range = tree.GetRange(2);
	auto trav = range.begin();
	Bpos cur(2);
	Bpos buf(2);

	//正順
	BindexType clssize = (BindexType)cls.size();
	for (BindexType i = 0; i < clssize; ++i)
	{
		cur[0] = i;
		const Class& c = cls[i];
		BindexType stusize = (BindexType)c.m_students.size();
		for (BindexType j = 0; j < stusize; ++j)
		{
			cur[1] = j;
			const Student& s = c.m_students[j];
			BindexType recsize = (BindexType)s.m_records.size();
			for (BindexType k = 0; k < recsize; ++k)
			{
				cur[2] = k;
				const Record& r = s.m_records[k];
				EXPECT_EQ(i, trav.GetPos(0));
				EXPECT_EQ(j, trav.GetPos(1));
				EXPECT_EQ(k, trav.GetPos(2));
				trav.GetBpos(buf);
				EXPECT_TRUE(cur == buf);

				EXPECT_EQ(get_field(Number<I08>{}, trav, class_), c.m_class);
				EXPECT_EQ(get_field(Number<Str>{}, trav, name), s.m_name);
				EXPECT_EQ(get_field(Number<I32>{}, trav, math), r.m_math);

				++trav;
			}
		}
	}
	EXPECT_EQ(trav, range.end());
}


template <any_tree Container, class Layer0, class Layer1, class Layer2>
void TestTraverserDecr(Container& tree, const std::vector<Class>& cls,
					   const Layer0& l0, const Layer1& l1, const Layer2& l2)
{
	using enum FieldType;
	//0層要素。学年とクラス。
	[[maybe_unused]] auto [grade, class_] = l0;
	//1層要素。出席番号、名前、生年月日。
	[[maybe_unused]] auto [number, name] = l1;
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	[[maybe_unused]] auto [exam, math, jpn, eng, sci, soc] = l2;

	auto range = tree.GetRange(2);
	auto trav = range.begin();
	Bpos cur(2);
	Bpos buf(2);
	trav.MoveToEnd();

	BindexType clssize = (BindexType)cls.size();
	//逆順
	for (BindexType i = (BindexType)clssize - 1; i >= 0; --i)
	{
		//forループをBindexTypeで回してはいけない。unsignedなのでi >= 0の判定が意味をなさない。
		cur[0] = i;
		const Class& c = cls[i];
		size_t stusize = c.m_students.size();
		for (BindexType j = (BindexType)stusize - 1; j >= 0; --j)
		{
			cur[1] = j;
			const Student& s = c.m_students[j];
			size_t recsize = s.m_records.size();
			for (BindexType k = (BindexType)recsize - 1; k >= 0; --k)
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
					trav.template TryJoin<MaxRank>(BackwardMovement{});
				}

				EXPECT_EQ(i, trav.GetPos(0));
				EXPECT_EQ(j, trav.GetPos(1));
				EXPECT_EQ(k, trav.GetPos(2));
				trav.GetBpos(buf);
				EXPECT_TRUE(cur == buf);

				EXPECT_EQ(get_field(Number<I08>{}, trav, class_), c.m_class);
				EXPECT_EQ(get_field(Number<Str>{}, trav, name), s.m_name);
				EXPECT_EQ(get_field(Number<I32>{}, trav, math), r.m_math);
			}
		}
	}
	EXPECT_EQ(trav, range.begin());
}

template <any_tree Container, class Layer0, class Layer1, class Layer2>
void TestTraverserMoveForward(Container& tree, const std::vector<Class>& cls,
							  const Layer0& l0, const Layer1& l1, const Layer2& l2)
{
	using enum FieldType;
	//0層要素。学年とクラス。
	auto [grade, class_] = l0;
	//1層要素。出席番号、名前、生年月日。
	auto [number, name] = l1;
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, jpn, eng, sci, soc] = l2;

	auto range = tree.GetRange(2);
	auto trav = range.begin();
	Bpos cur(2);
	Bpos buf(2);

	BindexType clssize = (BindexType)cls.size();
	//MoveForwardによる移動テスト
	for (BindexType i = 0; i < clssize; ++i)
	{
		cur[0] = i;
		const Class& c = cls[i];
		BindexType stusize = (BindexType)c.m_students.size();
		for (BindexType j = 0; j < stusize; ++j)
		{
			cur[1] = j;
			const Student& s = c.m_students[j];
			BindexType recsize = (BindexType)s.m_records.size();
			for (BindexType k = 0; k < recsize; ++k)
			{
				cur[2] = k;
				const Record& r = s.m_records[k];
				EXPECT_EQ(i, trav.GetPos(0));
				EXPECT_EQ(j, trav.GetPos(1));
				EXPECT_EQ(k, trav.GetPos(2));
				trav.GetBpos(buf);
				EXPECT_TRUE(cur == buf);

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
}
template <any_tree Container, class Layer0, class Layer1, class Layer2>
void TestTraverserMoveBackward(Container& tree, const std::vector<Class>& cls,
							   const Layer0& l0, const Layer1& l1, const Layer2& l2)
{
	using enum FieldType;
	//0層要素。学年とクラス。
	auto [grade, class_] = l0;
	//1層要素。出席番号、名前、生年月日。
	auto [number, name] = l1;
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, jpn, eng, sci, soc] = l2;

	auto range = tree.GetRange(2);
	auto trav = range.begin();
	Bpos cur(2);
	Bpos buf(2);
	trav.MoveToEnd();
	--trav;

	BindexType clssize = (BindexType)cls.size();
	//MoveBackward
	for (BindexType i = (BindexType)clssize - 1; i >= 0; --i)
	{
		//forループをBindexTypeで回してはいけない。unsignedなのでi >= 0の判定が意味をなさない。
		cur[0] = i;
		const Class& c = cls[i];
		size_t stusize = c.m_students.size();
		for (BindexType j = (BindexType)stusize - 1; j >= 0; --j)
		{
			cur[1] = j;
			const Student& s = c.m_students[j];
			size_t recsize = s.m_records.size();
			for (BindexType k = (BindexType)recsize - 1; k >= 0; --k)
			{
				cur[2] = k;
				const Record& r = s.m_records[k];
				//joined containerの場合、特にdelayed jointでは、
				//backward移動中の最初のアクセスのときにはBackwardFlagを与えないと、最初の要素に連結してしまう。
				//そこで、この時点で連結を強要する。
				if constexpr (joined_container<Container>)
				{
					constexpr RankType MaxRank = std::decay_t<decltype(tree)>::MaxRank;
					trav.template TryJoin<MaxRank>(BackwardMovement{});
				}
				EXPECT_EQ(i, trav.GetPos(0));
				EXPECT_EQ(j, trav.GetPos(1));
				EXPECT_EQ(k, trav.GetPos(2));
				trav.GetBpos(buf);
				EXPECT_TRUE(cur == buf);

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

template <class Container, class Layer0>
void TestTraverserIncr(Container& tree, const std::vector<Class>& cls,
					   const Layer0& l0)
{
	using enum FieldType;
	auto [class_, number, name, exam, math, jpn, eng, sci, soc] = l0;

	auto range = tree.GetRange(0_layer);
	auto trav = range.begin();
	Bpos cur(2);
	Bpos buf(2);

	//正順
	BindexType clssize = (BindexType)cls.size();
	for (BindexType i = 0; i < clssize; ++i)
	{
		cur[0] = i;
		const Class& c = cls[i];
		BindexType stusize = (BindexType)c.m_students.size();
		for (BindexType j = 0; j < stusize; ++j)
		{
			cur[1] = j;
			const Student& s = c.m_students[j];
			BindexType recsize = (BindexType)s.m_records.size();
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
}
template <class Container, class Layer0>
void TestTraverserDecr(Container& tree, const std::vector<Class>& cls,
					   const Layer0& l0)
{
	using enum FieldType;
	auto [class_, number, name, exam, math, jpn, eng, sci, soc] = l0;

	auto range = tree.GetRange(0_layer);
	auto trav = range.begin();
	Bpos cur(2);
	Bpos buf(2);
	trav.MoveToEnd();
	//逆順
	BindexType clssize = (BindexType)cls.size();
	for (BindexType i = (BindexType)clssize - 1; i >= 0; --i)
	{
		//forループをBindexTypeで回してはいけない。unsignedなのでi >= 0の判定が意味をなさない。
		cur[0] = i;
		const Class& c = cls[i];
		size_t stusize = c.m_students.size();
		for (BindexType j = (BindexType)stusize - 1; j >= 0; --j)
		{
			cur[1] = j;
			const Student& s = c.m_students[j];
			size_t recsize = s.m_records.size();
			for (BindexType k = (BindexType)recsize - 1; k >= 0; --k)
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