# Parser V2 - 改善版パーサー

## 概要

OpenADAPT Parser V2は、フィードバックに基づいて大幅に改善されたバージョンです。

## 改善点

### 1. ✅ APIの簡素化（必須）

**旧API (V1)**:
```cpp
// プレースホルダマップを手動で作成する必要があった
auto [x, y, z] = tree.GetPlaceholders("x", "y", "z");
std::map<std::string, DTree::RttiPlaceholder> ph_map;
ph_map["x"] = x;
ph_map["y"] = y;
ph_map["z"] = z;

auto lambda = ParseRttiFuncNode<DTree>("x + y", ph_map);
```

**新API (V2)**:
```cpp
// コンテナから自動的にフィールド名を取得
auto lambda = ParseRttiFuncNode(tree, "x + y");
```

**実装方法**:
- コンテナの`GetFieldNames()`（STree）または`GetFieldNamesIn()`（DTree）を使用
- すべての層から自動的にプレースホルダを取得
- 内部でマップを構築

### 2. ✅ 演算子優先順位の修正（必須）

**C++標準準拠**:
- 優先順位13: `*`, `/`, `%`
- 優先順位12: `+`, `-`
- 優先順位11: `<<`, `>>`
- 優先順位10: `<`, `<=`, `>`, `>=`
- 優先順位9: `==`, `!=`
- 優先順位8: `&`
- 優先順位7: `^`
- 優先順位6: `|`
- 優先順位5: `&&`
- 優先順位4: `||`

**テスト結果**:
```cpp
"x + y * z"  → x + (y * z)  // 正しく解釈される
"(x + y) * z" → (x + y) * z  // 括弧も正しく機能
```

### 3. ✅ メンテナンス性の向上（必須）

**中央管理**:
```cpp
namespace ops {
    // すべての演算子・関数名を一箇所で定義
    constexpr const char* ADD = "+";
    constexpr const char* SUB = "-";
    constexpr const char* MEAN = "mean";
    constexpr const char* SUM = "sum";
    // ...
}
```

**利点**:
- 演算子/関数名がコード全体に散らばらない
- 変更が容易
- 一貫性の保証

### 4. ✅ 関数サポート（必須）

#### 4.1 階層関数

**基本階層関数** (level 1):
```cpp
mean(score)     // 平均値
sum(score)      // 合計値
count(score)    // 要素数
exist(expr)     // 存在チェック
greatest(score) // 最大値
least(score)    // 最小値
dev(score)      // 標準偏差
```

**レベル指定階層関数**:
```cpp
mean2(score)  // 2層上まで平均
sum3(score)   // 3層上まで合計
// 1-10 まで対応
```

**複合式との組み合わせ**:
```cpp
mean(score) + 10      // 階層関数 + 算術演算（定数サポート後）
greatest(a) - least(b) // 複数の階層関数の組み合わせ
```

#### 4.2 プレースホルダメソッド（開発中）

```cpp
x.at(1)        // インデックス指定
x.at(1, 2)     // 複数インデックス
x.outer(0)     // 外側スコープアクセス
x.o(0)         // outer の省略形
```

**現状**: パーサーは構文を認識するが、RttiFieldNodeとの統合が必要

#### 4.3 通常の関数（計画中）

```cpp
abs(x)         // 絶対値
sqrt(x)        // 平方根
pow(x, 2)      // べき乗
cast_f64(x)    // 型変換
tostr(x)       // 文字列化
```

### 5. ⏭️ パーサージェネレータの検討（オプション）

**現時点**: 手書きパーサーで十分な機能と性能を実現

**将来的な検討事項**:
- より複雑な構文が必要になった場合
- Lemon または re2c の導入を検討
- 現在の実装は将来の移行を考慮した設計

## 使用例

### 例1: 基本的な使用

```cpp
#include <OpenADAPT/Evaluator/ParserV2.h>

DTree tree;
// ... ツリーの初期化 ...

// 式をパース
auto lambda = ParseRttiFuncNode(tree, "x + y * z");

// 評価
int result = lambda(tree, Bpos{0}).i32();
```

