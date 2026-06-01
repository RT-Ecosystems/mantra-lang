#include "browser.h"

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

void ensureArgs(const std::vector<MantraValue>& args, size_t expected, const std::string& name) {
    if (args.size() != expected) {
        ErrorHandler::printError(ErrorType::RUNTIME_ERROR,
                                 name + " requires " + std::to_string(expected) + " args",
                                 0, 0);
        throw std::runtime_error(name);
    }
}

} // anonymous namespace

MantraValue builtinBrowserOpen(const std::vector<MantraValue>& args) {
    ensureArgs(args, 1, "browserOpen");
    std::string url = requireString(args[0], "browserOpen");
    // Simulated web automation - opens a browser with the given URL
    // In a real implementation, this would use Selenium or similar
    std::printf("Browser opened with URL: %s\n", url.c_str());
    return MantraValue::boolean(true);
}

MantraValue builtinBrowserClick(const std::vector<MantraValue>& args) {
    ensureArgs(args, 1, "browserClick");
    std::string selector = requireString(args[0], "browserClick");
    // Simulated web automation - clicks on an element
    std::printf("Clicked element with selector: %s\n", selector.c_str());
    return MantraValue::boolean(true);
}

MantraValue builtinBrowserType(const std::vector<MantraValue>& args) {
    ensureArgs(args, 2, "browserType");
    std::string selector = requireString(args[0], "browserType");
    std::string text = requireString(args[1], "browserType");
    // Simulated web automation - types text into an input field
    std::printf("Typed '%s' into element with selector: %s\n", text.c_str(), selector.c_str());
    return MantraValue::boolean(true);
}

MantraValue builtinBrowserGetText(const std::vector<MantraValue>& args) {
    ensureArgs(args, 1, "browserGetText");
    std::string selector = requireString(args[0], "browserGetText");
    // Simulated web automation - extracts text from an element
    std::string result = "Text from " + selector;
    return MantraValue::string(result);
}

MantraValue builtinBrowserScreenshot(const std::vector<MantraValue>& args) {
    ensureArgs(args, 1, "browserScreenshot");
    std::string path = requireString(args[0], "browserScreenshot");
    // Simulated web automation - takes a screenshot
    std::printf("Screenshot saved to: %s\n", path.c_str());
    return MantraValue::boolean(true);
}

MantraValue builtinBrowserClose(const std::vector<MantraValue>& args) {
    ensureArgs(args, 0, "browserClose");
    // Simulated web automation - closes the browser
    std::printf("Browser closed\n");
    return MantraValue::boolean(true);
}

} // namespace mantra::stdlib
