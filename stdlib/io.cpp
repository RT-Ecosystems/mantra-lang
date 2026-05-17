#include "io.h"
#include "../error/error.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace mantra::stdlib {

static void ensureArgCount(const std::vector<MantraValue>& args,
                           size_t expected,
                           const std::string& name) {
    if (args.size() != expected) {
        ErrorHandler::throwError(ErrorType::RUNTIME_ERROR,
                                 name + " को " + std::to_string(expected) + " आर्ग्युमेंट चाहिए",
                                 0, 0, 1);
    }
}

static std::string requireStringArg(const std::vector<MantraValue>& args,
                                    size_t index,
                                    const std::string& name) {
    if (index >= args.size()) {
        ErrorHandler::throwError(ErrorType::RUNTIME_ERROR,
                                 name + " में पर्याप्त आर्ग्युमेंट नहीं हैं",
                                 0, 0, 1);
    }
    if (args[index].type != ValueType::String) {
        ErrorHandler::throwError(ErrorType::TYPE_ERROR,
                                 name + " के लिए स्ट्रिंग अपेक्षित है",
                                 0, 0, 1);
    }
    return args[index].string_value;
}

static void ensureNotEmpty(const std::string& value, const std::string& name) {
    if (value.empty()) {
        ErrorHandler::throwError(ErrorType::RUNTIME_ERROR,
                                 name + " के लिए खाली स्ट्रिंग नहीं चलेगी",
                                 0, 0, 1);
    }
}

MantraValue builtinPrint(const std::vector<MantraValue>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        std::cout << args[i].toString();
        if (i + 1 < args.size()) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
    std::cout.flush();
    return MantraValue::nullValue();
}

MantraValue builtinInput(const std::vector<MantraValue>& args) {
    if (!args.empty()) {
        for (const auto& arg : args) {
            std::cout << arg.toString();
        }
    }

    std::string line;
    std::getline(std::cin, line);
    return MantraValue::string(line);
}

MantraValue builtinReadFile(const std::vector<MantraValue>& args) {
    ensureArgCount(args, 1, "read_file/file_padho");

    std::string path = requireStringArg(args, 0, "read_file/file_padho");
    ensureNotEmpty(path, "read_file/file_padho");
    std::ifstream file(path);
    if (!file.is_open()) {
        ErrorHandler::throwError(ErrorType::FILE_NOT_FOUND,
                                 "फाइल नहीं खुली: " + path,
                                 0, 0, 1);
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    return MantraValue::string(buffer.str());
}

MantraValue builtinWriteFile(const std::vector<MantraValue>& args) {
    ensureArgCount(args, 2, "write_file/file_likho");

    std::string path = requireStringArg(args, 0, "write_file/file_likho");
    ensureNotEmpty(path, "write_file/file_likho");
    std::ofstream file(path);
    if (!file.is_open()) {
        ErrorHandler::throwError(ErrorType::IO_ERROR,
                                 "फाइल लिखने के लिए नहीं खुली: " + path,
                                 0, 0, 1);
    }

    file << args[1].toString();
    return MantraValue::nullValue();
}

} // namespace mantra::stdlib
