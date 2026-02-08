#include <iostream>
#include <string>

// Simple test to verify ParserV6 headers compile with Clang

#include <OpenADAPT/Evaluator/ParserV6.h>
#include <OpenADAPT/Container/Tree.h>

using namespace adapt;

int main()
{
    std::cout << "======================================\n";
    std::cout << "ParserV6 Minimal Clang Compilation Test\n";
    std::cout << "======================================\n\n";

    std::cout << "✓ ParserV6.h included successfully\n";
    std::cout << "✓ Headers compile with Clang\n";
    
    // Create a minimal tree
    using enum FieldType;
    DTree tree;
    ADAPT_D_SET_TOP_LAYER(tree, sch, Str);
    ADAPT_D_ADD_LAYER(tree, gr, I08, cls, I08);
    ADAPT_D_ADD_LAYER(tree, num, I32, nm, Str);
    ADAPT_D_ADD_LAYER(tree, ex, I08, score1, I32, score2, I32, score3, I32);
    tree.VerifyStructure();

    std::cout << "✓ Tree structure created\n";

    // Add minimal data
    tree.SetTopFields("Test School");
    tree.Push((int8_t)1, (int8_t)1);
    auto&& c1 = tree.Back();
    c1.Push((int32_t)1, "Alice");
    auto&& s1 = c1.Back();
    s1.Push((int8_t)0, (int32_t)85, (int32_t)90, (int32_t)88);

    std::cout << "✓ Test data added\n\n";

    std::cout << "Testing Parser:\n";
    std::cout << "---------------\n";

    // Test 1: Simple field expression
    std::cout << "Test 1: Field-to-field operations\n";
    try {
        auto lambda1 = eval::ParseRttiFuncNode(tree, "score1 + score2");
        std::cout << "  ✓ Parsed: score1 + score2\n";
    } catch (const std::exception& e) {
        std::cout << "  ✗ Error: " << e.what() << "\n";
    }

    try {
        auto lambda2 = eval::ParseRttiFuncNode(tree, "score1 > score2");
        std::cout << "  ✓ Parsed: score1 > score2\n";
    } catch (const std::exception& e) {
        std::cout << "  ✗ Error: " << e.what() << "\n";
    }

    try {
        auto lambda3 = eval::ParseRttiFuncNode(tree, "score1 * score2 + score3");
        std::cout << "  ✓ Parsed: score1 * score2 + score3\n";
    } catch (const std::exception& e) {
        std::cout << "  ✗ Error: " << e.what() << "\n";
    }

    // Test 2: Layer functions
    std::cout << "\nTest 2: Layer functions\n";
    try {
        auto lambda4 = eval::ParseRttiFuncNode(tree, "mean(score1)");
        std::cout << "  ✓ Parsed: mean(score1)\n";
    } catch (const std::exception& e) {
        std::cout << "  ✗ Error: " << e.what() << "\n";
    }

    try {
        auto lambda5 = eval::ParseRttiFuncNode(tree, "sum(score2)");
        std::cout << "  ✓ Parsed: sum(score2)\n";
    } catch (const std::exception& e) {
        std::cout << "  ✗ Error: " << e.what() << "\n";
    }

    try {
        auto lambda6 = eval::ParseRttiFuncNode(tree, "greatest(score3)");
        std::cout << "  ✓ Parsed: greatest(score3)\n";
    } catch (const std::exception& e) {
        std::cout << "  ✗ Error: " << e.what() << "\n";
    }

    // Test 3: Regular functions
    std::cout << "\nTest 3: Regular math functions\n";
    try {
        auto lambda7 = eval::ParseRttiFuncNode(tree, "abs(score1)");
        std::cout << "  ✓ Parsed: abs(score1)\n";
    } catch (const std::exception& e) {
        std::cout << "  ✗ Error: " << e.what() << "\n";
    }

    try {
        auto lambda8 = eval::ParseRttiFuncNode(tree, "sqrt(score1)");
        std::cout << "  ✓ Parsed: sqrt(score1)\n";
    } catch (const std::exception& e) {
        std::cout << "  ✗ Error: " << e.what() << "\n";
    }

    try {
        auto lambda9 = eval::ParseRttiFuncNode(tree, "pow(score1, score2)");
        std::cout << "  ✓ Parsed: pow(score1, score2)\n";
    } catch (const std::exception& e) {
        std::cout << "  ✗ Error: " << e.what() << "\n";
    }

    std::cout << "\n======================================\n";
    std::cout << "Compilation Test: SUCCESS\n";
    std::cout << "Parser compiles and instantiates with Clang!\n";
    std::cout << "======================================\n";

    return 0;
}
