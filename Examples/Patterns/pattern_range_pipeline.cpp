#include <format>
#include <iostream>
#include <vector>
#include <string>
#ifdef ADAPT_IMPORT_MODULE
#include <OpenADAPT/Macros.h>
import adapt;
#else
#include <OpenADAPT/ADAPT.h>
#endif

using namespace adapt;
using namespace adapt::lit;

namespace
{

adapt::DTable MakeTelemetryTable()
{
	// Purpose:
	// Show a flat time-series workflow where typed placeholders, positional access and
	// range adapters are combined into a reusable event-detection pipeline.
	//
	// When to use:
	// * The data is sequential and each row depends on nearby rows.
	// * You want to compare the current sample with the previous sample or with a baseline sample.
	// * You want to turn derived events into extracted reports or vectors for downstream tools.
	//
	// Key operations shown below:
	// * ADAPT_GET_TYPED_PLACEHOLDERS for a DTable
	// * pos0() and at(...) for row-relative access
	// * one Filter condition built from multiple predicates
	// * Evaluate / Extract / ToVector on derived time-series metrics

	using enum adapt::FieldType;
	adapt::DTable t;
	ADAPT_D_SET_TOP_LAYER(t, line, Str, sensor_id, Str);
	ADAPT_D_SET_LAYER(t, 0,
		minute, I32,
		temperature_c, F64,
		pressure_kpa, F64,
		vibration_mm_s, F64);
	t.VerifyStructure();
	t.SetTopFields("Line-7", "T-204");
	return t;
}

void StoreTelemetry(adapt::DTable& t)
{
	t.Reserve(9);
	t.Push(0,  61.0, 101.0, 0.40);
	t.Push(1,  61.4, 101.6, 0.42);
	t.Push(2,  62.1, 102.4, 0.44);
	t.Push(3,  62.3, 103.0, 0.47);
	t.Push(4,  69.8, 115.5, 0.92);
	t.Push(5,  70.6, 116.2, 0.95);
	t.Push(6,  71.0, 117.0, 0.98);
	t.Push(7,  64.2, 108.1, 0.55);
	t.Push(8,  63.8, 107.2, 0.50);
}

}

void PatternRangePipeline()
{
	std::cout << "[[Pattern Range Pipeline]]" << std::endl;

	auto telemetry = MakeTelemetryTable();
	StoreTelemetry(telemetry);
	ADAPT_GET_TYPED_PLACEHOLDERS(telemetry,
		line, Str,
		sensor_id, Str,
		minute, I32,
		temperature_c, F64,
		pressure_kpa, F64,
		vibration_mm_s, F64);

	auto row_pos = telemetry.pos0();
	auto baseline_temperature_c = temperature_c.at(0);
	auto baseline_pressure_kpa = pressure_kpa.at(0);
	auto delta_temperature_c = temperature_c - temperature_c.at(row_pos - 1);
	auto delta_pressure_kpa = pressure_kpa - pressure_kpa.at(row_pos - 1);
	auto delta_vibration_mm_s = vibration_mm_s - vibration_mm_s.at(row_pos - 1);
	auto temperature_rise_from_baseline_c = temperature_c - baseline_temperature_c;
	auto pressure_rise_from_baseline_kpa = pressure_kpa - baseline_pressure_kpa;
	auto event_label = if_(delta_temperature_c > 6.0,
		"rapid-heating",
		if_(delta_pressure_kpa > 10.0,
			"pressure-jump",
			if_(delta_vibration_mm_s > 0.30,
				"vibration-spike",
				"stable")));
	auto event_summary = tostr(minute) + "min " + sensor_id + " " + event_label;

	std::cout << "------Evaluate samples against the previous row------" << std::endl;
	for (auto [minute_, dtemp_, dpress_, label_] : telemetry
		| Filter(row_pos > 0)
		| Evaluate(minute, delta_temperature_c, delta_pressure_kpa, event_label))
	{
		std::cout << std::format("t={:>2} dT={:>5.1f} dP={:>5.1f} {}", minute_, dtemp_, dpress_, label_) << std::endl;
	}
	std::cout << std::endl;

	std::cout << "------Extract abrupt events------" << std::endl;
	auto abrupt_events = telemetry
		| Filter(row_pos > 0 && (delta_temperature_c > 6.0 || delta_pressure_kpa > 10.0 || delta_vibration_mm_s > 0.30))
		| Extract(minute.named("minute"),
			temperature_c.named("temperature_c"),
			delta_temperature_c.named("delta_temperature_c"),
			pressure_rise_from_baseline_kpa.named("pressure_rise_from_baseline_kpa"),
			event_label.named("event_label"),
			event_summary.named("event_summary"));
	auto [event_minute, event_temperature_c, event_delta_temperature_c, event_pressure_rise_from_baseline_kpa, event_label_, event_summary_] =
		abrupt_events.GetPlaceholders("minute", "temperature_c", "delta_temperature_c", "pressure_rise_from_baseline_kpa", "event_label", "event_summary");
	abrupt_events | Show("t={:>2} temp={:>5.1f} dT={:>5.1f} dP0={:>5.1f} {:>15} <{}>",
		event_minute, event_temperature_c, event_delta_temperature_c, event_pressure_rise_from_baseline_kpa, event_label_, event_summary_);
	std::cout << std::endl;

	std::cout << "------Convert derived metrics to vectors------" << std::endl;
	auto [vminute, vtemperature_rise_from_baseline_c, vpressure_rise_from_baseline_kpa] = telemetry
		| ToVector(minute, temperature_rise_from_baseline_c, pressure_rise_from_baseline_kpa);
	std::cout << std::format("samples={}, last rise=({:.1f}C, {:.1f}kPa) at t={}",
		vminute.size(),
		vtemperature_rise_from_baseline_c.back(),
		vpressure_rise_from_baseline_kpa.back(),
		vminute.back()) << std::endl;
	std::cout << std::endl;
}