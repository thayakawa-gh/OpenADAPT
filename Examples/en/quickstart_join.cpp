#include <format>
#include <iostream>
#include <OpenADAPT/ADAPT.h>


std::pair<adapt::DTree, adapt::DTree> MakeDTrees()
{
	using enum adapt::FieldType;
	adapt::DTree t0;
	{
		t0.SetTopLayer({ { "nation", Str }, { "capital", Str } });// Set root layer (-1).
		t0.AddLayer({ { "state", Str }, { "state capital", Str } });// Add layer 0.
		t0.AddLayer({ { "county", Str }, { "county seat", Str } });// Add layer 1.
		t0.AddLayer({ { "city", Str }, { "population", I32 }, { "area", F64 } });// Add layer 2.
		t0.VerifyStructure();// Verify if the structure is correct.

		t0.SetTopFields("USA", "Washington D.C.");// Set root layer.
		t0.Reserve(2);// Reserve elements for layer 0.
		t0.Push("California", "Sacramento");// Push an element to layer 0.
		t0.Push("Texas", "Austin");

		auto california = t0[0];// Reference layer 0 element.
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

		auto texas = t0[1];
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
	}

	// A list of citizens for each county.
	adapt::DTree t1;
	{
		t1.AddLayer({ { "county", Str } });
		t1.AddLayer({ { "name", Str }, { "age", I32 } });
		t1.VerifyStructure();

		t1.Reserve(4);
		t1.Push("Los Angeles County");
		auto los_angeles_county = t1[0];
		los_angeles_county.Reserve(3);
		los_angeles_county.Push("Ethan Mitchell", 58);
		los_angeles_county.Push("Olivia Foster", 22);
		los_angeles_county.Push("Isabella Morgan", 41);

		t1.Push("Harris County");
		auto harris_county = t1[1];
		harris_county.Reserve(2);
		harris_county.Push("Mason Johnson", 70);
		harris_county.Push("Logan Peterson", 18);

		t1.Push("San Diego County");
		auto san_diego_county = t1[2];
		san_diego_county.Reserve(1);
		san_diego_county.Push("Liam Reynolds", 35);

		t1.Push("Bexar County");
		auto bexar_county = t1[3];
		bexar_county.Reserve(1);
		bexar_county.Push("Noah Carter", 66);
	}

	return { std::move(t0), std::move(t1) };
}

