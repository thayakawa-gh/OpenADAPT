#include <format>
#include <iostream>
#include <OpenADAPT/ADAPT.h>
#include <matplot/matplot.h>

using namespace adapt::lit;

auto MakeSTable()
{
	// Define a STable, which determines a tabular structure and fields (like columns) at compile-time using templates.
	// The structure of STable is defined by adapt::NamedTuple and adapt::Named.
	// As a matter of fact, STable has completely the same functionality as STree with layer -1 and 0, except for the type name.

	// STree supports any types of fields, as long as they are default constructible.

	using adapt::NamedTuple;
	using adapt::Named;
	using TopLayer = NamedTuple<Named<"nation", std::string>, Named<"capital", std::string>>;
	using Layer0 = NamedTuple<Named<"state", std::string>, Named<"county", std::string>,
		Named<"city", std::string>, Named<"city_population", int32_t>, Named<"city_area", double>>;
	using STable_ = adapt::STree<TopLayer, Layer0>;
	STable_ t;
	t.ShowHierarchy();
	return t;
}

template <adapt::s_table STable_>
void StoreData(STable_& usa)
{
	// Store data in the STable using the following methods:
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
}

void QuickstartSTable()
{
	// Simple example of how to use STable.
	// There are few differences between STable and STree in terms of how to use them.

	std::cout << "------Create------" << std::endl;
	auto usa = MakeSTable();
	using STable_ = decltype(usa);



	std::cout << "------Store data------" << std::endl;
	StoreData(usa);



	std::cout << "------Placeholders------" << std::endl;

	// Placeholders are used to access/calculate data stored in STable.
	// Node that you should use _fld literal to get STable::CttiPlaceholder<Type, Layer> that has compile-time type/layer informations.
	// If no _fld literal name is used, the return type is STable::RttiPlaceholder that has runtime type/layer information.
	// RttiPlaceholder can also be used for STable, but its performance is lower than CttiPlaceholder, especially in the use of lambda functions.
	ADAPT_GET_PLACEHOLDERS(usa, nation, state, county, city, city_population, city_area);

	// ADAPT_GET_PLACEHOLDERS is a macro that expands to the following code:
	// auto [nation, state, county, city, city_population, city_area] =
	//	usa.GetPlaceholders("nation"_fld, "state"_fld, "county"_fld, "city"_fld, "city_population"_fld, "city_area"_fld);

	// Confirm the type and layer of CttiPlaceholder are correct.
	{
		using type_city_population = std::decay_t<decltype(city_population)>;
		static_assert(std::same_as<typename type_city_population::RetType, int32_t>);
		static_assert(type_city_population::Layer == 0);
	}
	// You can also obtain RttiPlaceholders by not using _fld literal.
	{
		auto [nation_rtti, state_rtti, county_rtti, city_rtti, city_population_rtti, city_area_rtti] =
			usa.GetPlaceholders("nation", "state", "county", "city", "city_population", "city_area");
	}



	std::cout << "------Access------" << std::endl;

	auto long_beach = usa[1];// reference to the second element, which is Long Beach.
	std::cout << long_beach[city] << std::endl;// Long Beach

	adapt::Bpos baytown_index = { 8 };
	auto baytown = usa.GetBranch(baytown_index);
	std::cout << baytown[city_population] << std::endl;// 83701



	std::cout << "------Calculate------" << std::endl;

	// Lambda functions are made from placeholders.
	// They can be used when you want to calculate something from data and filter/convert data like <ranges> library.

	//Make a lambda function to calculate population density of a city.
	auto population_density = city_population / city_area;
	static_assert(population_density.GetLayer() == 0);
	static_assert(std::same_as<decltype(population_density)::RetType, double>);
	adapt::Bpos oceanside_index = { 5 };
	std::cout << population_density(usa, oceanside_index) << std::endl;// 174068 / 105.1 = 1655.3

	auto cat_state_county_city = state + " - " + county + " - " + city;
	static_assert(cat_state_county_city.GetLayer() == 0_layer);
	static_assert(std::same_as<decltype(cat_state_county_city)::RetType, std::string>);
	std::cout << cat_state_county_city(usa, baytown_index) << std::endl;// Texas - Harris County - Baytown

	// Get the index of the city with the smallest population.
	auto index_to_smallest_city_population = index(isleast(city_population));
	static_assert(index_to_smallest_city_population.GetLayer() == -1);
	static_assert(std::same_as<decltype(index_to_smallest_city_population)::RetType, int64_t>);
	std::cout << index_to_smallest_city_population(usa) << std::endl;// The index of Baytown, which has the smallest population, is 8.

	// Get the name of the city with the smallest population.
	auto smallest_city = city.at(index(isleast(city_population)));
	static_assert(smallest_city.GetLayer() == -1);
	static_assert(std::same_as<decltype(smallest_city)::RetType, std::string>);
	// If the layer of the lambda function is -1, the lambda function can be called without a Bpos.
	std::cout << smallest_city(usa) << std::endl;// Baytown

	// Various operators and functions can be used in combination.
	// Take the average of the population density of all cities in the usa, and then convert /km^2 to /mi^2.
	auto average_population_density = tostr(mean(city_population / city_area) * 2.589988) + "(/mi^2)";
	static_assert(average_population_density.GetLayer() == -1);
	static_assert(std::same_as<decltype(average_population_density)::RetType, std::string>);
	std::cout << average_population_density(usa) << std::endl;

	//Ctti lambda functions can be converted to Rtti lambda functions if the return type is the one declared in adapt::FieldTypes.
	using Lambda = adapt::eval::RttiFuncNode<STable_>;
	Lambda average_population_density_rtti = ConvertToRttiFuncNode(mean(city_population / city_area) * 2.589988);
	std::cout << average_population_density_rtti(usa).f64() << "(/mi^2)" << std::endl;

	// The functions and operators available in the lambda functions are defined
	// in <OpenADAPT/Evaluator/Function.h> and <OpenADAPT/Evaluator/LayerFuncNode.h>

	std::cout << std::endl;



	std::cout << "------Traverse------" << std::endl;

	// Use traversers to iterate over the data.
	for (const auto& trav : usa.GetRange(0_layer))// STable has only one layer, so the layer must be 0.
	{
		// All the names of the states, counties and cities are output.
		std::cout << std::format("{:<12} {:<20} {:<12}\n", trav[state], trav[county], trav[city]);
	}

	// Lambda functions can also be used to filter data.
	// You can omit GetRange or place after Filter, like
	//  * usa | Filter(...)
	//  * usa | Filter(...) | GetRange(0_layer)
	for (const auto& trav : usa.GetRange(0_layer) | Filter(state == "Texas"))
	{
		// All the names, populations and areas of the cities in Texas are output.
		std::cout << std::format("{:<12} {:>8} {:>7.1f}\n", trav[city], trav[city_population], trav[city_area]);
	}
	std::cout << std::endl;

	// Use lambda function with traverser.
	// Count the number of cities with a larger population than the current city.
	auto population_rank = count(city_population > city_population.o(0_depth)) + 1;
	for (const auto& trav : usa.GetRange(0_layer))
	{
		std::cout << std::format("{:<12} {:>8} {:>7.1f} {:>2}\n", trav[city], trav[city_population], trav[city_area], population_rank(trav));
	}
	std::cout << std::endl;

	// Evaluate range adapter converts the return value of operator* from Traverser object to results of the lambda functions.
	for (auto [city_, population_density_] : usa | Filter(isgreatest(city_population / city_area))
		| Evaluate(city, city_population / city_area))
	{
		// The name and population density of the city with the largest population density is output.
		std::cout << std::format("{:<12} {:>7.1f}\n", city_, population_density_);
	}
	std::cout << std::endl;



	std::cout << "------Show data------" << std::endl;

	// Show the data of the city with the smallest area.
	usa | Filter(isleast(city_area)) | Show(state, city, city_population, tostr(city_area) + "km^2");
	// [   8]            Texas          Baytown       83701    32.700000km^2

	// You can also use Show with format string.
	usa | Filter(isleast(city_area)) | Show("{:>12}, {:>10}, {:>5.1f}x10^3, {:>5.1f}km^2", state, city, city_population / 1000., city_area);
	// [   8]       Texas,    Baytown,  83.7x10^3,  32.7km^2

	std::cout << std::endl;



	std::cout << "------Extract------" << std::endl;

	// Create a new STree by extracting the data from the original STable.
	// Note that the result of the extract in the current version is a STree, not a STable.
	// The extracted STree only has fields specified in Extract range adapter. 
	// If the names are not specified, default names: "fld0", "fld1", ... are used.
	// All the names should be with _fld literals to determine the tree structure statically.
	// Otherwise, the result of Extract will be a DTree.
	auto extracted = usa | Filter(city_area > 500.)
		| Extract(state.named("state"_fld), county.named("county"_fld), city.named("city"_fld), (city_population / city_area).named("population_density"_fld));
	extracted.ShowHierarchy();
	// Layer[-1] { }
	// Layer[ 0] { { "state", class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > } { "county", class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > } { "city", class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > } { "population_density", double } }

	auto [estate, ecounty, ecity, epopulation_density] = extracted.GetPlaceholders("state"_fld, "county"_fld, "city"_fld, "population_density"_fld);
	extracted | Show(estate, if_(len(ecounty) >= 16, substr(ecounty, 0, 13) + "...", ecounty), ecity, epopulation_density);

	//If no _fld literal names are used, the result of Extract will be a DTree, because the tree structure is not determined statically.
	adapt::DTree extracted_dtree = usa | Filter(city_area > 500.) | Extract(city.named("city"), (city_population / city_area).named("population_density"));

	std::cout << std::endl;



	std::cout << "------Plot data with Matplot++------" << std::endl;

	// Plot area vs population of the cities in California.
	namespace plot = adapt::plot;
	auto [vpop, varea, vcity] = usa | Filter(state == "California") | ToVector(city_population, city_area, "\"<-" + city + "\"");
	adapt::Canvas2D c("examples_en_stable.png");
	c.SetXLabel("Area (km^2)");
	c.SetYLabel("Population");
	c.SetLogX();
	c.SetLogY();
	c.SetTitle("Area vs Population of Cities in California");
	c.SetGrid();
	c.PlotPoints(varea, vpop, plot::pt_cir, plot::ps_med_large, plot::notitle).
		PlotLabels(varea, vpop, vcity, plot::labelpos = adapt::LabelPos::right, plot::notitle);

	// You can also use Matplot++ to plot the data.
	/*
	auto [vpopulation, varea] = usa | Filter(state == "California") | ToVector(cast_f64(city_population), city_area);
	auto fig = matplot::figure(true);
	auto ax = fig->current_axes();
	ax->scatter(varea, vpopulation);
	ax->xlabel("Area (km^2)");
	ax->ylabel("Population");
	ax->title("Area vs Population of Cities in California");
	ax->x_axis().scale(matplot::axis_type::axis_scale::log);
	ax->y_axis().scale(matplot::axis_type::axis_scale::log);
	fig->save("examples_en_stable.png");
	*/
}