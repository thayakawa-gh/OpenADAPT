#include <iostream>
#include "OpenADAPT/Container/Tree.h"
#include "OpenADAPT/Evaluator/ParserV6.h"

using namespace adapt;

int main()
{
    std::cout << "========================================\n";
    std::cout << "Parser V6 - Real Expression Test\n";
    std::cout << "========================================\n\n";
    
    try {
        // Create a simple DTree with test data
        DTree tree;
        tree.SetStructure(
            Container::CreateDim<int>("x"),
            Container::CreateDim<int>("y"),
            Container::CreateDim<int>("z")
        );
        
        // Add test data
        tree.Insert(CttiPlaceholder<0>{}, 10);  // x = 10
        tree.Insert(CttiPlaceholder<1>{}, 20);  // y = 20
        tree.Insert(CttiPlaceholder<2>{}, 5);   // z = 5
        
        tree.Commit();
        
        std::cout << "Test Data:\n";
        std::cout << "  x = 10\n";
        std::cout << "  y = 20\n";
        std::cout << "  z = 5\n\n";
        
        // Test 1: Simple addition
        std::cout << "Test 1: x + y\n";
        auto lambda1 = eval::ParseRttiFuncNode(tree, "x + y");
        auto result1 = lambda1(tree.GetTopNode());
        std::cout << "  Result: " << result1 << "\n";
        std::cout << "  Expected: 30\n";
        std::cout << "  Status: " << (result1 == 30 ? "✓ PASS" : "✗ FAIL") << "\n\n";
        
        // Test 2: Multiplication
        std::cout << "Test 2: x * z\n";
        auto lambda2 = eval::ParseRttiFuncNode(tree, "x * z");
        auto result2 = lambda2(tree.GetTopNode());
        std::cout << "  Result: " << result2 << "\n";
        std::cout << "  Expected: 50\n";
        std::cout << "  Status: " << (result2 == 50 ? "✓ PASS" : "✗ FAIL") << "\n\n";
        
        // Test 3: Complex expression
        std::cout << "Test 3: (x + y) * z\n";
        auto lambda3 = eval::ParseRttiFuncNode(tree, "(x + y) * z");
        auto result3 = lambda3(tree.GetTopNode());
        std::cout << "  Result: " << result3 << "\n";
        std::cout << "  Expected: 150\n";
        std::cout << "  Status: " << (result3 == 150 ? "✓ PASS" : "✗ FAIL") << "\n\n";
        
        std::cout << "========================================\n";
        std::cout << "All tests completed!\n";
        std::cout << "========================================\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
