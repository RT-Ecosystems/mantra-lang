#include "semantic_analyzer.h"

#include "../error/error.h"

#include <limits>
#include <utility>

namespace mantra {
SemanticAnalyzer::SemanticAnalyzer()
    : loop_depth(0), function_depth(0), had_error(false) {
    seedBuiltins();
}

bool SemanticAnalyzer::analyze(const ProgramNode& program) {
    had_error = false;
    scopes.clear();
    functions.clear();
    seedBuiltins();
    pushScope();
    analyzeStatements(program.statements);
    popScope();
    return !had_error;
}

void SemanticAnalyzer::pushScope() {
    scopes.emplace_back();
}

void SemanticAnalyzer::popScope() {
    if (!scopes.empty()) {
        scopes.pop_back();
    }
}

bool SemanticAnalyzer::declare(const std::string& name, SymbolKind kind, const MantraNode& node, const std::string& message) {
    if (scopes.empty()) {
        pushScope();
    }
    auto& scope = scopes.back();
    if (scope.find(name) != scope.end()) {
        report(node, message);
        return false;
    }
    scope.emplace(name, kind);
    return true;
}

bool SemanticAnalyzer::hasVariable(const std::string& name) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return found->second == SymbolKind::Variable;
        }
    }
    return false;
}

bool SemanticAnalyzer::isDefined(const std::string& name) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        const auto& scope = *it;
        auto found = scope.find(name);
        if (found != scope.end()) {
            return true;
        }
    }
    return functions.find(name) != functions.end() ||
           builtins.find(name) != builtins.end() ||
           constants.find(name) != constants.end();
}

const SemanticAnalyzer::FunctionSignature* SemanticAnalyzer::lookupFunction(const std::string& name) const {
    auto func = functions.find(name);
    if (func != functions.end()) {
        return &func->second;
    }
    auto builtin = builtins.find(name);
    if (builtin != builtins.end()) {
        return &builtin->second;
    }
    return nullptr;
}

void SemanticAnalyzer::registerFunction(const std::string& name, size_t min_arity, size_t max_arity) {
    functions[name] = FunctionSignature{min_arity, max_arity};
}

void SemanticAnalyzer::registerBuiltin(const std::string& name, size_t min_arity, size_t max_arity) {
    builtins[name] = FunctionSignature{min_arity, max_arity};
}

void SemanticAnalyzer::registerConstant(const std::string& name) {
    constants.insert(name);
}

void SemanticAnalyzer::analyzeStatements(const std::vector<std::unique_ptr<MantraNode>>& statements) {
    for (const auto& stmt : statements) {
        if (!stmt || stmt->type != NodeType::FUNC_DEF) {
            continue;
        }
        const auto& func = static_cast<const FuncDefNode&>(*stmt);
        declare(func.name, SymbolKind::Function, func, "Duplicate function '" + func.name + "'");
        registerFunction(func.name, func.parameters.size(), func.parameters.size());
    }

    for (const auto& stmt : statements) {
        if (stmt) {
            analyzeNode(*stmt);
        }
    }
}

