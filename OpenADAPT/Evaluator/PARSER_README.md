# OpenADAPT Parser

## 概要 / Overview

OpenADAPT Parserは、文字列からRttiFuncNode（ラムダ関数）を生成するためのパーサーライブラリです。

The OpenADAPT Parser is a library for generating RttiFuncNode (lambda functions) from string expressions.

## 機能 / Features

- **文字列式のパース**: プレースホルダ名と演算子を含む文字列式をパースします
- **演算子サポート**:
  - 算術演算子: `+`, `-`, `*`, `/`, `%`
  - 比較演算子: `==`, `!=`, `<`, `<=`, `>`, `>=`
  - 論理演算子: `&&`, `||`, `!`
  - ビット演算子: `&`, `|`, `^`, `<<`, `>>`
  - 単項演算子: `-`, `!`, `~`
- **括弧によるグルーピング**: 演算の優先順位を制御
- **演算子の優先順位**: C++と同様の優先順位ルール

- **String Expression Parsing**: Parses string expressions containing placeholder names and operators
- **Operator Support**:
  - Arithmetic operators: `+`, `-`, `*`, `/`, `%`
  - Comparison operators: `==`, `!=`, `<`, `<=`, `>`, `>=`
  - Logical operators: `&&`, `||`, `!`
  - Bitwise operators: `&`, `|`, `^`, `<<`, `>>`
  - Unary operators: `-`, `!`, `~`
- **Parentheses for Grouping**: Control operation precedence
- **Operator Precedence**: Following C++ precedence rules

## 使用方法 / Usage

### 基本的な使用例 / Basic Example

```cpp
#include <OpenADAPT/ADAPT.h>
#include <OpenADAPT/Evaluator/Parser.h>

using namespace adapt;
using namespace adapt::lit;
using namespace adapt::eval::parser;

int main()
{
    // DTreeの作成 / Create a DTree
    DTree tree;
    using enum adapt::FieldType;
    tree.SetTopLayer({});
    tree.AddLayer({ {"x", I32}, {"y", I32}, {"z", I32} });
    tree.VerifyStructure();
    
    // データの追加 / Add data
    tree.Push(10, 20, 30);
    tree.Push(5, 15, 25);
    
    // プレースホルダの取得 / Get placeholders
    auto [x, y, z] = tree.GetPlaceholders("x", "y", "z");
    
    // プレースホルダマップの作成 / Create placeholder map
    std::map<std::string, DTree::RttiPlaceholder> ph_map;
    ph_map["x"] = x;
    ph_map["y"] = y;
    ph_map["z"] = z;
    
    // 文字列式のパース / Parse string expression
    auto lambda = ParseRttiFuncNode<DTree>("x + y * z", ph_map);
    
    // 評価 / Evaluate
    std::cout << lambda(tree, Bpos{ 0 }).i32() << std::endl; // 610
    std::cout << lambda(tree, Bpos{ 1 }).i32() << std::endl; // 380
    
    return 0;
}
```

### ヘルパー関数 / Helper Function

プレースホルダマップを簡単に作成するためのヘルパー関数が用意されています:

A helper function is provided to easily create placeholder maps:

```cpp
// 配列から自動的にマップを生成 / Automatically create a map from an array
auto ph_map = MakePlaceholderMap(tree, std::array{ "x", "y", "z" });
```

### 式の例 / Expression Examples

```cpp
// 算術演算 / Arithmetic operations
"x + y"           // 加算 / Addition
"x - y"           // 減算 / Subtraction
"x * y"           // 乗算 / Multiplication
"x / y"           // 除算 / Division
"x % y"           // 剰余 / Modulus

// 複合式 / Complex expressions
"(x + y) * z"     // 括弧によるグルーピング / Grouping with parentheses
"x + y * z"       // 演算子の優先順位 / Operator precedence

// 比較演算 / Comparison operations
"x == y"          // 等しい / Equal
"x != y"          // 等しくない / Not equal
"x < y"           // より小さい / Less than
"x <= y"          // 以下 / Less than or equal
"x > y"           // より大きい / Greater than
"x >= y"          // 以上 / Greater than or equal

// 論理演算 / Logical operations
"x < y && y < z"  // 論理積 / Logical AND
"x > y || y < z"  // 論理和 / Logical OR
"!(x == y)"       // 論理否定 / Logical NOT

// ビット演算 / Bitwise operations
"x & y"           // ビット積 / Bitwise AND
"x | y"           // ビット和 / Bitwise OR
"x ^ y"           // ビット排他的論理和 / Bitwise XOR
"x << 2"          // 左シフト / Left shift
"x >> 2"          // 右シフト / Right shift

// 単項演算子 / Unary operators
"-x"              // 単項マイナス / Unary minus
"!x"              // 論理否定 / Logical NOT
"~x"              // ビット反転 / Bitwise NOT
```

