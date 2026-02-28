#ifndef ADAPT_TEST_COMMON_AGGREGATOR_H
#define ADAPT_TEST_COMMON_AGGREGATOR_H

#include <format>
#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include <complex>
#include <ranges>
#include <array>
#include <fstream>
#include <random>
#include <algorithm>
#include <string>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include <gtest/gtest.h>
#ifdef ADAPT_IMPORT_MODULE
#include <OpenADAPT/Macros.h>
import adapt;
#else
#include <OpenADAPT/ADAPT.h>
#endif

using namespace adapt;
using namespace adapt::lit;

struct Record
{
	int8_t m_exam;
	int8_t m_math;
	int8_t m_japanese;
	int8_t m_english;
	int8_t m_science;
	int8_t m_social;
};

struct Student
{
	int16_t m_number;
	std::string m_name;
	std::string m_name_furigana;
	int32_t m_date_of_birth;
	int32_t m_postal_code;
	std::string m_address;
	std::string m_phone;
	std::string m_email;
	std::vector<Record> m_records;
};
struct Class
{
	int8_t m_grade;
	int8_t m_class;
	std::vector<Student> m_students;
};

inline std::string NumName(const std::vector<Class>& cs, BindexType i, BindexType j, BindexType)
{
	const auto& s = cs[i].m_students[j];
	return std::to_string(s.m_number) + ":" + s.m_name;
}
inline int32_t TotalScore(const std::vector<Class>& cs, BindexType i, BindexType j, BindexType k)
{
	const auto& r = cs[i].m_students[j].m_records[k];
	return r.m_math + r.m_japanese + r.m_english + r.m_science + r.m_social;
}
inline bool HasFailed(const std::vector<Class>& c, BindexType i, BindexType j, BindexType k)
{
	const auto& s = c[i].m_students[j];
	const auto& r = s.m_records[k];
	if (r.m_math < 40 || r.m_japanese < 40) return true;
	return false;
}
inline bool BothFailed(const std::vector<Class>& c, BindexType i, BindexType j, BindexType k)
{
	const auto& s = c[i].m_students[j];
	const auto& r = s.m_records[k];
	if (r.m_math < 40 && r.m_japanese < 40) return true;
	return false;
}
inline bool All400(const std::vector<Class>& c, BindexType i, BindexType j, BindexType)
{
	const auto& s = c[i].m_students[j];
	size_t count = 0;
	for (auto& r : s.m_records) count += (size_t)((int64_t)r.m_math + (int64_t)r.m_japanese + (int64_t)r.m_english + (int64_t)r.m_science + (int64_t)r.m_social >= 400);
	return count == 4;
}
inline double AvgMath(const std::vector<Class>& c, BindexType i, BindexType j, BindexType)
{
	const auto& s = c[i].m_students[j];
	double math_sum = 0.;
	for (auto& r : s.m_records) math_sum += (double)r.m_math;
	return math_sum / 4.;
}
inline double AvgMathInClass(const std::vector<Class>& cs, BindexType i, BindexType, BindexType)
{
	const auto& c = cs[i];
	double math_sum = 0.;
	for (auto& s : c.m_students)
		math_sum += (double)s.m_records[0].m_math;
	return math_sum / (double)c.m_students.size();
}
inline double DevMathInClass(const std::vector<Class>& cs, BindexType i, BindexType, BindexType)
{
	const auto& c = cs[i];
	double sqsum = 0.;
	double sum = 0.;
	for (auto& s : c.m_students)
	{
		double math = (double)s.m_records[0].m_math;
		sqsum += math * math;
		sum += math;
	}
	double count = (double)c.m_students.size();
	sqsum /= count;
	sum /= count;
	double res = sqsum - sum * sum;
	if (res <= 0.) return 0.;
	else return std::sqrt(res);
}
inline double DevMath(const std::vector<Class>& cs, BindexType i, BindexType j, BindexType k)
{
	const auto& c = cs[i];
	const auto& s = c.m_students[j];
	double math = (double)s.m_records[0].m_math;
	double avg = AvgMathInClass(cs, i, j, k);
	double dev = DevMathInClass(cs, i, j, k);
	return (math - avg) * 10 / dev + 50.;
}
inline bool IsBest(const std::vector<Class>& cs, BindexType i, BindexType j, BindexType k)
{
	const auto& s = cs[i].m_students[j];
	int64_t max = 0;
	BindexType maxindex = 0;
	for (auto [ii, r] : views::Enumerate(s.m_records))
	{
		int64_t sum = (int64_t)r.m_math + (int64_t)r.m_japanese + (int64_t)r.m_english + (int64_t)r.m_science + (int64_t)r.m_social;
		if (max < sum)
		{
			max = sum;
			maxindex = (BindexType)ii;
		}
	}
	return maxindex == k;
}
inline int64_t NumStu200(const std::vector<Class>& cs, BindexType i, BindexType, BindexType)
{
	const auto& c = cs[i];
	int64_t count = 0;
	for (auto& s : c.m_students)
	{
		bool b = false;
		for (auto& r : s.m_records)
			b = b || (r.m_math + r.m_japanese + r.m_english >= 200);
		if (b) ++count;
	}
	return count;
}
inline float MeanMax(const std::vector<Class>& cs, BindexType i, BindexType, BindexType)
{
	const auto& c = cs[i];
	float res = 0;
	size_t count = 0;
	for (auto& s : c.m_students)
	{
		int32_t max = 0;
		for (auto& r : s.m_records)
		{
			int32_t sum = (int32_t)r.m_math + (int32_t)r.m_japanese + (int32_t)r.m_english + (int32_t)r.m_science + (int32_t)r.m_social;
			if (max < sum) max = sum;
		}
		res += (float)max;
		++count;
	}
	return res / count;
}
inline int64_t RankMathClass(const std::vector<Class>& cs, BindexType i, BindexType j, BindexType k)
{
	const auto& c = cs[i];
	const auto& r = cs[i].m_students[j].m_records[k];
	int64_t count = 0;
	for (auto& ss : c.m_students)
	{
		for (auto& rr : ss.m_records)
		{
			if (rr.m_exam != r.m_exam) continue;
			if (rr.m_math > r.m_math) ++count;
		}
	}
	return count;
}
inline int64_t RankMathAll(const std::vector<Class>& cs, BindexType i, BindexType j, BindexType k)
{
	const auto& r = cs[i].m_students[j].m_records[k];
	int64_t count = 0;
	for (auto& c : cs)
	{
		for (auto& ss : c.m_students)
		{
			for (auto& rr : ss.m_records)
			{
				if (rr.m_exam != r.m_exam) continue;
				if (rr.m_math > r.m_math) ++count;
			}
		}
	}
	return count;
}
inline int32_t SumMathOver60(const std::vector<Class>& cs, BindexType i, BindexType j, BindexType)
{
	const auto& s = cs[i].m_students[j];
	int32_t res = 0;
	for (auto& r : s.m_records)
	{
		if (r.m_math > 60) res += r.m_math;
	}
	return res;
}
inline float MeanMathEngOver60(const std::vector<Class>& cs, BindexType i, BindexType, BindexType)
{
	const auto& c = cs[i];
	float res = 0;
	size_t count = 0;
	for (auto& s : c.m_students)
	{
		int32_t sum_eng = 0;
		for (auto& r : s.m_records) sum_eng += r.m_english;
		if (sum_eng >= 240)
		{
			for (auto& r : s.m_records)
			{
				res += (float)r.m_math;
				++count;
			}
		}
	}
	return res / count;
}
inline bool IsSecondMaxMath(const std::vector<Class>& cs, BindexType i, BindexType j, BindexType k)
{
	const auto& s = cs[i].m_students[j];
	int32_t max = -1;
	int32_t second = -1;
	BindexType max_pos = std::numeric_limits<BindexType>::max();
	BindexType second_pos = std::numeric_limits<BindexType>::max();
	for (auto [ii, r] : views::Enumerate(s.m_records))
	{
		if (max < r.m_math)
		{
			//maxが更新された。
			second = max;
			max = r.m_math;
			second_pos = max_pos;
			max_pos = (BindexType)ii;
		}
		else if (second < r.m_math)
		{
			//secondが更新された。
			second = r.m_math;
			second_pos = (BindexType)ii;
		}
	}
	return second_pos == k;
}
inline bool IsWorstInThirdExam(const std::vector<Class>& cs, BindexType i, BindexType j, BindexType k)
{
	const auto& c = cs[i];
	int32_t min = std::numeric_limits<int32_t>::max();
	BindexType min_stu = std::numeric_limits<BindexType>::max();
	if (k != 2) return false;
	for (auto [ii, s] : views::Enumerate(c.m_students))
	{
		auto& r = s.m_records[k];
		int32_t total = r.m_math + r.m_japanese + r.m_english + r.m_science + r.m_social;
		if (min > total)
		{
			min = total;
			min_stu = (BindexType)ii;
		}
	}
	return min_stu == j;
}

