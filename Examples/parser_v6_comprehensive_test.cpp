#include <OpenADAPT/Evaluator/ParserV6.h>
#include <OpenADAPT/Container/Tree.h>
#include <OpenADAPT/Utility/Verbose.h>
#include <iostream>
#include <string>
#include <vector>

using namespace adapt;

// Test data structure
struct Record
{
	int8_t exam;
	int32_t math;
	int32_t jpn;
	int32_t eng;
};

struct Student
{
	int32_t number;
	std::string name;
	std::vector<Record> records;
};

struct Class
{
	int8_t grade;
	int8_t class_;
	std::vector<Student> students;
};

// Simple test tree builder
auto BuildTestTree()
{
	// Use dynamic tree for simplicity
	adapt::DTree tree;
	
	// Define structure
	using namespace adapt::FieldType;
	using adapt::FieldName;
	ADAPT_D_SET_TOP_LAYER(tree, FieldName("school"), Str);
	ADAPT_D_ADD_LAYER(tree, FieldName("grade"), I08, FieldName("class_"), I08);
	ADAPT_D_ADD_LAYER(tree, FieldName("number"), I32, FieldName("name"), Str);
	ADAPT_D_ADD_LAYER(tree, FieldName("exam"), I08, FieldName("math"), I32, FieldName("jpn"), I32, FieldName("eng"), I32);
	tree.VerifyStructure();
	tree.VerifyStructure();
	
	// Set top fields
	tree.SetTopFields("Test School");
	
	// Add simple test data
	tree.Push((int8_t)1, (int8_t)1);
	auto&& c1 = tree.Back();
	c1.Push((int32_t)1, "Alice");
	auto&& s1 = c1.Back();
	s1.Push((int8_t)0, (int32_t)85, (int32_t)90, (int32_t)88);
	s1.Push((int8_t)1, (int32_t)88, (int32_t)92, (int32_t)90);
	
	c1.Push((int32_t)2, "Bob");
	auto&& s2 = c1.Back();
	s2.Push((int8_t)0, (int32_t)75, (int32_t)80, (int32_t)78);
	s2.Push((int8_t)1, (int32_t)78, (int32_t)82, (int32_t)80);
	
	tree.Push((int8_t)1, (int8_t)2);
	auto&& c2 = tree.Back();
	c2.Push((int32_t)3, "Charlie");
	auto&& s3 = c2.Back();
	s3.Push((int8_t)0, (int32_t)95, (int32_t)88, (int32_t)92);
	s3.Push((int8_t)1, (int32_t)92, (int32_t)90, (int32_t)95);
	
	return tree;
}

// Test result tracking
struct TestResult
{
	std::string category;
	std::string test_name;
	bool passed;
	std::string error;
};

std::vector<TestResult> g_results;

void RecordPass(const std::string& category, const std::string& test_name)
{
	g_results.push_back({category, test_name, true, ""});
}

void RecordFail(const std::string& category, const std::string& test_name, const std::string& error)
{
	g_results.push_back({category, test_name, false, error});
}

// Test helpers
template<class Tree>
bool TestParse(const Tree& tree, const std::string& category, const std::string& expr)
{
	try
	{
		auto node = eval::ParseRttiFuncNode(tree, expr);
		RecordPass(category, expr);
		return true;
	}
	catch (const std::exception& e)
	{
		RecordFail(category, expr, e.what());
		return false;
	}
}

template<class Tree>
bool TestParseAndEval(const Tree& tree, const std::string& category, const std::string& expr)
{
	try
	{
		auto node = eval::ParseRttiFuncNode(tree, expr);
		auto range = tree.GetRange(2);
		auto trav = range.begin();
		node.Init(trav);
		auto result = node.Evaluate(trav);
		RecordPass(category, expr);
		return true;
	}
	catch (const std::exception& e)
	{
		RecordFail(category, expr, e.what());
		return false;
	}
}