void QuickstartJoin()
{
	using namespace adapt::lit;
	// t0 is the same as that in quickstart_dtree/stree.
	// t1 is a list of citizens for each county.
	auto [t0, t1] = MakeDTrees();

	// Join in ADAPT is similar to SQL/Pandas join, but combined with hierarchical structure.
	// For example, we have two trees, t0 and t1, and we want to join them by county.
	// Now, "county" in t0 is in the layer 1, and "county" in t1 is in the layer 0.
	// So we need to join them by t0 layer 1 to t1 layer 0 using KeyJoint: joining by the same keys.

	auto jt = Join(t0, 1_layer, 0_layer, t1);

	// In this case, t0 is rank 0 and t1 is rank 1. If you want, you can join additional trees like as follows:
	// Join(t0, 1_layer, 0_layer, t1, 1_layer, ?_layer, t2, ...)
	// Then we need to set the rule for joining rank 1 container.
	// Here we use "key joint", which is the most common way to join two containers by the same keys.

	auto t1_key_county = t1.GetPlaceholder("county");//Note that the key must be obtained from t1, not jt.
	auto jt0_key_county = jt.GetPlaceholder<0_rank>("county");
	// Set the rule as key joint.
	// Recommend to use Typed/Ctti placeholders or lambda functions for better performance,
	// especially for std::string and the other types that allocate their storage on the heap.
	if constexpr (true) jt.SetKeyJoint<1_rank>(jt0_key_county, t1_key_county.str());
	else jt.SetKeyJoint<1_rank>(jt0_key_county.str(), t1_key_county.str());
	// If you want to join with multiple keys, you can give by std::foward_as_tuple.
	// jt.SetKeyJoint<1_rank>(std::forward_as_tuple(jt0_key_county.str(), ...), std::forward_as_tuple(t2_key_county.str(), ...));

	// In the near future, various ways for joining other than KeyJoint will be added.

	// Finally, the hierarchical structure of jt is as follows:
	//      layer -1     layer 0     layer 1     layer 2
	// t1   "nation" --- "state" ---"county"--- ("city")
	//                                  |
	// t2                           "county"--- citizen

	// The element in t1 is joined to the element in t0 with the same county, and they are considered as the same layer 1 element.
	// The "citizen" element in t1 is considered as layer 2 element in jt.
	// The "city" element in t0 is hidden and restricted to access
	// unless the layer is raised to 1 or higher using "sum", "count", etc. or .at() function.

	// Join is similar to inner join or left join, but strictly speaking, it differs from both,
	// and there is no functionality like right join or outer join in ADAPT.


	std::cout << "------Access------" << std::endl;

	auto [jt0_state, jt0_county, jt0_population, jt0_area]
		= jt.GetPlaceholders<0_rank>("state", "county", "population", "area");
	auto [jt1_county, jt1_name, jt1_age] = jt.GetPlaceholders<1>("county", "name", "age");

	assert(jt0_state.GetLayer() == 0_layer);
	assert(jt0_county.GetLayer() == 1_layer);
	assert(jt1_name.GetLayer() == 2_layer);
	assert(jt1_age.GetLayer() == 2_layer);

	// Currently operator[] cannot be used for joined containers due to technical limitation.
	// Instead, you can use GetBranch with adapt::Bpos.
	adapt::Bpos bpos_to_liam{ 1, 0, 1 };// Texas -> Harris County -> Logan Peterson
	std::cout << jt.GetBranch(bpos_to_liam)[jt1_name].str() << std::endl;// Logan Peterson
	std::cout << jt.GetBranch(bpos_to_liam)[jt1_age].i32() << std::endl;// 35
	std::cout << jt.GetBranch(bpos_to_liam)[jt0_state].str() << std::endl;// Texas
	std::cout << jt.GetBranch(bpos_to_liam)[jt0_county].str() << std::endl;// Harris County
	// std::cout << jt.GetBranch(bpos)[jt0_population].i32();//Error: population is hidden.

	adapt::Bpos bpos_to_dallas{ 1, 1 };//Texas -> Dallas County
	try
	{
		// Joint to t1 is delayed until t1 element is accessed.
		// Although there is no Dallas elements in t1, the following code is valid due to no access to t1.
		std::cout << jt.GetBranch(bpos_to_dallas)[jt0_county].str() << std::endl;//Dallas County
		// But the following throws an exception because jt tries to access t1 and cannot find Dallas County.
		std::cout << jt.GetBranch(bpos_to_dallas)[jt1_county].str();
	}
	catch (adapt::JointError)
	{
		std::cout << "cannot access Dallas " << std::endl;
	}
	std::cout << std::endl;


	std::cout << "------Calculate------" << std::endl;

	using Lambda = adapt::eval::RttiFuncNode<decltype(jt)>;
	// Lambda functions can be used for joined containers in the same way as single containers.
	Lambda profile = "county: " + jt0_county + ", name: " + jt1_name + ", age: " + tostr(jt1_age);
	std::cout << profile(jt, bpos_to_liam).str() << std::endl;// county: Harris County, name: Logan Peterson, age: 18

	Lambda num_of_citizens_over_30 = count2(jt1_age > 30);
	adapt::Bpos bpos_to_california{ 0 };// California
	std::cout << num_of_citizens_over_30(jt, bpos_to_california).i64() << std::endl;// 3: Ethan Mitchell, Isabella Morgan and Liam Reynolds.
	adapt::Bpos bpos_to_texas{ 1 };// Texas
	std::cout << num_of_citizens_over_30(jt, bpos_to_texas).i64() << std::endl;// 1: Mason Johnson

	Lambda total_area_in_a_county = sum(jt0_area);
	assert(total_area_in_a_county.GetLayer() == 1_layer);
	adapt::Bpos bpos_to_harris{ 1, 0 };// Texas -> Harris County
	// The sum of jt0_area can be calculated because sum function raises the layer to 1.
	std::cout << total_area_in_a_county(jt, bpos_to_harris).f64() << std::endl;// 1500.7 + 114.4 + 32.7 = 1647.8

	Lambda citizens_per_area_in_a_county = size(jt1_name) / sum(jt0_area);
	std::cout << citizens_per_area_in_a_county(jt, bpos_to_harris).f64() << std::endl;// 2 / 1647.8 = 0.00121374

	std::cout << std::endl;


	std::cout << "------Traverse------" << std::endl;

	// There are two modes for tranversing: "prompt joint" and "delayed joint".
	// The default mode is prompt joint except when using in Filter, Extract and the other ranges.

	// In prompt joint mode, the lower rank element is immediately joined when the traverser is moved to the next "county" element.
	// If the element in t1 to be joined is not found, the traverser also skips the element in t0.
	// Roughly speaking, this is similar to inner join.
	// When the traverser is created by jt.GetRange(...) or jt | Filter(...) | GetRange(...), the traverser performs as prompt joint mode.
	// If you want to use prompt joint mode explicitly, you can get a range by jt.GetRange_prompt(...).
	Lambda profile2 = "county: " + jt0_county + ", name: " + jt1_name + ", age: " + tostr(jt1_age);
	for (const auto& trav : jt.GetRange_prompt(2_layer))
	{
		// Print all citizens except for Noah Carter, who is in Bexar County not included in t1.
		std::cout << std::format("{}", profile2(trav).str()) << std::endl;
	}
	std::cout << std::endl;
	for (const auto& trav : jt.GetRange_prompt(2_layer) | Filter(jt1_age < 40))
	{
		std::cout << std::format("{:<20} {:<20} {:>2}", trav[jt0_county].str(), trav[jt1_name].str(), trav[jt1_age].i32()) << std::endl;
		// Los Angeles County  Olivia Foster        22
		// San Diego County    Liam Reynolds        35
		// Harris County        Logan Peterson       18
	}
	std::cout << std::endl;

	// In the case of delayed joint, the lower rank element is joined only when the element is accessed or TryJoin is called.
	// Therefore, it is possible to make it behave like pseudo-left join.
	// This is designed for better performance in some range adapters and lambda functions like sum, count, etc.
	// If you want to use delayed joint mode explicitly, you can get a range by jt.GetRange_delayed(...).
	Lambda average_age_in_a_county = mean(cast_f64(jt1_age));
	for (const auto& trav : jt.GetRange_delayed(1_layer))
	{
		// Try to join the element in t1 to the element in t0.
		// If the element in t1 to be joined is found, the element in t1 is joined to that in t0, and true is returned.
		// Otherwise, TryJoint just returns false without joining.
		// When t1 element is accessed despite there is no joinable element, adapt::JointError is thrown.
		if (trav.TryJoin(1_rank))
			std::cout << std::format("{:<20} {:>4.1f}", trav[jt0_county].str(), average_age_in_a_county(trav).f64()) << std::endl;
		else
			// Even if the element in t1 is not found, the element in t0 is still accessible.
			std::cout << std::format("{:<20} no data", trav[jt0_county].str()) << std::endl;
	}

	size_t skip_count = 0;
	// You can also use Filter in delayed joint mode but have to call it after GetRange_delayed.
	for (const auto& trav : jt.GetRange_delayed(2_layer) | Filter(jt0_state == "California"))
	{
		// When traverser in delayed joint mode is moved by operator++ or Move function without accessing t1,
		// it skips joining and moves to the next t0 element.
		if (trav[jt0_county].str() == "Los Angeles County")
		{
			// There are 3 citizens in Los Angeles County.
			// But when moving to the next element without accessing or calling TryJoin to t1 element,
			// the traverser skips all "Los Angeles County" elements in t1 and
			// moves directly to the next element in t0, San Diego County.
			// So finally skip_count is 1.
			++skip_count;
			continue;
		}
		if (!trav.TryJoin(1_rank)) continue;
		std::cout << trav[jt1_name].str() << std::endl;
	}
	std::cout << std::format("skip_count == {}", skip_count) << std::endl;
	std::cout << std::endl;

	std::cout << std::endl;


	std::cout << "------Show------" << std::endl;

	// Show, Extract and the other range adapters/conversions can be used in the same way as single containers.
	jt | Filter(mean(jt1_age) > 35) | Show(jt0_state, substr(jt0_county, 0, 16), substr(jt1_name, 0, 16), jt1_age);
}