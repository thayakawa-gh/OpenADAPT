# OpenADAPT

ADAPT is a header-only data analysis and processing library for C++ offering the following features:

* Hierarchically structured data containers.
* Utilization of placeholders and lambda functions to access/aggregate container fields.
* Traversers and views for data filtering/transformation, mirroring `<ranges>` behavior, conforming to `std::ranges::input_range` and `std::ranges::viewable_range` concepts.
* Support for joining multiple containers, similar to SQL/Pandas' JOIN functionality.
* Header-only implementation requiring C++20, devoid of dependencies on third-party libraries.
* Plotting functionality with gnuplot.

Its primary objective is to enable C++ to perform the kind of analysis and manipulation that Pandas achieves in Python, albeit with a unique conceptual approach distinct from Pandas.


## Installation

ADAPT is a header-only library, so simply clone and add the `OpenADAPT` directory to your project's include directries.

### Install with CMake (optional)

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

The test and example codes are built by adding `-DBUILD_TEST=ON` and `-DBUILD_EXAMPLES=ON` to the cmake command, respectively. Note that GTest and yaml-cpp are required for these builds.


### Include ADAPT
```cpp
#include <OpenADAPT/ADAPT.h>

using namespace adapt::lit;// Import the ADAPT literals, such as "_fld" for field names.
```

## [For the detailed examples and explanations, please see Examples/en](Examples/en).

## Container basics

ADAPT provides hierarchically structured containers, STree/DTree, and table containers, STable/DTable.

* STree/STable have static structure defined by templates. They are optimized for fast access and calculation, and can contain any types of data.
* DTree/DTable have run-time structure defined by functions. They are available without templates, but slower than STree/STable and can only contain types declared in `adapt::FieldType`.

* STree/DTree can have a hierarchical structure but slower than STable/DTable.
* STable/DTable cannot have a hierarchical structure; they are similar to STree/DTree with only one layer.

|container|structure definition|max layer|type check|traversal performance|
|:----|:----|:----|:----|:----|
|STree|static|SHRT_MAX|static|slightly slower|
|DTree|dynamic|SHRT_MAX|dynamic|slower|
|STable|static|0|static|faster|
|DTable|dynamic|0|dynamic|slightly faster|

### Define container structure

```cpp
	// DTree/DTable
	adapt::DTree usa;
	ADAPT_D_SET_TOP_LAYER(usa, nation, Str, capital, Str);//Set root layer (-1).
	ADAPT_D_ADD_LAYER(usa, state, Str, state_capital, Str);//Add layer 0.
	ADAPT_D_ADD_LAYER(usa, county, Str, county_seat, Str);//Add layer 1.
	ADAPT_D_ADD_LAYER(usa, city, Str, population, I32, area, F64);//Add layer 2.
	// The helper macros above are equivalent to the following:
	// usa.SetTopLayer({ { "nation", adapt::FieldType::Str }, { "capital", adapt::FieldType::Str } });
	// usa.AddLayer({ { "state", adapt::FieldType::Str }, { "state_capital", adapt::FieldType::Str } });
	// usa.AddLayer({ { "county", adapt::FieldType::Str }, { "county_seat", adapt::FieldType::Str }});
	// usa.AddLayer({ { "city", adapt::FieldType::Str }, { "population", adapt::FieldType::I32 }, { "area", adapt::FieldType::F64 } });
	usa.VerifyStructure();// Verify if the structure is correct.

	// STree/STable
	using TopLayer = ADAPT_S_DEFINE_LAYER(nation, std::string, capital, std::string);
	using Layer0 = ADAPT_S_DEFINE_LAYER(state, std::string, state_capital, std::string);
	using Layer1 = ADAPT_S_DEFINE_LAYER(county, std::string, county_seat, std::string);
	using Layer2 = ADAPT_S_DEFINE_LAYER(city, std::string, population, int32_t, area, double);
	// The helper macros are equivalent to the following:
	// using TopLayer = adapt::NamedTuple<adapt::Named<"nation", std::string>, adapt::Named<"capital", std::string>>;
	// using Layer0 = adapt::NamedTuple<adapt::Named<"state", std::string>, adapt::Named<"state_capital", std::string>>;
	// using Layer1 = adapt::NamedTuple<adapt::Named<"county", std::string>, adapt::Named<"county_seat", std::string>>;
	// using Layer2 = adapt::NamedTuple<adapt::Named<"city", std::string>, adapt::Named<"population", int32_t>, adapt::Named<"area", double>>;
	using STree_ = adapt::STree<TopLayer, Layer0, Layer1, Layer2>;
	STree_ usa;
```

