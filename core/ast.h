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
    Program,
    Block,
    Print,
    If,
    While,
    For,
    Function,
    Return,
    Assign,
    Binary,
    Unary,
    Number,
    String,
    Identifier,
    Call,
    Boolean,
    ExprStatement
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

    virtual std::string prettyPrint(int indent = 0) const = 0;
};

struct ProgramNode : public MantraNode {
    std::vector<std::unique_ptr<MantraNode>> statements;

    ProgramNode(int node_line, int node_column)
        : MantraNode(NodeType::Program, node_line, node_column) {}

    std::string prettyPrint(int indent = 0) const override {
        std::ostringstream out;
        out << indentString(indent) << "Program" << "\n";
        for (const auto& stmt : statements) {
            out << stmt->prettyPrint(indent + 1);
        }
        return out.str();
    }
};

struct BlockNode : public MantraNode {
    std::vector<std::unique_ptr<MantraNode>> statements;

    BlockNode(int node_line, int node_column)
        : MantraNode(NodeType::Block, node_line, node_column) {}

    std::string prettyPrint(int indent = 0) const override {
        std::ostringstream out;
        out << indentString(indent) << "Block" << "\n";
        for (const auto& stmt : statements) {
            out << stmt->prettyPrint(indent + 1);
        }
        return out.str();
    }
};

struct PrintNode : public MantraNode {
    std::unique_ptr<MantraNode> expression;

    PrintNode(std::unique_ptr<MantraNode> expr, int node_line, int node_column)
        : MantraNode(NodeType::Print, node_line, node_column),
          expression(std::move(expr)) {}

    std::string prettyPrint(int indent = 0) const override {
        std::ostringstream out;
        out << indentString(indent) << "Print" << "\n";
        if (expression) {
            out << expression->prettyPrint(indent + 1);
        }
        return out.str();
    }
};

struct IfNode : public MantraNode {
    std::unique_ptr<MantraNode> condition;
    std::unique_ptr<BlockNode> then_branch;
    std::unique_ptr<BlockNode> else_branch;

    IfNode(std::unique_ptr<MantraNode> cond,
           std::unique_ptr<BlockNode> then_block,
           std::unique_ptr<BlockNode> else_block,
           int node_line,
           int node_column)
        : MantraNode(NodeType::If, node_line, node_column),
          condition(std::move(cond)),
          then_branch(std::move(then_block)),
          else_branch(std::move(else_block)) {}

    std::string prettyPrint(int indent = 0) const override {
        std::ostringstream out;
        out << indentString(indent) << "If" << "\n";
        out << indentString(indent + 1) << "Condition" << "\n";
        if (condition) {
            out << condition->prettyPrint(indent + 2);
        }
        out << indentString(indent + 1) << "Then" << "\n";
        if (then_branch) {
            out << then_branch->prettyPrint(indent + 2);
        }
        if (else_branch) {
            out << indentString(indent + 1) << "Else" << "\n";
            out << else_branch->prettyPrint(indent + 2);
        }
        return out.str();
    }
};

struct WhileNode : public MantraNode {
    std::unique_ptr<MantraNode> condition;
    std::unique_ptr<BlockNode> body;

    WhileNode(std::unique_ptr<MantraNode> cond,
              std::unique_ptr<BlockNode> body_block,
              int node_line,
              int node_column)
        : MantraNode(NodeType::While, node_line, node_column),
          condition(std::move(cond)),
          body(std::move(body_block)) {}

    std::string prettyPrint(int indent = 0) const override {
        std::ostringstream out;
        out << indentString(indent) << "While" << "\n";
        out << indentString(indent + 1) << "Condition" << "\n";
        if (condition) {
            out << condition->prettyPrint(indent + 2);
        }
        out << indentString(indent + 1) << "Body" << "\n";
        if (body) {
            out << body->prettyPrint(indent + 2);
        }
        return out.str();
    }
};