int main()
{
	std::cout << "===========================================\n";
	std::cout << "ParserV6 Comprehensive Test\n";
	std::cout << "===========================================\n\n";
	
	auto tree = BuildTestTree();
	
	// ========================================
	// Test All 21 Binary Operators
	// ========================================
	
	std::cout << "Testing Binary Operators (21 total)...\n";
	
	TestParseAndEval(tree, "Binary Ops", "math * 2");
	TestParseAndEval(tree, "Binary Ops", "math / 2");
	TestParseAndEval(tree, "Binary Ops", "math % 10");
	TestParseAndEval(tree, "Binary Ops", "math + jpn");
	TestParseAndEval(tree, "Binary Ops", "math - jpn");
	TestParseAndEval(tree, "Binary Ops", "exam << 1");
	TestParseAndEval(tree, "Binary Ops", "exam >> 1");
	TestParseAndEval(tree, "Binary Ops", "math < 80");
	TestParseAndEval(tree, "Binary Ops", "math <= 80");
	TestParseAndEval(tree, "Binary Ops", "math > 80");
	TestParseAndEval(tree, "Binary Ops", "math >= 80");
	TestParseAndEval(tree, "Binary Ops", "math == 85");
	TestParseAndEval(tree, "Binary Ops", "math != 85");
	TestParseAndEval(tree, "Binary Ops", "exam & 1");
	TestParseAndEval(tree, "Binary Ops", "exam ^ 1");
	TestParseAndEval(tree, "Binary Ops", "exam | 1");
	TestParseAndEval(tree, "Binary Ops", "math > 80 && jpn > 85");
	TestParseAndEval(tree, "Binary Ops", "math < 70 || jpn < 75");
	
	// Precedence tests
	TestParseAndEval(tree, "Binary Ops", "math + jpn * 2");
	TestParseAndEval(tree, "Binary Ops", "math > 80 && jpn > 85 || eng > 90");
	TestParseAndEval(tree, "Binary Ops", "(math + jpn + eng) / 3");
	
	// ========================================
	// Test All 3 Unary Operators
	// ========================================
	
	std::cout << "Testing Unary Operators (3 total)...\n";
	
	TestParseAndEval(tree, "Unary Ops", "-math");
	TestParseAndEval(tree, "Unary Ops", "!(math > 80)");
	TestParseAndEval(tree, "Unary Ops", "~exam");
	
	// ========================================
	// Test All 8 Layer Functions
	// ========================================
	
	std::cout << "Testing Layer Functions (8 functions x 2 forms = 16 tests)...\n";
	
	// Base forms
	TestParseAndEval(tree, "Layer Funcs", "size(math)");
	TestParseAndEval(tree, "Layer Funcs", "exist(math)");
	TestParseAndEval(tree, "Layer Funcs", "count(math > 80)");
	TestParseAndEval(tree, "Layer Funcs", "sum(math)");
	TestParseAndEval(tree, "Layer Funcs", "mean(math)");
	TestParseAndEval(tree, "Layer Funcs", "dev(math)");
	TestParseAndEval(tree, "Layer Funcs", "greatest(math)");
	TestParseAndEval(tree, "Layer Funcs", "least(math)");
	
	// Numbered forms (level 2)
	TestParseAndEval(tree, "Layer Funcs", "size2(math)");
	TestParseAndEval(tree, "Layer Funcs", "exist2(math)");
	TestParseAndEval(tree, "Layer Funcs", "count2(math > 80)");
	TestParseAndEval(tree, "Layer Funcs", "sum2(math)");
	TestParseAndEval(tree, "Layer Funcs", "mean2(math)");
	TestParseAndEval(tree, "Layer Funcs", "dev2(math)");
	TestParseAndEval(tree, "Layer Funcs", "greatest2(math)");
	TestParseAndEval(tree, "Layer Funcs", "least2(math)");
	
	// ========================================
	// Test All Regular Functions
	// ========================================
	
	std::cout << "Testing Regular Functions (47+ functions)...\n";
	
	// 1-arg math functions (30)
	TestParseAndEval(tree, "Regular Funcs", "abs(-5)");
	TestParseAndEval(tree, "Regular Funcs", "abs(math - 100)");
	TestParseAndEval(tree, "Regular Funcs", "sqrt(math)");
	TestParseAndEval(tree, "Regular Funcs", "cbrt(math)");
	TestParseAndEval(tree, "Regular Funcs", "square(math)");
	TestParseAndEval(tree, "Regular Funcs", "cube(math)");
	TestParseAndEval(tree, "Regular Funcs", "sin(math)");
	TestParseAndEval(tree, "Regular Funcs", "cos(math)");
	TestParseAndEval(tree, "Regular Funcs", "tan(math)");
	TestParseAndEval(tree, "Regular Funcs", "asin(0.5)");
	TestParseAndEval(tree, "Regular Funcs", "acos(0.5)");
	TestParseAndEval(tree, "Regular Funcs", "atan(math)");
	TestParseAndEval(tree, "Regular Funcs", "sinh(math)");
	TestParseAndEval(tree, "Regular Funcs", "cosh(math)");
	TestParseAndEval(tree, "Regular Funcs", "tanh(math)");
	TestParseAndEval(tree, "Regular Funcs", "asinh(math)");
	TestParseAndEval(tree, "Regular Funcs", "acosh(math)");
	TestParseAndEval(tree, "Regular Funcs", "atanh(0.5)");
	TestParseAndEval(tree, "Regular Funcs", "exp(1)");
	TestParseAndEval(tree, "Regular Funcs", "exp2(math)");
	TestParseAndEval(tree, "Regular Funcs", "log(math)");
	TestParseAndEval(tree, "Regular Funcs", "log10(math)");
	TestParseAndEval(tree, "Regular Funcs", "log2(math)");
	TestParseAndEval(tree, "Regular Funcs", "ceil(math / 10.0)");
	TestParseAndEval(tree, "Regular Funcs", "floor(math / 10.0)");
	TestParseAndEval(tree, "Regular Funcs", "isfinite(math)");
	TestParseAndEval(tree, "Regular Funcs", "isinf(math)");
	TestParseAndEval(tree, "Regular Funcs", "isnan(math)");
	TestParseAndEval(tree, "Regular Funcs", "isnormal(math)");
	TestParseAndEval(tree, "Regular Funcs", "len(name)");
	TestParseAndEval(tree, "Regular Funcs", "tostr(math)");
	
	// 2-arg functions (5)
	TestParseAndEval(tree, "Regular Funcs", "pow(math, 2)");
	TestParseAndEval(tree, "Regular Funcs", "atan2(math, jpn)");
	TestParseAndEval(tree, "Regular Funcs", "hypot(math, jpn)");
	TestParseAndEval(tree, "Regular Funcs", "min(math, jpn)");
	TestParseAndEval(tree, "Regular Funcs", "max(math, jpn)");
	
	// 3-arg functions (2)
	TestParseAndEval(tree, "Regular Funcs", "if_(math > 80, 1, 0)");
	TestParseAndEval(tree, "Regular Funcs", "substr(name, 0, 3)");
	
	// ========================================
	// Test Member Functions
	// ========================================
	
	std::cout << "Testing Member Functions...\n";
	
	// Field member .at() - accepts ANY nodes
	TestParseAndEval(tree, "Member Funcs", "math.at(0)");
	TestParseAndEval(tree, "Member Funcs", "math.at(exam)");
	
	// Field member .outer() - only integer constants
	TestParseAndEval(tree, "Member Funcs", "math.outer(0)");
	TestParseAndEval(tree, "Member Funcs", "jpn.o(0)");
	
	// Test error cases
	std::cout << "Testing Member Function Error Cases...\n";
	TestParse(tree, "Member Funcs", "math.outer(exam)");  // Should fail - not constant
	TestParse(tree, "Member Funcs", "5.at(0)");          // Should fail - not field
	
	// ========================================
	// Test Complex Expressions
	// ========================================
	
	std::cout << "Testing Complex Expressions...\n";
	
	TestParseAndEval(tree, "Complex", "(math + jpn + eng) / 3");
	TestParseAndEval(tree, "Complex", "abs(math - mean(math))");
	TestParseAndEval(tree, "Complex", "count(math > 80 && jpn > 85)");
	TestParseAndEval(tree, "Complex", "max(math, max(jpn, eng))");
	TestParseAndEval(tree, "Complex", "if_(math > mean(math), 1, 0)");
	TestParseAndEval(tree, "Complex", "sqrt(square(math) + square(jpn))");
	
	// ========================================
	// Test Type Suffixes
	// ========================================
	
	std::cout << "Testing Type Suffixes...\n";
	
	TestParseAndEval(tree, "Type Suffix", "math + 10i32");
	TestParseAndEval(tree, "Type Suffix", "math + 10i64");
	TestParseAndEval(tree, "Type Suffix", "math + 10.5f32");
	TestParseAndEval(tree, "Type Suffix", "math + 10.5f64");
	TestParseAndEval(tree, "Type Suffix", "exam + 5i8");
	TestParseAndEval(tree, "Type Suffix", "number + 100i16");
	
	// ========================================
	// Test String Operations
	// ========================================
	
	std::cout << "Testing String Operations...\n";
	
	TestParseAndEval(tree, "Strings", "name == \"Alice\"");
	TestParseAndEval(tree, "Strings", "len(name) > 3");
	TestParseAndEval(tree, "Strings", "substr(name, 0, 1)");
	
	// ========================================
	// Test Operator Precedence
	// ========================================
	
	std::cout << "Testing Operator Precedence...\n";
	
	TestParseAndEval(tree, "Precedence", "2 + 3 * 4");       // Should be 14, not 20
	TestParseAndEval(tree, "Precedence", "10 - 2 * 3");      // Should be 4, not 24
	TestParseAndEval(tree, "Precedence", "math > 80 && jpn > 85 || eng > 90");
	TestParseAndEval(tree, "Precedence", "math + jpn * 2 - eng / 2");
	TestParseAndEval(tree, "Precedence", "1 << 2 + 3");      // + has higher precedence than <<
	
	// ========================================
	// Print Results
	// ========================================
	
	std::cout << "\n===========================================\n";
	std::cout << "Test Results Summary\n";
	std::cout << "===========================================\n\n";
	
	std::unordered_map<std::string, std::pair<int, int>> category_stats;
	
	for (const auto& result : g_results)
	{
		auto& stats = category_stats[result.category];
		stats.second++; // total
		if (result.passed)
			stats.first++; // passed
		else
		{
			std::cout << "FAIL [" << result.category << "] " << result.test_name << "\n";
			std::cout << "  Error: " << result.error << "\n";
		}
	}
	
	std::cout << "\nCategory Statistics:\n";
	std::cout << "--------------------\n";
	
	int total_passed = 0;
	int total_tests = 0;
	
	for (const auto& [category, stats] : category_stats)
	{
		int passed = stats.first;
		int total = stats.second;
		total_passed += passed;
		total_tests += total;
		
		std::cout << category << ": " << passed << "/" << total;
		if (passed == total)
			std::cout << " ✓\n";
		else
			std::cout << " ✗\n";
	}
	
	std::cout << "\n===========================================\n";
	std::cout << "Overall: " << total_passed << "/" << total_tests << " tests passed\n";
	
	if (total_passed == total_tests)
	{
		std::cout << "SUCCESS! All tests passed! ✓\n";
		return 0;
	}
	else
	{
		std::cout << "FAILURE! Some tests failed. ✗\n";
		return 1;
	}
}