### Store data

Store data in the containers using various methods:
|Function name|Behavior|
|:---|:---|
|`Reserve(size)`|Increase the capacity of elements in the lower layer, like std::vector::reserve.|
|`Resize(size)`|Change the number of elements in the lower layer, like std::vector::resize.|
|`Push(fields...)`|Add an element to the end of the lower layer elements, like std::vector::push_back.|
|`Pop()`|Remove the last lower layer elements, like std::vector::pop_back.|
|`Assign(fields...)`|Assign fields, like an assignment operator.|
|`Insert(index, fields...)`|Insert an element at the specified index, like std::vector::insert.|
|`Erase(index)`|Erase an element at the specified index, like std::vector::erase.|

```cpp
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
```

## Data access and calculation using placeholders and lambda functions

### Get placeholders
Placeholders are used to access/calculate data stored in containers.
GetPlaceholders returns:
* `std::array` of RttiPlaceholders if the container is a DTree/DTable.
* `std::tuple` of CttiPlaceholders if the container is an STree/STable.

CttiPlaceholders have compile-time type and layer information, while RttiPlaceholders have runtime information.
Therefore the performance of CttiPlaceholders is better than RttiPlaceholders, especially in the use of lambda functions.

If desired, you can obtain CttiPlaceholders for DTree/DTable or RttiPlaceholders for STree/STable.
Please refer to the placeholder section of [quickstart_dtree.cpp](Examples/en/quickstart_dtree.cpp), [quickstart_dtable.cpp](Examples/en/quickstart_dtable.cpp), [quickstart_stree.cpp](Examples/en/quickstart_stree.cpp), [quickstart_stable.cpp](Examples/en/quickstart_stable.cpp) for details.

```cpp
	// Placeholders are used to access/calculate data stored in containers.
	using namespace adapt::lit;
	ADAPT_GET_PLACEHOLDERS(usa, capital, state, state_capital, county, city, population, area);
	// The above macro is equivalent to the following code.
	auto [capital, state, state_capital, county, city, population, area] = 
		usa.GetPlaceholders("capital"_fld, "state"_fld, "state_capital"_fld, "county"_fld, "city"_fld, "population"_fld, "area"_fld);
```

### Access data using placeholders
```cpp
	auto california = usa[0];
	std::cout << california[state_capital].str() << std::endl;// Sacramento

	auto san_diego = california[1][0];
	std::cout << san_diego[population].i32() << std::endl;// 1386932
	
	// If the placeholders are Ctti, explicit casting, such as .i32(), .f64(), .str(), is not required.
	std::cout << california[state_capital] << std::endl;
	std::cout << san_diego[population] << std::endl;
```

### Calculate using lambda functions
Lambda functions are made from placeholders.
They can be used when you want to calculate something from data and filter/convert data like `<ranges>` library.
```cpp
	auto population_density = population / area;
	adapt::Bpos baytown_index = { 1, 0, 2 };
	std::cout << population_density(usa, baytown_index).f64() << std::endl;// 83701 / 32.7 = 2559.66
	
	auto cat_state_county_city = state + " - " + county + " - " + city;
	std::cout << cat_state_county_city(usa, baytown_index).str() << std::endl;// Texas - Harris County - Baytown

	auto total_population_in_a_county = sum(population);
	adapt::Bpos harris_county_index = { 1, 0 };
	std::cout << total_population_in_a_county(usa, harris_county_index).i32() << std::endl;// 2304580 + 151950 + 83701 = 2548231

	auto total_population_in_a_state = sum2(population);
	adapt::Bpos texas_index = { 1 };
	std::cout << total_population_in_a_state(usa, texas_index).i32() << std::endl;// 2304580 + 151950 + 83701 + 1304379 + 256684 + 246918 = 4348212

	auto average_population_density = tostr(mean3(population / area) * 2.589988) + "(/mi^2)";
	assert(average_population_density.GetLayer() == -1_layer);
	// No need to specify the index when the layer of the result is -1, i.e., the root layer.
	std::cout << average_population_density(usa).str() << std::endl;// 5362.28(/mi^2).
```

