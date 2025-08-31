#include <format>
#include <iostream>
#include <OpenADAPT/ADAPT.h>

using namespace adapt::lit;

auto MakeSHist()
{
	// Define a SHist, which has a binned structure similar to N-dimensional histogram.
	// The layer 0 element corresponds to a bin of the histogram, and each bin contains the list of layer 1 elements that fall into that bin.

	// Users can define the axes fields of the histogram using SetAxesAttr method.

	// For example, the following code defines a SHist with two axes: "population" and "area", and an additional field: "name".
	// First, the layer 1 fields are defined using ADAPT_S_DEFINE_LAYER macro.
	// Note that the type of the axes is required to be double for SHist.
	using Layer1 = ADAPT_S_DEFINE_LAYER(population, double, area, double, name, std::string);
	// Then the SHist is defined using the Layer1 type and its dimensions (2 in this case).
	using SHist_ = adapt::SHist<2, Layer1>;

	// SHist also requires the axes attributes to be set, which define the binning of the histogram.
	// Unlike DHist, SHist already knows the field names and their types, thus it only needs the bin width and center of each axis.
	std::vector<adapt::AxisAttr> axis{ { 500000. }, { 200. } };
	SHist_ usa;
	usa.SetAxesAttr(axis);

	// The layer -1 and 0 fields are automatically created based on the axes attributes.
	// The layer -1 fields are the minimum and maximum bin indices of each axis, the bin widths, and the bin centers.
	// The layer 0 fields are the bin indices of each axis.
	usa.ShowHierarchy();
	// Layer[-1] { { "min0", int } { "min1", int } { "max0", int } { "max1", int } { "wbin0", double } { "wbin1", double } { "cbin0", double } { "cbin1", double } }
	// Layer[ 0] { { "ibin0", int } { "ibin1", int } }
	// Layer[ 1] { { "population", double } { "area", double } { "name", class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > } }


	return usa;
}

template <adapt::s_hist SHist_>
void StoreData(SHist_& usa)
{
	// Store data in the SHist using Append method.
	// Appended elements are automatically assigned to the appropriate bin based on the axes attributes.
	// The bin indices are calculated based on the following formula:
	// ibin[axis] = static_cast<int32_t>(std::floor((value[axis] - cbin[axis]) / wbin[axis]));

	// When storing data in SHist using Append, the bin range is automatically expanded if necessary.
	// Or you can use Resize to expand the bin range in advance,
	// in order to avoid the overhead of expanding the bin range multiple times.
	//adapt::Bin2D min{ 0, 0 }; // Minimum bin indices for each axis.
	//adapt::Bin2D max{ 7, 7 }; // Maximum bin indices for each axis.
	//usa.Resize(min, max);

	usa.Append(std::forward_as_tuple(3976322., 1213.9), std::forward_as_tuple("Los Angeles"));
	usa.Append(std::forward_as_tuple(466742., 130.6), std::forward_as_tuple("Long Beach"));
	usa.Append(std::forward_as_tuple(196543., 79.3), std::forward_as_tuple("Glendale"));
	usa.Append(std::forward_as_tuple(1386932., 842.2), std::forward_as_tuple("San Diego"));
	usa.Append(std::forward_as_tuple(275487., 126.6), std::forward_as_tuple("Chula Vista"));
	usa.Append(std::forward_as_tuple(174068., 105.1), std::forward_as_tuple("Oceanside"));
	usa.Append(std::forward_as_tuple(2304580., 1500.7), std::forward_as_tuple("Houston"));
	usa.Append(std::forward_as_tuple(151950., 114.4), std::forward_as_tuple("Pasadena"));
	usa.Append(std::forward_as_tuple(83701., 32.7), std::forward_as_tuple("Baytown"));
	usa.Append(std::forward_as_tuple(1304379., 887.2), std::forward_as_tuple("Dallas"));
	usa.Append(std::forward_as_tuple(256684., 174.1), std::forward_as_tuple("Irving"));
	usa.Append(std::forward_as_tuple(246918., 147.9), std::forward_as_tuple("Garland"));
}

