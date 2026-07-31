#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cassert>
#include <cmath>
#include <complex>
#include <ranges>
#ifdef ADAPT_IMPORT_MODULE
#include <OpenADAPT/Macros.h>
import adapt;
#else
#include <OpenADAPT/ADAPT.h>
#ifdef ADAPT_USE_RAPIDJSON
#include <OpenADAPT/FileIO/Json.h>
#endif
#endif

using namespace adapt::lit;

void QuickstartJson()
{
	std::cout << "[[Quickstart JSON file I/O]]" << std::endl;
	#ifndef ADAPT_USE_RAPIDJSON
	std::cout << "This example requires RapidJSON and ADAPT_USE_RAPIDJSON to be defined." << std::endl;
	return;
	#else

	// In many cases, JSON data comes from a file.
	// For this quickstart, we first create a small sample file so that the example is self-contained.
	const std::string input_path = "quickstart_input.json";
	const std::string output_path = "quickstart_output.json";
	const std::string remapped_output_path = "quickstart_output_remapped.json";
	{
		std::ofstream ofs(input_path);
		ofs << R"({
			"company": {
				"name": "OpenADAPT Inc.",
				"location": "Tokyo"
			},
			"department": [
				{
					"name": "Research",
					"employee": [
						{ "id": 1, "name": "Alice", "salary": 1200.0 },
						{ "id": 2, "name": "Bob",   "salary": 1350.0 }
					]
				},
				{
					"name": "Sales",
					"employee": [
						{ "id": 3, "name": "Carol", "salary": 1100.0 }
					]
				}
			]
		})";
	}

	std::cout << "------Read and parse JSON from a file------" << std::endl;
	// ADAPT uses RapidJSON for JSON parsing and writing.
	std::ifstream ifs(input_path);
	rapidjson::IStreamWrapper isw(ifs);
	rapidjson::Document doc;
	doc.ParseStream(isw);
	if (doc.HasParseError())
		throw adapt::InvalidArg("Failed to parse quickstart_input.json.");

	std::cout << "------Import JSON into a DTree------" << std::endl;
	// InferSchema(doc) inspects the JSON structure and builds a schema from it.
	// The inferred schema records not only field types but also JSON-side routes such as
	// where each lower layer is found and which JSON path each field belongs to.
	// The current importer is intended for object trees whose lower layers are arrays of objects.
	// Scalar arrays are not imported as DTree layers.
	adapt::json::Schema schema = adapt::json::InferSchema(doc);
	std::cout << std::endl;

	adapt::DTree tree = adapt::json::ImportJson(doc, schema);
	tree.ShowHierarchy();
	// Output:
	// Layer[-1] { { "company_name", Str } { "company_location", Str } }
	// Layer[ 0] { { "department_name", Str } }
	// Layer[ 1] { { "employee_id", I64 } { "employee_name", Str } { "employee_salary", F64 } }
	ADAPT_GET_PLACEHOLDERS(tree, company_name, company_location, department_name, employee_id, employee_name, employee_salary);
	tree | adapt::Show("{:>16} {:>10} {:>10} {:>3} {:>10} {:>7.1f}",
		company_name, company_location, department_name, employee_id, employee_name, employee_salary);
	// Output:
	//	[   0,   0]  OpenADAPT Inc.      Tokyo   Research   1      Alice  1200.0
	//	[   0,   1]  OpenADAPT Inc.      Tokyo   Research   2        Bob  1350.0
	//	[   1,   0]  OpenADAPT Inc.      Tokyo      Sales   3      Carol  1100.0
	std::cout << std::endl;

	std::cout << "------Export the DTree back to JSON------" << std::endl;
	// ExportJson(tree) first calls InferSchema(tree), then exports with that schema.
	// InferSchema(tree) only sees the DTree structure itself, so it reconstructs a default JSON layout
	// from layer names and field names. It does not preserve the original JSON-side routes inferred from doc.
	// If the original import schema matters, pass it explicitly as ExportJson(tree, schema).
	rapidjson::Document exported = adapt::json::ExportJson(tree);
	{
		std::ofstream ofs(output_path);
		rapidjson::OStreamWrapper osw(ofs);
		rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
		exported.Accept(writer);
	}
	std::cout << std::format("Saved the exported JSON to {}", output_path) << std::endl;
	std::cout << std::endl;

	std::cout << "------Export with a remapped JSON structure------" << std::endl;
	// If you want to change the exported JSON structure, edit the schema before calling ExportJson.
	// BindField(layer, field_name, path) changes where each field is written in one JSON object.
	// BindLayer(layer, route) changes where each lower layer is written.
	// InferSchema(tree) is convenient here because it starts from the current DTree structure,
	// then you can overwrite only the JSON mapping rules that you want to customize.
	// Or you can reuse the original schema inferred from the input JSON, then overwrite only the mapping rules you want to change.
	// ExportJson(tree, schema) uses the supplied schema as-is, so the output shape follows
	// that schema's BindLayer / BindField settings.
	// In this example:
	//   - layer 0 is written under "teams"
	//   - layer 1 is written under "members"
	//   - department_name is written as "name" in the team object.
	//   - employee_id and employee_name are nested under "profile", while employee_salary is at the top level of the member object.
	// Please see the resulting quickstart_output_remapped.json for the output shape.
	adapt::json::Schema remapped_schema = adapt::json::InferSchema(tree);
	remapped_schema.BindLayer(0_layer, { "teams" });
	remapped_schema.BindLayer(1_layer, { "members" });
	remapped_schema.BindField(0_layer, "department_name", { "name" });
	remapped_schema.BindField(1_layer, "employee_id", { "profile", "id" });
	remapped_schema.BindField(1_layer, "employee_name", { "profile", "name" });
	remapped_schema.BindField(1_layer, "employee_salary", { "salary" });
	rapidjson::Document remapped = adapt::json::ExportJson(tree, remapped_schema);
	{
		std::ofstream ofs(remapped_output_path);
		rapidjson::OStreamWrapper osw(ofs);
		rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
		remapped.Accept(writer);
	}
	std::cout << std::format("Saved the remapped JSON to {}", remapped_output_path) << std::endl;
	std::cout << std::endl;

	// This workflow is useful when you want to load a nested JSON file,
	// inspect or process it with OpenADAPT, and write the result back as JSON.
	#endif
}