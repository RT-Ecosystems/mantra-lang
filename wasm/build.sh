#!/bin/bash
# MANTRA WASM Build Script
# Compiles MANTRA to WebAssembly

echo "🚀 Building MANTRA WASM..."

# Check for emscripten
if ! command -v emcc &> /dev/null; then
    echo "❌ emscripten not found!"
    echo "Install: https://emscripten.org/docs/getting_started/downloads.html"
    exit 1
fi

# Create build directory
mkdir -p build/wasm
cd build/wasm

# Compile MANTRA core to WASM
echo "📦 Compiling core..."
emcc     -O3     -s WASM=1     -s EXPORTED_FUNCTIONS='["_mantra_init", "_mantra_alloc", "_mantra_free", "_mantra_run", "_wasm_callback"]'     -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]'     -s ALLOW_MEMORY_GROWTH=1     -s MODULARIZE=1     -s EXPORT_NAME='"MantraModule"'     -s ENVIRONMENT='web'     -o mantra.js     ../../wasm/mantra_wasm.cpp     ../../core/lexer.cpp     ../../core/parser.cpp     ../../core/interpreter.cpp     ../../core/keyword_normalizer.cpp     ../../core/typesystem.cpp     ../../core/module_system.cpp     ../../core/async_system.cpp     ../../core/ind_package_manager.cpp     ../../vm/vm.cpp     ../../stdlib/io.cpp     ../../stdlib/math.cpp     ../../stdlib/string.cpp     ../../stdlib/json.cpp     ../../stdlib/http.cpp     ../../stdlib/web_framework.cpp     ../../stdlib/testing.cpp

# Copy files to web directory
echo "📁 Copying files..."
cp mantra.js ../../wasm/
cp mantra.wasm ../../wasm/

echo "✅ Build complete!"
echo ""
echo "To test:"
echo "  cd ../../wasm"
echo "  python3 -m http.server 8080"
echo "  Open: http://localhost:8080/ide.html"
