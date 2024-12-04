#include <format>
#include <iostream>
#include <OpenADAPT/ADAPT.h>
#include <matplot/matplot.h>

using namespace adapt::lit;

adapt::DTable MakeDTable()
{
	// Define a DTable, which determines a tabular structure and fields (like columns) at runtime.
	// The structure of DTable is defined by calling SetTopLayer and SetLayer methods.
	// It is almost the same as DTree with layer -1 and 0, but the performance is better than DTree.

	// DTable supports the following types:
	// Integer:         I08 (int8_t), I16 (int16_t), I32 (int32_t), I64 (int64_t)
	// Floating point:  F32 (float), F64 (double)
	// Complex numbers: C32 (std::complex<float>), C64 (std::complex<double>)
	// String:          Str (std::string)
	// Index:           Jbp (adapt::JBpos) ... Something like an index of each layer.
	// std::chrono::year_month_day and others will be added in the near future.

	using enum adapt::FieldType;
	adapt::DTable t;
	t.SetTopLayer({ { "nation", Str }, { "capital", Str } });
	t.SetLayer(0, { { "state", Str }, { "county", Str }, { "city", Str }, { "city_population", I32 }, { "city_area", F64 } });
	t.VerifyStructure();

	t.ShowHierarchy();
	return t;
}

void StoreData(adapt::DTable& usa)
{
	// Store data in the DTable using the following methods:
	// Reserve(size)            ... Increase the capacity of elements in the lower layer, like std::vector::reserve.
	// Resize(size)             ... Change the number of elements in the lower layer, like std::vector::resize.
	// Push(fields...)          ... Add an element to the end of the lower layer elements, like std::vector::push_back.
	// Pop()                    ... Remove the last lower layer elements, like std::vector::pop_back.
	// Assign(fields...)        ... Assign fields, like an assignment operator.
	// Insert(index, fields...) ... Insert an element at the specified index, like std::vector::insert.
	// Erase(index)             ... Erase an element at the specified index, like std::vector::erase.

	usa.SetTopFields("USA", "Washington D.C.");
	usa.Reserve(12);
	usa.Push("California", "Los Angeles County", "Los Angeles", 3976322, 1213.9);
	usa.Push("California", "Los Angeles County", "Long Beach", 466742, 130.6);
	usa.Push("California", "Los Angeles County", "Glendale", 196543, 79.3);
	usa.Push("California", "San Diego County", "San Diego", 1386932, 842.2);
	usa.Push("California", "San Diego County", "Chula Vista", 275487, 126.6);
	usa.Push("California", "San Diego County", "Oceanside", 174068, 105.1);
	usa.Push("Texas", "Harris County", "Houston", 2304580, 1500.7);
	usa.Push("Texas", "Harris County", "Pasadena", 151950, 114.4);
	usa.Push("Texas", "Harris County", "Baytown", 83701, 32.7);
	usa.Push("Texas", "Dallas County", "Dallas", 1304379, 887.2);
	usa.Push("Texas", "Dallas County", "Irving", 256684, 174.1);
	usa.Push("Texas", "Dallas County", "Garland", 246918, 147.9);

	// For simplicity, other states, counties, and cities are omitted.
}

