# MANTRA - India's First Programming Language

![MANTRA](https://img.shields.io/badge/Language-MANTRA-blue) ![Status](https://img.shields.io/badge/Status-Active%20Development-yellow) ![License](https://img.shields.io/badge/License-MIT-green)

## What is MANTRA?

MANTRA (Multi-lingual Advanced Programming Language for Indian Region) is **India's first programming language** designed specifically for Indian developers and learners. It supports coding in 22 Indian language mixes using Roman script, allowing developers to write code in the language that feels most natural to them.

### Vision

MANTRA bridges the gap between programming and local languages, making coding accessible to millions of Indians who are more comfortable with their native languages mixed with Roman script (popularly known as "Hinglish", "Tanglish", etc.).

## Supported Languages (22 Language Mixes)

MANTRA supports programming in the following language combinations:

1. **HINGLISH** (Hindi + English) - `dikhao`, `agar`, `warna`, `baarbaar`
2. **TANGLISH** (Tamil + English) - `kaado`, `endral`, `illena`
3. **PUNGLISH** (Punjabi + English) - `dakho`, `jekr`, `mudmudke`
4. **BENGLISH** (Bengali + English) - `dekhao`, `jodi`, `nahole`
5. **GUJLISH** (Gujarati + English) - `batavo`, `jor`, `nahi_to`
6. **MARATHLISH** (Marathi + English) - `dakhav`, `jar`, `nahitar`
7. **TELGLISH** (Telugu + English) - `chupinchu`, `ante`, `lekapote`
8. **KANGLISH** (Kannada + English) - `toro`, `agre`, `illandre`
9. **MANGLISH** (Malayalam + English) - `kaaniku`, `engil`, `allenkil`
10. **ODLISH** (Oriya + English) - `jadi`, `jabat`, `phera`
11. **ASSLISH** (Assamese + English) - `nohole`, `jabotloi`, `ghurai`
12. **MAITHLISH** (Maithili + English) - `dekhaow`, `je`, `nahi_ta`
13. **KONKLISH** (Konkani + English) - `dakhoi`, `zor`, `naitar`
14. **NEPLISH** (Nepali + English) - `dekhau`, `yedi`, `nabhaye`
15. **SANGLISH** (Sanskrit + English) - `darshaya`, `yadi`, `anyatha`
16. **KASHLISH** (Kashmiri + English) - `waatav`, `galaw`
17. **SINDLISH** (Sindhi + English) - `dikhay`, `khoota`, `ain`
18. **DOGLISH** (Dogri + English) - `wekho`, `ger`, `nahin_ta`
19. **MANLISH** (Manipuri + English) - `nungsi`, `amare`, `natte`
20. **BODLISH** (Bodo + English) - `nangi`, `bodo`, `nangou`
21. **SANTLISH** (Santhali + English) - `dado`, `ama`, `bae`
22. **ENGLISH** (Pure English) - Standard English keywords

## Syntax Examples

### Basic Print (Hinglish)
```mantra
dikhao "Namaste Duniya"
```

### Conditional (Hinglish)
```mantra
agar x > 5 tab
  dikhao "bada hai"
warna
  dikhao "chota hai"
```

### Loop (Hinglish)
```mantra
baarbaar i = 0 se 10 tak
  dikhao i
```

### Function Definition (Hinglish)
```mantra
kaam jodoba(a, b)
  wapas a + b

result = jodoba(5, 3)
dikhao result
```

### Similar Examples in Other Languages

#### Tanglish (Tamil)
```mantra
kaado "Vanakam Ulagam"
```

#### Benglish (Bengali)
```mantra
dekhao "Nomoshkar Bishsho"
jodi x > 5 tab
  dekhao "Voro"
nahole
  dekhao "Choto"
```

#### Kanglish (Kannada)
```mantra
toro "Namaskara Lokada"
agre x > 5 tab
  toro "Doddakke"
illandre
  toro "Chiyadakke"
```

## Project Structure

```
mantra/
├── CMakeLists.txt           # Build configuration
├── README.md                # This file
├── core/
│   ├── token.h              # Token definitions
│   ├── lexer.h              # Lexer interface
│   └── lexer.cpp            # Lexer implementation
├── unicode/
│   ├── unicode.h            # Unicode utilities header
│   ├── unicode.cpp          # UTF-8 and character handling
│   ├── script_detect.h      # Script detection header
│   └── script_detect.cpp    # Script detection implementation
└── error/
    ├── error.h              # Error handling header
    └── error.cpp            # Error handling with bilingual messages
```

## Features

### ✅ Full UTF-8 Support
- Complete UTF-8 encoding/decoding
- Support for all Unicode scripts
- Proper handling of multi-byte characters

### ✅ Script Detection
- Automatic detection of Indian scripts
- Mixed script support (Hinglish, Tanglish, etc.)
- Confidence scoring for script detection

### ✅ Comprehensive Lexer
- Single and triple-quoted strings with escape sequences
- Integer, float, and hexadecimal number support
- Comments with `#` syntax
- Cross-platform line ending support (Unix/Windows)
- Detailed error messages

### ✅ Bilingual Error Messages
- All errors and warnings in both Hindi and English
- ANSI colored output for better readability
- Location tracking (line and column)

### ✅ Production Quality Code
- C++17 standard compliance
- No placeholder code
- All functions fully implemented
- Comprehensive keyword maps with no duplicates

## Building from Source

### Prerequisites
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2015+)
- CMake 3.15 or higher
- Git

