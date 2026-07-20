#include <gtest/gtest.h>
#include <OpenADAPT/ADAPT.h>

#include <OpenADAPT/FileIO/Json.h>

#if ADAPT_JSON_HAS_RAPIDJSON == 1

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
	auto tree = adapt::json::ImportDTree(doc, schema);

	ADAPT_GET_PLACEHOLDERS(tree, company_name, company_location, department_name, division_name, employee_id, employee_name, employee_salary);
	tree | Show("{:>10} {:>10} {:>24}", company_name, company_location, department_name);
	tree | Show("{:>10} {:>10} {:>24} {:>16}", company_name, company_location, department_name, division_name);
	tree | Show("{:>10} {:>10} {:>24} {:>16} {:>3} {:>10} {:>7.1f}", company_name, company_location, department_name, division_name, employee_id, employee_name, employee_salary);
}
#endif