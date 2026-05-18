#include "mantra/vm/bytecode.h"
#include "mantra/vm/vm.h"

#include <cstdlib>
#include <iostream>
#include <sstream>

namespace {

void require(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << std::endl;
        std::abort();
    }
}

void test_add_and_print() {
    mantra::BytecodeProgram program;
    const auto two = program.addConstant(mantra::Value::number(2));
    const auto three = program.addConstant(mantra::Value::number(3));

    program.emit(mantra::OpCode::PUSH_CONST, two);
    program.emit(mantra::OpCode::PUSH_CONST, three);
    program.emit(mantra::OpCode::ADD);
    program.emit(mantra::OpCode::PRINT);
    program.emit(mantra::OpCode::HALT);

    mantra::VM vm;
    std::ostringstream output;
    require(vm.execute(program, output), "VM execution failed");
    require(output.str() == "5\n", "Unexpected output");
}

void test_sub_and_halt() {
    mantra::BytecodeProgram program;
    const auto ten = program.addConstant(mantra::Value::number(10));
    const auto four = program.addConstant(mantra::Value::number(4));

    program.emit(mantra::OpCode::PUSH_CONST, ten);
    program.emit(mantra::OpCode::PUSH_CONST, four);
    program.emit(mantra::OpCode::SUB);
    program.emit(mantra::OpCode::PRINT);
    program.emit(mantra::OpCode::HALT);
    program.emit(mantra::OpCode::PRINT);

    mantra::VM vm;
    std::ostringstream output;
    require(vm.execute(program, output), "VM execution failed");
    require(output.str() == "6\n", "HALT did not stop execution");
}

void test_stack_underflow() {
    mantra::BytecodeProgram program;
    program.emit(mantra::OpCode::PRINT);
    program.emit(mantra::OpCode::HALT);

    mantra::VM vm;
    std::ostringstream output;
    require(!vm.execute(program, output), "Underflow should fail");
    require(vm.lastError() == "Stack underflow", "Unexpected underflow error");
}

void test_invalid_constant_index() {
    mantra::BytecodeProgram program;
    program.emit(mantra::OpCode::PUSH_CONST, 99);
    program.emit(mantra::OpCode::HALT);

    mantra::VM vm;
    std::ostringstream output;
    require(!vm.execute(program, output), "Invalid constant should fail");
    require(vm.lastError() == "Constant index out of range", "Unexpected constant error");
}

} // namespace

int main() {
    test_add_and_print();
    test_sub_and_halt();
    test_stack_underflow();
    test_invalid_constant_index();
    std::cout << "VM foundation test passed" << std::endl;
    return 0;
}
