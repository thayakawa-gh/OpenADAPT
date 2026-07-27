#include <gtest/gtest.h>
#include <OpenADAPT/ADAPT.h>

#include <OpenADAPT/FileIO/Json.h>

#ifdef ADAPT_USE_RAPIDJSON

using namespace adapt;

TEST(Random, Json_ParseAndInfer)
{
	const char* json = R"({"company":"A","departments":[{"name":"R&D","employees":[{"id":1,"name":"Alice","salary":1000.0}]}]})";
	rapidjson::Document doc;
	doc.Parse(json);
	ASSERT_FALSE(doc.HasParseError());
	auto schema = adapt::json::InferSchema(doc);
	EXPECT_GE(schema.GetLayerSpec(-1_layer).fields.size(), 1u);
	EXPECT_EQ(schema.GetLayerSpec(-1_layer).fields[0].first, "company");
	EXPECT_EQ(schema.GetLayerSpec(-1_layer).fields[0].second, FieldType::Str);
	EXPECT_EQ(schema.GetLayerSpec(0_layer).fields.size(), 1u);
	EXPECT_EQ(schema.GetLayerSpec(0_layer).fields[0].first, "departments_name");
}
constexpr char json_str[] = R"(
	{
		"company": {
			"name": "",
			"location": "New York"
		},
		"department": [
			{
				"name": "Human resources",
				"division": []
			},
			{
				"name": "Research&development",
				"division": [
					{
						"name": "AI",
						"employee": []
					},
					{
						"name": "Robotics",
						"employee": [
							{
								"id": 4,
								"name": "David",
								"salary": 1500.0
							}
						]
					}
				]
			},
			{
				"name": "Sales",
				"division": [
					{
						"name": "Domestic",
						"employee": [
							{
								"id": 5,
								"name": "Eve",
								"salary": 1100.0
							}
						]
					},
					{
						"name": "International",
						"employee": [
							{
								"id": 6,
								"name": "Frank",
								"salary": 1300.0
							}
						]
					}
				]
			}
		]
	})";
TEST(Random, Json_ParseNestedObject)
{
	rapidjson::Document doc;
	doc.Parse(json_str);
	ASSERT_FALSE(doc.HasParseError());
	auto schema = adapt::json::InferSchema(doc);
	EXPECT_GE(schema.GetLayerSpec(-1_layer).fields.size(), 2u);
	EXPECT_EQ(schema.GetLayerSpec(-1_layer).fields[0].first, "company_name");
	EXPECT_EQ(schema.GetLayerSpec(-1_layer).fields[0].second, FieldType::Str);
	EXPECT_EQ(schema.GetLayerSpec(-1_layer).fields[1].first, "company_location");
	EXPECT_EQ(schema.GetLayerSpec(-1_layer).fields[1].second, FieldType::Str);
}

TEST(Random, Json_ImportDTree)
{
	rapidjson::Document doc;
	doc.Parse(json_str);
	ASSERT_FALSE(doc.HasParseError());
	auto schema = adapt::json::InferSchema(doc);
	auto tree = adapt::json::ImportJson(doc, schema);

	ADAPT_GET_PLACEHOLDERS(tree, company_name, company_location, department_name, division_name, employee_id, employee_name, employee_salary);
	tree | Show("{:>10} {:>10} {:>24}", company_name, company_location, department_name);
	tree | Show("{:>10} {:>10} {:>24} {:>16}", company_name, company_location, department_name, division_name);
	tree | Show("{:>10} {:>10} {:>24} {:>16} {:>3} {:>10} {:>7.1f}", company_name, company_location, department_name, division_name, employee_id, employee_name, employee_salary);
}

TEST(Random, Json_ExportDTree)
{
	rapidjson::Document doc;
	doc.Parse(json_str);
	ASSERT_FALSE(doc.HasParseError());
	auto schema = adapt::json::InferSchema(doc);
	adapt::DTree tree = adapt::json::ImportJson(doc, schema);
	{
		// Import時のschemaを使ってExportする場合のテスト。
		rapidjson::Document exported = adapt::json::ExportJson(tree, schema);

		ASSERT_FALSE(exported.HasParseError());
		ASSERT_TRUE(exported.HasMember("company"));
		ASSERT_TRUE(exported["company"].IsObject());
		EXPECT_STREQ(exported["company"]["location"].GetString(), "New York");
		ASSERT_TRUE(exported.HasMember("department"));
		ASSERT_TRUE(exported["department"].IsArray());
		EXPECT_EQ(exported["department"].Size(), 3u);

		std::ofstream ofs("exported.json");
		rapidjson::OStreamWrapper osw(ofs);
		rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
		exported.Accept(writer);
	}
	{
		// Treeからschemaを推論してExportする場合のテスト。
		rapidjson::Document exported = adapt::json::ExportJson(tree);
		std::ofstream ofs("exported2.json");
		rapidjson::OStreamWrapper osw(ofs);
		rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
		exported.Accept(writer);
	}
	{
		// Treeから推論したschemaを、BindField/BindLayerで加工してExportする場合のテスト。
		adapt::json::Schema schema2 = adapt::json::InferSchema(tree);

		schema2.BindField(-1_layer, "company_name", { "company", "name" });
		schema2.BindField(-1_layer, "company_location", { "company", "location" });

		schema2.BindLayer(0_layer, { "department" });
		schema2.BindField(0_layer, "department_name", { "name" });

		schema2.BindLayer(1_layer, { "division" });
		schema2.BindField(1_layer, "division_name", { "name" });

		schema2.BindLayer(2_layer, { "employee" });
		schema2.BindField(2_layer, "employee_id", { "id" });
		schema2.BindField(2_layer, "employee_name", { "name" });
		schema2.BindField(2_layer, "employee_salary", { "salary" });

		rapidjson::Document exported = adapt::json::ExportJson(tree, schema2);
		std::ofstream ofs("exported3.json");
		rapidjson::OStreamWrapper osw(ofs);
		rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
		exported.Accept(writer);
	}
}
#endif