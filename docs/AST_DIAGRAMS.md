# MANTRA AST Diagrams: Abstract Syntax Tree Representation

## Overview

This document provides visual representations of MANTRA's Abstract Syntax Tree (AST) for key language constructs. Understanding AST structure is essential for:

- Understanding how the parser represents code
- Debugging parser issues
- Understanding semantic analysis
- Implementing language tools

## AST Node Hierarchy

```
MantraNode (Abstract Base)
├── ProgramNode
│   └── statements: MantraNode[]
│
├── BlockStmtNode
│   └── statements: MantraNode[]
│
├── PrintStmtNode
│   └── expression: MantraNode
│
├── AssignStmtNode
│   ├── name: string
│   ├── value: MantraNode
│   └── is_declaration: bool
│
├── IfStmtNode
│   ├── condition: MantraNode
│   ├── then_branch: BlockStmtNode
│   └── else_branch: BlockStmtNode (optional)
│
├── WhileStmtNode
│   ├── condition: MantraNode
│   └── body: BlockStmtNode
│
├── ForStmtNode
│   ├── variable: string
│   ├── start: MantraNode
│   ├── end: MantraNode
│   ├── step: MantraNode (optional)
│   └── body: BlockStmtNode
│
├── FuncDefNode
│   ├── name: string
│   ├── params: string[]
│   └── body: BlockStmtNode
│
├── ReturnStmtNode
│   └── value: MantraNode (optional)
│
├── BreakStmtNode (no children)
├── ContinueStmtNode (no children)
│
├── BinaryExprNode
│   ├── op: TokenType
│   ├── left: MantraNode
│   └── right: MantraNode
│
├── UnaryExprNode
│   ├── op: TokenType
│   └── operand: MantraNode
│
├── CallExprNode
│   ├── callee: MantraNode
│   └── args: MantraNode[]
│
├── IndexExprNode
│   ├── target: MantraNode
│   └── index: MantraNode
│
├── MemberExprNode
│   ├── object: MantraNode
│   └── field: string
│
├── NumberLitNode
│   └── value: double
│
├── StringLitNode
│   └── value: string
│
├── BoolLitNode
│   └── value: bool
│
├── NullLitNode (no children)
│
├── IdentifierNode
│   └── name: string
│
└── ArrayLitNode
    └── elements: MantraNode[]
```

## Example 1: Simple Print Statement

### Code
```mantra
dikhao "Hello World"
```

### AST Diagram
```
Program
└── PrintStmt (line 1, col 1)
    └── StringLit("Hello World")
```

### Semantic: Print the string "Hello World" to output

---

## Example 2: Variable Assignment

### Code
```mantra
x = 5
dikhao x
```

### AST Diagram
```
Program
├── AssignStmt(x, is_declaration=true) (line 1, col 1)
│   └── NumberLit(5)
└── PrintStmt (line 2, col 1)
    └── Identifier(x)
```

### Semantic: Assign 5 to x, then print x (output: 5)

---

## Example 3: Arithmetic Expression

### Code
```mantra
dikhao 2 + 3 * 4
```

### AST Diagram
```
Program
└── PrintStmt (line 1, col 1)
    └── BinaryExpr(+) (line 1, col 8)
        ├── NumberLit(2)
        └── BinaryExpr(*) (line 1, col 14)
            ├── NumberLit(3)
            └── NumberLit(4)
```

### Semantic: Print 2 + (3 * 4) = 14
**Note**: Multiplication has higher precedence than addition, so 3*4 is grouped first

---

## Example 4: Logical Expression

### Code
```mantra
agar x > 5 aur y < 10 tab
    dikhao "yes"
warna
    dikhao "no"
```

### AST Diagram
```
Program
└── IfStmt (line 1, col 1)
    ├── condition: BinaryExpr(and) (line 1, col 8)
    │   ├── BinaryExpr(>) (line 1, col 8)
    │   │   ├── Identifier(x)
    │   │   └── NumberLit(5)
    │   └── BinaryExpr(<) (line 1, col 16)
    │       ├── Identifier(y)
    │       └── NumberLit(10)
    ├── then_branch: Block (line 2, col 4)
    │   └── PrintStmt
    │       └── StringLit("yes")
    └── else_branch: Block (line 4, col 4)
        └── PrintStmt
            └── StringLit("no")
```

### Semantic: If (x > 5 AND y < 10), print "yes", else print "no"

---

## Example 5: Function Definition and Call

### Code
```mantra
kaam add(a, b)
    wapas a + b

result = add(3, 4)
dikhao result
```

### AST Diagram
```
Program
├── FuncDef(add) (line 1, col 1)
│   ├── params: [a, b]
│   └── body: Block
│       └── ReturnStmt
│           └── BinaryExpr(+)
│               ├── Identifier(a)
│               └── Identifier(b)
├── AssignStmt(result) (line 4, col 1)
│   └── CallExpr (line 4, col 11)
│       ├── callee: Identifier(add)
│       └── args: [NumberLit(3), NumberLit(4)]
└── PrintStmt (line 5, col 1)
    └── Identifier(result)
```

### Semantic: Define function add(a,b) returning a+b. Call add(3,4), assign result to result variable. Print result (output: 7)

---

## Example 6: Array Operations

### Code
```mantra
arr = [1, 2, 3, 4, 5]
dikhao arr[0]
dikhao arr[2]
```

