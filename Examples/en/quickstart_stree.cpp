#include <format>
#include <iostream>
#include <OpenADAPT/ADAPT.h>
#include <matplot/matplot.h>

using namespace adapt::lit;

auto MakeSTree()
{
	// Define a STree, which determines hierarchical structure and fields (like columns) at compile-time using templates.
	// The structure of STree is defined by adapt::NamedTuple and adapt::Named.
	// It is similar to std::tuple<Fields...> + std::vector<std::tuple<Fields...>, std::vector<std::tuple<Fields...>>>;

	// STree supports any types of fields, as long as they are default constructible.

	using TopLayer = ADAPT_S_DEFINE_LAYER(nation, std::string, capital, std::string);
	using Layer0 = ADAPT_S_DEFINE_LAYER(state, std::string, state_capital, std::string);
	using Layer1 = ADAPT_S_DEFINE_LAYER(county, std::string, county_seat, std::string);
	using Layer2 = ADAPT_S_DEFINE_LAYER(city, std::string, population, int32_t, area, double);
	// The helper macros above are expanded to the following code:
	// using TopLayer = adapt::NamedTuple<adapt::Named<"nation", std::string>, adapt::Named<"capital", std::string>>;
	// using Layer0 = adapt::NamedTuple<adapt::Named<"state", std::string>, adapt::Named<"state_capital", std::string>>;
	// using Layer1 = adapt::NamedTuple<adapt::Named<"county", std::string>, adapt::Named<"county_seat", std::string>>;
	// using Layer2 = adapt::NamedTuple<adapt::Named<"city", std::string>, adapt::Named<"population", int32_t>, adapt::Named<"area", double>>;
	using STree_ = adapt::STree<TopLayer, Layer0, Layer1, Layer2>;

	static_assert(adapt::s_hierarchy<STree_>);

	STree_ t;
	t.ShowHierarchy();
	return t;
}

