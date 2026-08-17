#include <format>
#include <iostream>
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

auto MakeCompanyTree()
{
	// Purpose:
	// Show practical aggregation over parent/child relationships.
	//
	// When to use:
	// * The data is naturally hierarchical.
	// * Team- or department-level summaries must be computed from child rows.
	// * You want layer-aware lambdas such as sum, sum_if, sum2, count_if and isgreatest.
	//
	// Key operations shown below:
	// * Ctti placeholders
	// * layer-raising aggregations
	// * filtering parent rows by child-derived metrics
	// * ranking / best-member style queries inside each group

	using TopLayer = ADAPT_S_DEFINE_LAYER(company, std::string, fiscal_year, int32_t);
	// budget_kusd / salary_kusd mean values measured in thousand USD.
	using Layer0 = ADAPT_S_DEFINE_LAYER(team, std::string, manager, std::string, budget_kusd, double);
	using Layer1 = ADAPT_S_DEFINE_LAYER(member, std::string, salary_kusd, double, years, int32_t, projects, int32_t);
	using Tree = adapt::STree<TopLayer, Layer0, Layer1>;

	Tree t;
	t.SetTopFields("OpenADAPT Labs", 2025);
	t.Reserve(3);

	t.Push("Core", "Ava", 1200.0);
	t.Push("Applied", "Mia", 980.0);
	t.Push("Platform", "Noah", 1100.0);

	auto core = t[0];
	core.Reserve(3);
	core.Push("Liam", 190.0, 8, 7);
	core.Push("Emma", 160.0, 5, 6);
	core.Push("Lucas", 135.0, 2, 4);

	auto applied = t[1];
	applied.Reserve(4);
	applied.Push("Olivia", 150.0, 4, 5);
	applied.Push("Sophia", 145.0, 6, 5);
	applied.Push("James", 118.0, 2, 3);
	applied.Push("Mason", 108.0, 1, 2);

	auto platform = t[2];
	platform.Reserve(3);
	platform.Push("Ethan", 175.0, 7, 8);
	platform.Push("Amelia", 155.0, 5, 6);
	platform.Push("Harper", 132.0, 3, 4);

	return t;
}

}

void PatternHierarchicalAnalysis()
{
	std::cout << "[[Pattern Hierarchical Analysis]]" << std::endl;

	auto company_tree = MakeCompanyTree();
	auto [company, fiscal_year, team, manager, budget_kusd, member, salary_kusd, years, projects] =
		company_tree.GetPlaceholders("company"_fld, "fiscal_year"_fld, "team"_fld, "manager"_fld,
			"budget_kusd"_fld, "member"_fld, "salary_kusd"_fld, "years"_fld, "projects"_fld);

	// sum(...) raises the layer by one level: member -> team.
	auto team_total_salary_kusd = sum(salary_kusd);
	auto team_average_salary_kusd = mean(salary_kusd);
	auto team_senior_member_count = count_if(years >= 5);
	auto team_senior_total_salary_kusd = sum_if(salary_kusd, years >= 5);
	auto team_utilization_ratio = team_total_salary_kusd / budget_kusd;

	// sum2(...) raises the layer by two levels: member -> company.
	auto company_total_salary_kusd = sum2(salary_kusd);
	auto company_senior_member_count = count_if2(years >= 5);

	auto team_top_contributor = isgreatest(projects);
	auto member_salary_rank_in_team = count_if(salary_kusd > salary_kusd.o(0_depth)) + 1;

	std::cout << "------Company summary (sum2 / count_if2)------" << std::endl;
	std::cout << std::format("{} FY{} total_salary={:.1f} senior_members={}",
		company(company_tree), fiscal_year(company_tree),
		company_total_salary_kusd(company_tree),
		company_senior_member_count(company_tree)) << std::endl;
	std::cout << std::endl;

	std::cout << "------Team summary------" << std::endl;
	for (auto [team_, manager_, avg_salary_, total_salary_, senior_count_, senior_total_salary_, utilization_] : company_tree
		| Evaluate(team, manager, team_average_salary_kusd, team_total_salary_kusd,
			team_senior_member_count, team_senior_total_salary_kusd, team_utilization_ratio))
	{
		std::cout << std::format("{:>8} {:>6} avg={:>6.1f} total={:>6.1f} senior_count={:>2} senior_total_salary={:>6.1f} util={:>5.2f}",
			team_, manager_, avg_salary_, total_salary_, senior_count_, senior_total_salary_, utilization_) << std::endl;
	}
	std::cout << std::endl;

	std::cout << "------Filter teams with high utilization------" << std::endl;
	company_tree
		| Filter(team_utilization_ratio > 0.40)
		| Show("{:>8} {:>6} util={:>5.2f} avg_salary={:>6.1f} senior_total_salary={:>6.1f}",
			team, manager, team_utilization_ratio, team_average_salary_kusd, team_senior_total_salary_kusd);
	std::cout << std::endl;

	std::cout << "------Best contributors in each team------" << std::endl;
	for (auto [team_, member_, projects_, salary_rank_] : company_tree
		| Filter(team_top_contributor)
		| Evaluate(team, member, projects, member_salary_rank_in_team))
	{
		std::cout << std::format("{:>8} {:>8} projects={:>2} salary_rank={}", team_, member_, projects_, salary_rank_) << std::endl;
	}
	std::cout << std::endl;
}
