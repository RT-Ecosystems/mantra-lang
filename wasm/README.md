# MANTRA WebAssembly (WASM) Target

Run MANTRA language directly in browsers!

## 🌟 Features

- **Browser Runtime** — No installation needed
- **Web IDE** — Code editor with syntax highlighting
- **22 Languages** — All MANTRA languages supported
- **DOM Access** — Manipulate HTML from MANTRA
- **HTTP Requests** — Fetch API integration
- **LocalStorage** — Persistent storage
- **Async/Await** — Non-blocking operations

## 🚀 Quick Start

### 1. Build WASM

```bash
cd wasm
./build.sh
```

### 2. Start Web Server

```bash
cd wasm
python3 -m http.server 8080
```

### 3. Open IDE

Open browser: http://localhost:8080/ide.html

## 📁 Files

| File | Description |
|------|-------------|
| `mantra_wasm.js` | JavaScript bridge |
| `mantra_wasm.cpp` | C++ WASM implementation |
| `mantra.wasm` | Compiled WASM binary |
| `ide.html` | Web IDE |
| `build.sh` | Build script |

## 🎯 Examples

### Browser Alert
```mantra
# This runs in browser!
dikhao "Hello from Browser!"

# Access DOM
rakho element = dom_getElementById("output")
dom_setInnerHTML(element, "<h1>MANTRA in Browser!</h1>")
```

### Fetch Data
```mantra
# HTTP request from browser
rakho data = http_get("https://api.example.com/data")
dikhao data

# POST request
rakho body = json_stringify({"name": "MANTRA"})
rakho response = http_post("https://api.example.com/submit", body)
```

### LocalStorage
```mantra
# Save data
localStorage_setItem("username", "Ashutosh")

# Read data
rakho username = localStorage_getItem("username")
dikhao username
```

### Async Operations
```mantra
async kaam fetchUserData() {
    rakho data = await http_get("https://api.example.com/user")
    wapas json_parse(data)
}

async kaam displayUser() {
    rakho user = await fetchUserData()
    dom_setInnerHTML("user-info", user.name)
}

async displayUser()
```

## 🏗️ Architecture

```
Browser
├── JavaScript (mantra_wasm.js)
│   ├── DOM API
│   ├── Fetch API
│   ├── LocalStorage
│   └── Event Handling
│
└── WebAssembly (mantra.wasm)
    ├── MANTRA Interpreter
    ├── Memory Management
    └── JS Bridge
```

## 🤝 Contributing

See main repo: https://github.com/RT-Ecosystems/mantra-lang
