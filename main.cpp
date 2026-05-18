#include "mantra/vm/bytecode.h"
#include "mantra/vm/vm.h"

#include <iostream>

int main() {
    mantra::BytecodeProgram program;
    const auto left = program.addConstant(mantra::Value::number(2));
    const auto right = program.addConstant(mantra::Value::number(3));

    program.emit(mantra::OpCode::PUSH_CONST, left);
    program.emit(mantra::OpCode::PUSH_CONST, right);
    program.emit(mantra::OpCode::ADD);
    program.emit(mantra::OpCode::PRINT);
    program.emit(mantra::OpCode::HALT);

    mantra::VM vm;
    if (!vm.execute(program, std::cout)) {
        std::cerr << vm.lastError() << std::endl;
        return 1;
    }

    return 0;
}