struct ForNode : public MantraNode {
    std::string variable;
    std::unique_ptr<MantraNode> start;
    std::unique_ptr<MantraNode> end;
    std::unique_ptr<BlockNode> body;

    ForNode(std::string var,
            std::unique_ptr<MantraNode> start_expr,
            std::unique_ptr<MantraNode> end_expr,
            std::unique_ptr<BlockNode> body_block,
            int node_line,
            int node_column)
        : MantraNode(NodeType::For, node_line, node_column),
          variable(std::move(var)),
          start(std::move(start_expr)),
          end(std::move(end_expr)),
          body(std::move(body_block)) {}

    std::string prettyPrint(int indent = 0) const override {
        std::ostringstream out;
        out << indentString(indent) << "For" << " " << variable << "\n";
        out << indentString(indent + 1) << "Start" << "\n";
        if (start) {
            out << start->prettyPrint(indent + 2);
        }
        out << indentString(indent + 1) << "End" << "\n";
        if (end) {
            out << end->prettyPrint(indent + 2);
        }
        out << indentString(indent + 1) << "Body" << "\n";
        if (body) {
            out << body->prettyPrint(indent + 2);
        }
        return out.str();
    }
};

struct FunctionNode : public MantraNode {
    std::string name;
    std::vector<std::string> parameters;
    std::unique_ptr<BlockNode> body;

    FunctionNode(std::string func_name,
                 std::vector<std::string> params,
                 std::unique_ptr<BlockNode> body_block,
                 int node_line,
                 int node_column)
        : MantraNode(NodeType::Function, node_line, node_column),
          name(std::move(func_name)),
          parameters(std::move(params)),
          body(std::move(body_block)) {}

    std::string prettyPrint(int indent = 0) const override {
        std::ostringstream out;
        out << indentString(indent) << "Function" << " " << name << "(";
        for (size_t i = 0; i < parameters.size(); ++i) {
            out << parameters[i];
            if (i + 1 < parameters.size()) {
                out << ", ";
            }
        }
        out << ")" << "\n";
        if (body) {
            out << body->prettyPrint(indent + 1);
        }
        return out.str();
    }
};

struct ReturnNode : public MantraNode {
    std::unique_ptr<MantraNode> value;

    ReturnNode(std::unique_ptr<MantraNode> expr, int node_line, int node_column)
        : MantraNode(NodeType::Return, node_line, node_column),
          value(std::move(expr)) {}

    std::string prettyPrint(int indent = 0) const override {
        std::ostringstream out;
        out << indentString(indent) << "Return" << "\n";
        if (value) {
            out << value->prettyPrint(indent + 1);
        }
        return out.str();
    }
};

struct AssignNode : public MantraNode {
    std::string name;
    std::unique_ptr<MantraNode> value;

    AssignNode(std::string var_name,
               std::unique_ptr<MantraNode> expr,
               int node_line,
               int node_column)
        : MantraNode(NodeType::Assign, node_line, node_column),
          name(std::move(var_name)),
          value(std::move(expr)) {}

    std::string prettyPrint(int indent = 0) const override {
        std::ostringstream out;
        out << indentString(indent) << "Assign" << " " << name << "\n";
        if (value) {
            out << value->prettyPrint(indent + 1);
        }
        return out.str();
    }
};

struct BinaryNode : public MantraNode {
    TokenType op;
    std::unique_ptr<MantraNode> left;
    std::unique_ptr<MantraNode> right;

    BinaryNode(TokenType oper,
               std::unique_ptr<MantraNode> lhs,
               std::unique_ptr<MantraNode> rhs,
               int node_line,
               int node_column)
        : MantraNode(NodeType::Binary, node_line, node_column),
          op(oper),
          left(std::move(lhs)),
          right(std::move(rhs)) {}

    std::string prettyPrint(int indent = 0) const override {
        std::ostringstream out;
        out << indentString(indent) << "Binary" << " " << tokenTypeToString(op) << "\n";
        if (left) {
            out << left->prettyPrint(indent + 1);
        }
        if (right) {
            out << right->prettyPrint(indent + 1);
        }
        return out.str();
    }
};