void SemanticAnalyzer::analyzeNode(const MantraNode& node) {
    switch (node.type) {
        case NodeType::PROGRAM:
            analyzeStatements(static_cast<const ProgramNode&>(node).statements);
            break;
        case NodeType::BLOCK_STMT:
            analyzeBlock(static_cast<const BlockStmtNode&>(node));
            break;
        case NodeType::PRINT_STMT:
            if (static_cast<const PrintStmtNode&>(node).expression) {
                analyzeExpression(*static_cast<const PrintStmtNode&>(node).expression);
            }
            break;
        case NodeType::ASSIGN_STMT: {
            const auto& assign = static_cast<const AssignStmtNode&>(node);
            if (assign.is_declaration) {
                declare(assign.name, SymbolKind::Variable, node, "Duplicate variable '" + assign.name + "'");
            } else if (!isDefined(assign.name)) {
                declare(assign.name, SymbolKind::Variable, node, "");
            }
            if (assign.value) {
                analyzeExpression(*assign.value);
            }
            break;
        }
        case NodeType::IF_STMT: {
            const auto& if_node = static_cast<const IfStmtNode&>(node);
            if (if_node.condition) {
                analyzeExpression(*if_node.condition);
            }
            if (if_node.then_branch) {
                analyzeBlock(*if_node.then_branch);
            }
            if (if_node.else_branch) {
                analyzeBlock(*if_node.else_branch);
            }
            break;
        }
        case NodeType::WHILE_STMT: {
            const auto& while_node = static_cast<const WhileStmtNode&>(node);
            if (while_node.condition) {
                analyzeExpression(*while_node.condition);
            }
            ++loop_depth;
            if (while_node.body) {
                analyzeBlock(*while_node.body);
            }
            --loop_depth;
            break;
        }
        case NodeType::FOR_STMT: {
            const auto& for_node = static_cast<const ForStmtNode&>(node);
            if (for_node.start) {
                analyzeExpression(*for_node.start);
            }
            if (for_node.end) {
                analyzeExpression(*for_node.end);
            }
            if (for_node.step) {
                analyzeExpression(*for_node.step);
            }
            ++loop_depth;
            pushScope();
            declare(for_node.variable, SymbolKind::Variable, node, "Duplicate loop variable '" + for_node.variable + "'");
            if (for_node.body) {
                analyzeBlock(*for_node.body);
            }
            popScope();
            --loop_depth;
            break;
        }
        case NodeType::FUNC_DEF: {
            const auto& func = static_cast<const FuncDefNode&>(node);
            ++function_depth;
            pushScope();
            for (const auto& param : func.parameters) {
                declare(param, SymbolKind::Variable, node, "Duplicate parameter '" + param + "'");
            }
            if (func.body) {
                analyzeBlock(*func.body);
            }
            popScope();
            --function_depth;
            break;
        }
        case NodeType::RETURN_STMT: {
            const auto& ret = static_cast<const ReturnStmtNode&>(node);
            if (function_depth == 0) {
                report(node, "return outside function");
            }
            if (ret.value) {
                analyzeExpression(*ret.value);
            }
            break;
        }
        case NodeType::BREAK_STMT:
            if (loop_depth == 0) {
                report(node, "break outside loop");
            }
            break;
        case NodeType::CONTINUE_STMT:
            if (loop_depth == 0) {
                report(node, "continue outside loop");
            }
            break;
        case NodeType::EXPR_STMT: {
            const auto& expr = static_cast<const ExprStmtNode&>(node);
            if (expr.expression) {
                analyzeExpression(*expr.expression);
            }
            break;
        }
        default:
            analyzeExpression(node);
            break;
    }
}

void SemanticAnalyzer::analyzeBlock(const BlockStmtNode& block) {
    pushScope();
    analyzeStatements(block.statements);
    popScope();
}

void SemanticAnalyzer::analyzeExpression(const MantraNode& node) {
    switch (node.type) {
        case NodeType::IDENTIFIER: {
            const auto& ident = static_cast<const IdentifierNode&>(node);
            if (!isDefined(ident.name)) {
                report(node, "Unknown variable '" + ident.name + "'");
            }
            break;
        }
        case NodeType::BINARY_EXPR: {
            const auto& expr = static_cast<const BinaryExprNode&>(node);
            if (expr.left) {
                analyzeExpression(*expr.left);
            }
            if (expr.right) {
                analyzeExpression(*expr.right);
            }
            break;
        }
        case NodeType::UNARY_EXPR: {
            const auto& expr = static_cast<const UnaryExprNode&>(node);
            if (expr.operand) {
                analyzeExpression(*expr.operand);
            }
            break;
        }
        case NodeType::CALL_EXPR: {
            const auto& call = static_cast<const CallExprNode&>(node);
            if (call.callee) {
                if (call.callee->type == NodeType::IDENTIFIER) {
                    const auto& ident = static_cast<const IdentifierNode&>(*call.callee);
                    const auto* signature = lookupFunction(ident.name);
                    if (signature && (call.arguments.size() < signature->min_arity || call.arguments.size() > signature->max_arity)) {
                        if (signature->min_arity == signature->max_arity) {
                            report(node, "Function '" + ident.name + "' expects " + std::to_string(signature->min_arity) +
                                           " arguments but got " + std::to_string(call.arguments.size()));
                        } else {
                            report(node, "Function '" + ident.name + "' expects between " +
                                           std::to_string(signature->min_arity) + " and " +
                                           std::to_string(signature->max_arity) + " arguments but got " +
                                           std::to_string(call.arguments.size()));
                        }
                    } else if (!signature && !isDefined(ident.name)) {
                        report(node, "Undefined function '" + ident.name + "'");
                    }
                } else {
                    analyzeExpression(*call.callee);
                }
            }
            for (const auto& arg : call.arguments) {
                if (arg) {
                    analyzeExpression(*arg);
                }
            }
            break;
        }
        case NodeType::ARRAY_LIT: {
            const auto& array = static_cast<const ArrayLitNode&>(node);
            for (const auto& element : array.elements) {
                if (element) {
                    analyzeExpression(*element);
                }
            }
            break;
        }
        case NodeType::INDEX_EXPR: {
            const auto& index = static_cast<const IndexExprNode&>(node);
            if (index.collection) {
                analyzeExpression(*index.collection);
            }
            if (index.index) {
                analyzeExpression(*index.index);
            }
            break;
        }
        case NodeType::NUMBER_LIT:
        case NodeType::STRING_LIT:
        case NodeType::BOOL_LIT:
        case NodeType::NULL_LIT:
            break;
        default:
            analyzeNode(node);
            break;
    }
}