template <any_tree Tree>
void MakeContainer(std::optional<Tree>& tree, const std::vector<Class>& cs)
{
	if (tree) return;
	tree.emplace();
	Tree& t = *tree;
	if constexpr (d_tree<Tree>)
	{
		ADAPT_D_SET_TOP_LAYER(t, school, Str);
		ADAPT_D_ADD_LAYER(t, grade, I08, class_, I08);
		ADAPT_D_ADD_LAYER(t, number, I16, name, Str, date_of_birth, I32, phone, Str, email, Str);
		ADAPT_D_ADD_LAYER(t, exam, I08, math, I32, japanese, I32, english, I32, science, I32, social, I32);
		t.VerifyStructure();
	}

	t.SetTopFields("胴差県立散布流中学校");
	t.Reserve((BindexType)cs.size());
	for (auto& c : cs)
	{
		t.Push(c.m_grade, c.m_class);
		auto&& ce = t.Back();
		ce.Reserve((BindexType)c.m_students.size());
		for (auto& s : c.m_students)
		{
			ce.Push(s.m_number, s.m_name, s.m_date_of_birth, s.m_phone, s.m_email);
			auto&& se = ce.Back();
			se.Reserve((BindexType)s.m_records.size());
			for (auto& r : s.m_records)
			{
				se.Push(r.m_exam, r.m_math, r.m_japanese, r.m_english, r.m_science, r.m_social);
			}
		}
	}
}
template <any_table Table>
void MakeContainer(std::optional<Table>& table, const std::vector<Class>& cs)
{
	if (table) return;
	table.emplace();
	Table& t = *table;
	if constexpr (d_table<Table>)
	{
		ADAPT_D_SET_TOP_LAYER(t, school, Str);
		ADAPT_D_SET_LAYER(t, 0, grade, I08, class_, I08,
						  number, I16, name, Str, date_of_birth, I32, phone, Str, email, Str,
						  exam, I08, math, I32, japanese, I32, english, I32, science, I32, social, I32);
		t.VerifyStructure();
	}

	t.SetTopFields("胴差県立散布流中学校");
	for (auto& c : cs)
	{
		for (auto& s : c.m_students)
		{
			for (auto& r : s.m_records)
			{
				t.Push(c.m_grade, c.m_class,
					   s.m_number, s.m_name, s.m_date_of_birth, s.m_phone, s.m_email,
					   r.m_exam, r.m_math, r.m_japanese, r.m_english, r.m_science, r.m_social);
			}
		}
	}
}
inline void Generate()
{
	//すでにファイルが存在するのなら何もしない。
	if (std::filesystem::exists("OpenADAPT-records.yaml")) return;

	std::random_device rd;
	std::mt19937_64 mt(rd());

	std::normal_distribution<double> random_dev(60, 20);

	FILE* fp = fopen("OpenADAPT-student_list.txt", "r");
	if (fp == nullptr) throw Exception();
	char buf[1024];

	std::vector<Class> classes(4);
	classes[0] = { 3, 0, std::vector<Student>(30) };
	classes[1] = { 3, 1, std::vector<Student>(30) };
	classes[2] = { 3, 2, std::vector<Student>(30) };
	classes[3] = { 3, 3, std::vector<Student>(30) };
	int32_t count = 0;
	while (fgets(buf, sizeof(buf), fp) != nullptr)
	{
		char name[64];
		char furigana[64];
		int32_t dateofbirth;
		int32_t postal;
		char address[256];
		char phone[64];
		char email[256];

		int8_t class_ = int8_t(count % 4);
		int16_t number = int16_t(count / 4);

		if (sscanf(buf, "%s %s %d %d %s %s %s", name, furigana, &dateofbirth, &postal, address, phone, email) != 7)
			throw Exception();

		Student s{ number, name, furigana, dateofbirth, postal, address, phone, email, std::vector<Record>(4) };

		std::normal_distribution<double> random_sub(random_dev(mt), 10);

		std::normal_distribution<double> random_score_mat(random_sub(mt), 10);
		std::normal_distribution<double> random_score_jpn(random_sub(mt), 10);
		std::normal_distribution<double> random_score_eng(random_sub(mt), 10);
		std::normal_distribution<double> random_score_sci(random_sub(mt), 10);
		std::normal_distribution<double> random_score_soc(random_sub(mt), 10);

		for (size_t i = 0; i < 4; ++i)
		{
			auto gen_score = [&mt](std::normal_distribution<double>& r) { return (int8_t)std::clamp(r(mt), 0., 100.); };
			Record rec{
				(int8_t)i,
				gen_score(random_score_mat),
				gen_score(random_score_jpn),
				gen_score(random_score_eng),
				gen_score(random_score_sci),
				gen_score(random_score_soc)
			};
			s.m_records[i] = rec;
		}
		classes[class_].m_students[number] = std::move(s);

		++count;
	}

	YAML::Node nclist;
	for (auto& c : classes)
	{
		YAML::Node nc;
		nc["grade"] = (int32_t)c.m_grade;
		nc["class_"] = (int32_t)c.m_class;
		auto nslist = nc["students"];
		for (auto& s : c.m_students)
		{
			YAML::Node ns;
			ns["number"] = s.m_number;
			ns["name"] = s.m_name;
			ns["name_furigana"] = s.m_name_furigana;
			ns["date_of_birth"] = s.m_date_of_birth;
			ns["postal_code"] = s.m_postal_code;
			ns["address"] = s.m_address;
			ns["phone"] = s.m_phone;
			ns["email"] = s.m_email;
			auto nrlist = ns["record"];
			for (auto& r : s.m_records)
			{
				YAML::Node nr;
				nr["math"] = (int32_t)r.m_math;
				nr["japanese"] = (int32_t)r.m_japanese;
				nr["english"] = (int32_t)r.m_english;
				nr["science"] = (int32_t)r.m_science;
				nr["social"] = (int32_t)r.m_social;
				nrlist.push_back(nr);
				nr.SetStyle(YAML::EmitterStyle::Flow);
			}
			nslist.push_back(ns);
		}
		nclist.push_back(nc);
	}
	std::ofstream out("OpenADAPT-records.yaml");
	out << nclist;
}
inline std::vector<Class> MakeClass()
{
	std::vector<Class> res;
	Generate();
	YAML::Node cs = YAML::LoadFile("OpenADAPT-records.yaml");
	res.resize(cs.size());
	for (auto [class_, c] : views::Zip(res, cs))
	{
		class_.m_grade = c["grade"].as<int8_t>();
		class_.m_class = c["class_"].as<int8_t>();
		YAML::Node ss = c["students"];
		class_.m_students.resize(ss.size());
		for (auto [stu, s] : views::Zip(class_.m_students, ss))
		{
			stu.m_number = s["number"].as<int16_t>();
			stu.m_name = s["name"].as<std::string>();
			stu.m_name_furigana = s["name_furigana"].as<std::string>();
			stu.m_date_of_birth = s["date_of_birth"].as<int32_t>();
			stu.m_postal_code = s["postal_code"].as<int32_t>();
			stu.m_address = s["address"].as<std::string>();
			stu.m_phone = s["phone"].as<std::string>();
			stu.m_email = s["email"].as<std::string>();
			YAML::Node rs = s["record"];
			stu.m_records.resize(rs.size());
			for (auto [e, rec, r] : views::Enumerate(stu.m_records, rs))
			{
				rec.m_exam = (int8_t)e;
				rec.m_math = r["math"].as<int8_t>();
				rec.m_japanese = r["japanese"].as<int8_t>();
				rec.m_english = r["english"].as<int8_t>();
				rec.m_science = r["science"].as<int8_t>();
				rec.m_social = r["social"].as<int8_t>();
			}
		}
	}
	return res;
}
class Aggregator : public ::testing::Test
{
public:
	using TopLayer = ADAPT_S_DEFINE_LAYER(school, std::string);
	using Layer0 = ADAPT_S_DEFINE_LAYER(grade, int8_t, class_, int8_t);
	using Layer1 = ADAPT_S_DEFINE_LAYER(number, int16_t, name, std::string, date_of_birth, int32_t, phone, std::string, email, std::string);
	using Layer2 = ADAPT_S_DEFINE_LAYER(exam, int8_t, math, int32_t, japanese, int32_t, english, int32_t, science, int32_t, social, int32_t);