## Data processing based on ranges

### Traverse
```cpp
	// Use traversers to iterate over the data.
	for (const auto& trav : usa | GetRange(1_layer))// Traverse layer 1.
	{
		// All the names of states, counties and county seats are output.
		// During traversal of layer 1, you can access the fields from layer -1 to 1.
		std::cout << std::format("{:<12} {:<12} {:<12}\n", trav[state], trav[county], trav[county_seat]);
	}
	// Output:
	// California   Los Angeles County   Los Angeles
	// California   San Diego County     San Diego
	// Texas        Harris County        Houston
	// Texas        Dallas County        Dallas

	for (const auto& trav : usa | GetRange(2_layer))// Traverse layer 2 = cities.
	{
		// All the names of states, counties and cities are output.
		std::cout << std::format("{:<12} {:<12} {:<12}\n", trav[state], trav[county], trav[city]);
	}
	// Output:
	// California   Los Angeles County   Los Angeles
	// California   Los Angeles County   Long Beach
	// California   Los Angeles County   Glendale
	// California   San Diego County     San Diego
	// California   San Diego County     Chula Vista
	// California   San Diego County     Oceanside
	// Texas        Harris County        Houston
	// Texas        Harris County        Pasadena
	// Texas        Harris County        Baytown
	// Texas        Dallas County        Dallas
	// Texas        Dallas County        Irving
	// Texas        Dallas County        Garland
```

### Filter
```cpp
	// traverse with filter
	auto population_density = population / area;
	for (const auto& trav : usa | Filter(city == county_seat) | GetRange(2_layer))
	{
		// All the state/county/city names and population densities of the cities that are the county seats are output.
		std::cout << std::format("{:<12} {:<12} {:<12} {:>7.1f}\n",
					 trav[state], trav[county], trav[city], population_density(trav));
	}
	// Output:
	// California   Los Angeles County   Los Angeles   3275.7
	// California   San Diego County     San Diego     1646.8
	// Texas        Harris County        Houston       1535.7
	// Texas        Dallas County        Dallas        1470.2
```

### Show
Show the data in the container in a formatted way.
`Show` range adapter accepts placeholders or lambda functions as arguments.
```cpp
	usa | Show(state, county, tostr(sum(area)) + "km^2");
	// Output:
	// [   0,   0]       CaliforniaLos Angeles County  1423.800000km^2
	// [   0,   1]       California San Diego County  1073.900000km^2
	// [   1,   0]            Texas    Harris County  1647.800000km^2
	// [   1,   1]            Texas    Dallas County  1209.200000km^2

	usa | Filter(isleast2(area)) | Show(state, city, population, tostr(area) + "km^2");
	// Output:
	// [   0,   0,   2]       California         Glendale      196543    79.300000km^2
	// [   1,   0,   2]            Texas          Baytown       83701    32.700000km^2
	
	usa | Filter(isleast2(area)) | Show("{:>12}, {:>10}, {:>5.1f}x10^3, {:>5.1f}km^2", state, city, population / 1000., area);
	// [   0,   0,   2]  California,   Glendale, 196.5x10^3,  79.3km^2
	// [   1,   0,   2]       Texas,    Baytown,  83.7x10^3,  32.7km^2
```