void SemanticAnalyzer::report(const MantraNode& node, const std::string& message) {
    had_error = true;
    ErrorHandler::printErrorWithContext(ErrorType::SEMANTIC_ERROR, message, "", node.line, node.column);
}

void SemanticAnalyzer::seedBuiltins() {
    builtins.clear();
    constants.clear();
    const size_t variadic = std::numeric_limits<size_t>::max();
    registerBuiltin("print", 0, variadic);
    registerBuiltin("println", 0, variadic);
    registerBuiltin("dikhao", 0, variadic);
    registerBuiltin("kaado", 0, variadic);
    registerBuiltin("dakho", 0, variadic);
    registerBuiltin("dekhao", 0, variadic);
    registerBuiltin("batavo", 0, variadic);
    registerBuiltin("dakhav", 0, variadic);
    registerBuiltin("chupinchu", 0, variadic);
    registerBuiltin("toro", 0, variadic);
    registerBuiltin("kaaniku", 0, variadic);
    registerBuiltin("jadi", 0, variadic);
    registerBuiltin("nohole", 0, variadic);
    registerBuiltin("dekhaow", 0, variadic);
    registerBuiltin("dakhoi", 0, variadic);
    registerBuiltin("dekhau", 0, variadic);
    registerBuiltin("darshaya", 0, variadic);
    registerBuiltin("waatav", 0, variadic);
    registerBuiltin("dikhay", 0, variadic);
    registerBuiltin("wekho", 0, variadic);
    registerBuiltin("nungsi", 0, variadic);
    registerBuiltin("nangi", 0, variadic);
    registerBuiltin("dado", 0, variadic);

    registerBuiltin("input", 0, variadic);
    registerBuiltin("lo", 0, variadic);
    registerBuiltin("loo", 0, variadic);
    registerBuiltin("length", 1, 1);
    registerBuiltin("lambai", 1, 1);
    registerBuiltin("concat", 2, 2);
    registerBuiltin("jodo_shabd", 2, 2);
    registerBuiltin("toNumber", 1, 1);
    registerBuiltin("sankhya", 1, 1);
    registerBuiltin("toString", 1, 1);
    registerBuiltin("shabd", 1, 1);
    registerBuiltin("type", 1, 1);
    registerBuiltin("exit", 0, variadic);
    registerBuiltin("roke", 0, variadic);
    registerBuiltin("clear", 0, variadic);
    registerBuiltin("saaf", 0, variadic);
    registerBuiltin("upper", 1, 1);
    registerBuiltin("lower", 1, 1);
    registerBuiltin("trim", 1, 1);
    registerBuiltin("split", 2, 2);
    registerBuiltin("replace", 3, 3);
    registerBuiltin("slice", 2, 3);
    registerBuiltin("contains", 2, 2);
    registerBuiltin("startsWith", 2, 2);
    registerBuiltin("endsWith", 2, 2);
    registerBuiltin("repeat", 2, 2);

    registerBuiltin("jodo", 1, variadic);
    registerBuiltin("koodal", 1, variadic);
    registerBuiltin("joran", 1, variadic);
    registerBuiltin("jogao", 1, variadic);
    registerBuiltin("ghato", 2, variadic);
    registerBuiltin("kayal", 2, variadic);
    registerBuiltin("tafawut", 2, variadic);
    registerBuiltin("guno", 1, variadic);
    registerBuiltin("guna", 1, variadic);
    registerBuiltin("perukku", 1, variadic);
    registerBuiltin("bhaago", 2, variadic);
    registerBuiltin("vagat", 2, variadic);
    registerBuiltin("vani", 2, variadic);
    registerBuiltin("baki", 2, variadic);
    registerBuiltin("meechi", 2, variadic);
    registerBuiltin("seshamu", 2, variadic);
    registerBuiltin("abs", 1, 1);
    registerBuiltin("mutlak", 1, 1);
    registerBuiltin("nirnayaka", 1, 1);
    registerBuiltin("sqrt", 1, 1);
    registerBuiltin("varga", 1, 1);
    registerBuiltin("karan", 1, 1);
    registerBuiltin("vargamul", 1, 1);
    registerBuiltin("mul", 1, 1);
    registerBuiltin("pow", 2, 2);
    registerBuiltin("shakti", 2, 2);
    registerBuiltin("mattu", 2, 2);
    registerBuiltin("potens", 2, 2);
    registerBuiltin("round", 1, 1);
    registerBuiltin("gol", 1, 1);
    registerBuiltin("suzhal", 1, 1);
    registerBuiltin("ceil", 1, 1);
    registerBuiltin("upar", 1, 1);
    registerBuiltin("melu", 1, 1);
    registerBuiltin("floor", 1, 1);
    registerBuiltin("neeche", 1, 1);
    registerBuiltin("kizh", 1, 1);
    registerBuiltin("madhyaman", 1, variadic);
    registerBuiltin("mean", 1, variadic);
    registerBuiltin("average", 1, variadic);
    registerBuiltin("madhyika", 1, variadic);
    registerBuiltin("median", 1, variadic);
    registerBuiltin("bahulak", 1, variadic);
    registerBuiltin("mode", 1, variadic);
    registerBuiltin("vistar", 1, variadic);
    registerBuiltin("range", 1, variadic);
    registerBuiltin("pramaan", 1, variadic);
    registerBuiltin("std_deviation", 1, variadic);
    registerBuiltin("yogfal", 1, variadic);
    registerBuiltin("sum", 1, variadic);
    registerBuiltin("adhiktam", 1, variadic);
    registerBuiltin("maximum", 1, variadic);
    registerBuiltin("max", 1, variadic);
    registerBuiltin("nyuntam", 1, variadic);
    registerBuiltin("minimum", 1, variadic);
    registerBuiltin("min", 1, variadic);
    registerBuiltin("random", 0, variadic);
    registerBuiltin("pi", 0, variadic);
    registerBuiltin("e", 0, variadic);
    registerConstant("PI");
    registerConstant("E");
    registerBuiltin("infinity", 0, variadic);
    registerBuiltin("nan", 0, variadic);
    registerBuiltin("is_infinity", 1, 1);
    registerBuiltin("is_nan", 1, 1);

    // AI/NeuroBrain functions
    registerBuiltin("hash", 1, 1);
    registerBuiltin("similar", 2, 2);
    registerBuiltin("sikho", 1, 1);
    registerBuiltin("puchho", 1, 1);
    registerBuiltin("beej", 0, 1);
    registerBuiltin("gyan", 0, 1);
    registerBuiltin("vidyarthi", 0, 0);
    registerBuiltin("saveModel", 1, 1);
    registerBuiltin("loadModel", 1, 1);

    // File functions
    registerBuiltin("readFile", 1, 1);
    registerBuiltin("writeFile", 2, 2);
    registerBuiltin("fileExists", 1, 1);

    // DateTime functions
    registerBuiltin("now", 0, 0);
    registerBuiltin("today", 0, 0);

    // JSON functions
    registerBuiltin("jsonParse", 1, 1);
    registerBuiltin("jsonStringify", 1, 1);

    // CSV functions
    registerBuiltin("csvRead", 1, 1);
    registerBuiltin("csvWrite", 2, 2);

    // HTTP functions
    registerBuiltin("httpGet", 1, 1);
    registerBuiltin("httpPost", 2, 2);

    // Random functions
    registerBuiltin("randomInt", 2, 2);
    registerBuiltin("choice", 1, 1);

    // OS functions
    registerBuiltin("getEnv", 1, 1);
    registerBuiltin("system", 1, 1);

    // Base64 functions
    registerBuiltin("base64Encode", 1, 1);
    registerBuiltin("base64Decode", 1, 1);

    // Regex functions
    registerBuiltin("regexMatch", 2, 2);
    registerBuiltin("regexReplace", 3, 3);

    // UUID functions
    registerBuiltin("uuid", 0, 0);

    // TempFile functions
    registerBuiltin("tempFile", 0, 0);

    // SQLite functions
    registerBuiltin("sqlOpen", 1, 1);
    registerBuiltin("sqlExec", 1, 1);
    registerBuiltin("sqlClose", 0, 0);

    // Zip functions
    registerBuiltin("zipCreate", 2, 2);
    registerBuiltin("zipExtract", 2, 2);
    registerBuiltin("zipList", 1, 1);

    // Compress functions
    registerBuiltin("gzip", 1, 1);
    registerBuiltin("gunzip", 1, 1);
    registerBuiltin("bzip2", 1, 1);
    registerBuiltin("bunzip2", 1, 1);

    // XML functions
    registerBuiltin("xmlParse", 1, 1);
    registerBuiltin("xmlGetNode", 2, 2);
    registerBuiltin("xmlSetNode", 3, 3);

    // YAML functions
    registerBuiltin("yamlLoad", 1, 1);
    registerBuiltin("yamlDump", 1, 1);

    // INI functions
    registerBuiltin("iniRead", 1, 1);
    registerBuiltin("iniWrite", 2, 2);

    // Process functions
    registerBuiltin("processRun", 1, 2);
    registerBuiltin("processOutput", 1, 1);
    registerBuiltin("processWait", 1, 1);
    registerBuiltin("processKill", 1, 1);

    // Socket functions
    registerBuiltin("tcpConnect", 2, 2);
    registerBuiltin("tcpSend", 2, 2);
    registerBuiltin("tcpRecv", 1, 1);
    registerBuiltin("tcpClose", 1, 1);
    registerBuiltin("tcpListen", 2, 2);

    // Crypto functions
    registerBuiltin("aesEncrypt", 2, 2);
    registerBuiltin("aesDecrypt", 2, 2);
    registerBuiltin("sha256", 1, 1);
    registerBuiltin("md5", 1, 1);

    // Image functions
    registerBuiltin("imgLoad", 1, 1);
    registerBuiltin("imgResize", 3, 3);
    registerBuiltin("imgSave", 2, 2);
    registerBuiltin("imgInfo", 1, 1);

    // Audio functions
    registerBuiltin("audioLoad", 1, 1);
    registerBuiltin("audioPlay", 1, 1);
    registerBuiltin("audioInfo", 1, 1);

    // Hash functions
    registerBuiltin("sha1", 1, 1);
    registerBuiltin("sha512", 1, 1);
    registerBuiltin("crc32", 1, 1);

    // GUI functions
    registerBuiltin("guiWindow", 1, 1);
    registerBuiltin("guiButton", 2, 2);
    registerBuiltin("guiLabel", 1, 1);
    registerBuiltin("guiRun", 0, 0);

    // Browser functions
    registerBuiltin("browserOpen", 1, 1);
    registerBuiltin("browserClick", 1, 1);
    registerBuiltin("browserType", 2, 2);
    registerBuiltin("browserGetText", 1, 1);
    registerBuiltin("browserScreenshot", 1, 1);
    registerBuiltin("browserClose", 0, 0);

    // App functions
    registerBuiltin("appWindow", 3, 3);
    registerBuiltin("appButton", 5, 5);
    registerBuiltin("appLabel", 3, 3);
    registerBuiltin("appInput", 4, 4);
    registerBuiltin("appRun", 0, 0);

    // Chart functions
    registerBuiltin("chartBar", 2, 2);
    registerBuiltin("chartLine", 2, 2);
    registerBuiltin("chartPie", 1, 1);
    registerBuiltin("chartSave", 1, 1);

    // PDF functions
    registerBuiltin("pdfCreate", 1, 1);
    registerBuiltin("pdfText", 2, 2);
    registerBuiltin("pdfImage", 1, 1);
    registerBuiltin("pdfSave", 1, 1);

    // Email functions
    registerBuiltin("emailSend", 3, 3);
    registerBuiltin("emailSetServer", 3, 3);

    // Logging functions
    registerBuiltin("logInfo", 1, 1);
    registerBuiltin("logError", 1, 1);
    registerBuiltin("logWarn", 1, 1);
    registerBuiltin("logSetFile", 1, 1);

    // Serial functions
    registerBuiltin("serialOpen", 2, 2);
    registerBuiltin("serialRead", 1, 1);
    registerBuiltin("serialWrite", 2, 2);
    registerBuiltin("serialClose", 1, 1);

    // Config functions
    registerBuiltin("configLoad", 1, 1);
    registerBuiltin("configGet", 2, 2);
    registerBuiltin("configSet", 3, 3);
    registerBuiltin("configSave", 0, 0);

    // Hacking module
    registerBuiltin("portScan", 3, 3);
    registerBuiltin("networkSniff", 1, 1);
    registerBuiltin("pingHost", 1, 1);
    registerBuiltin("whoisLookup", 1, 1);
    registerBuiltin("dnsLookup", 1, 1);
    registerBuiltin("hashCrack", 2, 2);

    // Compiler module
    registerBuiltin("lexerTokenize", 1, 1);
    registerBuiltin("parserParse", 1, 1);
    registerBuiltin("codeGen", 1, 1);
    registerBuiltin("optimizeIR", 1, 1);
    registerBuiltin("emitBinary", 2, 2);
}

} // namespace mantra