	using STree_ = STree<TopLayer, Layer0, Layer1, Layer2>;
	using STable_ = STable<TopLayer, NamedTupleCat_t<Layer0, Layer1, Layer2>>;

protected:
	inline static std::vector<Class> m_class = MakeClass();
};

class Aggregator_DTree : public Aggregator
{
public:
	using Aggregator::Aggregator;
protected:
	virtual void SetUp() override
	{
		Aggregator::SetUp();
		MakeContainer(m_tree, Aggregator::m_class);
	}
	inline static std::optional<DTree> m_tree = {};
};

class Aggregator_STree : public Aggregator
{
public:
	using Aggregator::Aggregator;
protected:
	virtual void SetUp() override
	{
		Aggregator::SetUp();
		MakeContainer(m_tree, Aggregator::m_class);
	}
	inline static std::optional<STree_> m_tree = {};
};

class Aggregator_DTable : public Aggregator
{
public:
	using Aggregator::Aggregator;
protected:
	virtual void SetUp() override
	{
		Aggregator::SetUp();
		MakeContainer(m_table, Aggregator::m_class);
	}
	inline static std::optional<DTable> m_table = {};
};
class Aggregator_STable : public Aggregator
{
public:
	using Aggregator::Aggregator;
protected:
	virtual void SetUp() override
	{
		Aggregator::SetUp();
		MakeContainer(m_table, Aggregator::m_class);
	}
	inline static std::optional<STable_> m_table = {};
};

