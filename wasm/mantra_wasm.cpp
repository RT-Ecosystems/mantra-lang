#include <emscripten.h>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>

// External JS functions
extern "C" {
    void js_console_log(const char* ptr, int len);
    void js_console_error(const char* ptr, int len);
    int js_dom_getElementById(const char* ptr, int len);
    void js_dom_setInnerHTML(int objId, const char* ptr, int len);
    void js_dom_setValue(int objId, const char* ptr, int len);
    int js_dom_getValue(int objId);
    void js_dom_addEventListener(int objId, const char* eventPtr, int eventLen, int callbackId);
    int js_http_get(const char* urlPtr, int urlLen);
    int js_http_post(const char* urlPtr, int urlLen, const char* bodyPtr, int bodyLen);
    void js_setTimeout(int callbackId, int delay);
    double js_math_random();
    double js_math_floor(double x);
    double js_math_ceil(double x);
    double js_math_sqrt(double x);
    double js_math_pow(double x, double y);
    double js_date_now();
    int js_localStorage_getItem(const char* keyPtr, int keyLen);
    void js_localStorage_setItem(const char* keyPtr, int keyLen, const char* valuePtr, int valueLen);
    int js_json_parse(const char* ptr, int len);
    int js_json_stringify(const char* ptr, int len);
}

// Memory management
static char* wasm_memory = nullptr;
static size_t wasm_memory_size = 0;
static size_t wasm_memory_used = 0;

extern "C" {

    // Initialize MANTRA runtime
    EMSCRIPTEN_KEEPALIVE
    void mantra_init() {
        wasm_memory = new char[1024 * 1024]; // 1MB
        wasm_memory_size = 1024 * 1024;
        wasm_memory_used = 0;
    }

    // Allocate memory
    EMSCRIPTEN_KEEPALIVE
    char* mantra_alloc(int size) {
        if (wasm_memory_used + size > wasm_memory_size) {
            // Grow memory
            char* new_memory = new char[wasm_memory_size * 2];
            std::memcpy(new_memory, wasm_memory, wasm_memory_used);
            delete[] wasm_memory;
            wasm_memory = new_memory;
            wasm_memory_size *= 2;
        }

        char* ptr = wasm_memory + wasm_memory_used;
        wasm_memory_used += size;
        return ptr;
    }

    // Free memory (simplified)
    EMSCRIPTEN_KEEPALIVE
    void mantra_free(char* ptr) {
        // In real implementation, use proper allocator
    }

    // Run MANTRA code
    EMSCRIPTEN_KEEPALIVE
    const char* mantra_run(const char* code) {
        // Parse and execute MANTRA code
        std::string result = executeMantraCode(code);

        // Store result in memory
        char* resultPtr = mantra_alloc(result.length() + 1);
        std::strcpy(resultPtr, result.c_str());
        return resultPtr;
    }

    // Callback from JS
    EMSCRIPTEN_KEEPALIVE
    void wasm_callback(int callbackId) {
        // Execute registered callback
        // In real implementation, maintain callback registry
    }

    // Print to console
    void mantra_print(const char* str) {
        js_console_log(str, std::strlen(str));
    }

    // Print error
    void mantra_error(const char* str) {
        js_console_error(str, std::strlen(str));
    }

    // HTTP GET
    std::string mantra_http_get(const std::string& url) {
        int resultPtr = js_http_get(url.c_str(), url.length());
        // Read result from memory
        return "HTTP GET result";
    }

    // HTTP POST
    std::string mantra_http_post(const std::string& url, const std::string& body) {
        int resultPtr = js_http_post(url.c_str(), url.length(), body.c_str(), body.length());
        return "HTTP POST result";
    }

    // Sleep (async)
    void mantra_sleep(int ms) {
        js_setTimeout(0, ms); // Simplified
    }

    // Math functions
    double mantra_random() { return js_math_random(); }
    double mantra_floor(double x) { return js_math_floor(x); }
    double mantra_ceil(double x) { return js_math_ceil(x); }
    double mantra_sqrt(double x) { return js_math_sqrt(x); }
    double mantra_pow(double x, double y) { return js_math_pow(x, y); }

    // Date
    double mantra_now() { return js_date_now(); }

    // LocalStorage
    std::string mantra_localStorage_getItem(const std::string& key) {
        int resultPtr = js_localStorage_getItem(key.c_str(), key.length());
        return "localStorage value";
    }

    void mantra_localStorage_setItem(const std::string& key, const std::string& value) {
        js_localStorage_setItem(key.c_str(), key.length(), value.c_str(), value.length());
    }

    // JSON
    std::string mantra_json_parse(const std::string& str) {
        int resultPtr = js_json_parse(str.c_str(), str.length());
        return "parsed JSON";
    }

    std::string mantra_json_stringify(const std::string& obj) {
        int resultPtr = js_json_stringify(obj.c_str(), obj.length());
        return "stringified JSON";
    }
}

// Execute MANTRA code (simplified)
std::string executeMantraCode(const std::string& code) {
    // In real implementation:
    // 1. Lexical analysis
    // 2. Parsing
    // 3. Semantic analysis
    // 4. Bytecode generation
    // 5. Execution

    // For now, simple echo
    return "Executed: " + code;
}
