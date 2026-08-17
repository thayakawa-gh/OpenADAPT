void PatternRangePipeline();
void PatternHierarchicalAnalysis();
void PatternJoinAnalysis();

int main()
{
	// Usage patterns intended to be easy to search, reuse and reinterpret.
	// Each file focuses on a practical workflow rather than a full API overview.
	//
	// * pattern_range_pipeline.cpp
	//   Time-series event detection using typed placeholders, pos0() and at(...).
	// * pattern_hierarchical_analysis.cpp
	//   Hierarchical aggregation with sum_if / sum2 and layer-aware ranking.
	// * pattern_join_analysis.cpp
	//   Join followed by filtering, aggregation and report-like extraction.

	PatternRangePipeline();
	PatternHierarchicalAnalysis();
	PatternJoinAnalysis();
	return 0;
}

#include "pattern_range_pipeline.cpp"
#include "pattern_hierarchical_analysis.cpp"
#include "pattern_join_analysis.cpp"
