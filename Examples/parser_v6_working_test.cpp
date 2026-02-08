#include <iostream>
#include <OpenADAPT/ADAPT.h>
#include "OpenADAPT/Evaluator/ParserV6.h"

using namespace adapt;
using namespace adapt::lit;

int main()
{
    std::cout << "========================================\n";
    std::cout << "Parser V6 - Real Expression Test\n";
    std::cout << "========================================\n\n";
    
    try {
        // Create a DTree with test data
        using enum adapt::FieldType;
        DTree tree;
        ADAPT_D_SET_TOP_LAYER(tree, x, I32, y, I32, z, I32);
        tree.VerifyStructure();
        
        // Add test data at top layer
        tree.SetTopFields(10, 20, 5);
        
        std::cout << "Test Data:\n";
        std::cout << "  x = 10\n";
        std::cout << "  y = 20\n";
        std::cout << "  z = 5\n\n";
        
        // Bpos for top layer
        adapt::Bpos top_bpos = {};
        
        // Test 1: Simple addition
        std::cout << "Test 1: x + y\n";
        try {
            auto lambda1 = eval::ParseRttiFuncNode(tree, "x + y");
            auto result1 = lambda1(tree, top_bpos).i32();
            std::cout << "  Result: " << result1 << "\n";
            std::cout << "  Expected: 30\n";
            std::cout << "  Status: " << (result1 == 30 ? "✓ PASS" : "✗ FAIL") << "\n\n";
        } catch (const std::exception& e) {
            std::cout << "  Error: " << e.what() << "\n\n";
        }
        
        // Test 2: Multiplication
        std::cout << "Test 2: x * z\n";
        try {
            auto lambda2 = eval::ParseRttiFuncNode(tree, "x * z");
            auto result2 = lambda2(tree, top_bpos).i32();
            std::cout << "  Result: " << result2 << "\n";
            std::cout << "  Expected: 50\n";
            std::cout << "  Status: " << (result2 == 50 ? "✓ PASS" : "✗ FAIL") << "\n\n";
        } catch (const std::exception& e) {
            std::cout << "  Error: " << e.what() << "\n\n";
        }
        
        // Test 3: Complex expression
        std::cout << "Test 3: (x + y) * z\n";
        try {
            auto lambda3 = eval::ParseRttiFuncNode(tree, "(x + y) * z");
            auto result3 = lambda3(tree, top_bpos).i32();
            std::cout << "  Result: " << result3 << "\n";
            std::cout << "  Expected: 150\n";
            std::cout << "  Status: " << (result3 == 150 ? "✓ PASS" : "✗ FAIL") << "\n\n";
        } catch (const std::exception& e) {
            std::cout << "  Error: " << e.what() << "\n\n";
        }
        
        std::cout << "========================================\n";
        std::cout << "Tests completed!\n";
        std::cout << "========================================\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
}
