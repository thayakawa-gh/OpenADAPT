#include <format>
#include <iostream>
#include <OpenADAPT/ADAPT.h>
#include <matplot/matplot.h>

using namespace adapt::lit;

adapt::DTree MakeDTree()
{
	// Define a DTree, which determines a hierarchical structure and fields (like columns) at runtime.
	// The structure of DTree is defined by calling SetTopLayer and AddLayer methods.
	// It is similar to a nested structure of Fields + std::vector<Fields + std::vector<Fields + std::vector<Fields>>>;
	// Fields of each layer and a vector of lower-layer elements.

	// DTree supports the following types:
	// Integer:         I08 (int8_t), I16 (int16_t), I32 (int32_t), I64 (int64_t)
	// Floating point:  F32 (float), F64 (double)
	// Complex numbers: C32 (std::complex<float>), C64 (std::complex<double>)
	// String:          Str (std::string)
	// Index:           Jbp (adapt::JBpos) ... Something like an index of each layer.
	// std::chrono::year_month_day and others will be added in the near future.

	using enum adapt::FieldType;
	adapt::DTree t;
	t.SetTopLayer({ { "nation", Str }, { "capital", Str } });///Set root layer (-1).
	t.AddLayer({ { "state", Str }, { "state capital", Str } });//Add layer 0.
	t.AddLayer({ { "county", Str }, { "county seat", Str }});//Add layer 1.
	t.AddLayer({ { "city", Str }, { "population", I32 }, { "area", F64 } });//Add layer 2.
	t.VerifyStructure();// Verify if the structure is correct.

	t.ShowHierarchy();
	return t;
}

void StoreData(adapt::DTree& usa)
{
	// Store data in the DTree using various methods:
	// Reserve(size)            ... Increase the capacity of elements in the lower layer, like std::vector::reserve.
	// Resize(size)             ... Change the number of elements in the lower layer, like std::vector::resize.
	// Push(fields...)          ... Add an element to the end of the lower layer elements, like std::vector::push_back.
	// Pop()                    ... Remove the last lower layer elements, like std::vector::pop_back.
	// Assign(fields...)        ... Assign fields, like an assignment operator.
	// Insert(index, fields...) ... Insert an element at the specified index, like std::vector::insert.
	// Erase(index)             ... Erase an element at the specified index, like std::vector::erase.

	usa.SetTopFields("USA", "Washington D.C.");// Set top layer elements.
	usa.Reserve(2);// Reserve elements for layer 0.
	usa.Push("California", "Sacramento");// Push an element to layer 0.
	usa.Push("Texas", "Austin");

	auto california = usa[0];// Reference layer 0 element.
	california.Reserve(2);// Reserve elements for layer 1.
	california.Push("Los Angeles County", "Los Angeles");
	california.Push("San Diego County", "San Diego");

	auto los_angeles_county = california[0];
	los_angeles_county.Reserve(3);
	los_angeles_county.Push("Los Angeles", 3976322, 1213.9);// The unit of area is km^2.
	los_angeles_county.Push("Long Beach", 466742, 130.6);
	los_angeles_county.Push("Glendale", 196543, 79.3);

	auto san_diego_county = california[1];
	san_diego_county.Reserve(3);
	san_diego_county.Push("San Diego", 1386932, 842.2);
	san_diego_county.Push("Chula Vista", 275487, 126.6);
	san_diego_county.Push("Oceanside", 174068, 105.1);

	auto texas = usa[1];
	texas.Reserve(2);
	texas.Push("Harris County", "Houston");
	texas.Push("Dallas County", "Dallas");

	auto harris_county = texas[0];
	harris_county.Reserve(3);
	harris_county.Push("Houston", 2304580, 1500.7);
	harris_county.Push("Pasadena", 151950, 114.4);
	harris_county.Push("Baytown", 83701, 32.7);

	auto dallas_county = texas[1];
	dallas_county.Reserve(3);
	dallas_county.Push("Dallas", 1304379, 887.2);
	dallas_county.Push("Irving", 256684, 174.1);
	dallas_county.Push("Garland", 246918, 147.9);

	// For simplicity, other states, counties, and cities are omitted.
	// All the data are cited from Wikipedia.
}