### Build Instructions

#### Linux/macOS
```bash
git clone https://github.com/RT-Ecosystems/mantra-lang.git
cd mantra-lang
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
make install
```

#### Windows (Visual Studio)
```bash
git clone https://github.com/RT-Ecosystems/mantra-lang.git
cd mantra-lang
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
cmake --install . --config Release
```

### Build Options
```bash
# Debug build with symbols
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Install to custom location
cmake --install . --prefix /custom/path
```

## Compiler Structure

The MANTRA compiler consists of several components:

### 1. **Lexer** (Lexical Analysis)
- Tokenizes source code into meaningful tokens
- Recognizes keywords from all 22 language mixes
- Handles string literals, numbers, and operators
- Reports lexical errors with precise location information

### 2. **Unicode Handler**
- Converts between codepoints and UTF-8 representation
- Validates UTF-8 sequences
- Provides character classification functions
- Supports digits from all Indian scripts

### 3. **Script Detector**
- Identifies the script of individual characters
- Detects dominant script in mixed-script text
- Provides confidence scores for detection
- Supports 9 different Indian scripts + Roman

### 4. **Error Handler**
- Bilingual error and warning messages (Hindi/English)
- ANSI colored output for visual clarity
- Verbosity levels for different output modes
- Detailed location information (line and column)

## Token Types Supported

The compiler recognizes:
- **Literals**: Numbers (int, float, hex), strings
- **Keywords**: 160+ keywords from 22 language mixes
- **Operators**: Arithmetic, comparison, logical
- **Delimiters**: Parentheses, braces, brackets
- **Special**: Comments, newlines, EOF

## Examples

### Example 1: Hello World in Hinglish
```mantra
dikhao "Namaste Duniya"
```

### Example 2: Loop in Tanglish
```mantra
baarbaar i = 0 se 10 tak
  kaado i
```

### Example 3: Function in Benglish
```mantra
kaam yogkoro(a, b)
  wapas a + b

phal = yogkoro(10, 20)
dekhao phal
```

### Example 4: Conditional in Kanglish
```mantra
age = 18
agre age >= 18 tab
  toro "Vegavantha"
illandre
  toro "Kanya"
```

## Contributing

Contributions are welcome! Please ensure:
1. All code follows C++17 standards
2. No compiler warnings with `-Wall -Wextra -Wpedantic`
3. New features include comprehensive error handling
4. Documentation is updated accordingly
5. Tests are added for new functionality

## Future Roadmap

- [ ] Parser implementation
- [ ] Abstract Syntax Tree (AST)
- [ ] Semantic analyzer
- [ ] Intermediate code generation
- [ ] Code optimizer
- [ ] Backend compiler (LLVM/native)
- [ ] Native script support (Devanagari, Tamil, etc.)
- [ ] Package manager
- [ ] Standard library

## Project Statistics

- **Total Keywords**: 160+ from 22 language mixes
- **Supported Scripts**: 9 Indian scripts + Roman
- **Code Files**: 9 (5 source + 4 headers)
- **Lines of Code**: 1000+ production code
- **Unicode Support**: Full UTF-8 compliance

## Performance

The MANTRA compiler is optimized for:
- Fast tokenization with O(n) complexity
- Efficient script detection
- Minimal memory overhead
- Cross-platform compatibility

## License

MANTRA is open-sourced under the MIT License. See LICENSE file for details.

## Authors

RT-Ecosystems Development Team

## Support

For issues, questions, or suggestions:
- Create an issue on GitHub
- Contact: support@rt-ecosystems.com

---

**MANTRA - Coding in Your Language, Building Your Dreams**