class Aggregator_DJTree0 : public Aggregator_DTree
{
public:
	using Aggregator_DTree::Aggregator_DTree;

protected:
	virtual void SetUp() override
	{
		Aggregator_DTree::SetUp();
		m_tree.emplace(Join(*Aggregator_DTree::m_tree, 0_layer, 0_layer, *Aggregator_DTree::m_tree));

		auto x = Aggregator_DTree::m_tree->GetPlaceholder("class_").i08();
		auto y = Aggregator_DTree::m_tree->GetPlaceholder("grade").i08();
		auto hash = *Aggregator_DTree::m_tree | Hash(x, y);
		auto& jtree = *m_tree;
		//0層要素。学年とクラス。
		auto [gg, cc] = jtree.GetPlaceholders<0>("grade"_fld, "class_"_fld);
		[[maybe_unused]] ADAPT_GET_RANKED_PLACEHOLDERS(jtree, 1, grade, class_);
		//1層要素。出席番号、名前。
		[[maybe_unused]] ADAPT_GET_RANKED_PLACEHOLDERS(jtree, 1, number, name);
		//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
		[[maybe_unused]] ADAPT_GET_RANKED_PLACEHOLDERS(jtree, 1, exam, math, japanese, english, science, social);
		jtree.SetKeyJoint<1>(std::move(hash), cc, gg);
	}
	using DJoined = decltype(Join(std::declval<DTree&>(), (LayerType)0, (LayerType)0, std::declval<DTree&>()));
	std::optional<DJoined> m_tree;
};

