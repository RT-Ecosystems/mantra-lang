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

} // namespace

int main() {
    test_add_and_print();
    std::cout << "VM foundation test passed" << std::endl;
    return 0;
}