### 例2: 階層関数

```cpp
// 2層構造のツリー
// Layer 0: 生徒
// Layer 1: 試験スコア

// 各生徒の平均点
auto avg = ParseRttiFuncNode(tree, "mean(score)");
int avg_tanaka = avg(tree, Bpos{0}).i32();

// 全体の平均点（2層上がる）
auto avg_all = ParseRttiFuncNode(tree, "mean2(score)");
int avg_total = avg_all(tree).i32();
```

### 例3: 複合式

```cpp
// 最高点と最低点の差
auto range = ParseRttiFuncNode(tree, "greatest(score) - least(score)");

// 合計が200以上の生徒が存在するか
// （定数サポート後）
// auto has_high = ParseRttiFuncNode(tree, "exist(sum(score) > 200)");
```

## 実装の詳細

### アーキテクチャ

```
ParseRttiFuncNode()
    ↓
Parser::Parse()
    ↓
ParseExpression() ← 演算子優先順位を考慮
    ↓
ParseUnary() ← 単項演算子
    ↓
ParsePostfix() ← メンバ関数呼び出し
    ↓
ParsePrimary() ← 基本要素（プレースホルダ、関数呼び出し）
```

### 演算子の適用

すべての演算子は`ApplyBinaryOperator()`または`ApplyUnaryOperator()`で集中管理:

```cpp
NodeType ApplyBinaryOperator(const std::string& op, NodeType left, NodeType right)
{
    if (op == ops::ADD) return left + right;
    if (op == ops::SUB) return left - right;
    // ... 他の演算子 ...
}
```

### 階層関数の適用

マクロを使用してレベルごとに関数を登録:

```cpp
#define APPLY_LAYER_FUNC_CASE(CONST_NAME, FUNC_NAME, LEVEL) \
    if (func_name == ops::CONST_NAME && level == LEVEL) { \
        return eval::FUNC_NAME##LEVEL(std::move(arg)); \
    }

APPLY_LAYER_FUNC_CASE(MEAN, mean, 1)
APPLY_LAYER_FUNC_CASE(MEAN, mean, 2)
// ... レベル1-10まで
```

## テスト結果

### 基本演算子テスト（✓）

```
x + y               ✓
(x + y) * z         ✓
x + y * z           ✓ (正しい優先順位)
x < y && y < z      ✓
-x + y              ✓
```

### 階層関数テスト（✓）

```
mean(score)         ✓
sum(score)          ✓
count(score)        ✓
greatest(score)     ✓
least(score)        ✓
mean2(score)        ✓
```

### 未サポート機能（開発中）

```
score + 10          ✗ (定数リテラル未サポート)
exist(score > 90)   ✗ (定数リテラル未サポート)
score.at(1)         ✗ (メンバ関数未サポート)
```

## 今後の開発

### 短期目標

1. **定数リテラルのサポート**
   - 整数、浮動小数点数、文字列
   - 型推論の実装
   
2. **プレースホルダメンバ関数**
   - `.at()` の実装
   - `.outer()` の実装

### 中期目標

3. **通常の関数**
   - 数学関数（abs, sqrt, pow, etc.）
   - 型変換関数（cast_*, tostr）
   
4. **ユーザー定義関数**
   - `UserFunc()` との統合

### 長期目標

5. **パーサージェネレータの検討**
   - 必要に応じてLemonまたはre2cへの移行
   - より複雑な構文のサポート

## まとめ

Parser V2は、主要な改善要件の大部分を達成しました:

✅ APIの簡素化 - 完了  
✅ C++準拠の演算子優先順位 - 完了  
✅ メンテナンス性の向上 - 完了  
✅ 階層関数のサポート - 完了  
🔄 その他の関数 - 進行中  
⏭️ パーサージェネレータ - 将来の検討事項

現在の実装は、実用的かつ拡張可能な基盤を提供しています。
