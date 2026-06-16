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
	EXPECT_GE(schema.TopFields().size(), 1u);
	EXPECT_FALSE(schema.Layers().empty());
	EXPECT_EQ(schema.TopFields()[0].first, "company");
	EXPECT_EQ(schema.TopFields()[0].second, FieldType::Str);
	EXPECT_EQ(schema.Layers()[0].fields.size(), 1u);
	EXPECT_EQ(schema.Layers()[0].fields[0].first, "departments_name");
}
constexpr char json_str[] = R"(
	{
		"company": {
			"name": "A",
			"location": "NY"
		},
		"departments": [
			{
				"name": "HR",
				"employees": []
			},
			{
				"name": "R&D",
				"employees": [
					{
						"id": 1,
						"name": "Alice",
						"salary": 1000.0
					},
					{
						"id": 3,
						"name": "Charlie",
						"salary": 1200.0
					}
				]
			},
			{
				"name": "Sales",
				"employees": [
					{
						"id": 2,
						"name": "Bob",
						"salary": 800.0
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
	EXPECT_GE(schema.TopFields().size(), 2u);
	EXPECT_FALSE(schema.Layers().empty());
	EXPECT_EQ(schema.TopFields()[0].first, "company_name");
	EXPECT_EQ(schema.TopFields()[0].second, FieldType::Str);
	EXPECT_EQ(schema.TopFields()[1].first, "company_location");
	EXPECT_EQ(schema.TopFields()[1].second, FieldType::Str);
}

TEST(Random, Json_ImportDTree)
{
	rapidjson::Document doc;
	doc.Parse(json_str);
	ASSERT_FALSE(doc.HasParseError());
	auto schema = adapt::json::InferSchema(doc);
	//auto tree = adapt::json::ImportDTree(doc, schema);

}
#endif