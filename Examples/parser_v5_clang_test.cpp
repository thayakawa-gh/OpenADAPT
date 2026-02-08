#include <iostream>
#include <OpenADAPT/ADAPT.h>
#include <OpenADAPT/Evaluator/ParserV5.h>

using namespace adapt;
using namespace adapt::lit;
using enum adapt::FieldType;

int main()
{
    std::cout << "===========================================\n";
    std::cout << "ParserV5 Clang Compile & Execute Test\n";
    std::cout << "===========================================\n\n";

    try
    {
        // Create a simple DTree with test data
        DTree tree;
        ADAPT_D_SET_TOP_LAYER(tree, x, I32, y, I32, z, I32);
        tree.VerifyStructure();
        
        tree.SetTopFields(10, 20, 5);
        
        std::cout << "Test data: x=10, y=20, z=5\n\n";
        
        // For top layer, we use empty index
        Bpos empty_index{};
        
        // Test 1: Simple addition "x + y"
        std::cout << "Test 1: x + y\n";
        try
        {
            auto lambda1 = eval::ParseRttiFuncNode(tree, "x + y");
            auto result1 = lambda1(tree, empty_index).i32();
            std::cout << "  Result: " << result1 << "\n";
            std::cout << "  Expected: 30\n";
            if (result1 == 30)
                std::cout << "  ✓ PASS\n";
            else
                std::cout << "  ✗ FAIL\n";
        }
        catch (const std::exception& e)
        {
            std::cout << "  ✗ ERROR: " << e.what() << "\n";
        }
        std::cout << "\n";
        
        // Test 2: Multiplication "x * z"
        std::cout << "Test 2: x * z\n";
        try
        {
            auto lambda2 = eval::ParseRttiFuncNode(tree, "x * z");
            auto result2 = lambda2(tree, empty_index).i32();
            std::cout << "  Result: " << result2 << "\n";
            std::cout << "  Expected: 50\n";
            if (result2 == 50)
                std::cout << "  ✓ PASS\n";
            else
                std::cout << "  ✗ FAIL\n";
        }
        catch (const std::exception& e)
        {
            std::cout << "  ✗ ERROR: " << e.what() << "\n";
        }
        std::cout << "\n";
        
        // Test 3: Complex expression "(x + y) * z"
        std::cout << "Test 3: (x + y) * z\n";
        try
        {
            auto lambda3 = eval::ParseRttiFuncNode(tree, "(x + y) * z");
            auto result3 = lambda3(tree, empty_index).i32();
            std::cout << "  Result: " << result3 << "\n";
            std::cout << "  Expected: 150\n";
            if (result3 == 150)
                std::cout << "  ✓ PASS\n";
            else
                std::cout << "  ✗ FAIL\n";
        }
        catch (const std::exception& e)
        {
            std::cout << "  ✗ ERROR: " << e.what() << "\n";
        }
        std::cout << "\n";
        
        std::cout << "===========================================\n";
        std::cout << "Tests completed!\n";
        std::cout << "===========================================\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