class Aggregator_DJTree1 : public Aggregator_DTree
{
public:
	using Aggregator_DTree::Aggregator_DTree;

protected:
	virtual void SetUp() override
	{
		Aggregator_DTree::SetUp();
		m_tree.emplace(Join(*Aggregator_DTree::m_tree, 1_layer, 1_layer, *Aggregator_DTree::m_tree));

		auto x = Aggregator_DTree::m_tree->GetPlaceholder("number").i16();//MakeHashmapはキーの型を特定する必要があるため、
		auto y = Aggregator_DTree::m_tree->GetPlaceholder("name").str();//これらのPlaceholderは予め型情報を与えなければならない。
		auto hash = *Aggregator_DTree::m_tree | Hash(x, y);
		auto& jtree = *m_tree;
		//0層要素。学年とクラス。
		[[maybe_unused]] ADAPT_GET_RANKED_PLACEHOLDERS(jtree, 0, grade, class_);
		//1層要素。出席番号、名前。
		auto [nu, na] = jtree.GetPlaceholders<0>("number"_fld, "name"_fld);
		[[maybe_unused]] ADAPT_GET_RANKED_PLACEHOLDERS(jtree, 1, number, name);
		//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
		[[maybe_unused]] ADAPT_GET_RANKED_PLACEHOLDERS(jtree, 1, exam, math, japanese, english, science, social);

		jtree.SetKeyJoint<1>(std::move(hash), nu, na);
	}
	using DJoined = decltype(Join(std::declval<DTree&>(), (LayerType)0, (LayerType)0, std::declval<DTree&>()));
	std::optional<DJoined> m_tree;
};