struct UnaryNode : public MantraNode {
    TokenType op;
    std::unique_ptr<MantraNode> operand;

    UnaryNode(TokenType oper,
              std::unique_ptr<MantraNode> expr,
              int node_line,
              int node_column)
        : MantraNode(NodeType::Unary, node_line, node_column),
          op(oper),
          operand(std::move(expr)) {}

    std::string prettyPrint(int indent = 0) const override {
        std::ostringstream out;
        out << indentString(indent) << "Unary" << " " << tokenTypeToString(op) << "\n";
        if (operand) {
            out << operand->prettyPrint(indent + 1);
        }
        return out.str();
    }
};

struct NumberNode : public MantraNode {
    double value;
    bool is_integer;
    std::string raw;

    NumberNode(double num_value,
               bool is_int,
               std::string raw_text,
               int node_line,
               int node_column)
        : MantraNode(NodeType::Number, node_line, node_column),
          value(num_value),
          is_integer(is_int),
          raw(std::move(raw_text)) {}

    std::string prettyPrint(int indent = 0) const override {
        std::ostringstream out;
        out << indentString(indent) << "Number" << " " << raw << "\n";
        return out.str();
    }
};

struct StringNode : public MantraNode {
    std::string value;

    StringNode(std::string str_value, int node_line, int node_column)
        : MantraNode(NodeType::String, node_line, node_column),
          value(std::move(str_value)) {}

    std::string prettyPrint(int indent = 0) const override {
        std::ostringstream out;
        out << indentString(indent) << "String" << " \"" << value << "\"" << "\n";
        return out.str();
    }
};

struct IdentifierNode : public MantraNode {
    std::string name;

    IdentifierNode(std::string ident, int node_line, int node_column)
        : MantraNode(NodeType::Identifier, node_line, node_column),
          name(std::move(ident)) {}

    std::string prettyPrint(int indent = 0) const override {
        std::ostringstream out;
        out << indentString(indent) << "Identifier" << " " << name << "\n";
        return out.str();
    }
};

struct CallNode : public MantraNode {
    std::unique_ptr<MantraNode> callee;
    std::vector<std::unique_ptr<MantraNode>> arguments;

    CallNode(std::unique_ptr<MantraNode> call_target,
             std::vector<std::unique_ptr<MantraNode>> args,
             int node_line,
             int node_column)
        : MantraNode(NodeType::Call, node_line, node_column),
          callee(std::move(call_target)),
          arguments(std::move(args)) {}

    std::string prettyPrint(int indent = 0) const override {
        std::ostringstream out;
        out << indentString(indent) << "Call" << "\n";
        if (callee) {
            out << callee->prettyPrint(indent + 1);
        }
        for (const auto& arg : arguments) {
            out << arg->prettyPrint(indent + 1);
        }
        return out.str();
    }
};

struct BooleanNode : public MantraNode {
    bool value;

    BooleanNode(bool bool_value, int node_line, int node_column)
        : MantraNode(NodeType::Boolean, node_line, node_column),
          value(bool_value) {}

    std::string prettyPrint(int indent = 0) const override {
        std::ostringstream out;
        out << indentString(indent) << "Boolean" << " " << (value ? "true" : "false")
            << "\n";
        return out.str();
    }
};

struct ExprStatementNode : public MantraNode {
    std::unique_ptr<MantraNode> expression;

    ExprStatementNode(std::unique_ptr<MantraNode> expr, int node_line, int node_column)
        : MantraNode(NodeType::ExprStatement, node_line, node_column),
          expression(std::move(expr)) {}

    std::string prettyPrint(int indent = 0) const override {
        std::ostringstream out;
        out << indentString(indent) << "Expression" << "\n";
        if (expression) {
            out << expression->prettyPrint(indent + 1);
        }
        return out.str();
    }
};

} // namespace mantra

#endif // MANTRA_AST_H
