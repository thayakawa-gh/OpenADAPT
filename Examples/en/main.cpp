void QuickstartDTree();
void QuickstartSTree();
void QuickstartDTable();
void QuickstartSTable();
void QuickstartJoin();
void DifferenceCttiRttiTyped();
void QuickstartPlot();

int main()
{
	// Examples of how to use ADAPT.
	// ADAPT provides hierarchically structured containers, STree/DTree, and table containers, STable/DTable.
	// 
	// * STree/STable have static structure defined by templates.
	// * DTree/DTable have run-time structure defined by functions.
	// 
	// * STree/STable are optimized for fast access and calculation and can contain any types of data.
	// * DTree/DTable are available without any templates, but slower than STree/STable and can only contain types declared in adapt::FieldType.
	//
	// STable/DTable cannot have a hierarchical structure, but faster than STree/DTree.

	// The containers in ADAPT are designed to be row-oriented for usability and operability,
	// rather than the column-oriented structure that is popular these days.

	// The containers in ADAPT supports Join functionality to combine multiple containers.
	// It is similar to Join in SQL/Pandas and other data frame libraries, but unique in its combination with hierarchical structure.


	// To access/calculate data in the containers above, ADAPT provides placeholders and lambda functions.
	// Placeholders are used to access data in the containers, and lambda functions are made from placeholders and used to calculate data in the containers.
	// There are 3 types of placeholders/lambda functions: Ctti, Rtti and Typed.
	// * Ctti placeholders and lambda functions have compile-time type/layer information, making them the fastest.
	// * Rtti placeholders and lambda functions have run-time type/layer information, making them easy to manage with std::vector etc. but slowest.
	//   Additionally, they can only hold types declared in adapt::FieldType.
	// * Typed placeholders and lambda functions have compile-time type information, but run-time layer information.
	//   Typically, they are created by adding types to Rtti ones.

	// Any type of container and placeholder/lambda function can be used almost the same way.

	// Basically, we recommend you to use DTree/DTable with Rtti/Typed and STree/STable with Ctti.
	// But you can use them in any way you want.


	QuickstartDTree();
	QuickstartSTree();
	QuickstartDTable();
	QuickstartSTable();
	QuickstartJoin();
	DifferenceCttiRttiTyped();
	QuickstartPlot();
	return 0;
}