### Extract
Create a new Tree by extracting the data from the original Tree/Table.
The arguments of `Extract` become the fields of the new Tree,
and the hierarchical structure is automatically determined by the layers of the arguments.
```cpp
	auto population_density = population / area;
	// Create a new container with the fields state, county, city and population_density.
	// When ADAPT_EXTRACT is used, the names of the fields are automatically determined from the variable names,
	// or you can also specify the names individually like `Extract(state.named("foo"), county.named("bar"), ...)`.
	// If no names are specified, like `Extract(state, county, city, population_density)`, default names "fld0", "fld1", "fld2", "fld3" are used.
	auto extracted = usa | Filter(area > 500.) | ADAPT_EXTRACT(state, county, city, population_density);
	// If all the arguments of Extract are Ctti, the return value is an STree.
	// Otherwise, the return value is a DTree.
	extracted.ShowHierarchy();
	// Output:
	// Layer[-1] { }
	// Layer[ 0] { { "state", Str } }
	// Layer[ 1] { { "county", Str } }
	// Layer[ 2] { { "city", Str } { "population_density", F64 } }

	auto [estate, ecounty, ecity, epopulation_density] = extracted.GetPlaceholders("state", "county", "city", "population_density");
	extracted | Show(estate, if_(len(ecounty) >= 16, substr(ecounty, 0, 13) + "...", ecounty), ecity, epopulation_density);
	// Output:
	// [   0,   0,   0]       California Los Angeles C...      Los Angeles      3275.658621
	// [   0,   1,   0]       California San Diego Cou...        San Diego      1646.796485
	// [   1,   0,   0]            Texas    Harris County          Houston      1535.670021
	// [   1,   1,   0]            Texas    Dallas County           Dallas      1470.219793
```

## Data visualization powered by gnuplot

### Plot data
```cpp
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
```
<img width="480" alt="Area vs Population of Cities in California" src="https://github.com/user-attachments/assets/598bf0f5-110d-4d30-bfaa-329d448e2c99">

