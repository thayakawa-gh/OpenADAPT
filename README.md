# OpenADAPT

ADAPT is a header-only data analysis and processing library for C++ offering the following features:

* Hierarchically structured data containers.
* Support for joining multiple containers, similar to SQL/Pandas' JOIN functionality.
* Utilization of placeholders and lambda functions to access/aggregate container fields.
* Traversers and views for data filtering/transformation, mirroring `<ranges>` behavior, conforming to `std::ranges::input_range` and `std::ranges::viewable_range` concepts.
* Header-only implementation requiring C++20, devoid of dependencies on third-party libraries.

Its primary objective is to enable C++ to perform the kind of analysis and manipulation that Pandas achieves in Python, albeit with a unique conceptual approach distinct from Pandas.


## Install and use in your project

ADAPT is a header-only library, so simply clone and add the `OpenADAPT` directory to your project's include directries.

If needed, it can be built and installed with CMake using the following commands:
```cmake
git clone https://github.com/thayakawa-gh/OpenADAPT.git
mkdir build
cd build
cmake ../OpenADAPT -DCMAKE_INSTALL_PREFIX=path_to_install_dir
make
make install
```
and can be incorporated into your project using the find_package command:

```cmake
find_package(OpenADAPT REQUIRED)
target_link_libraries(YOUR_PACKAGE_NAME PRIVATE OpenADAPT::OpenADAPT)
```

The test and example codes are built by adding `-DBUILD_TEST=ON` and `-DBUILD_EXAMPLES=ON` to the cmake command, respectively. Note that GTest, yaml-cpp and matplot++ are required for these builds.

## Quickstart

For more detailed examples and explanations, please see [Examples/en](Examples/en).

### Define container structure
```cpp
	// DTree/DTable
	using enum adapt::FieldType;
	adapt::DTree t;
	t.SetTopLayer({ { "nation", Str }, { "capital", Str } });///Set root layer (-1).
	t.AddLayer({ { "state", Str }, { "state capital", Str } });//Add layer 0.
	t.AddLayer({ { "county", Str }, { "county seat", Str }});//Add layer 1.
	t.AddLayer({ { "city", Str }, { "population", I32 }, { "area", F64 } });//Add layer 2.
	t.VerifyStructure();// Verify if the structure is correct.

	// STree/STable
	using adapt::NamedTuple;
	using adapt::Named;
	using TopLayer = NamedTuple<Named<"nation", std::string>, Named<"capital", std::string>>;
	using Layer0 = NamedTuple<Named<"state", std::string>, Named<"state capital", std::string>>;
	using Layer1 = NamedTuple<Named<"county", std::string>, Named<"county seat", std::string>>;
	using Layer2 = NamedTuple<Named<"city", std::string>, Named<"population", int32_t>, Named<"area", double>>;
	using STree_ = adapt::STree<TopLayer, Layer0, Layer1, Layer2>;
	STree_ t;
```

### Store data
```cpp
	t.SetTopFields("USA", "Washington D.C.");//Set top layer elements.
	t.Reserve(2);//Reserve elements for layer 0.
	t.Push("California", "Sacramento");//Push an element to layer 0.
	t.Push("Texas", "Austin");

	auto california = t[0];//Reference layer 0 element.
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

	auto texas = t[1];
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
```

### Get placeholders
```cpp
	// Placeholders are used to access/calculate data stored in containers.
	using namespace adapt::lit;
	ADAPT_GET_PLACEHOLDERS(t, "capital", "state", "state_capital", "county", "city", "population", "area");
	// The above macro is equivalent to the following code.
	auto [capital, state, state_capital, county, city, population, area] = 
		t.GetPlaceholders("capital"_fld, "state"_fld, "state_capital", "county"_fld, "city"_fld, "population"_fld, "area"_fld);

	// GetPlaceholders returns:
	// * std::array of RttiPlaceholders if the container is a DTree/DTable.
	// * std::tuple of CttiPlaceholders if the container is an STree/STable.
	// CttiPlaceholders have static type information, while RttiPlaceholders have runtime type information.
```

### Access data using placeholders
```cpp
	auto california = t[0];// Get the first element of layer 0.
	std::cout << california[state_capital].str() << std::endl;// Output: Sacramento

	auto san_diego = california[1][0];// Get the first element of layer 2 in the second element of layer 1.
	std::cout << san_diego[population].i32() << std::endl;// Output: 1386932

	// If the placeholders are Ctti, explicit casting, such as .i32(), .f64(), .str(), is not required.
	std::cout << california[state_capital] << std::endl;
	std::cout << san_diego[population] << std::endl;
```

### Calculate using placeholders
```cpp
	auto population_density = population / area;
	adapt::Bpos baytown_index = { 1, 0, 2 };
	std::cout << population_density(t, baytown_index).f64() << std::endl;// 83701 / 32.7 = 2559.6

	auto total_population_in_a_county = sum(population);
	adapt::Bpos harris_county_index = { 1, 0 };
	std::cout << total_population_in_a_county(t, harris_county_index).i32() << std::endl;// 2304580 + 151950 + 83701 = 2548231
```

### Traverse
```cpp
	for (const auto& trav : t | GetRange(2_layer))
	{
		// All the names of states, counties and citiees are output.
		std::cout << std::format("{:<12} {:<12} {:<12}\n", trav[state].str(), trav[county].str(), trav[city].str());
	}
	
	auto population_density = population / area;
	for (const auto& trav : t | Filter(city == county_seat) | GetRange(2_layer))
	{
		// All the names, populations and areas of the cities that are the county seats are output.
		std::cout << std::format("{:<12} {:<12} {:<12} {:>7.1f}\n",
								 trav[state].str(), trav[county].str(), trav[city].str(), population_density(trav).f64());
	}
```

### Extract
```cpp
	auto population_density = population / area;
	auto extracted = usa | Filter(area > 500.) | Extract(state, county, city, population_density);
	// If all the arguments of Extract are Ctti, the return value is an STree.
	// Otherwise, the return value is a DTree.
	extracted.ShowHierarchy();
	// Output:
	// Layer[-1] { }
	// Layer[ 0] { { "fld0", Str } }
	// Layer[ 1] { { "fld1", Str } }
	// Layer[ 2] { { "fld2", Str } { "fld3", F64 } }

	ADAPT_GET_PLACEHOLDERS(extracted, fld0, fld1, fld2, fld3);
	for (const auto& trav : extracted | GetRange(2_layer))
	{
		std::cout << std::format("{:<12} {:<12} {:<12} {:>8.1f}\n",
								 trav[fld0].str(), trav[fld1].str(), trav[fld2].str(), trav[fld3].f64());
		// Output:
		// California   Los Angeles  Los Angeles   3275.7
		// California   San Diego    San Diego     1646.8
		// Texas        Harris       Houston       1535.7
		// Texas        Dallas       Dallas        1470.2
	}
```

### Plot
```cpp
	// Plot area vs population of the cities in California.
	namespace plot = adapt::plot;
	auto plot_range = t | Filter(state == "California") | GetRange(2_layer);
	auto rpop = plot_range | Evaluate(population);
	auto rarea = plot_range | Evaluate(area);
	adapt::Canvas2D c("examples_en_stree.png");
	c.SetXLabel("Area (km^2)");
	c.SetYLabel("Population");
	c.SetLogX();
	c.SetLogY();
	c.SetTitle("Area vs Population of Cities in California");
	c.SetGrid();
	c.PlotPoints(rarea, rpop, plot::pt_cir, plot::ps_med_large, plot::notitle);
```