## 制限事項 / Limitations

現在のバージョンでは、以下の制限があります:

The current version has the following limitations:

- **定数リテラルは未サポート**: 数値や文字列の定数はまだサポートされていません。将来のバージョンで追加される予定です。
- **プレースホルダのみ**: 式にはプレースホルダ名のみを含めることができます。

- **Constant literals not supported**: Number and string constants are not yet supported. They will be added in a future version.
- **Placeholders only**: Expressions can only contain placeholder names.

## エラーハンドリング / Error Handling

パーサーは以下の場合に`std::runtime_error`を投げます:

The parser throws `std::runtime_error` in the following cases:

- 未知のプレースホルダ名 / Unknown placeholder name
- 構文エラー / Syntax error
- 括弧の不一致 / Mismatched parentheses
- 不正なトークン / Invalid token

```cpp
try {
    auto lambda = ParseRttiFuncNode<DTree>("unknown_name + y", ph_map);
} catch (const std::runtime_error& e) {
    std::cerr << "Parse error: " << e.what() << std::endl;
}
```

## API リファレンス / API Reference

### ParseRttiFuncNode

```cpp
template <class Container>
auto ParseRttiFuncNode(
    std::string_view expr,
    const std::map<std::string, typename Container::RttiPlaceholder>& placeholders
)
```

文字列式をパースしてRttiFuncNodeを生成します。

Parses a string expression and generates an RttiFuncNode.

**Parameters:**
- `expr`: パースする文字列式 / String expression to parse
- `placeholders`: プレースホルダ名からRttiPlaceholderへのマップ / Map from placeholder names to RttiPlaceholder

**Returns:** 生成されたRttiFuncNode / Generated RttiFuncNode

**Throws:** `std::runtime_error` - パースエラーの場合 / On parse error

### MakePlaceholderMap

```cpp
template <class Container, size_t N>
auto MakePlaceholderMap(
    const Container& container,
    const std::array<std::string, N>& names
)
```

プレースホルダ名の配列からマップを作成します。

Creates a map from an array of placeholder names.

**Parameters:**
- `container`: コンテナオブジェクト / Container object
- `names`: プレースホルダ名の配列 / Array of placeholder names

**Returns:** プレースホルダ名からRttiPlaceholderへのマップ / Map from placeholder names to RttiPlaceholder

## 実装の詳細 / Implementation Details

パーサーは以下のコンポーネントで構成されています:

The parser consists of the following components:

1. **Tokenizer**: 文字列をトークンに分割 / Splits string into tokens
2. **Parser**: 再帰下降パーサーによる構文解析 / Syntax analysis using recursive descent parser
3. **Operator Precedence**: 演算子優先順位テーブル / Operator precedence table

演算子の優先順位は以下の通りです（高い順）:

Operator precedence (highest to lowest):

1. 単項演算子 / Unary operators: `!`, `-`, `~` (11)
2. 乗算・除算 / Multiplication/Division: `*`, `/`, `%` (10)
3. 加算・減算 / Addition/Subtraction: `+`, `-` (9)
4. シフト / Shift: `<<`, `>>` (8)
5. 比較 / Comparison: `<`, `<=`, `>`, `>=` (7)
6. 等価 / Equality: `==`, `!=` (6)
7. ビット積 / Bitwise AND: `&` (5)
8. ビット排他的論理和 / Bitwise XOR: `^` (4)
9. ビット和 / Bitwise OR: `|` (3)
10. 論理積 / Logical AND: `&&` (2)
11. 論理和 / Logical OR: `||` (1)

## 今後の拡張予定 / Future Enhancements

- 定数リテラルのサポート / Support for constant literals
- 関数呼び出しのサポート（mean, sum, etc.） / Support for function calls (mean, sum, etc.)
- より詳細なエラーメッセージ / More detailed error messages
- パフォーマンスの最適化 / Performance optimization

## ライセンス / License

このライブラリはOpenADAPTプロジェクトの一部として提供されます。

This library is provided as part of the OpenADAPT project.