### Other plotting functionalities
<table>
	<thead>
		<tr><th scope="col" colspan="3">plot examples</th></tr>
	</thead>
	<tbody>
		<tr>
			<th scope="col">
				<figure>
					<img width="720" height="540" alt="example_scatter-inmemory" src="https://github.com/user-attachments/assets/23abf29f-ccb0-4265-80c6-c195a0a6e613">
					<figurecaption><a href="https://github.com/thayakawa-gh/OpenADAPT/blob/62ad5419371d3a279c9a8d432f45ea6b03715032/Examples/en/quickstart_plot.cpp#L136">example scatter</a></figurecaption>
				</figure>
			</th>
			<th scope="col">
				<figure><img width="720" height="540" alt="example_labels-inmemory" src="https://github.com/user-attachments/assets/86c727ff-7700-4934-b977-b8257f62b931">
					<figurecaption><a href="https://github.com/thayakawa-gh/OpenADAPT/blob/62ad5419371d3a279c9a8d432f45ea6b03715032/Examples/en/quickstart_plot.cpp#L160">example labels</a></figurecaption>
				</figure>
			</th>
			<th scope="col">
				<figure><figurecaption></figurecaption>
				</figure>
			</th>
		</tr>
	</tbody>
	<tbody>
		<tr>
			<th scope="col">
				<figure>
					<img width="720" height="540" alt="example_2d-inmemory" src="https://github.com/user-attachments/assets/6015031c-eba9-47a1-9ccd-e386199a8159">
					<figurecaption><a href="https://github.com/thayakawa-gh/OpenADAPT/blob/62ad5419371d3a279c9a8d432f45ea6b03715032/Examples/en/quickstart_plot.cpp#L6">example 2d</a></figurecaption>
				</figure>
			</th>
			<th scope="col">
				<figure>
					<img width="720" height="540" alt="example_histogram-inmemory" src="https://github.com/user-attachments/assets/1a130808-544e-4859-9456-9a0b3835fd53">
					<figurecaption><a href="https://github.com/thayakawa-gh/OpenADAPT/blob/62ad5419371d3a279c9a8d432f45ea6b03715032/Examples/en/quickstart_plot.cpp#L90">example histogram</a></figurecaption>
				</figure>
			</th>
			<th scope="col">
				<figure>
					<img width="720" height="540" alt="example_string_label-inmemory" src="https://github.com/user-attachments/assets/5281afb1-db81-4db4-b084-dca516ec02d6">
					<figurecaption><a href="https://github.com/thayakawa-gh/OpenADAPT/blob/62ad5419371d3a279c9a8d432f45ea6b03715032/Examples/en/quickstart_plot.cpp#L595">example string</a></figurecaption>
				</figure>
			</th>
		</tr>
	</tbody>
	<tbody>
		<tr>
			<th scope="col">
				<figure><img width="720" height="540" alt="example_filledcurve-inmemory" src="https://github.com/user-attachments/assets/8c054de2-437e-45c5-b3d2-861a3db2016d">
					<figurecaption><a href="https://github.com/thayakawa-gh/OpenADAPT/blob/62ad5419371d3a279c9a8d432f45ea6b03715032/Examples/en/quickstart_plot.cpp#L470">example filledcurve</a></figurecaption>
				</figure>
			</th>
			<th scope="col">
				<figure><img width="720" height="540" alt="example_datetime-inmemory" src="https://github.com/user-attachments/assets/19c05a63-a57c-4a51-8474-8957da07ebc3">
					<figurecaption><a href="https://github.com/thayakawa-gh/OpenADAPT/blob/62ad5419371d3a279c9a8d432f45ea6b03715032/Examples/en/quickstart_plot.cpp#L564">example datetime</a></figurecaption>
				</figure>
			</th>
			<th scope="col">
				<img width="720" height="540" alt="example_for_loop-inmemory" src="https://github.com/user-attachments/assets/ad144e38-ef5c-4d7e-a66e-9165028b73c2"><figure>
					<figurecaption><a href="https://github.com/thayakawa-gh/OpenADAPT/blob/62ad5419371d3a279c9a8d432f45ea6b03715032/Examples/en/quickstart_plot.cpp#L623">example for loop</a></figurecaption>
				</figure>
			</th>
		</tr>
	</tbody>
	<tbody>
		<tr>
			<th scope="col" colspan="2">
				<figure>
					<img width="1200" height="600" alt="example_colormap-inmemory" src="https://github.com/user-attachments/assets/2b747891-2328-41f3-a63b-ab159b161e23">
					<figurecaption><a href="https://github.com/thayakawa-gh/OpenADAPT/blob/62ad5419371d3a279c9a8d432f45ea6b03715032/Examples/en/quickstart_plot.cpp#L223">example colormap</a></figurecaption>
				</figure>
			</th>
			<th scope="col">
				<figure>
					<img width="720" height="540" alt="example_surface-inmemory" src="https://github.com/user-attachments/assets/faee143b-232d-481c-b730-ddd4e19b4ef7">
					<figurecaption><a href="https://github.com/thayakawa-gh/OpenADAPT/blob/62ad5419371d3a279c9a8d432f45ea6b03715032/Examples/en/quickstart_plot.cpp#L409">example surface</a></figurecaption>
				</figure>
			</th>
		</tr>
	</tbody>
	<tbody>
		<tr>
			<th scope="col" colspan="2">
				<figure>
					<img width="1200" height="600" alt="example_binscatter-inmemory" src="https://github.com/user-attachments/assets/0663bfcf-2958-43e6-bac2-ae3b472aaf8d">
					<figurecaption><a href="https://github.com/thayakawa-gh/OpenADAPT/blob/62ad5419371d3a279c9a8d432f45ea6b03715032/Examples/en/quickstart_plot.cpp#L336">example binscatter</a></figurecaption>
				</figure>
			</th>
			<th scope="col">
				<figure>
					<img width="720" height="540" alt="example_labels_on_colormap-inmemory" src="https://github.com/user-attachments/assets/7d32c01d-afc5-486b-b1ba-8ab7abf4d8d4">
					<figurecaption><a href="https://github.com/thayakawa-gh/OpenADAPT/blob/62ad5419371d3a279c9a8d432f45ea6b03715032/Examples/en/quickstart_plot.cpp#L377">example labels on colormap</a></figurecaption>
				</figure>
			</th>
		</tr>
	</tbody>
</table>
