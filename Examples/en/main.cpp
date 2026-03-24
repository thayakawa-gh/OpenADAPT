void QuickstartDTree();
void QuickstartSTree();
void QuickstartDTable();
void QuickstartSTable();
void QuickstartDHist();
void QuickstartSHist();
void QuickstartParser();
void QuickstartKeyJoin();
void QuickstartCrossJoin();
void DifferenceCttiRttiTyped();
void QuickstartPlot();

int main()
{
	// Examples of how to use ADAPT.
	// ADAPT provides hierarchically structured containers, STree/DTree, and table containers, STable/DTable.
	// 
	// * STree/STable have compile-time structure defined by templates.
	// * DTree/DTable have run-time structure defined by functions.
	// 
	// * STree/STable are optimized for fast access and calculation and can contain any types of data.
	// * DTree/DTable are available vial non-template APIs, but slower than STree/STable
	//   and limited to types declared in adapt::FieldType.
	//
	// * STable/DTable cannot have a hierarchical structure, but faster than STree/DTree.
	//   Use Tree containers when you need parent-child relationships; use Table containers for flat datasets.

	// In addition to them, ADAPT provides SHist/DHist, which have a binned structure similar to N-dimensional histogram.
	// Its layer 0 element corresponds to a bin of the histogram, and each bin contains the list of layer 1 elements that fall into that bin.
	// They derive from STree/DTree, and have additional functionalities for histogram, such as automatic binning when storing data and access with bin indices.

	// The containers in ADAPT are designed to be row-oriented for usability and operability,
	// rather than column-oriented designs commonly used in data-frame libraries.

	// The containers in ADAPT support Join functionality to combine multiple containers.
	// It is similar to Join in SQL/Pandas and other data frame libraries, but unique in its combination with hierarchical structure.
	// Supported joins include key-based joins and cross joins, as shown in the examples below.

	// To access/calculate data in the containers above, ADAPT provides placeholders and lambda functions.
	// Placeholders are used to access data in the containers, and lambda functions are made from placeholders and used to calculate data in the containers.
	// There are 3 types of placeholders/lambda functions: Ctti, Rtti and Typed.
	// * Ctti placeholders and lambda functions have compile-time type/layer information, making them the fastest.
	// * Rtti placeholders and lambda functions have run-time type/layer information, making them easy to manage with std::vector etc. but slowest.
	//   Additionally, they can only hold types declared in adapt::FieldType.
	// * Typed placeholders and lambda functions have compile-time type information, but run-time layer information.
	//   They are designed to be created by adding types to Rtti ones for use with DTree/DTable
	//   when you want compile-time type safety.

	// Any type of container and placeholder/lambda function can be used almost the same way.

	// Basically, we recommend you to use DTree/DTable/DHist with Rtti/Typed and STree/STable/SHist with Ctti.
	// But you can use them in any way you want.

	QuickstartDTree();
	QuickstartSTree();
	QuickstartDTable();
	QuickstartSTable();
	QuickstartDHist();
	QuickstartSHist();

	QuickstartKeyJoin();
	QuickstartCrossJoin();

	DifferenceCttiRttiTyped();

	// The parser example shows how to build lambda functions from string expressions for DTree/DTable.
	QuickstartParser();

	// The plotting example demonstrates ADAPT's gnuplot-based visualization utilities (Canvas2D/Canvas3D)
	// and how to output common chart types.
	QuickstartPlot();

	return 0;
}