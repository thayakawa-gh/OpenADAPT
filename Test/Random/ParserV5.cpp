#include <iostream>
#include <OpenADAPT/Container/DTree.h>
#include <OpenADAPT/Container/STree.h>
#include <OpenADAPT/Evaluator/ParserV5.h>

using namespace adapt;

int main()
{
	std::cout << "=== Parser V5 Test ===" << std::endl;
	
	try
	{
		// Create test tree structure
		DTree tree;
		tree.SetTableInfo<int32_t, double, double, std::string>(
			{"id", "x", "y", "name"}
		);
		
		tree.AddRow(0_layer, 1, 10.0, 20.0, "Alice");
		tree.AddRow(0_layer, 2, 15.0, 25.0, "Bob");
		tree.AddRow(0_layer, 3, 20.0, 30.0, "Charlie");
		
		std::cout << "\n--- Test 1: Basic arithmetic ---" << std::endl;
		{
			auto lambda = eval::ParseRttiFuncNode(tree, "x + y");
			std::cout << "x + y = ";
			for (auto&& [v] : Extract(tree, lambda))
				std::cout << v << " ";
			std::cout << std::endl;
		}
		
		std::cout << "\n--- Test 2: Arithmetic with literals ---" << std::endl;
		{
			auto lambda = eval::ParseRttiFuncNode(tree, "x + 10");
			std::cout << "x + 10 = ";
			for (auto&& [v] : Extract(tree, lambda))
				std::cout << v << " ";
			std::cout << std::endl;
		}
		
		std::cout << "\n--- Test 3: Complex expression ---" << std::endl;
		{
			auto lambda = eval::ParseRttiFuncNode(tree, "(x + y) * 2");
			std::cout << "(x + y) * 2 = ";
			for (auto&& [v] : Extract(tree, lambda))
				std::cout << v << " ";
			std::cout << std::endl;
		}
		
		std::cout << "\n--- Test 4: Comparison operators ---" << std::endl;
		{
			auto lambda = eval::ParseRttiFuncNode(tree, "x < 18");
			std::cout << "x < 18: ";
			for (auto&& [v] : Extract(tree, lambda))
				std::cout << (v ? "true" : "false") << " ";
			std::cout << std::endl;
		}
		
		std::cout << "\n--- Test 5: Regular functions ---" << std::endl;
		{
			auto lambda = eval::ParseRttiFuncNode(tree, "abs(x - 15)");
			std::cout << "abs(x - 15) = ";
			for (auto&& [v] : Extract(tree, lambda))
				std::cout << v << " ";
			std::cout << std::endl;
		}
		
		{
			auto lambda = eval::ParseRttiFuncNode(tree, "sqrt(x)");
			std::cout << "sqrt(x) = ";
			for (auto&& [v] : Extract(tree, lambda))
				std::cout << v << " ";
			std::cout << std::endl;
		}
		
		{
			auto lambda = eval::ParseRttiFuncNode(tree, "pow(x, 2)");
			std::cout << "pow(x, 2) = ";
			for (auto&& [v] : Extract(tree, lambda))
				std::cout << v << " ";
			std::cout << std::endl;
		}
		
		{
			auto lambda = eval::ParseRttiFuncNode(tree, "min(x, y)");
			std::cout << "min(x, y) = ";
			for (auto&& [v] : Extract(tree, lambda))
				std::cout << v << " ";
			std::cout << std::endl;
		}
		
		{
			auto lambda = eval::ParseRttiFuncNode(tree, "max(x, y)");
			std::cout << "max(x, y) = ";
			for (auto&& [v] : Extract(tree, lambda))
				std::cout << v << " ";
			std::cout << std::endl;
		}
		
		std::cout << "\n--- Test 6: Type suffixes ---" << std::endl;
		{
			auto lambda = eval::ParseRttiFuncNode(tree, "x + 5i32");
			std::cout << "x + 5i32 = ";
			for (auto&& [v] : Extract(tree, lambda))
				std::cout << v << " ";
			std::cout << std::endl;
		}
		
		{
			auto lambda = eval::ParseRttiFuncNode(tree, "x + 2.5f32");
			std::cout << "x + 2.5f32 = ";
			for (auto&& [v] : Extract(tree, lambda))
				std::cout << v << " ";
			std::cout << std::endl;
		}
		
		// Create nested tree for layer functions
		std::cout << "\n--- Test 7: Layer functions ---" << std::endl;
		DTree tree2;
		tree2.SetTableInfo<int32_t, double>({"id", "score"});
		
		auto g1 = tree2.AddRow(0_layer, 1, 0.0);
		tree2.AddRow(g1, 1, 85.0);
		tree2.AddRow(g1, 1, 90.0);
		tree2.AddRow(g1, 1, 78.0);
		
		auto g2 = tree2.AddRow(0_layer, 2, 0.0);
		tree2.AddRow(g2, 2, 92.0);
		tree2.AddRow(g2, 2, 88.0);
		
		{
			auto lambda = eval::ParseRttiFuncNode(tree2, "mean(score)");
			std::cout << "mean(score) = ";
			for (auto&& [v] : Extract(tree2, lambda, 0_layer))
				std::cout << v << " ";
			std::cout << std::endl;
		}
		
		{
			auto lambda = eval::ParseRttiFuncNode(tree2, "sum(score)");
			std::cout << "sum(score) = ";
			for (auto&& [v] : Extract(tree2, lambda, 0_layer))
				std::cout << v << " ";
			std::cout << std::endl;
		}
		
		{
			auto lambda = eval::ParseRttiFuncNode(tree2, "count(score)");
			std::cout << "count(score) = ";
			for (auto&& [v] : Extract(tree2, lambda, 0_layer))
				std::cout << v << " ";
			std::cout << std::endl;
		}
		
		{
			auto lambda = eval::ParseRttiFuncNode(tree2, "greatest(score)");
			std::cout << "greatest(score) = ";
			for (auto&& [v] : Extract(tree2, lambda, 0_layer))
				std::cout << v << " ";
			std::cout << std::endl;
		}
		
		{
			auto lambda = eval::ParseRttiFuncNode(tree2, "least(score)");
			std::cout << "least(score) = ";
			for (auto&& [v] : Extract(tree2, lambda, 0_layer))
				std::cout << v << " ";
			std::cout << std::endl;
		}
		
		std::cout << "\n--- Test 8: Operator precedence ---" << std::endl;
		{
			auto lambda = eval::ParseRttiFuncNode(tree, "x + y * 2");
			std::cout << "x + y * 2 = ";
			for (auto&& [v] : Extract(tree, lambda))
				std::cout << v << " ";
			std::cout << " (should be x + (y * 2))" << std::endl;
		}
		
		{
			auto lambda = eval::ParseRttiFuncNode(tree, "x * 2 + y");
			std::cout << "x * 2 + y = ";
			for (auto&& [v] : Extract(tree, lambda))
				std::cout << v << " ";
			std::cout << " (should be (x * 2) + y)" << std::endl;
		}
		
		std::cout << "\n--- Test 9: More math functions ---" << std::endl;
		{
			auto lambda = eval::ParseRttiFuncNode(tree, "ceil(x / 3.0)");
			std::cout << "ceil(x / 3.0) = ";
			for (auto&& [v] : Extract(tree, lambda))
				std::cout << v << " ";
			std::cout << std::endl;
		}
		
		{
			auto lambda = eval::ParseRttiFuncNode(tree, "floor(x / 3.0)");
			std::cout << "floor(x / 3.0) = ";
			for (auto&& [v] : Extract(tree, lambda))
				std::cout << v << " ";
			std::cout << std::endl;
		}
		
		std::cout << "\n=== All tests completed successfully! ===" << std::endl;
		return 0;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
}