void QuickstartSHist()
{
	std::cout << "[[Quickstart SHist]]" << std::endl;
	std::cout << "------Create------" << std::endl;
	auto usa = MakeSHist();
	std::cout << std::endl;

	// You can also create a SHist from another container by using Hist (or ADAPT_HIST macro) range conversion.
	// Please make sure that all the placeholders or lambdas used in Hist are Ctti,
	// that is, they must have compile-time type and layer information.
	// auto [stree_population, stree_area] = stree.GetPlaceholders("population"_fld, "area"_fld);// Get Ctti placeholders.
	// auto fpopulation = cast_f64(dtree_population);
	// auto farea = cast_f64(dtree_area);// Both fpopulation and farea are Ctti lambda functions.
	// auto usa = stree | Filter(...) | Hist(fpopulation, 500000., farea, 200.);

	std::cout << "------Store data------" << std::endl;
	StoreData(usa);

	std::cout << std::endl;



	std::cout << "------Histogram structure------" << std::endl;
	auto [xmin, xmax] = usa.GetAxisRange(0);
	std::cout << "Axis 0: " << xmin << " to " << xmax << std::endl;// Axis 0: 0 to 7
	auto [ymin, ymax] = usa.GetAxisRange(1);
	std::cout << "Axis 1: " << ymin << " to " << ymax << std::endl;// Axis 1: 0 to 7

	std::cout << std::endl;



	std::cout << "------Show histogram data------" << std::endl;
	ADAPT_GET_PLACEHOLDERS(usa, ibin0, ibin1, population, area, name);
	usa | Show(" bin[{:>1},{:>1}] size:{:>2}", ibin0, ibin1, size(area));
	// [   0] bin[0, 0] size: 8
	// [   1] bin[0, 1] size: 0
	// [   1] bin[0, 1] size: 0
	// [   2] bin[0, 2] size: 0
	// [   3] bin[0, 3] size: 0
	// [   4] bin[0, 4] size: 0
	// [   5] bin[0, 5] size: 0
	// [   6] bin[0, 6] size: 0
	// [   7] bin[0, 7] size: 0
	// [   8] bin[1, 0] size: 0
	// ...
	// [  19] bin[2, 3] size: 0
	// [  20] bin[2, 4] size: 2
	// [  21] bin[2, 5] size: 0
	// ...
	// [  38] bin[4, 6] size: 0
	// [  39] bin[4, 7] size: 1
	// [  40] bin[5, 0] size: 0
	// ...
	// [  61] bin[7, 5] size: 0
	// [  62] bin[7, 6] size: 1
	// [  63] bin[7, 7] size: 0
	std::cout << std::endl;

	usa | Show(" bin[{:>1},{:>1}] {:>12} population:{:>6.1f}x10^3 area:{:>6.1f}km^2", ibin0, ibin1, name, population / 1000., area);
	// [   0,   0] bin[0, 0]   Long Beach population: 466.7x10^3 area: 130.6km^2
	// [   0,   1] bin[0, 0]     Glendale population: 196.5x10^3 area:  79.3km^2
	// [   0,   2] bin[0, 0]  Chula Vista population: 275.5x10^3 area: 126.6km^2
	// [   0,   3] bin[0, 0]    Oceanside population: 174.1x10^3 area: 105.1km^2
	// [   0,   4] bin[0, 0]     Pasadena population: 151.9x10^3 area: 114.4km^2
	// [   0,   5] bin[0, 0]      Baytown population:  83.7x10^3 area:  32.7km^2
	// [   0,   6] bin[0, 0]       Irving population: 256.7x10^3 area: 174.1km^2
	// [   0,   7] bin[0, 0]      Garland population: 246.9x10^3 area: 147.9km^2
	// [  20,   0] bin[2, 4]    San Diego population:1386.9x10^3 area: 842.2km^2
	// [  20,   1] bin[2, 4]       Dallas population:1304.4x10^3 area: 887.2km^2
	// [  39,   0] bin[4, 7]      Houston population:2304.6x10^3 area:1500.7km^2
	// [  62,   0] bin[7, 6]  Los Angeles population:3976.3x10^3 area:1213.9km^2
	std::cout << std::endl;



	std::cout << "------Access data------" << std::endl;
	adapt::Bin2D bin{ 2, 4 };
	auto bin_2_4 = usa[bin];
	assert(bin_2_4[ibin0] == 2);
	assert(bin_2_4[ibin1] == 4);
	adapt::BindexType num_of_bin_entries = bin_2_4.GetSize();
	std::cout << "num of bin entries: " << num_of_bin_entries << std::endl; // num of bin entries: 2
	for (adapt::BindexType i = 0; i < num_of_bin_entries; ++i)
	{
		auto entry = bin_2_4[i];

		assert(bin_2_4[ibin0] == static_cast<int32_t>(std::floor(entry[population] / 500000.)));
		assert(bin_2_4[ibin1] == static_cast<int32_t>(std::floor(entry[area] / 200.)));

		std::cout << std::format("{:>12} population:{:>6.1f}x10^3 area:{:>6.1f}km^2\n",
								 entry[name], entry[population] / 1000., entry[area]);
		//   San Diego population:1386.9x10^3 area: 842.2km^2
		//      Dallas population:1304.4x10^3 area: 887.2km^2
	}
	std::cout << std::endl;



	std::cout << "------Calculate------" << std::endl;
	auto max_population_density = greatest(population / area);
	adapt::Bpos pos = usa.BinToBpos(bin);
	std::cout << std::format("max population density in bin{:1>}: {:>6.1f}\n", bin, max_population_density(usa, pos));
	// max population density in bin[2,4]: 1646.8

	auto maxentry = greatest(size(area));
	std::cout << std::format("largest bin entry: {}\n", maxentry(usa)); // largest bin entry: 8

	std::cout << std::endl;



	std::cout << "------Traverse------" << std::endl;
	auto population_density = population / area;
	for (const auto& trav : usa.GetRange(1_layer) | Filter(ibin0 == 0))
	{
		// Traverse the layer 1 elements in the bins [0, x] and output the name, population and area of each city.
		std::cout << std::format("{:<12} {:>7.1f}\n", trav[name], population_density(trav));
		// Long Beach    3573.8
		// Glendale      2478.5
		// Chula Vista   2176.0
		// Oceanside     1656.2
		// Pasadena      1328.2
		// Baytown       2559.7
		// Irving        1474.3
		// Garland       1669.5
	}
	std::cout << std::endl;
}