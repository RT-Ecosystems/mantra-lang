#pragma once

#include <cstddef>

namespace mantra {

enum class OpCode {
    Constant,
    Null,
    TrueValue,
    FalseValue,
    Pop,
    PushScope,
    PopScope,
    GetName,
    DefineName,
    SetName,
    DefineFunction,
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,
    Equal,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    Negate,
    Not,
    Jump,
    JumpIfFalse,
    Loop,
    Print,
    BuildArray,
    GetIndex,
    Call,
    Return
};

inline const char* opCodeToString(OpCode op) {
    switch (op) {
        case OpCode::Constant: return "Constant";
        case OpCode::Null: return "Null";
        case OpCode::TrueValue: return "True";
        case OpCode::FalseValue: return "False";
        case OpCode::Pop: return "Pop";
        case OpCode::PushScope: return "PushScope";
        case OpCode::PopScope: return "PopScope";
        case OpCode::GetName: return "GetName";
        case OpCode::DefineName: return "DefineName";
        case OpCode::SetName: return "SetName";
        case OpCode::DefineFunction: return "DefineFunction";
        case OpCode::Add: return "Add";
        case OpCode::Subtract: return "Subtract";
        case OpCode::Multiply: return "Multiply";
        case OpCode::Divide: return "Divide";
        case OpCode::Modulo: return "Modulo";
        case OpCode::Equal: return "Equal";
        case OpCode::NotEqual: return "NotEqual";
        case OpCode::Less: return "Less";
        case OpCode::LessEqual: return "LessEqual";
        case OpCode::Greater: return "Greater";
        case OpCode::GreaterEqual: return "GreaterEqual";
        case OpCode::Negate: return "Negate";
        case OpCode::Not: return "Not";
        case OpCode::Jump: return "Jump";
        case OpCode::JumpIfFalse: return "JumpIfFalse";
        case OpCode::Loop: return "Loop";
        case OpCode::Print: return "Print";
        case OpCode::BuildArray: return "BuildArray";
        case OpCode::GetIndex: return "GetIndex";
        case OpCode::Call: return "Call";
        case OpCode::Return: return "Return";
        default: return "Unknown";
    }
}

} // namespace mantra