void QuickstartDTree()
{
	// Simple example of how to use DTree.

	std::cout <<  "------Create------" << std::endl;
	adapt::DTree usa = MakeDTree();



	std::cout << "------Store data------" << std::endl;
	StoreData(usa);



	std::cout << "------Placeholders------" << std::endl;

	// Placeholders are used to access/calculate data stored in containers.
	// The default return value of GetPlaceholders for DTree is DTree::RttiPlaceholder, which has runtime type and layer information.
	// RttiPlaceholder can be converted to TypedPlaceholder using i32(), str(), etc. methods, which has compile-time type information.
	// The performance of the TypedPlaceholder is better than RttiPlaceholder, especially in the use of lambda functions.
	auto [nation, state, state_capital, county, county_seat, city, population, area] =
		usa.GetPlaceholders("nation", "state", "state capital", "county", "county seat", "city", "population", "area");
	assert(       nation.GetLayer() == -1_layer &&        nation.GetType() == adapt::FieldType::Str);
	assert(        state.GetLayer() ==  0_layer &&         state.GetType() == adapt::FieldType::Str);
	assert(state_capital.GetLayer() ==  0_layer && state_capital.GetType() == adapt::FieldType::Str);
	assert(      county.GetLayer() ==  1_layer &&       county.GetType() == adapt::FieldType::Str);
	assert( county_seat.GetLayer() ==  1_layer &&  county_seat.GetType() == adapt::FieldType::Str);
	assert(         city.GetLayer() ==  2_layer &&          city.GetType() == adapt::FieldType::Str);
	assert(   population.GetLayer() ==  2_layer &&    population.GetType() == adapt::FieldType::I32);
	assert(         area.GetLayer() ==  2_layer &&          area.GetType() == adapt::FieldType::F64);



	std::cout << "------Access------" << std::endl;

	// Access data stored in DTree, using the [] operator.
	auto california = usa[0];
	std::cout << california[state_capital].str() << std::endl;// Sacramento
	// You can also use str_unsafe instead of str, which is faster but has no type check.

	auto san_diego = california[1][0];
	std::cout << san_diego[population].i32() << std::endl;// 1386932

	// Access data using adapt::Bpos. Bpos is a list of indices for each layer.
	// Note that adapt::Bpos allocates an array of indices dynamically if it has more than 3 indices.
	adapt::Bpos harris_index = { 1, 0 };
	auto harris = usa.GetBranch(harris_index);
	std::cout << harris[county].str() << std::endl;// Harris County

	std::cout << std::endl;



	std::cout << "------Calculate------" << std::endl;

	// Lambda functions are made from placeholders.
	// They can be used when you want to calculate something from data and filter/convert data like <ranges> library.

	using Lambda = adapt::eval::RttiFuncNode<adapt::DTree>;

	// Make a lambda function to calculate population density of a city.
	Lambda population_density = population / area;
	// Lambda functions also have their types and layers.
	// This lambda function has layer 2, because it can be calculated from a city (layer 2 element).
	assert(population_density.GetLayer() == 2_layer && population_density.GetType() == adapt::FieldType::F64);
	adapt::Bpos baytown_index = { 1, 0, 2 };
	std::cout << population_density(usa, baytown_index).f64() << std::endl;// 83701 / 32.7 = 2559.6

	// Make a lambda function to calculate total population in a county.
	// As the result of "sum", the layer of the lambda function is raised by 1,
	// because the sum can be calculated from a county (layer 1 element).
	Lambda total_population_in_a_county = sum(population);
	assert(total_population_in_a_county.GetLayer() == 1_layer && total_population_in_a_county.GetType() == adapt::FieldType::I32);
	std::cout << total_population_in_a_county(usa, harris_index).i32() << std::endl;// 2304580 + 151950 + 83701 = 2540231

	// Make a lambda function to calculate total population in a state.
	// In general, the sumN and the other similar functions raise the layer by N, except the case of outer field access.
	Lambda total_population_in_a_state = sum2(population);
	assert(total_population_in_a_state.GetLayer() == 0_layer && total_population_in_a_state.GetType() == adapt::FieldType::I32);
	adapt::Bpos california_index = { 0 };
	std::cout << total_population_in_a_state(usa, california_index).i32() << std::endl;// 10824306


	Lambda total_population_in_a_nation = sum3(population);
	assert(total_population_in_a_nation.GetLayer() == -1_layer && total_population_in_a_nation.GetType() == adapt::FieldType::I32);
	// If the layer of a lambda function is -1, the lambda function can be called without a Bpos,
	// because there is only one layer -1 element, usa.
	std::cout << total_population_in_a_nation(usa).i32() << std::endl;// 6476094 + 1304379 + 256684 + 246918 = 8286975

	// Various operators and functions can be used in combination.
	// Take the average of the population density of all cities in the usa, and then convert /km^2 to /mi^2.
	Lambda average_population_density = mean3(population / area) * 2.589988;
	assert(average_population_density.GetLayer() == -1_layer && average_population_density.GetType() == adapt::FieldType::F64);
	std::cout << average_population_density(usa).f64() << "(/mi^2)" << std::endl;

	// Convert RttiPlaceholder to TypedPlaceholder.
	auto population_typed = population.i32();
	auto area_typed = area.f64();
	// If all the placeholers are Typed (may include Ctti), the resulting lambda function is also Typed.
	// If the lambda function components include at least one Rtti placeholder, the resulting lambda function is Rtti.
	auto average_population_density_typed = mean3(population_typed / area_typed) * 2.589988;
	// average_population_density_typed has compile-time type information, but runtime layer information.
	static_assert(std::same_as<typename decltype(average_population_density_typed)::RetType, double>);
	std::cout << average_population_density_typed(usa) << "(/mi^2)" << std::endl;// No need to call f64() since the lambda function knows its own type.

	// The functions and operators available in the lambda functions are defined
	// in <OpenADAPT/Evaluator/Function.h> and <OpenADAPT/Evaluator/LayerFuncNode.h>

	std::cout << std::endl;



	std::cout << "------Traverse------" << std::endl;

	// Use traversers to iterate over the data.
	for (const auto& trav : usa.GetRange(1_layer))// Traverse layer 1.
	{
		// All the names of states, counties and county seats are output.
		// During traversal of layer 1, you can access the fields from layer -1 to 1.
		std::cout << std::format("{:<12} {:<12} {:<12}\n", trav[state].str(), trav[county].str(), trav[county_seat].str());
	}
	std::cout << std::endl;

	// Lambda functions can also be used to filter the data.
	// You can omit GetRange or place after Filter, like
	//  * usa | Filter(...)
	//     usa is automatically converted to a range when being passed to Filter.
	//     The traversal layer matches the bottom layer of the lambda functions in Filter.
	//  * usa | Filter(...) | GetRange(2_layer)
	//     usa is automatically converted to a range.
	//     Then traversal layer is lowered to layer 2 if the bottom layer of the lambda functions is higher than 2.
	for (const auto& trav : usa.GetRange(2_layer) | Filter(city == county_seat))
	{
		// All the names, populations and areas of the cities that are the county seats are output.
		std::cout << std::format("{:<12} {:>8} {:>7.1f}\n", trav[city].str(), trav[population].i32(), trav[area].f64());
	}
	std::cout << std::endl;

	// Use lambda functions with traverser.
	// count the number of cities with a larger population than the current city.
	Lambda population_rank = count3(population > population.o(0_depth)) + 1;
	for (const auto& trav : usa.GetRange(2_layer))
	{
		// Calculate the population ranking.
		std::cout << std::format("{:<12} {:>8} {:>2}\n", trav[city].str(), trav[population].i32(), population_rank(trav).i64());
	}
	std::cout << std::endl;

	// Evaluate range adapter converts the return value of operator* from Traverser object to results of the lambda functions.
	for (auto [county_, population_density_] : usa | Filter(isgreatest(sum(population) / sum(area)))
													| Evaluate(county.str(), (sum(population) / sum(area)).f64()))
	{
		// All the names and population densities of the counties with the largest population densities in the states are output.
		std::cout << std::format("{:<12} {:>7.1f}\n", county_, population_density_);
	}

	std::cout << std::endl;



	std::cout << "------Show data------" << std::endl;

	// Show the data of the cities with the smallest areas in the states.
	usa | Filter(isleast2(area)) | Show(state, city, population, tostr(area) + "km^2");
	// [   0,   0,   2]       California         Glendale      196543    79.300000km^2
	// [   1,   0,   2]            Texas          Baytown       83701    32.700000km^2

	std::cout << std::endl;



	std::cout << "------Extract------" << std::endl;

	// Create a new DTree by extracting the data from the original DTree.
	// The arguments of Extract functions become the fields of the new DTree,
	// and the hierarchical structure is automatically determined by the arguments.
	// If the names are not specified, default names: "fld0", "fld1", ... are used.
	adapt::DTree extracted = usa | Filter(area > 500.)
		| Extract(state.named("state"), county.named("county"), city.named("city"), (population / area).named("population_density"));
	extracted.ShowHierarchy();
	// Layer[-1] { }
	// Layer[ 0] { { "state", Str } }
	// Layer[ 1] { { "county", Str } }
	// Layer[ 2] { { "city", Str } { "population_density", F64 } }

	auto [estate, ecounty, ecity, epopulation_density] = extracted.GetPlaceholders("state", "county", "city", "population_density");
	for (const auto& trav : extracted.GetRange(2_layer))
	{
		std::cout << std::format("{:<12} {:<20} {:<12} {:>7.1f}\n",
			trav[estate].str(), trav[ecounty].str(), trav[ecity].str(), trav[epopulation_density].f64());
	}

	std::cout << std::endl;



	std::cout << "------Plot data with Matplot++------" << std::endl;

	// Plot area vs population of the cities in California.
	// Matplot++ requires numerical data to be in std::vector<double>, and text data in std::vector<std::string>,
	// so convert the data to std::vector using ToVector.
	auto [vcity, vpopulation, varea] = usa | Filter(state == "California") | ToVector(("<-" + city).str(), cast_f64(population).f64(), area.f64());
	auto fig = matplot::figure(true);
	auto ax = fig->current_axes();
	ax->scatter(varea, vpopulation);
	ax->xlabel("Area (km^2)");
	ax->ylabel("Population");
	ax->title("Area vs Population of Cities in California");
	ax->text(varea, vpopulation, vcity);
	ax->x_axis().scale(matplot::axis_type::axis_scale::log);
	ax->y_axis().scale(matplot::axis_type::axis_scale::log);
	fig->save("examples_en_dtree.png");
}