void QuickstartDTable()
{
	// Simple example of how to use DTable.
	// There are few differences between DTable and DTree in terms of how to use them.

	std::cout << "------Create------" << std::endl;
	adapt::DTable usa = MakeDTable();



	std::cout << "------Store data------" << std::endl;
	StoreData(usa);



	std::cout << "------Placeholders------" << std::endl;

	// Placeholders are used to access/calculate data stored in DTable.
	// The default return value of GetPlaceholders for DTable is DTable::RttiPlaceholder, which has runtime type and layer information.
	// RttiPlaceholder can be converted to TypedPlaceholder using i32(), str(), etc. methods, which has compile-time type information.
	// The performance of the TypedPlaceholder is better than RttiPlaceholder, especially in the use of lambda functions.
	auto [nation, state, county, city, city_population, city_area] =
		usa.GetPlaceholders("nation", "state", "county", "city", "city_population", "city_area");

	// DTable only contains layer -1 or 0 elements, so all placeholders have layer -1 or 0.
	assert(        nation.GetLayer() == -1_layer &&          nation.GetType() == adapt::FieldType::Str);
	assert(          state.GetLayer() == 0_layer &&           state.GetType() == adapt::FieldType::Str);
	assert(        county.GetLayer() == 0_layer &&         county.GetType() == adapt::FieldType::Str);
	assert(           city.GetLayer() == 0_layer &&            city.GetType() == adapt::FieldType::Str);
	assert(city_population.GetLayer() == 0_layer && city_population.GetType() == adapt::FieldType::I32);
	assert(      city_area.GetLayer() == 0_layer &&       city_area.GetType() == adapt::FieldType::F64);



	std::cout << "------Access------" << std::endl;

	auto long_beach = usa[1];// reference to the second element, which is Long Beach.
	std::cout << long_beach[city].str() << std::endl;// Long Beach

	adapt::Bpos baytown_index = { 8 };
	auto baytown = usa.GetBranch(baytown_index);
	std::cout << baytown[city_population].i32() << std::endl;// 83701



	std::cout << "------Calculate------" << std::endl;

	// Lambda functions are made from placeholders.
	// They can be used when you want to calculate something from data and filter/convert data like <ranges> library.

	using Lambda = adapt::eval::RttiFuncNode<adapt::DTable>;
	//Make a lambda function to calculate population density of a city.
	Lambda population_density = city_population / city_area;
	assert(population_density.GetLayer() == 0_layer && population_density.GetType() == adapt::FieldType::F64);
	adapt::Bpos oceanside_index = { 5 };
	std::cout << population_density(usa, oceanside_index).f64() << std::endl;// 174068 / 105.1 = 1655.3

	// Get the index of the city with the smallest population.
	Lambda index_to_smallest_city_population = index(isleast(city_population));
	assert(index_to_smallest_city_population.GetLayer() == -1_layer && index_to_smallest_city_population.GetType() == adapt::FieldType::I64);
	std::cout << index_to_smallest_city_population(usa).i64() << std::endl;// The index of Baytown, which has the smallest population, is 8.

	// Get the name of the city with the smallest population.
	Lambda smallest_city = city.at(index(isleast(city_population)));
	assert(smallest_city.GetLayer() == -1_layer && smallest_city.GetType() == adapt::FieldType::Str);
	// If the layer of the lambda function is -1, the lambda function can be called without a Bpos.
	std::cout << smallest_city(usa).str() << std::endl;// Baytown

	// Various operators and functions can be used in combination.
	// Take the average of the population density of all cities in the usa, and then convert /km^2 to /mi^2.
	Lambda average_population_density = mean(city_population / city_area) * 2.589988;
	assert(average_population_density.GetLayer() == -1_layer && average_population_density.GetType() == adapt::FieldType::F64);
	std::cout << average_population_density(usa).f64() << "(/mi^2)" << std::endl;

	// Convert RttiPlaceholder to TypedPlaceholder.
	auto city_population_typed = city_population.i32();
	auto city_area_typed = city_area.f64();
	// If all the placeholers are Typed (may include Ctti), the resulting lambda function is also Typed.
	// If the lambda function components include at least one Rtti placeholder, the resulting lambda function is Rtti.
	auto average_population_density_typed = mean(city_population_typed / city_area_typed) * 2.589988;
	// average_population_density_typed has compile-time type information, but runtime layer information.
	static_assert(std::same_as<typename decltype(average_population_density_typed)::RetType, double>);
	std::cout << average_population_density_typed(usa) << "(/mi^2)" << std::endl;// No need to call f64() since the lambda function knows its own type.

	// The functions and operators available in the lambda functions are defined
	// in <OpenADAPT/Evaluator/Function.h> and <OpenADAPT/Evaluator/LayerFuncNode.h>

	std::cout << std::endl;



	std::cout << "------Traverse------" << std::endl;

	// Use traversers to iterate over the data.
	for (const auto& trav : usa.GetRange(0_layer))// DTable has only one layer, so the layer must be 0.
	{
		// All the names of the states, counties and cities are output.
		std::cout << std::format("{:<12} {:<12} {:<12}\n", trav[state].str(), trav[county].str(), trav[city].str());
	}

	// Lambda functions can also be used to filter data.
	// You can omit GetRange or place after Filter, like
	//  * usa | Filter(...)
	//  * usa | Filter(...) | GetRange(0_layer)
	for (const auto& trav : usa.GetRange(0_layer) | Filter(state == "Texas"))
	{
		// All the names, populations and areas of the cities in Texas are output.
		std::cout << std::format("{:<12} {:>8} {:>7.1f}\n", trav[city].str(), trav[city_population].i32(), trav[city_area].f64());
	}
	std::cout << std::endl;

	// Use lambda function with traverser.
	// Count the number of cities with a larger population than the current city.
	Lambda population_rank = count(city_population > city_population.o(0_depth)) + 1;
	for (const auto& trav : usa.GetRange(0_layer))
	{
		std::cout << std::format("{:<12} {:>8} {:>7.1f} {:>2}\n", trav[city].str(), trav[city_population].i32(), trav[city_area].f64(), population_rank(trav).i64());
	}
	std::cout << std::endl;

	// Evaluate range adapter converts the return value of operator* from Traverser object to results of the lambda functions.
	for (auto [city_, population_density_] : usa | Filter(isgreatest(city_population / city_area))
		| Evaluate(city.str(), (city_population / city_area).f64()))
	{
		// The name and population density of the city with the largest population density is output.
		std::cout << std::format("{:<12} {:>7.1f}\n", city_, population_density_);
	}
	std::cout << std::endl;



	std::cout << "------Show data------" << std::endl;

	// Show the data of the city with the smallest area.
	usa | Filter(isleast(city_area)) | Show(state, city, city_population, tostr(city_area) + "km^2");
	// [   8]            Texas          Baytown       83701    32.700000km^2

	std::cout << std::endl;



	std::cout << "------Extract------" << std::endl;

	// Create a new DTree by extracting the data from the original DTable.
	// Note that the result of the extract in the current version is a DTree, not a DTable.
	// The extracted DTree only has fields specified in Extract range adapter. 
	// If the names are not specified, default names: "fld0", "fld1", ... are used.
	adapt::DTree extracted = usa | Filter(city_area > 500.)
		| Extract(state.named("state"), county.named("county"), city.named("city"), (city_population / city_area).named("population_density"));
	extracted.ShowHierarchy();
	// Layer[-1] { }
	// Layer[ 0] { { "state", Str } { "county", Str } { "city", Str } { "population_density", F64 } }

	auto [estate, ecounty, ecity, epopulation_density] = extracted.GetPlaceholders("state", "county", "city", "population_density");
	for (const auto& trav : extracted.GetRange(0_layer))
	{
		std::cout << std::format("{:<12} {:<20} {:<12} {:>7.1f}\n",
			trav[estate].str(), trav[ecounty].str(), trav[ecity].str(), trav[epopulation_density].f64());
	}

	std::cout << std::endl;



	std::cout << "------Plot data with Matplot++------" << std::endl;

	// Plot area vs population of the cities in California.
	// Matplot++ requires numerical data to be in std::vector<double>, and text data in std::vector<std::string>,
	// so convert the data to std::vector using ToVector.
	auto [vcity, vpopulation, varea] = usa | Filter(state == "California") | ToVector(("<-" + city).str(), cast_f64(city_population).f64(), city_area.f64());
	auto fig = matplot::figure(true);
	auto ax = fig->current_axes();
	ax->scatter(varea, vpopulation);
	ax->xlabel("Area (km^2)");
	ax->ylabel("Population");
	ax->title("Area vs Population of Cities in California");
	ax->text(varea, vpopulation, vcity);
	ax->x_axis().scale(matplot::axis_type::axis_scale::log);
	ax->y_axis().scale(matplot::axis_type::axis_scale::log);
	fig->save("examples_en_dtable.png");
}