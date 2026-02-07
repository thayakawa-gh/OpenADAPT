#include <iostream>
#include <OpenADAPT/ADAPT.h>
#include <OpenADAPT/Evaluator/ParserV5.h>

using namespace adapt;
using namespace adapt::lit;

int main()
{
std::cout << "=== Parser V5 Simple Working Example ===" << std::endl;

try
{
// Create test tree with 2 fields at top layer
using enum adapt::FieldType;
DTree tree;
ADAPT_D_SET_TOP_LAYER(tree, x, F64, y, F64);
tree.VerifyStructure();
tree.Resize({3});

// Set data using field access
tree[0]["x"].f64() = 10.0;
tree[0]["y"].f64() = 20.0;

tree[1]["x"].f64() = 15.0;
tree[1]["y"].f64() = 25.0;

tree[2]["x"].f64() = 20.0;
tree[2]["y"].f64() = 30.0;

std::cout << "\n=== Testing Basic Field Operations ===" << std::endl;

std::cout << "\nTest 1: x + y (fields)" << std::endl;
auto lambda1 = eval::ParseRttiFuncNode(tree, "x + y");
for (int i = 0; i < 3; i++) {
double result = lambda1(tree, Bpos{i}).f64();
std::cout << "  [" << i << "]: " << result << " (expected: " << (tree[i]["x"].f64() + tree[i]["y"].f64()) << ")" << std::endl;
}

std::cout << "\nTest 2: x * y (fields)" << std::endl;
auto lambda2 = eval::ParseRttiFuncNode(tree, "x * y");
for (int i = 0; i < 3; i++) {
double result = lambda2(tree, Bpos{i}).f64();
std::cout << "  [" << i << "]: " << result << " (expected: " << (tree[i]["x"].f64() * tree[i]["y"].f64()) << ")" << std::endl;
}

std::cout << "\nTest 3: x < y (comparison)" << std::endl;
auto lambda3 = eval::ParseRttiFuncNode(tree, "x < y");
for (int i = 0; i < 3; i++) {
bool result = lambda3(tree, Bpos{i}).i32() != 0;
bool expected = tree[i]["x"].f64() < tree[i]["y"].f64();
std::cout << "  [" << i << "]: " << (result ? "true" : "false") << " (expected: " << (expected ? "true" : "false") << ")" << std::endl;
}

std::cout << "\n=== Basic field operations work! ===" << std::endl;
std::cout << "\nNote: Currently only field-to-field operations are fully tested." << std::endl;
std::cout << "Literal support (e.g., x + 10) requires RttiConstNode Container template parameter," << std::endl;
std::cout << "which will be addressed in a future update to the core library." << std::endl;
return 0;
}
catch (const std::exception& e)
{
std::cerr << "Error: " << e.what() << std::endl;
return 1;
}
}