### AST Diagram
```
Program
├── AssignStmt(arr) (line 1, col 1)
│   └── ArrayLit
│       ├── NumberLit(1)
│       ├── NumberLit(2)
│       ├── NumberLit(3)
│       ├── NumberLit(4)
│       └── NumberLit(5)
├── PrintStmt (line 2, col 1)
│   └── IndexExpr
│       ├── target: Identifier(arr)
│       └── index: NumberLit(0)
└── PrintStmt (line 3, col 1)
    └── IndexExpr
        ├── target: Identifier(arr)
        └── index: NumberLit(2)
```

### Semantic: Create array. Print arr[0]=1. Print arr[2]=3.

---

## Example 7: Loop

### Code
```mantra
baarbaar i = 0 se 3 tak
    dikhao i
```

### AST Diagram
```
Program
└── ForStmt (line 1, col 1)
    ├── variable: i
    ├── start: NumberLit(0)
    ├── end: NumberLit(3)
    ├── step: null (default increment)
    └── body: Block
        └── PrintStmt
            └── Identifier(i)
```

### Semantic: Loop variable i from 0 to 3 (inclusive). Print i each iteration. (output: 0, 1, 2, 3)

---

## Example 8: Unary Operation

### Code
```mantra
x = 5
dikhao -x
dikhao not true
```

### AST Diagram
```
Program
├── AssignStmt(x)
│   └── NumberLit(5)
├── PrintStmt
│   └── UnaryExpr(-) (line 2, col 8)
│       └── Identifier(x)
└── PrintStmt
    └── UnaryExpr(not) (line 3, col 8)
        └── BoolLit(true)
```

### Semantic: Assign 5 to x. Print -5. Print false. (output: -5, false)

---

## Example 9: Nested Blocks

### Code
```mantra
agar true tab
    agar false tab
        dikhao "A"
    warna
        dikhao "B"
warna
    dikhao "C"
```

### AST Diagram
```
Program
└── IfStmt (outer)
    ├── condition: BoolLit(true)
    ├── then_branch: Block
    │   └── IfStmt (inner) (line 2, col 5)
    │       ├── condition: BoolLit(false)
    │       ├── then_branch: Block
    │       │   └── PrintStmt
    │       │       └── StringLit("A")
    │       └── else_branch: Block
    │           └── PrintStmt
    │               └── StringLit("B")
    └── else_branch: Block
        └── PrintStmt
            └── StringLit("C")
```

### Semantic: Outer if true, so enter then-branch. Inner if false, so go to inner else-branch, print "B". (output: B)

---

## Example 10: Complex Expression with Precedence

### Code
```mantra
dikhao 2 + 3 * 4 - 5 / 2
```

### AST Diagram
```
Program
└── PrintStmt
    └── BinaryExpr(-) (precedence 5, level 5)
        ├── BinaryExpr(+) (precedence 5, level 5)
        │   ├── NumberLit(2)
        │   └── BinaryExpr(*) (precedence 6, level 6)
        │       ├── NumberLit(3)
        │       └── NumberLit(4)
        └── BinaryExpr(/) (precedence 6, level 6)
            ├── NumberLit(5)
            └── NumberLit(2)
```

### Evaluation Order
1. 3 * 4 = 12
2. 5 / 2 = 2 (integer division)
3. 2 + 12 = 14
4. 14 - 2 = 12
5. Print 12

### Semantic: Following operator precedence (*/  before +/-), evaluate to 12

---

## AST Traversal Examples

### In-order traversal (for expression):
`2 + (3 * 4) - (5 / 2)`
```
Left -> Node -> Right
2 + (3 * 4 - 5 / 2)
```

### Pre-order traversal (reverse Polish):
`+ 2 * 3 4 - / 5 2`
```
Node -> Left -> Right
```

### Post-order traversal (Polish):
`2 3 4 * + 5 2 / -`
```
Left -> Right -> Node
```

---

## AST vs Parse Tree

**Parse Tree**: Includes all syntactic detail (tokens, delimiters, etc.)
```
Expression
├── Term
│   ├── Factor
│   │   └── Number(2)
│   └── +
├── Term
│   ├── Factor
│   │   └── Number(3)
│   └── *
│   ├── Factor
│   │   └── Number(4)
```

**AST**: Simplified, only semantic information
```
BinaryExpr(+)
├── Number(2)
└── BinaryExpr(*)
    ├── Number(3)
    └── Number(4)
```

AST is what the semantic analyzer and interpreter use.

---

## Key Observations

1. **Node Location**: Each node has `line` and `column` for error reporting
2. **Left Associativity**: `a + b + c` becomes `(a + b) + c` in AST
3. **Right Associativity**: `!-a` becomes `!(−a)` in AST
4. **Operator Precedence**: Higher precedence operators are deeper in tree
5. **Block Structure**: Compound statements wrap body in Block node
6. **Optional Nodes**: Null/optional nodes represented with (optional)

---

## AST Construction Process

1. **Lexer**: Input → Tokens
2. **Parser**: Tokens → Parse Tree → AST
3. **Semantic Analyzer**: AST → Annotated AST (type info, resolved names)
4. **Interpreter**: Annotated AST → Execution

Each stage transforms representation toward executable form.

---

## References

- GRAMMAR.ebnf - Grammar rules that generate these ASTs
- PARSER_DESIGN.md - How parser constructs ASTs
- core/ast.h - C++ AST node definitions