template <adapt::s_tree STree_>
void StoreData(STree_& usa)
{
	// Store data in the DTree using various methods:
	// Reserve(size)            ... Increase the capacity of elements in the lower layer, like std::vector::reserve.
	// Resize(size)             ... Change the number of elements in the lower layer, like std::vector::resize.
	// Push(fields...)          ... Add an element to the end of the lower layer elements, like std::vector::push_back.
	// Pop()                    ... Remove the last lower layer elements, like std::vector::pop_back.
	// Assign(fields...)        ... Assign fields, like an assignment operator.
	// Insert(index, fields...) ... Insert an element at the specified index, like std::vector::insert.
	// Erase(index)             ... Erase an element at the specified index, like std::vector::erase.

	usa.SetTopFields("USA", "Washington D.C.");//Set top layer elements.
	usa.Reserve(2);//Reserve elements for layer 0.
	usa.Push("California", "Sacramento");//Push an element to layer 0.
	usa.Push("Texas", "Austin");

	auto california = usa[0];//Reference layer 0 element.
	california.Reserve(2);//Reserve elements for layer 1.
	california.Push("Los Angeles County", "Los Angeles");
	california.Push("San Diego County", "San Diego");

	auto los_angeles_county = california[0];
	los_angeles_county.Reserve(3);
	los_angeles_county.Push("Los Angeles", 3976322, 1213.9);//The unit of area is km^2.
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

void QuickstartSTree()
{
	// Simple example of using STree.

	std::cout << "[[Quickstart STree]]" << std::endl;

	std::cout << "------Create------\n";
	auto usa = MakeSTree();
	using STree_ = decltype(usa);
	std::cout << std::endl;



	std::cout << "------Store Data------\n";
	StoreData(usa);
	std::cout << std::endl;



	std::cout << "------Placeholders------" << std::endl;

	// Placeholders are used to access/calculate data stored in containers.
	// Node that you should use _fld literal to get STree::CttiPlaceholder<Type, Layer> that has compile-time type/layer informations.
	// If no _fld literal is used, the return type is STree::RttiPlaceholder that has runtime type/layer information.
	// RttiPlaceholder can also be used for STree, but its performance is lower than CttiPlaceholder, especially in the use of lambda functions.
	ADAPT_GET_PLACEHOLDERS(usa, nation, state, state_capital, county, county_seat, city, population, area);

	// ADAPT_GET_PLACEHOLDERS is a macro that expands to the following code:
	// auto [nation, state, state_capital, county, county_seat, city, population, area] =
	//	usa.GetPlaceholders("nation"_fld, "state"_fld, "state capital"_fld, "county"_fld, "county seat"_fld, "city"_fld, "population"_fld, "area"_fld);

	// Confirm the type and layer of CttiPlaceholder are correct.
	{
		using type_population = std::decay_t<decltype(population)>;
		static_assert(std::same_as<typename type_population::RetType, int32_t>);
		static_assert(type_population::Layer == 2_layer);
	}
	// You can also obtain RttiPlaceholders by not using _fld literal.
	{
		auto [nation_rtti, state_rtti, state_capital_rtti, county_rtti, county_seat_rtti, city_rtti, population_rtti, area_rtti] =
			usa.GetPlaceholders("nation", "state", "state_capital", "county", "county_seat", "city", "population", "area");
	}
	std::cout << std::endl;



	std::cout << "------Access------" << std::endl;

	// Access data stored in STree, using the [] operator.
	auto california = usa[0];
	std::cout << california[state_capital] << std::endl;// Sacramento

	auto san_diego = california[1][0];
	std::cout << san_diego[population] << std::endl;// 1386932

	// Access data using adapt::Bpos. Bpos is a list of indices for each layer.
	// Note that adapt::Bpos allocates an array of indices dynamically if it has more than 3 indices.
	adapt::Bpos harris_index = { 1, 0 };
	auto harris = usa.GetBranch(harris_index);
	std::cout << harris[county] << std::endl;// Harris County

	std::cout << std::endl;



	std::cout << "------Calculate------" << std::endl;

	// Lambda functions are made from placeholders.
	// They can be used when you want to calculate something from the data and filter/convert data like <ranges> library.

	//Make a lambda function to calculate population density of a city.
	auto population_density = population / area;
	//Lambda functions also have their types and layers.
	//This lambda function has layer 2, because it can be calculated from a city (layer 2 element).
	static_assert(population_density.GetLayer() == 2);
	static_assert(std::same_as<decltype(population_density)::RetType, double>);
	adapt::Bpos baytown_index = { 1, 0, 2 };
	std::cout << population_density(usa, baytown_index) << std::endl;// 83701 / 32.7 = 2559.6

	auto cat_state_county_city = state + " - " + county + " - " + city;
	static_assert(cat_state_county_city.GetLayer() == 2_layer);
	static_assert(std::same_as<decltype(cat_state_county_city)::RetType, std::string>);
	std::cout << cat_state_county_city(usa, baytown_index) << std::endl;// Texas - Harris County - Baytown

	// Make a lambda function to calculate total population in a county.
	// As the result of "sum", the layer of the lambda function is raised by 1,
	// because the sum can be calculated by specifying a county (layer 1 element).
	auto total_population_in_a_county = sum(population);
	static_assert(total_population_in_a_county.GetLayer() == 1);
	static_assert(std::same_as<decltype(total_population_in_a_county)::RetType, int32_t>);
	std::cout << total_population_in_a_county(usa, harris_index) << std::endl;// 2304580 + 151950 + 83701 = 2540231

	// Make a lambda function to calculate total population in a state.
	// In general, the sumN and the other similar functions raise the layer by N, except the case of outer field access.
	auto total_population_in_a_state = sum2(population);
	static_assert(total_population_in_a_state.GetLayer() == 0);
	static_assert(std::same_as<decltype(total_population_in_a_state)::RetType, int32_t>);
	adapt::Bpos california_index = { 0 };
	std::cout << total_population_in_a_state(usa, california_index) << std::endl;// 3976322 + 466742 + 196543 + 1386932 + 275487 + 174068 = 6476094

	auto total_population_in_a_nation = sum3(population);
	static_assert(total_population_in_a_nation.GetLayer() == -1);
	static_assert(std::same_as<decltype(total_population_in_a_nation)::RetType, int32_t>);
	//If the layer of a lambda function is -1, the lambda function can be called without a Bpos,
	//because there is only one layer -1 element, usa.
	std::cout << total_population_in_a_nation(usa) << std::endl;// 6476094 + 1304379 + 256684 + 246918 = 8286975

	// Various operators and functions can be used in combination.
	// Take the average of the population density of all cities in the usa, and then convert /km^2 to /mi^2.
	auto average_population_density = tostr(mean3(population / area) * 2.589988) + "(/mi^2)";
	static_assert(average_population_density.GetLayer() == -1);
	static_assert(std::same_as<decltype(average_population_density)::RetType, std::string>);
	std::cout << average_population_density(usa) << std::endl;

	// Ctti lambda functions can be converted to Rtti lambda functions if the return type is the one declared in adapt::FieldTypes.
	using Lambda = adapt::eval::RttiFuncNode<STree_>;
	Lambda average_population_density_rtti = ConvertToRttiFuncNode(mean3(population / area) * 2.589988);
	std::cout << average_population_density_rtti(usa).f64() << "(/mi^2)" << std::endl;// Need to call f64() to get the result since the compile-time type info is lost.

	// The functions and operators available in the lambda functions are defined
	// in <OpenADAPT/Evaluator/Function.h> and <OpenADAPT/Evaluator/LayerFuncNode.h>

	std::cout << std::endl;



	std::cout << "------Traverse------" << std::endl;

	// Use traversers to iterate over the data.
	for (const auto& trav : usa.GetRange(1_layer))// Traverse layer 1.
	{
		// All the names of the counties and county seats are output.
		// During traversal of layer 1, you can access the fields from layer -1 to 1.
		std::cout << std::format("{:<12} {:<20} {:<12}\n", trav[state], trav[county], trav[county_seat]);
	}
	std::cout << std::endl;

	for (const auto& trav : usa.GetRange(2_layer))// Traverse layer 2 = cities.
	{
		// All the names of states, counties and cities are output.
		std::cout << std::format("{:<12} {:<20} {:<12}\n", trav[state], trav[county], trav[city]);
	}
	std::cout << std::endl;

	//Lambda functions can also be used to filter the data.
	//You can omit GetRange or place after Filter range adapter, like
	// * usa | Filter(...)
	//    usa is automatically converted to a range when being passed to Filter.
	//    The traversal layer matches the bottom layer of the lambda functions in Filter.
	// * usa | Filter(...) | GetRange(2_layer)
	//    usa is automatically converted to range.
	//    Then traversal layer is lowered to layer 2 if the bottom layer of the lambda functions is higher than 2.
	for (const auto& trav : usa.GetRange(2_layer) | Filter(city == county_seat))
	{
		// All the names, populations and areas of the cities that are the county seats are output.
		std::cout << std::format("{:<12} {:>8} {:>7.1f}\n", trav[city], trav[population], trav[area]);
	}
	std::cout << std::endl;

	// Use lambda functions with traverser.
	// count the number of cities with a larger population than the current city.
	auto population_rank = count3(population > population.o(0_depth)) + 1;
	for (const auto& trav : usa.GetRange(2_layer))
	{
		// Calculate the population ranking.
		std::cout << std::format("{:<12} {:>8} {:>2}\n", trav[city], trav[population], population_rank(trav));
	}
	std::cout << std::endl;

	// Evaluate range adapter converts the return value of operator* from Traverser object to results of the lambda functions.
	for (auto [county_, population_density_] : usa | Filter(isgreatest(sum(population) / sum(area)))
													| Evaluate(county, sum(population) / sum(area)))
	{
		// All the names and population densities of the countries with the largest population densities in the states are output.
		std::cout << std::format("{:<12} {:>7.1f}\n", county_, population_density_);
	}

	std::cout << std::endl;



	std::cout << "------Show data------" << std::endl;

	//Show the data of the cities with the smallest areas in the states.
	usa | Filter(isleast2(area)) | Show(state, city, population, tostr(area) + "km^2");
	//[   0,   0,   2]       California         Glendale      196543    79.300000km^2
	//[   1,   0,   2]            Texas          Baytown       83701    32.700000km^2

	// You can also use Show with format string.
	usa | Filter(isleast2(area)) | Show("{:>12}, {:>10}, {:>5.1f}x10^3, {:>5.1f}km^2", state, city, population / 1000., area);
	// [   0,   0,   2]  California,   Glendale, 196.5x10^3,  79.3km^2
	// [   1,   0,   2]       Texas,    Baytown,  83.7x10^3,  32.7km^2
	std::cout << std::endl;



	std::cout << "------Extract------" << std::endl;

	// Create a new STree by extracting the data from the original STree.
	// The arguments of Extract functions become the fields of the new STree,
	// and the hierarchical structure is automatically determined by the arguments.
	// If the names are not specified, default names: "fld0", "fld1", ... are used.
	// The names should be with _fld literals to determine the tree structure statically.
	// Otherwise, the result of Extract will be a DTree.
	auto extracted = usa | Filter(area > 500.)
		| Extract(state.named("state"_fld), county.named("county"_fld), city.named("city"_fld), (population / area).named("population_density"_fld));
	static_assert(adapt::s_tree<decltype(extracted)>);
	extracted.ShowHierarchy();
	// Layer[-1] { }
	// Layer[ 0] { { "state", class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > } }
	// Layer[ 1] { { "county", class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > } }
	// Layer[ 2] { { "city", class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > } { "population_density", double } }
	auto [estate, ecounty, ecity, epopulation_density] = extracted.GetPlaceholders("state"_fld, "county"_fld, "city"_fld, "population_density"_fld);
	extracted | Show(estate, if_(len(ecounty) >= (size_t)16, substr(ecounty, 0, 13) + "...", ecounty), ecity, epopulation_density);

	//If no _fld literal names are used, the result of Extract will be a DTree, because the tree structure is not determined statically.
	adapt::DTree extracted_dtree = usa | Filter(area > 500.) | Extract(city.named("city"), (population / area).named("population_density"));

	std::cout << std::endl;



	std::cout << "------Plot data------" << std::endl;

	// Plot area vs population of the cities in California.
	namespace plot = adapt::plot;
	auto [vpop, varea, vcity] = usa | Filter(state == "California") | ToVector(population, area, "\"<-" + city + "\"");
	adapt::Canvas2D c("examples_en_stree.png");
	c.SetXLabel("Area (km^2)");
	c.SetYLabel("Population");
	c.SetLogX();
	c.SetLogY();
	c.SetTitle("Area vs Population of Cities in California");
	c.SetGrid();
	c.PlotPoints(varea, vpop, plot::pt_cir, plot::ps_med_large, plot::notitle).
		PlotLabels(varea, vpop, vcity, plot::labelpos = adapt::LabelPos::right, plot::notitle);

	std::cout << std::endl;

	// You can also use Matplot++ to plot the data.
	/*
	auto [vpopulation, varea] = usa | Filter(state == "California") | ToVector(cast_f64(population), area);
	auto fig = matplot::figure(true);
	auto ax = fig->current_axes();
	ax->scatter(varea, vpopulation);
	ax->xlabel("Area (km^2)");
	ax->ylabel("Population");
	ax->title("Area vs Population of Cities in California");
	ax->x_axis().scale(matplot::axis_type::axis_scale::log);
	ax->y_axis().scale(matplot::axis_type::axis_scale::log);
	fig->save("examples_en_stree.png");
	*/
}
