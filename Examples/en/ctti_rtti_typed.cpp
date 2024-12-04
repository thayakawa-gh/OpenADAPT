#include <OpenADAPT/ADAPT.h>

template <class Container>
void StoreData(Container& t)
{
	t.SetTopFields(0);

	t.Reserve(1);
	t.Push(1.0, 2.0);

	if constexpr (adapt::any_tree<Container>)
	{
		auto ref = t.Back();
		ref.Reserve(1);
		if constexpr (adapt::s_tree<Container>)
			ref.Push(std::vector<int>{ 3, 4 }, "5");
		else
			ref.Push(34, "5");
	}
}

void DifferenceCttiRttiTyped()
{
	// In this function, we will show the difference between Ctti, Typed and Rtti placeholders.
	// They can be used almost the same way, but there are some differences.

	using TopLayer = adapt::NamedTuple<adapt::Named<"a", int>>;
	using Layer0 = adapt::NamedTuple<adapt::Named<"b", double>, adapt::Named<"c", double>>;
	using Layer1 = adapt::NamedTuple<adapt::Named<"d", std::vector<int>>, adapt::Named<"e", std::string>>;

	using STree_ = adapt::STree<TopLayer, Layer0, Layer1>;
	STree_ stree;
	StoreData(stree);

	using STable_ = adapt::STable<TopLayer, Layer0>;
	STable_ stable;
	StoreData(stable);

	adapt::DTree dtree;
	using enum adapt::FieldType;
	dtree.SetTopLayer({ { "a", I32 } });
	dtree.AddLayer({ { "b", F64 }, { "c", F64 } });
	dtree.AddLayer({ { "d", I64 }, { "e", Str } });
	dtree.VerifyStructure();
	StoreData(dtree);

	adapt::DTable dtable;
	dtable.SetTopLayer({ { "a", I32 } });
	dtable.SetLayer(0, { { "b", F64 }, { "c", F64 } });
	dtable.VerifyStructure();
	StoreData(dtable);



	// #------How to get Ctti placeholders------

	// ## STree/STable
	//    Use GetPlaceholder or GetPlaceholders and arguments with _fld literals. 
	using namespace adapt::lit;
	auto [a_stree_ctti, b_stree_ctti, d_stree_ctti] = stree.GetPlaceholders("a"_fld, "b"_fld, "d"_fld);
	auto [a_stable_ctti, b_stable_ctti] = stable.GetPlaceholders("a"_fld, "b"_fld);

	// ## DTree/DTable
	//    It is a little bit complicated, you have to use GetPlaceholder with the static type and layer of the fields.
	//    Even if you pass the names with _fld literals, 
	auto a_dtree_ctti = dtree.GetPlaceholder<-1_layer, int32_t>("a");
	auto b_dtree_ctti = dtree.GetPlaceholder<0_layer, double>("b");
	auto a_dtable_ctti = dtable.GetPlaceholder<-1_layer, int32_t>("a");
	auto b_dtable_ctti = dtable.GetPlaceholder<0_layer, double>("b");



	// #------How to get Rtti placeholders------
	//    Use GetPlaceholders with no _fld literals.
	//    The return tyope is a std::array of Container::RttiPlaceholder.
	auto [a_stree_rtti, b_stree_rtti] = stree.GetPlaceholders("a", "b");
	auto [a_stable_rtti, b_stable_rtti] = stable.GetPlaceholders("a", "b");
	auto [a_dtree_rtti, b_dtree_rtti] = dtree.GetPlaceholders("a", "b");
	auto [a_dtable_rtti, b_dtable_rtti] = dtable.GetPlaceholders("a", "b");

	//    You cannot use placeholders of the type not declared in the adapt::FieldType.
	//auto d_stree_rtti = stree.GetPlaceholder("d");



	// #------How to get Typed placeholders------
	//    Convert RttiPlaceholders to Typed ones with i32(), str(), f64() etc.
	auto a_stree_typed = a_stree_rtti.i32();
	auto b_stree_typed = b_stree_rtti.f64();
	auto a_stable_typed = a_stable_rtti.i32();
	auto b_stable_typed = b_stable_rtti.f64();
	auto a_dtree_typed = a_dtree_rtti.i32();
	auto b_dtree_typed = b_dtree_rtti.f64();
	auto a_dtable_typed = a_dtable_rtti.i32();
	auto b_dtable_typed = b_dtable_rtti.f64();



	// #------How to use placeholders------
	auto usage = [](auto& container,
		const auto& a_ctti, const auto& b_ctti,
		const auto& a_rtti, const auto& b_rtti,
		const auto& a_typed, const auto& b_typed)
	{
		// No need to call i32(), f64() etc. for Ctti placeholders when accessing to fields.
		std::cout << container[0][b_ctti] << std::endl;// get the value of "a" in the 1st element of STree.
		// Also no need for Typed placeholders.
		std::cout << container[0][b_typed] << std::endl;
		// But you have to call i32(), f64() etc. for Rtti placeholders.
		std::cout << container[0][b_rtti].f64() << std::endl;

		// Lambda functions created from Ctti placeholders are Ctti.
		auto lambda_ctti = a_ctti * b_ctti;
		std::cout << lambda_ctti(container, adapt::Bpos{ 0 }) << std::endl;

		// If the components of the lambda function includes at least one Rtti placeholder, the lambda function becomes Rtti.
		// All Rtti lambda functions have the same type, adapt::eval::RttiFuncNode<Container>, Container is STree, etc.
		// When you call the lambda function, you have to call i32(), f64() etc. for the return value.
		using Container = std::decay_t<decltype(container)>;
		adapt::eval::RttiFuncNode<Container> lambda_rtti = a_rtti * b_ctti;
		std::cout << lambda_rtti(container, adapt::Bpos{ 0 }).f64() << std::endl;

		// If the components of the lambda function includes at least one Typed placeholder, and the others are Ctti,
		// the lambda function becomes Typed.
		auto lambda_typed = a_typed * b_ctti;
		std::cout << lambda_typed(container, adapt::Bpos{ 0 }) << std::endl;

		// You can also convert Rtti lambda functions to pseudo Typed ones.
		// But it is just for giving the static type. The performance is the same as Rtti.
		auto lambda_typed_from_rtti = lambda_rtti.f64();
		std::cout << lambda_typed_from_rtti(container, adapt::Bpos{ 0 }) << std::endl;
	};

	usage(stree, a_stree_ctti, b_stree_ctti, a_stree_rtti, b_stree_rtti, a_stree_typed, b_stree_typed);
	usage(stable, a_stable_ctti, b_stable_ctti, a_stable_rtti, b_stable_rtti, a_stable_typed, b_stable_typed);
	usage(dtree, a_dtree_ctti, b_dtree_ctti, a_dtree_rtti, b_dtree_rtti, a_dtree_typed, b_dtree_typed);
	usage(dtable, a_dtable_ctti, b_dtable_ctti, a_dtable_rtti, b_dtable_rtti, a_dtable_typed, b_dtable_typed);
}