class Aggregator_DJTree2 : public Aggregator_DTree
{
public:
	using Aggregator_DTree::Aggregator_DTree;

protected:
	virtual void SetUp() override
	{
		Aggregator_DTree::SetUp();
		m_tree.emplace(Join(*Aggregator_DTree::m_tree, 2_layer, 2_layer, *Aggregator_DTree::m_tree));

		auto a = Aggregator_DTree::m_tree->GetPlaceholder("number").i16();//MakeHashmapはキーの型を特定する必要があるため、
		auto b = Aggregator_DTree::m_tree->GetPlaceholder("name").str();//これらのPlaceholderは予め型情報を与えなければならない。
		auto c = Aggregator_DTree::m_tree->GetPlaceholder("exam").i08();
		auto hash = *Aggregator_DTree::m_tree | Hash(a, b, c);
		auto& jtree = *m_tree;
		//0層要素。学年とクラス。
		[[maybe_unused]] ADAPT_GET_RANKED_PLACEHOLDERS(jtree, 0, grade, class_);
		//1層要素。出席番号、名前。
		[[maybe_unused]] ADAPT_GET_RANKED_PLACEHOLDERS(jtree, 0, number, name);
		//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
		auto ee = jtree.GetPlaceholder<0>("exam"_fld);
		[[maybe_unused]] ADAPT_GET_RANKED_PLACEHOLDERS(jtree, 1, exam, math, japanese, english, science, social);

		jtree.SetKeyJoint<1>(std::move(hash), number, name, ee);
	}
	using DJoined = decltype(Join(std::declval<DTree&>(), (LayerType)0, (LayerType)0, std::declval<DTree&>()));
	std::optional<DJoined> m_tree;
};
class Aggregator_DJTree0_1 : public Aggregator_DTree
{
public:
	using Aggregator_DTree::Aggregator_DTree;

protected:
	virtual void SetUp() override
	{
		Aggregator_DTree::SetUp();
		m_tree.emplace(Join(*Aggregator_DTree::m_tree, 0_layer,
							0_layer, *Aggregator_DTree::m_tree, 1_layer,
							1_layer, *Aggregator_DTree::m_tree));

		auto c = Aggregator_DTree::m_tree->GetPlaceholder("class_").i08();//MakeHashmapはキーの型を特定する必要があるため、
		auto g = Aggregator_DTree::m_tree->GetPlaceholder("grade").i08();//これらのPlaceholderは予め型情報を与えなければならない。
		auto hash1 = *Aggregator_DTree::m_tree | Hash(c, g);

		auto a = Aggregator_DTree::m_tree->GetPlaceholder("number").i16();//MakeHashmapはキーの型を特定する必要があるため、
		auto b = Aggregator_DTree::m_tree->GetPlaceholder("name").str();//これらのPlaceholderは予め型情報を与えなければならない。
		auto hash2 = *Aggregator_DTree::m_tree | Hash(a, b);

		auto& jtree = *m_tree;
		//0層要素。学年とクラス。
		[[maybe_unused]] ADAPT_GET_RANKED_PLACEHOLDERS(jtree, 0, grade, class_);
		//1層要素。出席番号、名前。
		[[maybe_unused]] ADAPT_GET_RANKED_PLACEHOLDERS(jtree, 1, number, name);
		//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
		//[[maybe_unused]] auto ee = jtree.GetPlaceholder<1>("exam"_fld);
		[[maybe_unused]] ADAPT_GET_RANKED_PLACEHOLDERS(jtree, 2, exam, math, japanese, english, science, social);

		jtree.SetKeyJoint<1>(std::move(hash1), class_, grade);
		jtree.SetKeyJoint<2>(std::move(hash2), number, name);
	}
	using DJoined = decltype(Join(std::declval<DTree&>(), (LayerType)0, (LayerType)0, std::declval<DTree&>(), (LayerType)0, (LayerType)0, std::declval<DTree&>()));
	std::optional<DJoined> m_tree;
};
class Aggregator_DJTree1_2 : public Aggregator_DTree
{
public:
	using Aggregator_DTree::Aggregator_DTree;

protected:
	virtual void SetUp() override
	{
		Aggregator_DTree::SetUp();
		m_tree.emplace(Join(*Aggregator_DTree::m_tree, 1_layer,
							 1_layer, *Aggregator_DTree::m_tree, 2_layer,
							 2_layer, *Aggregator_DTree::m_tree));

		auto a = Aggregator_DTree::m_tree->GetPlaceholder("number").i16();//MakeHashmapはキーの型を特定する必要があるため、
		auto b = Aggregator_DTree::m_tree->GetPlaceholder("name").str();//これらのPlaceholderは予め型情報を与えなければならない。
		auto hash1 = *Aggregator_DTree::m_tree | Hash(a, b);

		auto c = Aggregator_DTree::m_tree->GetPlaceholder("exam").i08();
		auto hash2 = *Aggregator_DTree::m_tree | Hash(a, b, c);

		auto& jtree = *m_tree;
		//0層要素。学年とクラス。
		[[maybe_unused]] ADAPT_GET_RANKED_PLACEHOLDERS(jtree, 0, grade, class_);
		//1層要素。出席番号、名前。
		auto [nu, na] = jtree.GetPlaceholders<0>("number"_fld, "name"_fld);
		[[maybe_unused]] ADAPT_GET_RANKED_PLACEHOLDERS(jtree, 1, number, name);
		//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
		auto ee = jtree.GetPlaceholder<1>("exam"_fld);
		[[maybe_unused]] ADAPT_GET_RANKED_PLACEHOLDERS(jtree, 2, exam, math, japanese, english, science, social);

		jtree.SetKeyJoint<1>(std::move(hash1), nu, na);
		jtree.SetKeyJoint<2>(std::move(hash2), nu, na, ee);
	}
	using DJoined = decltype(Join(std::declval<DTree&>(), (LayerType)0, (LayerType)0, std::declval<DTree&>(), (LayerType)0, (LayerType)0, std::declval<DTree&>()));
	std::optional<DJoined> m_tree;
};

