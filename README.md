# MANTRA (मंत्र) Programming Language

<p align="center">
  <img src="https://img.shields.io/badge/version-1.0.0-blue.svg" alt="Version">
  <img src="https://img.shields.io/badge/license-MIT-green.svg" alt="License">
  <img src="https://img.shields.io/badge/C++-17-orange.svg" alt="C++17">
  <img src="https://img.shields.io/badge/platforms-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey.svg" alt="Platforms">
</p>

<p align="center">
  <b>A Programming Language for Every Indian</b><br>
  <i>22 Languages | Dual Paradigms | Built for India</i>
</p>

---

## 🌟 What is MANTRA?

**MANTRA** is a revolutionary programming language designed specifically for **Indian developers** — from beginners to professionals. It breaks two major barriers that prevent Indians from learning programming:

1. **🗣️ Language Barrier** — Code in your mother tongue! 22 Indian languages + English + Roman versions
2. **🧠 Syntax Barrier** — Two coding paradigms: Simple (bracket-free) for beginners, Professional (Python-like) for experts

---

## 🎯 Key Features

### 🇮🇳 22 Indian Languages + English

| Language | Script | Example (Print) | Example (If) |
|----------|--------|-----------------|--------------|
| **Hinglish** | Hindi + English | `dikhao` | `agar` |
| **Tanglish** | Tamil + English | `kaado` | `endral` |
| **Punglish** | Punjabi + English | `dakho` | `jekr` |
| **Benglish** | Bengali + English | `dekhao` | `jodi` |
| **Gujlish** | Gujarati + English | `batavo` | `jor` |
| **Marathlish** | Marathi + English | `dakhav` | `jar` |
| **Telglish** | Telugu + English | `chupinchu` | `ante` |
| **Kanglish** | Kannada + English | `toro` | `agre` |
| **Manglish** | Malayalam + English | `kaaniku` | `engil` |
| **+ 13 more** | ... | ... | ... |
| **English** | English | `print` | `if` |

**Total: 50+ language variants** (22 native + 22 Roman + English)

### 🔄 Dual Coding Paradigms

#### Beginner Mode (No brackets, simple syntax)
```mantra
# Perfect for first-time programmers
dikhao "Hello Duniya"           # print without brackets
rakho naam = "Ashutosh"         # simple assignment
agar naam == "Ashutosh"         # if without brackets
    dikhao "Namaste!"
warna
    dikhao "Koi aur"

baarbaar count < 10             # while loop
    dikhao count
    count = count + 1
```

#### Professional Mode (Python-like, with brackets)
```mantra
# For experienced developers
print("Hello World")
let name = "Ashutosh"
if (name == "Ashutosh") {
    print("Hello!")
} else {
    print("Someone else")
}

while (count < 10) {
    print(count)
    count = count + 1
}
```

### 🧠 Smart Features

| Feature | Description |
|---------|-------------|
| **🔤 Typo Tolerance** | `dikho` → suggests `dikhao`, `afgr` → suggests `agar` |
| **🌐 Auto Language Detection** | Automatically detects which language you're using |
| **📦 Package Manager (IND)** | `ind_install "networking"` — like npm/pip |
| **🔄 Async/Await** | `async`, `await`, `sleep` for non-blocking code |
| **🌐 Web Framework** | Express.js-style routing built-in |
| **🧪 Testing Suite** | `assert`, `test`, `expect` built-in |
| **📄 Web Templates (.mtr)** | HTML/CSS/JS generation in 22 languages |

---

## 🚀 Quick Start

### Installation

```bash
# Clone the repository
git clone https://github.com/RT-Ecosystems/mantra-lang.git
cd mantra-lang

# Build
mkdir build && cd build
cmake ..
make

# Install (optional)
sudo make install
```

### Hello World in 5 Languages

```bash
# Hinglish (Hindi)
./mantra -c 'dikhao "Namaste Duniya!"'

# Tanglish (Tamil)
./mantra -c 'kaado "Vanakkam Ulagam!"'

# Punglish (Punjabi)
./mantra -c 'dakho "Sat Sri Akal!"'

# English
./mantra -c 'print "Hello World!"'
```

### Run a Program

```bash
# Create a file: hello.mtr
dikhao "Hello from MANTRA!"
rakho naam = "Ashutosh"
dikhao "Namaste, " + naam

# Run it
./mantra hello.mtr
```

---

## 📚 Examples

### Variables and Data Types
```mantra
# Numbers
rakho age = 25
rakho pi = 3.14159

# Strings
rakho name = "Ashutosh Singh Rajput"
rakho greeting = "Namaste"

# Booleans
rakho is_active = sach      # true
rakho is_deleted = jhooth    # false

# Arrays
rakho numbers = [1, 2, 3, 4, 5]
rakho names = ["Ram", "Shyam", "Geeta"]
```

### Functions
```mantra
# Simple function
kaam greet(name) {
    wapas "Namaste, " + name
}

dikhao greet("Ashutosh")  # Output: Namaste, Ashutosh

# Function with multiple parameters
kaam add(a, b) {
    wapas a + b
}

dikhao add(5, 3)  # Output: 8
```

