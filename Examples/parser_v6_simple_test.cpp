#include <OpenADAPT/Evaluator/ParserV6.h>
#include <OpenADAPT/Container/Tree.h>
#include <iostream>
#include <vector>
#include <string>

using namespace adapt;

int main()
{
std::cout << "===========================================\n";
std::cout << "ParserV6 Comprehensive Test\n";
std::cout << "===========================================\n\n";

// Create a simple dynamic tree
DTree tree;
using namespace FieldType;
using FieldName;
ADAPT_D_SET_TOP_LAYER(tree, FieldName("school"), Str);
ADAPT_D_ADD_LAYER(tree, FieldName("grade"), I08, FieldName("class_"), I08);
ADAPT_D_ADD_LAYER(tree, FieldName("number"), I32, FieldName("name"), Str);
ADAPT_D_ADD_LAYER(tree, FieldName("exam"), I08, FieldName("math"), I32, FieldName("jpn"), I32, FieldName("eng"), I32);
tree.VerifyStructure();

tree.SetTopFields("Test School");
tree.Push((int8_t)1, (int8_t)1);
auto&& c1 = tree.Back();
c1.Push((int32_t)1, "Alice");
auto&& s1 = c1.Back();
s1.Push((int8_t)0, (int32_t)85, (int32_t)90, (int32_t)88);

int passed = 0;
int total = 0;

auto test = [&](const std::string& category, const std::string& expr) {
total++;
try {
auto node = eval::ParseRttiFuncNode(tree, expr);
std::cout << "PASS [" << category << "] " << expr << "\n";
passed++;
return true;
} catch (const std::exception& e) {
std::cout << "FAIL [" << category << "] " << expr << "\n";
std::cout << "  Error: " << e.what() << "\n";
return false;
}
};

// Test Binary Operators (18 ops)
std::cout << "\nTesting Binary Operators...\n";
test("Binary Ops", "math * 2");
test("Binary Ops", "math / 2");
test("Binary Ops", "math % 10");
test("Binary Ops", "math + jpn");
test("Binary Ops", "math - jpn");
test("Binary Ops", "exam << 1");
test("Binary Ops", "exam >> 1");
test("Binary Ops", "math < 80");
test("Binary Ops", "math <= 80");
test("Binary Ops", "math > 80");
test("Binary Ops", "math >= 80");
test("Binary Ops", "math == 85");
test("Binary Ops", "math != 85");
test("Binary Ops", "exam & 1");
test("Binary Ops", "exam ^ 1");
test("Binary Ops", "exam | 1");
test("Binary Ops", "math > 80 && jpn > 85");
test("Binary Ops", "math < 70 || jpn < 75");

// Test Unary Operators (3 ops)
std::cout << "\nTesting Unary Operators...\n";
test("Unary Ops", "-math");
test("Unary Ops", "!(math > 80)");
test("Unary Ops", "~exam");

// Test Layer Functions (16 tests)
std::cout << "\nTesting Layer Functions...\n";
test("Layer Funcs", "size(math)");
test("Layer Funcs", "exist(math)");
test("Layer Funcs", "count(math > 80)");
test("Layer Funcs", "sum(math)");
test("Layer Funcs", "mean(math)");
test("Layer Funcs", "dev(math)");
test("Layer Funcs", "greatest(math)");
test("Layer Funcs", "least(math)");
test("Layer Funcs", "size2(math)");
test("Layer Funcs", "count2(math > 80)");
test("Layer Funcs", "sum2(math)");
test("Layer Funcs", "mean2(math)");
test("Layer Funcs", "dev2(math)");
test("Layer Funcs", "greatest2(math)");
test("Layer Funcs", "least2(math)");
test("Layer Funcs", "exist1(math)");

// Test Regular Functions (47 functions)
std::cout << "\nTesting Regular Functions...\n";
test("Regular Funcs", "abs(math - 100)");
test("Regular Funcs", "sqrt(math)");
test("Regular Funcs", "cbrt(math)");
test("Regular Funcs", "square(math)");
test("Regular Funcs", "cube(math)");
test("Regular Funcs", "sin(math)");
test("Regular Funcs", "cos(math)");
test("Regular Funcs", "tan(math)");
test("Regular Funcs", "asin(0.5)");
test("Regular Funcs", "acos(0.5)");
test("Regular Funcs", "atan(math)");
test("Regular Funcs", "sinh(math)");
test("Regular Funcs", "cosh(math)");
test("Regular Funcs", "tanh(math)");
test("Regular Funcs", "asinh(math)");
test("Regular Funcs", "acosh(math)");
test("Regular Funcs", "atanh(0.5)");
test("Regular Funcs", "exp(1)");
test("Regular Funcs", "exp2(math)");
test("Regular Funcs", "log(math)");
test("Regular Funcs", "log10(math)");
test("Regular Funcs", "log2(math)");
test("Regular Funcs", "ceil(math / 10.0)");
test("Regular Funcs", "floor(math / 10.0)");
test("Regular Funcs", "isfinite(math)");
test("Regular Funcs", "isinf(math)");
test("Regular Funcs", "isnan(math)");
test("Regular Funcs", "isnormal(math)");
test("Regular Funcs", "len(name)");
test("Regular Funcs", "tostr(math)");
test("Regular Funcs", "pow(math, 2)");
test("Regular Funcs", "atan2(math, jpn)");
test("Regular Funcs", "hypot(math, jpn)");
test("Regular Funcs", "min(math, jpn)");
test("Regular Funcs", "max(math, jpn)");
test("Regular Funcs", "if_(math > 80, 1, 0)");
test("Regular Funcs", "substr(name, 0, 3)");

// Test Member Functions
std::cout << "\nTesting Member Functions...\n";
test("Member Funcs", "math.at(0)");
test("Member Funcs", "math.at(exam)");
test("Member Funcs", "math.outer(0)");
test("Member Funcs", "jpn.o(0)");

// Test Complex Expressions
std::cout << "\nTesting Complex Expressions...\n";
test("Complex", "(math + jpn + eng) / 3");
test("Complex", "abs(math - mean(math))");
test("Complex", "count(math > 80 && jpn > 85)");
test("Complex", "max(math, max(jpn, eng))");
test("Complex", "if_(math > mean(math), 1, 0)");
test("Complex", "sqrt(square(math) + square(jpn))");

// Test Operator Precedence
std::cout << "\nTesting Operator Precedence...\n";
test("Precedence", "2 + 3 * 4");
test("Precedence", "10 - 2 * 3");
test("Precedence", "math > 80 && jpn > 85 || eng > 90");
test("Precedence", "math + jpn * 2 - eng / 2");

// Print Summary
std::cout << "\n===========================================\n";
std::cout << "Overall: " << passed << "/" << total << " tests passed\n";

if (passed == total) {
std::cout << "SUCCESS! All tests passed! ✓\n";
return 0;
} else {
std::cout << "FAILURE! " << (total - passed) << " tests failed. ✗\n";
return 1;
}
}
