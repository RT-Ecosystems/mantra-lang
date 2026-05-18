#ifndef MANTRA_AST_H
#define MANTRA_AST_H

#include "token.h"
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace mantra {

enum class NodeType {
    PROGRAM,
    BLOCK_STMT,
    PRINT_STMT,
    ASSIGN_STMT,
    IF_STMT,
    WHILE_STMT,
    FOR_STMT,
    FUNC_DEF,
    RETURN_STMT,
    BREAK_STMT,
    EXPR_STMT,
    BINARY_EXPR,
    UNARY_EXPR,
    CALL_EXPR,
    NUMBER_LIT,
    STRING_LIT,
    BOOL_LIT,
    NULL_LIT,
    IDENTIFIER,
    ARRAY_LIT,
    INDEX_EXPR
};

inline std::string indentString(int indent) {
    return std::string(static_cast<size_t>(indent) * 2, ' ');
}

struct MantraNode {
    NodeType type;
    int line;
    int column;

    MantraNode(NodeType node_type, int node_line, int node_column)
        : type(node_type), line(node_line), column(node_column) {}

    virtual ~MantraNode() = default;

    virtual std::string toString() const { return toStringIndented(0); }
    virtual std::string toStringIndented(int indent) const = 0;

protected:
    std::string locationString() const {
        std::ostringstream out;
        out << "(line " << line << ", col " << column << ")";
        return out.str();
    }
};

struct ProgramNode : public MantraNode {
    std::vector<std::unique_ptr<MantraNode>> statements;

    ProgramNode(int node_line, int node_column)
        : MantraNode(NodeType::PROGRAM, node_line, node_column) {}

    std::string toStringIndented(int indent) const override {
        std::ostringstream out;
        out << indentString(indent) << "Program " << locationString() << "\n";
        for (const auto& stmt : statements) {
            if (stmt) {
                out << stmt->toStringIndented(indent + 1);
            }
        }
        return out.str();
    }
};

struct BlockStmtNode : public MantraNode {
    std::vector<std::unique_ptr<MantraNode>> statements;

    BlockStmtNode(int node_line, int node_column)
        : MantraNode(NodeType::BLOCK_STMT, node_line, node_column) {}

    std::string toStringIndented(int indent) const override {
        std::ostringstream out;
        out << indentString(indent) << "Block " << locationString() << "\n";
        for (const auto& stmt : statements) {
            if (stmt) {
                out << stmt->toStringIndented(indent + 1);
            }
        }
        return out.str();
    }
};

struct PrintStmtNode : public MantraNode {
    std::unique_ptr<MantraNode> expression;

    PrintStmtNode(std::unique_ptr<MantraNode> expr, int node_line, int node_column)
        : MantraNode(NodeType::PRINT_STMT, node_line, node_column),
          expression(std::move(expr)) {}

    std::string toStringIndented(int indent) const override {
        std::ostringstream out;
        out << indentString(indent) << "PrintStmt " << locationString() << "\n";
        if (expression) {
            out << expression->toStringIndented(indent + 1);
        }
        return out.str();
    }
};

struct AssignStmtNode : public MantraNode {
    std::string name;
    std::unique_ptr<MantraNode> value;

    AssignStmtNode(std::string var_name,
                   std::unique_ptr<MantraNode> expr,
                   int node_line,
                   int node_column)
        : MantraNode(NodeType::ASSIGN_STMT, node_line, node_column),
          name(std::move(var_name)),
          value(std::move(expr)) {}

    std::string toStringIndented(int indent) const override {
        std::ostringstream out;
        out << indentString(indent) << "AssignStmt " << name << " " << locationString() << "\n";
        if (value) {
            out << value->toStringIndented(indent + 1);
        }
        return out.str();
    }
};

struct IfStmtNode : public MantraNode {
    std::unique_ptr<MantraNode> condition;
    std::unique_ptr<BlockStmtNode> then_branch;
    std::unique_ptr<BlockStmtNode> else_branch;

    IfStmtNode(std::unique_ptr<MantraNode> cond,
               std::unique_ptr<BlockStmtNode> then_block,
               std::unique_ptr<BlockStmtNode> else_block,
               int node_line,
               int node_column)
        : MantraNode(NodeType::IF_STMT, node_line, node_column),
          condition(std::move(cond)),
          then_branch(std::move(then_block)),
          else_branch(std::move(else_block)) {}

    std::string toStringIndented(int indent) const override {
        std::ostringstream out;
        out << indentString(indent) << "IfStmt " << locationString() << "\n";
        out << indentString(indent + 1) << "Condition\n";
        if (condition) {
            out << condition->toStringIndented(indent + 2);
        }
        out << indentString(indent + 1) << "Then\n";
        if (then_branch) {
            out << then_branch->toStringIndented(indent + 2);
        }
        if (else_branch) {
            out << indentString(indent + 1) << "Else\n";
            out << else_branch->toStringIndented(indent + 2);
        }
        return out.str();
    }
};

struct WhileStmtNode : public MantraNode {
    std::unique_ptr<MantraNode> condition;
    std::unique_ptr<BlockStmtNode> body;

    WhileStmtNode(std::unique_ptr<MantraNode> cond,
                  std::unique_ptr<BlockStmtNode> body_block,
                  int node_line,
                  int node_column)
        : MantraNode(NodeType::WHILE_STMT, node_line, node_column),
          condition(std::move(cond)),
          body(std::move(body_block)) {}

    std::string toStringIndented(int indent) const override {
        std::ostringstream out;
        out << indentString(indent) << "WhileStmt " << locationString() << "\n";
        out << indentString(indent + 1) << "Condition\n";
        if (condition) {
            out << condition->toStringIndented(indent + 2);
        }
        out << indentString(indent + 1) << "Body\n";
        if (body) {
            out << body->toStringIndented(indent + 2);
        }
        return out.str();
    }
};

struct ForStmtNode : public MantraNode {
    std::string variable;
    std::unique_ptr<MantraNode> start;
    std::unique_ptr<MantraNode> end;
    std::unique_ptr<MantraNode> step;
    std::unique_ptr<BlockStmtNode> body;

    ForStmtNode(std::string var,
                std::unique_ptr<MantraNode> start_expr,
                std::unique_ptr<MantraNode> end_expr,
                std::unique_ptr<MantraNode> step_expr,
                std::unique_ptr<BlockStmtNode> body_block,
                int node_line,
                int node_column)
        : MantraNode(NodeType::FOR_STMT, node_line, node_column),
          variable(std::move(var)),
          start(std::move(start_expr)),
          end(std::move(end_expr)),
          step(std::move(step_expr)),
          body(std::move(body_block)) {}

    std::string toStringIndented(int indent) const override {
        std::ostringstream out;
        out << indentString(indent) << "ForStmt " << variable << " " << locationString() << "\n";
        out << indentString(indent + 1) << "Start\n";
        if (start) {
            out << start->toStringIndented(indent + 2);
        }
        out << indentString(indent + 1) << "End\n";
        if (end) {
            out << end->toStringIndented(indent + 2);
        }
        if (step) {
            out << indentString(indent + 1) << "Step\n";
            out << step->toStringIndented(indent + 2);
        }
        out << indentString(indent + 1) << "Body\n";
        if (body) {
            out << body->toStringIndented(indent + 2);
        }
        return out.str();
    }
};

struct FuncDefNode : public MantraNode {
    std::string name;
    std::vector<std::string> parameters;
    std::unique_ptr<BlockStmtNode> body;

    FuncDefNode(std::string func_name,
                std::vector<std::string> params,
                std::unique_ptr<BlockStmtNode> body_block,
                int node_line,
                int node_column)
        : MantraNode(NodeType::FUNC_DEF, node_line, node_column),
          name(std::move(func_name)),
          parameters(std::move(params)),
          body(std::move(body_block)) {}

    std::string toStringIndented(int indent) const override {
        std::ostringstream out;
        out << indentString(indent) << "FuncDef " << name << "(";
        for (size_t i = 0; i < parameters.size(); ++i) {
            out << parameters[i];
            if (i + 1 < parameters.size()) {
                out << ", ";
            }
        }
        out << ") " << locationString() << "\n";
        if (body) {
            out << body->toStringIndented(indent + 1);
        }
        return out.str();
    }
};

struct ReturnStmtNode : public MantraNode {
    std::unique_ptr<MantraNode> value;

    ReturnStmtNode(std::unique_ptr<MantraNode> expr, int node_line, int node_column)
        : MantraNode(NodeType::RETURN_STMT, node_line, node_column),
          value(std::move(expr)) {}

    std::string toStringIndented(int indent) const override {
        std::ostringstream out;
        out << indentString(indent) << "ReturnStmt " << locationString() << "\n";
        if (value) {
            out << value->toStringIndented(indent + 1);
        }
        return out.str();
    }
};

struct BreakStmtNode : public MantraNode {
    BreakStmtNode(int node_line, int node_column)
        : MantraNode(NodeType::BREAK_STMT, node_line, node_column) {}

    std::string toStringIndented(int indent) const override {
        std::ostringstream out;
        out << indentString(indent) << "BreakStmt " << locationString() << "\n";
        return out.str();
    }
};

struct ExprStmtNode : public MantraNode {
    std::unique_ptr<MantraNode> expression;

    ExprStmtNode(std::unique_ptr<MantraNode> expr, int node_line, int node_column)
        : MantraNode(NodeType::EXPR_STMT, node_line, node_column),
          expression(std::move(expr)) {}

    std::string toStringIndented(int indent) const override {
        std::ostringstream out;
        out << indentString(indent) << "ExprStmt " << locationString() << "\n";
        if (expression) {
            out << expression->toStringIndented(indent + 1);
        }
        return out.str();
    }
};

struct BinaryExprNode : public MantraNode {
    TokenType op;
    std::unique_ptr<MantraNode> left;
    std::unique_ptr<MantraNode> right;

    BinaryExprNode(TokenType oper,
                   std::unique_ptr<MantraNode> lhs,
                   std::unique_ptr<MantraNode> rhs,
                   int node_line,
                   int node_column)
        : MantraNode(NodeType::BINARY_EXPR, node_line, node_column),
          op(oper),
          left(std::move(lhs)),
          right(std::move(rhs)) {}

    std::string toStringIndented(int indent) const override {
        std::ostringstream out;
        out << indentString(indent) << "BinaryExpr " << tokenTypeToString(op) << " "
            << locationString() << "\n";
        if (left) {
            out << left->toStringIndented(indent + 1);
        }
        if (right) {
            out << right->toStringIndented(indent + 1);
        }
        return out.str();
    }
};

struct UnaryExprNode : public MantraNode {
    TokenType op;
    std::unique_ptr<MantraNode> operand;

    UnaryExprNode(TokenType oper,
                  std::unique_ptr<MantraNode> expr,
                  int node_line,
                  int node_column)
        : MantraNode(NodeType::UNARY_EXPR, node_line, node_column),
          op(oper),
          operand(std::move(expr)) {}

    std::string toStringIndented(int indent) const override {
        std::ostringstream out;
        out << indentString(indent) << "UnaryExpr " << tokenTypeToString(op) << " "
            << locationString() << "\n";
        if (operand) {
            out << operand->toStringIndented(indent + 1);
        }
        return out.str();
    }
};

struct CallExprNode : public MantraNode {
    std::unique_ptr<MantraNode> callee;
    std::vector<std::unique_ptr<MantraNode>> arguments;

    CallExprNode(std::unique_ptr<MantraNode> call_target,
                 std::vector<std::unique_ptr<MantraNode>> args,
                 int node_line,
                 int node_column)
        : MantraNode(NodeType::CALL_EXPR, node_line, node_column),
          callee(std::move(call_target)),
          arguments(std::move(args)) {}

    std::string toStringIndented(int indent) const override {
        std::ostringstream out;
        out << indentString(indent) << "CallExpr " << locationString() << "\n";
        if (callee) {
            out << callee->toStringIndented(indent + 1);
        }
        for (const auto& arg : arguments) {
            if (arg) {
                out << arg->toStringIndented(indent + 1);
            }
        }
        return out.str();
    }
};

struct NumberLitNode : public MantraNode {
    double value;
    bool is_integer;
    std::string raw;

    NumberLitNode(double num_value,
                  bool is_int,
                  std::string raw_text,
                  int node_line,
                  int node_column)
        : MantraNode(NodeType::NUMBER_LIT, node_line, node_column),
          value(num_value),
          is_integer(is_int),
          raw(std::move(raw_text)) {}

    std::string toStringIndented(int indent) const override {
        std::ostringstream out;
        out << indentString(indent) << "NumberLit " << raw << " " << locationString() << "\n";
        return out.str();
    }
};

struct StringLitNode : public MantraNode {
    std::string value;

    StringLitNode(std::string str_value, int node_line, int node_column)
        : MantraNode(NodeType::STRING_LIT, node_line, node_column),
          value(std::move(str_value)) {}

    std::string toStringIndented(int indent) const override {
        std::ostringstream out;
        out << indentString(indent) << "StringLit \"" << value << "\" "
            << locationString() << "\n";
        return out.str();
    }
};

struct BoolLitNode : public MantraNode {
    bool value;

    BoolLitNode(bool bool_value, int node_line, int node_column)
        : MantraNode(NodeType::BOOL_LIT, node_line, node_column),
          value(bool_value) {}

    std::string toStringIndented(int indent) const override {
        std::ostringstream out;
        out << indentString(indent) << "BoolLit " << (value ? "true" : "false") << " "
            << locationString() << "\n";
        return out.str();
    }
};

struct NullLitNode : public MantraNode {
    NullLitNode(int node_line, int node_column)
        : MantraNode(NodeType::NULL_LIT, node_line, node_column) {}

    std::string toStringIndented(int indent) const override {
        std::ostringstream out;
        out << indentString(indent) << "NullLit " << locationString() << "\n";
        return out.str();
    }
};

struct IdentifierNode : public MantraNode {
    std::string name;

    IdentifierNode(std::string ident, int node_line, int node_column)
        : MantraNode(NodeType::IDENTIFIER, node_line, node_column),
          name(std::move(ident)) {}

    std::string toStringIndented(int indent) const override {
        std::ostringstream out;
        out << indentString(indent) << "Identifier " << name << " " << locationString() << "\n";
        return out.str();
    }
};

struct ArrayLitNode : public MantraNode {
    std::vector<std::unique_ptr<MantraNode>> elements;

    ArrayLitNode(std::vector<std::unique_ptr<MantraNode>> elems,
                 int node_line,
                 int node_column)
        : MantraNode(NodeType::ARRAY_LIT, node_line, node_column),
          elements(std::move(elems)) {}

    std::string toStringIndented(int indent) const override {
        std::ostringstream out;
        out << indentString(indent) << "ArrayLit " << locationString() << "\n";
        for (const auto& element : elements) {
            if (element) {
                out << element->toStringIndented(indent + 1);
            }
        }
        return out.str();
    }
};

struct IndexExprNode : public MantraNode {
    std::unique_ptr<MantraNode> collection;
    std::unique_ptr<MantraNode> index;

    IndexExprNode(std::unique_ptr<MantraNode> target,
                  std::unique_ptr<MantraNode> idx,
                  int node_line,
                  int node_column)
        : MantraNode(NodeType::INDEX_EXPR, node_line, node_column),
          collection(std::move(target)),
          index(std::move(idx)) {}

    std::string toStringIndented(int indent) const override {
        std::ostringstream out;
        out << indentString(indent) << "IndexExpr " << locationString() << "\n";
        if (collection) {
            out << collection->toStringIndented(indent + 1);
        }
        if (index) {
            out << index->toStringIndented(indent + 1);
        }
        return out.str();
    }
};

} // namespace mantra

#endif // MANTRA_AST_H