class Aggregator_DJTable : public Aggregator_DTable
{
public:
	using Aggregator_DTable::Aggregator_DTable;

protected:
	virtual void SetUp() override
	{
		Aggregator_DTable::SetUp();
		m_table.emplace(Join(*Aggregator_DTable::m_table, 0_layer, 0_layer, *Aggregator_DTable::m_table));

		auto a = Aggregator_DTable::m_table->GetPlaceholder("number").i16();//MakeHashmapはキーの型を特定する必要があるため、
		auto b = Aggregator_DTable::m_table->GetPlaceholder("name").str();//これらのPlaceholderは予め型情報を与えなければならない。
		auto c = Aggregator_DTable::m_table->GetPlaceholder("exam").i08();
		auto hash = *Aggregator_DTable::m_table | Hash(a, b, c);
		[[maybe_unused]] ADAPT_GET_RANKED_PLACEHOLDERS(*m_table, 0, grade, class_, number, name);
		auto exam0 = m_table->GetPlaceholder<0>("exam"_fld);
		[[maybe_unused]] ADAPT_GET_RANKED_PLACEHOLDERS(*m_table, 1, math, japanese, english, science, social);

		m_table->SetKeyJoint<1>(std::move(hash), number, name, exam0);
	}
	using DJoined = decltype(Join(std::declval<DTable&>(), (LayerType)0, (LayerType)0, std::declval<DTable&>()));
	std::optional<DJoined> m_table;
};