### Control Flow
```mantra
# If-Else
rakho marks = 85

agar marks >= 90
    dikhao "Grade: A+"
warna_agar marks >= 80
    dikhao "Grade: A"
warna_agar marks >= 70
    dikhao "Grade: B"
warna
    dikhao "Grade: C"

# While Loop
rakho count = 1
baarbaar count <= 5
    dikhao "Count: " + count
    count = count + 1

# For Loop
rakho fruits = ["Apple", "Banana", "Mango"]
har_ek fruit in fruits
    dikhao fruit
```

### HTTP and Networking
```mantra
# HTTP GET
rakho response = http_get("https://api.example.com/data")
dikhao response

# HTTP POST
rakho data = json_stringify({"name": "Ashutosh", "age": 25})
rakho response = http_post("https://api.example.com/users", data)
dikhao response

# Create HTTP Server
web_get "/" kaam(req) {
    wapas html_response("<h1>Welcome to MANTRA!</h1>")
}

web_get "/users/:id" kaam(req) {
    rakho userId = req.params["id"]
    wapas json_response({"id": userId, "name": "User " + userId})
}

web_start(8080)
dikhao "Server running on http://localhost:8080"
```

### Async/Await
```mantra
async kaam fetchData() {
    dikhao "Fetching data..."
    await sleep(1000)
    rakho data = http_get("https://api.example.com/data")
    wapas data
}

async kaam processData() {
    rakho task = async fetchData()
    dikhao "Waiting for data..."
    rakho result = await task
    dikhao "Received: " + result
}

async processData()
```

### Module System
```mantra
# Import modules
import "math"
import "string"
import "json"

# Use imported functions
rakho result = math.add(5, 10)
dikhao result

rakho text = string.upper("hello")
dikhao text

# Export your own functions
export kaam myFunction() {
    dikhao "Hello from exported function!"
}
```

### Package Manager (IND)
```mantra
# Install packages
ind_install "networking"
ind_install "ai"
ind_install "database"

# List installed packages
rakho packages = ind_list()
dikhao packages

# Search for packages
rakho results = ind_search "http"
dikhao results

# Update packages
ind_update "networking"
ind_update  # Update all

# Create new package
ind_init "mera_package" "Ashutosh Singh Rajput"
```

### Testing
```mantra
test_reset()

test "math_operations" kaam() {
    assert_equal 2 + 2 4 "Addition should work"
    assert_equal 10 - 3 7 "Subtraction should work"
    assert_equal 4 * 5 20 "Multiplication should work"
    assert_equal 10 / 2 5 "Division should work"
}

test "string_operations" kaam() {
    rakho name = "Ashutosh"
    assert_equal length(name) 8 "Length should be correct"
    assert_true name != "" "Name should not be empty"
}

test "boolean_logic" kaam() {
    assert_true sach "True should be true"
    assert_false jhooth "False should be false"
    assert 5 > 3 "5 should be greater than 3"
}

rakho results = test_results()
dikhao "Passed: " + results["passed"]
dikhao "Failed: " + results["failed"]
dikhao "Total: " + results["total"]

agar results["failed"] > 0
    dikhao "Some tests failed!"
    dikhao results["failures"]
warna
    dikhao "All tests passed!"
```

---

## 🏗️ Architecture

```
MANTRA Language Architecture
├── Core
│   ├── Lexer (22 languages support)
│   ├── Parser (Dual paradigms)
│   ├── Semantic Analyzer
│   ├── Interpreter
│   ├── VM (Bytecode execution)
│   ├── Module System
│   ├── Async System
│   └── Package Manager (IND)
├── Standard Library
│   ├── I/O, Math, String
│   ├── File, SQLite, Network
│   ├── HTTP, Socket, Server
│   ├── JSON, XML, YAML
│   ├── ZIP, PDF, GUI
│   ├── Browser, Crypto, Hash
│   ├── Web Framework
│   └── Testing Suite
└── Web
    └── MTR Parser (HTML generation)
```

---

## 🧪 Testing

```bash
# Run all tests
./mantra tests/run_all_tests.mtr

# Run specific test
./mantra tests/test_vm.mtr
./mantra tests/test_http.mtr
./mantra tests/test_json.mtr
./mantra tests/test_module.mtr
./mantra tests/test_async.mtr
./mantra tests/test_web.mtr
```

---

## 📦 Standard Library Modules

| Module | Description | Keywords |
|--------|-------------|----------|
| `io` | Input/Output | `dikhao`, `input`, `length`, `concat` |
| `math` | Mathematics | `sin`, `cos`, `tan`, `sqrt`, `pow` |
| `string` | String operations | `upper`, `lower`, `split`, `replace` |
| `file` | File I/O | `read`, `write`, `append`, `delete` |
| `sqlite` | Database | `connect`, `query`, `insert`, `fetch` |
| `network` | Networking | `http_get`, `http_post`, `socket` |
| `json` | JSON parsing | `json_parse`, `json_stringify` |
| `web` | Web framework | `web_get`, `web_post`, `web_start` |
| `async` | Async/Await | `async`, `await`, `sleep` |
| `test` | Testing | `assert`, `test`, `expect` |

---

## 🤝 Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for details.

### How to Contribute
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests
5. Submit a pull request

---

## 📄 License

MANTRA is licensed under the [MIT License](LICENSE).

---

## 🙏 Acknowledgments

- **Creator:** Ashutosh Singh Rajput
- **Vision:** Programming for Every Indian
- **Mission:** Break language and syntax barriers

---

<p align="center">
  <b>Made with ❤️ for India</b><br>
  <i>जय हिंद! 🇮🇳</i>
</p>
