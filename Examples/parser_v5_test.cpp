#include <iostream>
#include <OpenADAPT/ADAPT.h>
#include <OpenADAPT/Evaluator/ParserV5.h>

using namespace adapt;
using namespace adapt::lit;

int main()
{
	std::cout << "=== Parser V5 Quick Test ===" << std::endl;
	
	try
	{
		// Create test tree
		DTree tree;
		tree.SetStructure("x"_fld, "y"_fld, "id"_fld);
		tree.Resize({3});
		
		tree[0]["id"].i32() = 1;
		tree[0]["x"].f64() = 10.0;
		tree[0]["y"].f64() = 20.0;
		
		tree[1]["id"].i32() = 2;
		tree[1]["x"].f64() = 15.0;
		tree[1]["y"].f64() = 25.0;
		
		tree[2]["id"].i32() = 3;
		tree[2]["x"].f64() = 20.0;
		tree[2]["y"].f64() = 30.0;
		
		std::cout << "\nTest 1: x + y" << std::endl;
		auto lambda1 = eval::ParseRttiFuncNode(tree, "x + y");
		std::cout << "Results: ";
		for (auto&& [v] : Extract(tree, lambda1))
			std::cout << v << " ";
		std::cout << std::endl;
		
		std::cout << "\nTest 2: x + 10" << std::endl;
		auto lambda2 = eval::ParseRttiFuncNode(tree, "x + 10");
		std::cout << "Results: ";
		for (auto&& [v] : Extract(tree, lambda2))
			std::cout << v << " ";
		std::cout << std::endl;
		
		std::cout << "\nTest 3: (x + y) * 2" << std::endl;
		auto lambda3 = eval::ParseRttiFuncNode(tree, "(x + y) * 2");
		std::cout << "Results: ";
		for (auto&& [v] : Extract(tree, lambda3))
			std::cout << v << " ";
		std::cout << std::endl;
		
		std::cout << "\nTest 4: sqrt(x)" << std::endl;
		auto lambda4 = eval::ParseRttiFuncNode(tree, "sqrt(x)");
		std::cout << "Results: ";
		for (auto&& [v] : Extract(tree, lambda4))
			std::cout << v << " ";
		std::cout << std::endl;
		
		std::cout << "\nTest 5: pow(x, 2)" << std::endl;
		auto lambda5 = eval::ParseRttiFuncNode(tree, "pow(x, 2)");
		std::cout << "Results: ";
		for (auto&& [v] : Extract(tree, lambda5))
			std::cout << v << " ";
		std::cout << std::endl;
		
		std::cout << "\n=== All tests passed! ===" << std::endl;
		return 0;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
}
