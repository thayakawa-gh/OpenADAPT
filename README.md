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
target_link_libraries(YOUR_PACKAGE_NAME OpenADAPT::OpenADAPT)
```

The test and example codes are built by adding `-DBUILD_TEST=ON` and `-DBUILD_EXAMPLES` to the cmake command, respectively. Note that GTest, yaml-cpp and matplot++ are required for these builds.

## Examples

### Store data in a container
```cpp
	using enum adapt::FieldType;
	adapt::DTree t;
	t.SetTopLayer({ { "school", Str } });
	t.AddLayer({ { "grade", I08 }, { "class", I08 } });
	t.AddLayer({ { "number", I16 }, { "name", Str } });
	t.AddLayer({ { "exam", I08 },  {"jpn", I32}, {"math", I32}, {"eng", I32}, {"sci", I32}, {"soc", I32 } });
	t.VerifyStructure();
	
	t.SetTopFields("胴差県立散布流中学校");

	t.Reserve(1);
	t.Push((int8_t)3, (int8_t)1);

	auto cls_1 = t[0];
	cls_1.Reserve(3);
	cls_1.Push((int16_t)0, "濃伊田美衣子");
	cls_1.Push((int16_t)1, "角兎野誠人");
	cls_1.Push((int16_t)2, "子虚烏有花");

	auto dummy_dummy_ko = cls_1[0];
	dummy_dummy_ko.Reserve(4);
	dummy_dummy_ko.Push((int8_t)0, 46, 10, 32, 3, 29);
	dummy_dummy_ko.Push((int8_t)1, 65, 21, 35, 0, 18);
	dummy_dummy_ko.Push((int8_t)2, 50, 15, 44, 22, 37);
	dummy_dummy_ko.Push((int8_t)3, 48, 8, 24, 11, 26);

	auto kakuu_no_seito = cls_1[1];
	kakuu_no_seito.Reserve(4);
	kakuu_no_seito.Push((int8_t)0, 76, 91, 88, 94, 69);
	kakuu_no_seito.Push((int8_t)1, 80, 99, 74, 85, 71);
	kakuu_no_seito.Push((int8_t)2, 72, 95, 72, 93, 75);
	kakuu_no_seito.Push((int8_t)3, 84, 89, 77, 83, 82);

	auto shikyo_uyuu_ka = cls_1[2];
	shikyo_uyuu_ka.Push((int8_t)0, 98, 71, 85, 64, 99);
	shikyo_uyuu_ka.Push((int8_t)1, 86, 69, 91, 70, 100);
	shikyo_uyuu_ka.Push((int8_t)2, 90, 75, 78, 80, 92);
	shikyo_uyuu_ka.Push((int8_t)3, 94, 67, 81, 76, 96);
```

### Access, traverse and extract data
```cpp
	auto [name, jpn, math, eng] = t.GetPlaceholders("name", "jpn", "math", "eng");
	auto dummy_dummy_ko = t[0][0];
	std::cout << dummy_dummy_ko[name].str() << std::endl;//Output: 濃伊田美衣子

	using Lambda = adapt::eval::RttiFuncNode<adapt::DTree>;
	Lambda total_3subjs = jpn + math + eng;
	std::cout << total_3subjs(t, adapt::Bpos{ 0, 0, 1 }).i32() << std::endl;//Output: 121

	Lambda exist_240 = exist(total_3subjs >= 240);
	for (auto& trav : t | Filter(exist_240) | GetRange(2_layer))
	{
		std::cout << std::format("{}: {}\n", trav[name].str(), total_3subjs(trav).i32());
		//Output: 
		//角兎野誠人: 255
		//角兎野誠人: 253
		//角兎野誠人: 239
		//角兎野誠人: 250
		//子虚烏有花: 254
		//子虚烏有花: 246
		//子虚烏有花: 243
		//子虚烏有花: 242
	}

	t | Filter(isgreatest(math.at(0))) | Show(name, math.at(0));//Output: [   0,   1]  角兎野誠人     91

	adapt::DTree e = t | Filter(eng < 35) | Extract(name, eng, greatest(eng), least(eng));
	auto [fld0, fld1, fld2, fld3] = e.GetPlaceholders("fld0", "fld1", "fld2", "fld3");
	e | Show(fld0, fld1, fld2, fld3);
	//Output: 
	//[   0,   0,   0] 濃伊田美衣子          32          44          24
	//[   0,   0,   1] 濃伊田美衣子          24          44          24

	matplot::figure_handle f = matplot::figure(true);
	std::vector<double> v_jpn = t | ToVector(cast_f64(jpn.at(2)).f64());
	matplot::hist(v_jpn);
	matplot::save("quickstart_dtree.png");
```

For more detailed examples and explanations, please refer to [Examples/main.cpp](Examples/main.cpp).
