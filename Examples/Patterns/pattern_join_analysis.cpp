#include <format>
#include <iostream>
#include <string>
#ifdef ADAPT_IMPORT_MODULE
#include <OpenADAPT/Macros.h>
import adapt;
#else
#include <OpenADAPT/ADAPT.h>
#endif

using namespace adapt::lit;

namespace
{

std::pair<adapt::DTree, adapt::DTree> MakeSalesAndCatalog()
{
	// Purpose:
	// Show a practical key-join workflow where operational records are enriched by master data.
	//
	// When to use:
	// * One container stores events or transactions.
	// * Another container stores lookup/master data keyed by a shared identifier.
	// * The analysis needs both transactional fields and joined metadata.
	//
	// Key operations shown below:
	// * key-based join
	// * joined placeholders from multiple ranks
	// * delayed join to tolerate missing master data
	// * aggregate over joined child rows
	//
	// revenue_thousand_usd below means revenue measured in thousand USD.

	using enum adapt::FieldType;

	adapt::DTree sales;
	sales.SetTopLayer({ { "company", Str } });
	sales.AddLayer({ { "channel", Str } });
	sales.AddLayer({ { "product_id", Str }, { "product_name", Str }, { "units", I32 }, { "revenue_thousand_usd", F64 } });
	sales.VerifyStructure();
	sales.SetTopFields("OpenADAPT Store");
	sales.Reserve(2);
	sales.Push("Online");
	sales.Push("Retail");

	auto online = sales[0];
	online.Reserve(3);
	online.Push("P-100", "Edge Sensor", 120, 84.0);
	online.Push("P-200", "Flow Meter", 90, 67.5);
	online.Push("P-999", "Legacy Relay", 30, 9.0);

	auto retail = sales[1];
	retail.Reserve(3);
	retail.Push("P-100", "Edge Sensor", 70, 49.0);
	retail.Push("P-300", "Vision Kit", 40, 88.0);
	retail.Push("P-400", "Controller", 65, 71.5);

	adapt::DTree catalog;
	catalog.AddLayer({ { "product_id", Str }, { "category", Str } });
	catalog.AddLayer({ { "supplier", Str }, { "lead_days", I32 } });
	catalog.VerifyStructure();
	catalog.Reserve(4);
	catalog.Push("P-100", "Sensors");
	catalog.Push("P-200", "Meters");
	catalog.Push("P-300", "Vision");
	catalog.Push("P-400", "Control");

	auto p100 = catalog[0];
	p100.Reserve(2);
	p100.Push("NorthWorks", 12);
	p100.Push("BluePeak", 18);

	auto p200 = catalog[1];
	p200.Reserve(1);
	p200.Push("NorthWorks", 10);

	auto p300 = catalog[2];
	p300.Reserve(2);
	p300.Push("VisionForge", 16);
	p300.Push("BluePeak", 20);

	auto p400 = catalog[3];
	p400.Reserve(1);
	p400.Push("ControlHub", 8);

	return { std::move(sales), std::move(catalog) };
}

}

void PatternJoinAnalysis()
{
	std::cout << "[[Pattern Join Analysis]]" << std::endl;

	auto [sales, catalog] = MakeSalesAndCatalog();
	auto jt = Join(sales, 1_layer, 0_layer, catalog);
	auto sales_product_id = jt.GetPlaceholder<0_rank>("product_id");
	auto catalog_product_id = catalog.GetPlaceholder("product_id");
	jt.SetKeyJoint<1_rank>(sales_product_id, catalog_product_id.str());

	auto [jt0_channel, jt0_product_id, jt0_product_name, jt0_units, jt0_revenue_thousand_usd] =
		jt.GetPlaceholders<0_rank>("channel", "product_id", "product_name", "units", "revenue_thousand_usd");
	auto [jt1_category, jt1_supplier, jt1_lead_days] = jt.GetPlaceholders<1_rank>("category", "supplier", "lead_days");

	auto supplier_count = countall(jt1_supplier);
	auto average_lead_days = mean(cast_f64(jt1_lead_days));
	auto revenue_thousand_usd_per_unit = jt0_revenue_thousand_usd / cast_f64(jt0_units);
	auto supply_summary = jt0_product_id + " / " + jt1_category + " / suppliers=" + tostr(supplier_count);

	std::cout << "------Joined product summary------" << std::endl;
	jt
		| Filter(average_lead_days > 11.0)
		| Show("{:>7} {:>12} {:>10} suppliers={:>1} lead={:>5.1f} rpu={:>4.2f}",
			jt0_channel, jt0_product_name, jt1_category, supplier_count, average_lead_days, revenue_thousand_usd_per_unit);
	std::cout << std::endl;

	std::cout << "------Evaluate joined tuples------" << std::endl;
	for (auto [channel_, summary_, lead_days_] : jt
		| Filter(average_lead_days > 11.0)
		| Evaluate(jt0_channel.str(), supply_summary.str(), average_lead_days.f64()))
	{
		std::cout << std::format("{:>7} {} avg_lead={:>5.1f}", channel_, summary_, lead_days_) << std::endl;
	}
	std::cout << std::endl;

	std::cout << "------Delayed join for incomplete master data------" << std::endl;
	for (const auto& trav : jt.GetRange_delayed(1_layer))
	{
		if (!trav.TryJoin(1_rank))
		{
			std::cout << std::format("missing master: {:>7} {:>12}", trav[jt0_channel].str(), trav[jt0_product_id].str()) << std::endl;
			continue;
		}
		std::cout << std::format("matched master: {:>7} {:>12} {:>10}",
			trav[jt0_channel].str(), trav[jt0_product_id].str(), trav[jt1_category].str()) << std::endl;
	}
	std::cout << std::endl;
}