inline double MeanMath0(const std::vector<Class>& a)
{
	double res = 0;
	size_t count = 0;
	for (auto& c : a)
	{
		for (auto& s : c.m_students)
		{
			for (auto& r : s.m_records)
			{
				if (r.m_exam != 0) continue;
				res += r.m_math;
				++count;
			}
		}
	}
	return res / (double)count;
}


#define DECL_TREE_PH_SET(tree)\
	[[maybe_unused]] ADAPT_GET_PLACEHOLDERS(tree, grade, class_);\
	[[maybe_unused]] ADAPT_GET_PLACEHOLDERS(tree, number, name);\
	[[maybe_unused]] ADAPT_GET_PLACEHOLDERS(tree, exam, math, japanese, english, science, social);\
	[[maybe_unused]] auto layer0 = std::make_tuple(grade, class_);\
	[[maybe_unused]] auto layer1 = std::make_tuple(number, name);\
	[[maybe_unused]] auto layer2 = std::make_tuple(exam, math, japanese, english, science, social);

#define DECL_PH_SET_DJTree(a, b, c) \
	[[maybe_unused]] ADAPT_GET_RANKED_PLACEHOLDERS(*m_tree, a, grade, class_);\
	[[maybe_unused]] ADAPT_GET_RANKED_PLACEHOLDERS(*m_tree, b, number, name);\
	[[maybe_unused]] ADAPT_GET_RANKED_PLACEHOLDERS(*m_tree, c, exam, math, japanese, english, science, social);\
	[[maybe_unused]] auto layer0 = std::make_tuple(grade, class_);\
	[[maybe_unused]] auto layer1 = std::make_tuple(number, name);\
	[[maybe_unused]] auto layer2 = std::make_tuple(exam, math, japanese, english, science, social);

#define DECL_PH_SET_DTree DECL_TREE_PH_SET(*m_tree)
#define DECL_PH_SET_STree DECL_TREE_PH_SET(*m_tree)

#define DECL_PH_SET_DJTree0 DECL_PH_SET_DJTree(0, 1, 1)
#define DECL_PH_SET_DJTree1 DECL_PH_SET_DJTree(0, 1, 1)
#define DECL_PH_SET_DJTree2 DECL_PH_SET_DJTree(0, 0, 1)
#define DECL_PH_SET_DJTree0_1 DECL_PH_SET_DJTree(0, 1, 2)
#define DECL_PH_SET_DJTree1_2 DECL_PH_SET_DJTree(0, 1, 2)

#define DECL_TABLE_PH_SET(table)\
	[[maybe_unused]] ADAPT_GET_PLACEHOLDERS(table, class_, number, name, exam, math, japanese, english, science, social);\
	[[maybe_unused]] auto layer0 = std::make_tuple(class_, number, name, exam, math, japanese, english, science, social);

#define DECL_PH_SET_DTable DECL_TABLE_PH_SET(*m_table)
#define DECL_PH_SET_STable DECL_TABLE_PH_SET(*m_table)

#define DECL_PH_SET_DJTable \
	[[maybe_unused]] auto [grade, class_, number, name, exam0] = m_table->GetPlaceholders<0>("grade", "class_", "number", "name", "exam");\
	[[maybe_unused]] auto [exam1, math, jpn, eng, sci, soc] = m_table->GetPlaceholders<1>("exam", "math", "japanese", "english", "science", "social");\
	[[maybe_unused]] auto layer0 = std::make_tuple(class_, number, name, exam1, math, jpn, eng, sci, soc);


#endif
