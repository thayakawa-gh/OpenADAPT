#include <OpenADAPT/Evaluator/ParserV6.h>
#include <OpenADAPT/Container/Tree.h>
#include <iostream>
#include <string>

using namespace adapt;

int main()
{
    std::cout << "===========================================\n";
    std::cout << "ParserV6 Clang Test\n";
    std::cout << "===========================================\n\n";

    // Create a simple dynamic tree using helper macros
    using enum FieldType;
    DTree tree;
    ADAPT_D_SET_TOP_LAYER(tree, school, Str);
    ADAPT_D_ADD_LAYER(tree, grade, I08, class_, I08);
    ADAPT_D_ADD_LAYER(tree, number, I32, name, Str);
    ADAPT_D_ADD_LAYER(tree, exam, I08, math, I32, jpn, I32, eng, I32);
    tree.VerifyStructure();

    // Add some data
    tree.SetTopFields("Test School");
    tree.Push((int8_t)1, (int8_t)1);
    auto&& c1 = tree.Back();
    c1.Push((int32_t)1, "Alice");
    auto&& s1 = c1.Back();
    s1.Push((int8_t)0, (int32_t)85, (int32_t)90, (int32_t)88);
    s1.Push((int8_t)1, (int32_t)92, (int32_t)88, (int32_t)95);
    
    c1.Push((int32_t)2, "Bob");
    auto&& s2 = c1.Back();
    s2.Push((int8_t)0, (int32_t)78, (int32_t)82, (int32_t)80);
    s2.Push((int8_t)1, (int32_t)88, (int32_t)85, (int32_t)90);

    std::cout << "Tree structure created successfully!\n";
    std::cout << "Number of students: 2\n\n";

    // Test 1: Field-to-Field operations (no literals needed)
    std::cout << "Test 1: Field Operations\n";
    std::cout << "------------------------\n";
    
    try {
        auto lambda1 = eval::ParseRttiFuncNode(tree, "math + jpn");
        std::cout << "✓ Parsed: math + jpn\n";
        
        auto lambda2 = eval::ParseRttiFuncNode(tree, "(math + jpn + eng) / 3");
        std::cout << "✓ Parsed: (math + jpn + eng) / 3 (But division by constant will fail)\n";
        
        auto lambda3 = eval::ParseRttiFuncNode(tree, "math > jpn");
        std::cout << "✓ Parsed: math > jpn\n";
        
        auto lambda4 = eval::ParseRttiFuncNode(tree, "math * math");
        std::cout << "✓ Parsed: math * math\n";
    } catch (const std::exception& e) {
        std::cout << "✗ Error: " << e.what() << "\n";
    }
    
    std::cout << "\nTest 2: Layer Functions\n";
    std::cout << "-----------------------\n";
    
    try {
        auto lambda5 = eval::ParseRttiFuncNode(tree, "mean(math)");
        std::cout << "✓ Parsed: mean(math)\n";
        
        auto lambda6 = eval::ParseRttiFuncNode(tree, "greatest(math)");
        std::cout << "✓ Parsed: greatest(math)\n";
        
        auto lambda7 = eval::ParseRttiFuncNode(tree, "sum(math)");
        std::cout << "✓ Parsed: sum(math)\n";
    } catch (const std::exception& e) {
        std::cout << "✗ Error: " << e.what() << "\n";
    }
    
    std::cout << "\nTest 3: Regular Functions\n";
    std::cout << "-------------------------\n";
    
    try {
        auto lambda8 = eval::ParseRttiFuncNode(tree, "abs(math)");
        std::cout << "✓ Parsed: abs(math)\n";
        
        auto lambda9 = eval::ParseRttiFuncNode(tree, "sqrt(math)");
        std::cout << "✓ Parsed: sqrt(math)\n";
        
        auto lambda10 = eval::ParseRttiFuncNode(tree, "pow(math, math)");
        std::cout << "✓ Parsed: pow(math, math)\n";
    } catch (const std::exception& e) {
        std::cout << "✗ Error: " << e.what() << "\n";
    }
    
    std::cout << "\nTest 4: Member Functions\n";
    std::cout << "------------------------\n";
    
    try {
        // Note: These might not work due to implementation details
        auto lambda11 = eval::ParseRttiFuncNode(tree, "math.outer(0)");
        std::cout << "✓ Parsed: math.outer(0) (but may not execute)\n";
    } catch (const std::exception& e) {
        std::cout << "✗ Error: " << e.what() << "\n";
    }
    
    std::cout << "\n===========================================\n";
    std::cout << "Summary: Parser compiles with Clang!\n";
    std::cout << "Note: Some expressions may not execute due to\n";
    std::cout << "      RttiConstNode lacking Container parameter.\n";
    std::cout << "===========================================\n";

    return 0;
}
