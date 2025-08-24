#include <format>
#include <iostream>
#include <OpenADAPT/ADAPT.h>

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
	ADAPT_D_SET_TOP_LAYER(t, nation, Str, capital, Str);//Set root layer (-1).
	ADAPT_D_ADD_LAYER(t, state, Str, state_capital, Str);//Add layer 0.
	ADAPT_D_ADD_LAYER(t, county, Str, county_seat, Str);//Add layer 1.
	ADAPT_D_ADD_LAYER(t, city, Str, population, I32, area, F64);//Add layer 2.
	// These helper macros are equivalent to the following:
	// t.SetTopLayer({ { "nation", adapt::FieldType::Str }, { "capital", adapt::FieldType::Str } });
	// t.AddLayer({ { "state", adapt::FieldType::Str }, { "state_capital", adapt::FieldType::Str } });
	// t.AddLayer({ { "county", adapt::FieldType::Str }, { "county_seat", adapt::FieldType::Str }});
	// t.AddLayer({ { "city", adapt::FieldType::Str }, { "population", adapt::FieldType::I32 }, { "area", adapt::FieldType::F64 } });
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

	std::cout << "[[Quickstart DTree]]" << std::endl;

	std::cout <<  "------Create------" << std::endl;
	adapt::DTree usa = MakeDTree();
	std::cout << std::endl;



	std::cout << "------Store data------" << std::endl;
	StoreData(usa);
	std::cout << std::endl;



	std::cout << "------Placeholders------" << std::endl;

	// Placeholders are used to access/calculate data stored in containers.
	// The default return value of GetPlaceholders for DTree is DTree::RttiPlaceholder, which has runtime type and layer information.
	// RttiPlaceholder can be converted to TypedPlaceholder using i32(), str(), etc. methods, which has compile-time type information.
	// The performance of the TypedPlaceholder is better than RttiPlaceholder, especially in the use of lambda functions.
	ADAPT_GET_PLACEHOLDERS(usa, nation, state, state_capital, county, county_seat, city, population, area);
	// ADAPT_GET_PLACEHOLDERS is a macro that expands to the following code:
	// auto [nation, state, state_capital, county, county_seat, city, population, area] =
	//	usa.GetPlaceholders("nation"_fld, "state"_fld, "state_capital"_fld, "county"_fld, "county_seat"_fld, "city"_fld, "population"_fld, "area"_fld);
	assert(       nation.GetLayer() == -1_layer &&        nation.GetType() == adapt::FieldType::Str);
	assert(        state.GetLayer() ==  0_layer &&         state.GetType() == adapt::FieldType::Str);
	assert(state_capital.GetLayer() ==  0_layer && state_capital.GetType() == adapt::FieldType::Str);
	assert(       county.GetLayer() ==  1_layer &&        county.GetType() == adapt::FieldType::Str);
	assert(  county_seat.GetLayer() ==  1_layer &&   county_seat.GetType() == adapt::FieldType::Str);
	assert(         city.GetLayer() ==  2_layer &&          city.GetType() == adapt::FieldType::Str);
	assert(   population.GetLayer() ==  2_layer &&    population.GetType() == adapt::FieldType::I32);
	assert(         area.GetLayer() ==  2_layer &&          area.GetType() == adapt::FieldType::F64);

	// Convert RttiPlaceholder to TypedPlaceholder.
	{
		auto nation_typed = nation.str();
		auto state_typed = state.str();
		auto city_typed = city.str();
		auto population_typed = population.i32();
	}

	// You can also obtain CttiPlaceholders from DTree by explicitly providing static types and layers.
	{
		auto nation_ctti = usa.GetPlaceholder<-1, std::string>("nation");
		auto state_ctti = usa.GetPlaceholder<0, std::string>("state");
		auto city_ctti = usa.GetPlaceholder<2, std::string>("city");
		auto polulation_ctti = usa.GetPlaceholder<2, int32_t>("population");
	}
	std::cout << std::endl;



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

	Lambda cat_state_county_city = state + " - " + county + " - " + city;
	assert(cat_state_county_city.GetLayer() == 2_layer && cat_state_county_city.GetType() == adapt::FieldType::Str);
	std::cout << cat_state_county_city(usa, baytown_index).str() << std::endl;// Texas - Harris County - Baytown

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
	Lambda average_population_density = tostr(mean3(population / area) * 2.589988) + "(/mi^2)";
	assert(average_population_density.GetLayer() == -1_layer && average_population_density.GetType() == adapt::FieldType::Str);
	std::cout << average_population_density(usa).str() << std::endl;

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
		std::cout << std::format("{:<12} {:<20} {:<12}\n", trav[state].str(), trav[county].str(), trav[county_seat].str());
	}
	std::cout << std::endl;

	for (const auto& trav : usa.GetRange(2_layer))// Traverse layer 2 = cities.
	{
		// All the names of states, counties and cities are output.
		// Hint: results of accessing with rtti placeholders or calculating with rtti lambda functions
		//       can be used in std::format without any type conversion.
		std::cout << std::format("{:<12} {:<20} {:<12}\n", trav[state], trav[county], trav[city]);
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
		std::cout << std::format("{:<12} {:>8} {:>7.1f}\n", trav[city], trav[population], trav[area]);
	}
	std::cout << std::endl;

	// traverse with filter
	auto population_density2 = population / area;
	for (const auto& trav : usa | Filter(city == county_seat) | GetRange(2_layer))
	{
		// All the names, populations and areas of the cities that are the county seats are output.
		std::cout << std::format("{:<12} {:<20} {:<12} {:>7.1f}\n",
								 trav[state], trav[county], trav[city], population_density2(trav));
	}
	std::cout << std::endl;
	// Use lambda functions with traverser.
	// count the number of cities with a larger population than the current city.
	Lambda population_rank = count3(population > population.o(0_depth)) + 1;
	for (const auto& trav : usa.GetRange(2_layer))
	{
		// Calculate the population ranking.
		std::cout << std::format("{:<12} {:>8} {:>2}\n", trav[city], trav[population], population_rank(trav));
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

	usa | Show(state, county, tostr(sum(area)) + "km^2");

	// Show the data of the cities with the smallest areas in the states.
	usa | Filter(isleast2(area)) | Show(state, city, population, tostr(area) + "km^2");
	// [   0,   0,   2]       California         Glendale      196543    79.300000km^2
	// [   1,   0,   2]            Texas          Baytown       83701    32.700000km^2

	// You can also use Show with format string.
	usa | Filter(isleast2(area)) | Show("{:>12}, {:>10}, {:>5.1f}x10^3, {:>5.1f}km^2", state, city, population / 1000., area);
	// [   0,   0,   2]  California,   Glendale, 196.5x10^3,  79.3km^2
	// [   1,   0,   2]       Texas,    Baytown,  83.7x10^3,  32.7km^2
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
	extracted | Show(estate, if_(len(ecounty) >= 16, substr(ecounty, 0, 13) + "...", ecounty), ecity, epopulation_density);

	// Instead of using named("...") to specify field names, you can also use ADAPT_EXTRACT macro.
	adapt::DTree extracted2 = usa | Filter(area > 500.) | ADAPT_EXTRACT(state, county, city, population / area);
	// This macro is equivalent to the following code:
	// adapt::DTree extracted2 = usa | Filter(area > 500.) | Extract((state).named("state"_fld), (county).named("county"_fld), (city).named("city"_fld), (population / area).named("population / area"_fld));

	std::cout << std::endl;



	std::cout << "------Hist------" << std::endl;
	// Create a DHist by binning the data in the DTree.
	// The following code creates a 2-dimensional DHist with population and area as the x-y axes.
	// The x-axis bin width is 500000 and the center is 0.
	// The y-axis bin width is 200. In this case the y-axis center is not specified, so it is set to 0. by default.
	// "city" at the end is not used as an axis, but is used as an additional field.
	// Note that the type of the axis is required to be F64 (double).
	adapt::DHist hist = usa | Hist(cast_f64(population), 500000., 0., cast_f64(area), 200., city);
	hist.ShowHierarchy();
	// Layer[-1] { { "min0", I32 } { "min1", I32 } { "max0", I32 } { "max1", I32 } { "wbin0", F64 } { "wbin1", F64 } { "cbin0", F64 } { "cbin1", F64 } }
	// Layer[ 0] { { "ibin0", I32 } { "ibin1", I32 } }
	// Layer[ 1] { { "axis0", F64 } { "axis1", F64 } { "fld0", Str } }

	// The details of the DHist are described in "quickstart_dhist.cpp".

	auto fpopulation = cast_f64(population); // Convert to F64 for histogram.
	auto farea = cast_f64(area); // Convert to F64 for histogram.
	// You can specity the names of the axes and the additional fields in the same way as Extract.
	adapt::DHist hist2 = usa | Hist(fpopulation.named("fpopulation"), 500000., 0., farea.named("farea"), 200., city.named("city"));
	adapt::DHist hist3 = usa | ADAPT_HIST(fpopulation, 500000., 0., farea, 200., city);// Equivalent to the above code.
	hist2.ShowHierarchy();
	// Layer[-1] { { "min0", I32 } { "min1", I32 } { "max0", I32 } { "max1", I32 } { "wbin0", F64 } { "wbin1", F64 } { "cbin0", F64 } { "cbin1", F64 } }
	// Layer[ 0] { { "ibin0", I32 } { "ibin1", I32 } }
	// Layer[ 1] { { "fpopulation", F64 } { "farea", F64 } { "city", Str } }


	std::cout << std::endl;



	std::cout << "------Plot data------" << std::endl;

	// Plot area vs population of the cities in California.
	namespace plot = adapt::plot;
	auto [vpop, varea, vcity] = usa | Filter(state == "California") | ToVector(population.i32(), area.f64(), ("\"<-" + city + "\"").str());
	adapt::Canvas2D c("examples_en_dtree.png");
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
	auto [varea, vpopulation] = usa | Filter(state == "California") | ToVector(area.f64(), cast_F64(population).f64());
	auto fig = matplot::figure(true);
	auto ax = fig->current_axes();
	ax->scatter(varea, vpopulation);
	ax->xlabel("Area (km^2)");
	ax->ylabel("Population");
	ax->title("Area vs Population of Cities in California");
	ax->x_axis().scale(matplot::axis_type::axis_scale::log);
	ax->y_axis().scale(matplot::axis_type::axis_scale::log);
	fig->save("examples_en_dtree.png");
	*/
}