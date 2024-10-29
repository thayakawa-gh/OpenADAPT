#ifndef ADAPT_TEST_COMMON_H
#define ADAPT_TEST_COMMON_H

#include <random>
#include <algorithm>
#include <fstream>
#include <tuple>
#include <string>
#include <optional>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include <gtest/gtest.h>
#include <OpenADAPT/ADAPT.h>

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

class Aggregator : public ::testing::Test
{
	using TopLayer = NamedTuple<Named<"school", std::string>>;
	using Layer0 = NamedTuple<Named<"grade", int8_t>, Named<"class_", int8_t>>;
	using Layer1 = NamedTuple<Named<"number", int16_t>, Named<"name", std::string>, Named<"date_of_birth", int32_t>, Named<"phone", std::string>, Named<"email", std::string>>;
	using Layer2 = NamedTuple<Named<"exam", int8_t>, Named<"math", int32_t>, Named<"japanese", int32_t>, Named<"english", int32_t>, Named<"science", int32_t>, Named<"social", int32_t>>;

	using STree_ = STree<TopLayer, Layer0, Layer1, Layer2>;
	using STable_ = STable<TopLayer, NamedTupleCat_t<Layer0, Layer1, Layer2>>;

	void Generate()
	{
		std::random_device rd;
		std::mt19937_64 mt(rd());

		std::normal_distribution<double> random_dev(60, 20);

		//すでにファイルが存在するのなら何もしない。
		if (std::filesystem::exists("OpenADAPT-records.yaml")) return;

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
	void MakeClass()
	{
		YAML::Node cs = YAML::LoadFile("OpenADAPT-records.yaml");
		m_class.resize(cs.size());
		for (auto [class_, c] : views::Zip(m_class, cs))
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
	}

	template <class Tree>
	static void MakeTree(Tree& t, const std::vector<Class>& cs)
	{
		using enum FieldType;
		if constexpr (d_tree<Tree>)
		{
			t.AddLayer({ { "grade", I08 }, { "class_", I08 } });
			t.AddLayer({ { "number", I16 }, { "name", Str }, { "date_of_birth", I32 },
					     { "phone", Str }, { "email", Str } });
			t.AddLayer({ { "exam", I08 },  {"math", I32}, {"japanese", I32}, {"english", I32}, {"science", I32}, {"social", I32}});
			t.SetTopLayer({ { "school", Str } });
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
				ce.Push(s.m_number, s.m_name, /*s.m_name_furigana,*/ s.m_date_of_birth,
						/*s.m_postal_code, s.m_address,*/ s.m_phone, s.m_email);
				auto&& se = ce.Back();
				se.Reserve((BindexType)s.m_records.size());
				for (auto& r : s.m_records)
				{
					se.Push(r.m_exam, r.m_math, r.m_japanese, r.m_english, r.m_science, r.m_social);
				}
			}
		}
	}
	template <class Table>
	static void MakeTable(Table& t, const std::vector<Class>& cs)
	{
		using enum FieldType;
		if constexpr (d_table<Table>)
		{
			t.SetLayer(0, { { "grade", I08 }, { "class_", I08 },
						 { "number", I16 }, { "name", Str }, { "date_of_birth", I32 },
						 { "phone", Str }, { "email", Str },
						 { "exam", I08 },  {"math", I32}, {"japanese", I32}, {"english", I32}, {"science", I32}, {"social", I32} });
			t.SetTopLayer({ { "school", Str } });
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
						s.m_number, s.m_name, /*s.m_name_furigana,*/ s.m_date_of_birth,
						/*s.m_postal_code, s.m_address,*/ s.m_phone, s.m_email,
						r.m_exam, r.m_math, r.m_japanese, r.m_english, r.m_science, r.m_social);
				}
			}
		}
	}

protected:
	virtual void SetUp() override
	{
		Generate();
		MakeClass();
		MakeTree(m_stree, m_class);
		MakeTree(m_dtree, m_class);
		MakeTable(m_stable, m_class);
		MakeTable(m_dtable, m_class);
	}

	DTree m_dtree;
	STree_ m_stree;
	DTable m_dtable;
	STable_ m_stable;
	std::vector<Class> m_class;
};

inline bool All400(const Student& s)
{
	size_t count = 0;
	for (auto& r : s.m_records) count += (size_t)((int64_t)r.m_math + (int64_t)r.m_japanese + (int64_t)r.m_english + (int64_t)r.m_science + (int64_t)r.m_social >= 400);
	return count == 4;
}
inline double AvgMath(const Student& s)
{
	double math_sum = 0.;
	for (auto& r : s.m_records)
		math_sum += (double)r.m_math;
	return math_sum / 4.;
}
inline double AvgMathInClass(const Class& c)
{
	double math_sum = 0.;
	for (auto& s : c.m_students)
		math_sum += (double)s.m_records[0].m_math;
	return math_sum / (double)c.m_students.size();
}
inline double DevMathInClass(const Class& c)
{
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
inline double DevMath(const Class& c, const Student& s)
{
	double math = (double)s.m_records[0].m_math;
	double avg = AvgMathInClass(c);
	double dev = DevMathInClass(c);
	return (math - avg) * 10 / dev + 50.;
}
inline bool IsBest(const Student& s, BindexType ri)
{
	int64_t max = 0;
	BindexType maxindex = 0;
	for (auto [i, r] : views::Enumerate(s.m_records))
	{
		int64_t sum = (int64_t)r.m_math + (int64_t)r.m_japanese + (int64_t)r.m_english + (int64_t)r.m_science + (int64_t)r.m_social;
		if (max < sum)
		{
			max = sum;
			maxindex = (BindexType)i;
		}
	}
	return maxindex == ri;
}
inline int64_t NumStu200(const Class& c)
{
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
inline int64_t RankMathClass(const Class& c, const Record& r)
{
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
inline int64_t RankMathAll(const std::vector<Class>& a, const Record& r)
{
	int64_t count = 0;
	for (auto& c : a)
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
inline float MeanMax(const Class& c)
{
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
inline int32_t Sum5Subjs(const Record& r)
{
	return r.m_math + r.m_japanese + r.m_english + r.m_science + r.m_social;
}
#endif
