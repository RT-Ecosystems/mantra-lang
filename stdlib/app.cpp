#include "app.h"

#include "../error/error.h"

#include <cstdio>
#include <cstdlib>
#include <stdexcept>

namespace mantra::stdlib {
namespace {

std::string requireString(const MantraValue& value, const std::string& name) {
    if (value.type != ValueType::String) {
        ErrorHandler::printError(ErrorType::TYPE_ERROR, name + " expects string", 0, 0);
        throw std::runtime_error(name);
    }
    return value.string_value;
}

double requireNumber(const MantraValue& value, const std::string& name) {
    if (value.type != ValueType::Number) {
        ErrorHandler::printError(ErrorType::TYPE_ERROR, name + " expects number", 0, 0);
        throw std::runtime_error(name);
    }
    return value.number_value;
}

void ensureArgs(const std::vector<MantraValue>& args, size_t expected, const std::string& name) {
    if (args.size() != expected) {
        ErrorHandler::printError(ErrorType::RUNTIME_ERROR,
                                 name + " requires " + std::to_string(expected) + " args",
                                 0, 0);
        throw std::runtime_error(name);
    }
}

} // anonymous namespace

MantraValue builtinAppWindow(const std::vector<MantraValue>& args) {
    ensureArgs(args, 3, "appWindow");
    std::string title = requireString(args[0], "appWindow");
    double width = requireNumber(args[1], "appWindow");
    double height = requireNumber(args[2], "appWindow");
    // Simulated desktop/mobile app development - creates a window
    std::printf("Created window: title='%s', width=%d, height=%d\n", 
                title.c_str(), (int)width, (int)height);
    return MantraValue::boolean(true);
}

MantraValue builtinAppButton(const std::vector<MantraValue>& args) {
    ensureArgs(args, 5, "appButton");
    std::string text = requireString(args[0], "appButton");
    double x = requireNumber(args[1], "appButton");
    double y = requireNumber(args[2], "appButton");
    double width = requireNumber(args[3], "appButton");
    double height = requireNumber(args[4], "appButton");
    // Simulated app development - adds a button to the app
    std::printf("Added button: text='%s', x=%d, y=%d, width=%d, height=%d\n",
                text.c_str(), (int)x, (int)y, (int)width, (int)height);
    return MantraValue::boolean(true);
}

MantraValue builtinAppLabel(const std::vector<MantraValue>& args) {
    ensureArgs(args, 3, "appLabel");
    std::string text = requireString(args[0], "appLabel");
    double x = requireNumber(args[1], "appLabel");
    double y = requireNumber(args[2], "appLabel");
    // Simulated app development - adds a label to the app
    std::printf("Added label: text='%s', x=%d, y=%d\n", text.c_str(), (int)x, (int)y);
    return MantraValue::boolean(true);
}

MantraValue builtinAppInput(const std::vector<MantraValue>& args) {
    ensureArgs(args, 4, "appInput");
    double x = requireNumber(args[0], "appInput");
    double y = requireNumber(args[1], "appInput");
    double width = requireNumber(args[2], "appInput");
    double height = requireNumber(args[3], "appInput");
    // Simulated app development - adds an input field to the app
    std::printf("Added input field: x=%d, y=%d, width=%d, height=%d\n",
                (int)x, (int)y, (int)width, (int)height);
    return MantraValue::boolean(true);
}

MantraValue builtinAppRun(const std::vector<MantraValue>& args) {
    ensureArgs(args, 0, "appRun");
    // Simulated app development - runs the app event loop
    std::printf("App loop started\n");
    return MantraValue::boolean(true);
}

} // namespace mantra::stdlib
