# MANTRA Syntax Migration Guide v2.0

## Overview

This guide provides step-by-step migration instructions from MANTRA v1.x (current) to v2.0 (new grammar-stabilized version).

**TL;DR**: Main change is print statement syntax. Most code migrates automatically.

## Breaking Changes Summary

| Change | Impact | Severity | Auto-fixable |
|--------|--------|----------|--------------|
| Print syntax: `dikhao = X` → `dikhao X` | All print statements | MAJOR | YES |
| No other syntax changes | - | - | - |

**Migration Effort**: 5-10 minutes for typical program

## Change 1: Print Statement Syntax (MAJOR)

### Before (v1.x)
```mantra
dikhao = "Hello World"
kaado = "Vanakkam"
dekhao = "Nomoshkar"
print = "English"
```

### After (v2.0)
```mantra
dikhao "Hello World"
kaado "Vanakkam"
dekhao "Nomoshkar"
print "English"
```

### Rationale
- Eliminates grammar ambiguity between print and assignment
- Simplifies parser: print keyword + expression (no = operator)
- Makes intent clearer: `dikhao x` is obviously a print, not assignment

### Migration Steps

#### Step 1: Identify print statements
Look for pattern: `<print_keyword> = <expression>`

Print keywords across all 22 languages:
- Hinglish: `dikhao`
- Tanglish: `kaado`
- Punglish: `dakho`
- Benglish: `dekhao`
- Gujarathi: `batavo`
- Marathi: `dakhav`
- Telugu: `chupinchu`
- Kannada: `toro`
- Malayalam: `kaaniku`
- Maithili: `dekhaow`
- Konkani: `dakhoi`
- Nepali: `dekhau`
- Sanskrit: `darshaya`
- Kashmiri: `waatav`
- Sindhi: `dikhay`
- Dogri: `wekho`
- Manipuri: `nungsi`
- Bodo: `nangi`
- Santhali: `dado`
- English: `print`

#### Step 2: Remove "=" operator

**Before**:
```mantra
dikhao = x + 5
dikhao = "string"
dikhao = my_function()
```

**After**:
```mantra
dikhao x + 5
dikhao "string"
dikhao my_function()
```

#### Step 3: Auto-fixer tool (RECOMMENDED)

Use the provided auto-fixer to migrate all files automatically:

```bash
# Migrate single file
mantra-migrate --syntax v1-to-v2 old_program.mtr > new_program.mtr

# Migrate all files in directory
mantra-migrate --syntax v1-to-v2 --recursive ./src/

# Dry-run (show what would change, don't modify)
mantra-migrate --syntax v1-to-v2 --dry-run program.mtr
```

#### Step 4: Manual verification

After auto-migration, verify:
1. All print statements now use form: `keyword <expression>`
2. No "=" immediately after print keywords
3. Assignments still use "=" form: `variable = <expression>`
4. Let statements still use "=" form: `let/rakho variable = <expression>`

### Examples

#### Example 1: Simple print

**Before**:
```mantra
dikhao = "Namaste"
```

**After**:
```mantra
dikhao "Namaste"
```

**Verification**: Both print the string "Namaste"

#### Example 2: Print with expression

**Before**:
```mantra
x = 5
dikhao = x + 10
```

**After**:
```mantra
x = 5
dikhao x + 10
```

**Verification**: Both print 15

#### Example 3: Print function result

**Before**:
```mantra
kaam add(a, b)
    wapas a + b

dikhao = add(3, 4)
```

**After**:
```mantra
kaam add(a, b)
    wapas a + b

dikhao add(3, 4)
```

**Verification**: Both print 7

#### Example 4: Complex expression

**Before**:
```antra
x = 10
y = 20
dikhao = x + y * 2
```

**After**:
```mantra
x = 10
y = 20
dikhao x + y * 2
```

**Verification**: Both print 50 (not 60, due to operator precedence)

### Assignment vs Print: Disambiguation

**Watch out**: Don't confuse assignment with print!

**WRONG** (still looks like assignment):
```mantra
dikhao = x     # This is syntax error in v2.0
result = x     # This is correct assignment
```

**CORRECT**:
```mantra
dikhao x       # This is print
result = x     # This is assignment
```

**Key Rule**: Print keywords CANNOT be followed by "="

### Multilingual Examples

All print keywords migrate the same way:

#### Hinglish
```mantra
dikhao = "Namaste"        # Before
dikhao "Namaste"          # After
```

#### Tanglish
```mantra
kaado = "Vanakkam"        # Before
kaado "Vanakkam"          # After
```

#### Bengali
```mantra
dekhao = "Nomoshkar"      # Before
dekhao "Nomoshkar"        # After
```

#### Malayalam
```mantra
kaaniku = "Namaskaram"    # Before
kaaniku "Namaskaram"      # After
```

#### English
```mantra
print = "Hello"           # Before
print "Hello"             # After
```

## No Other Breaking Changes

The following continue to work exactly as before:

### Assignments
```mantra
x = 5          # Still works
name = "John"  # Still works
result = x + 5 # Still works
```

### Let bindings
```mantra
rakho x = 5           # Still works
let y = 10            # Still works
rakho name = "Alice"  # Still works
```

### Conditionals
```mantra
agar x > 5 tab
    dikhao "large"
warna
    dikhao "small"
```

### Loops
```mantra
baarbaar i = 0 se 10 tak
    dikhao i
```

### Functions
```mantra
kaam add(a, b)
    wapas a + b
```

### All other syntax
- Operators: `+`, `-`, `*`, `/`, `%`, `==`, `!=`, `<`, `>`, `<=`, `>=`, `&&`, `||`, `!`
- Data structures: Arrays `[1, 2, 3]`
- Comments: Still supported (if implemented)
- Whitespace and newlines: Same rules
- Language mixing: Still allowed

## Migration Checklist

Use this checklist to ensure complete migration:

- [ ] Identified all print statements in codebase
- [ ] Removed "=" after all print keywords
- [ ] Verified all assignments still use "=" operator
- [ ] Verified all let bindings still use "=" operator
- [ ] Ran auto-fixer tool to double-check
- [ ] Tested migrated programs with new parser
- [ ] All tests pass
- [ ] Code review completed

## Rollback Procedure

If you need to stay on v1.x temporarily:

1. Use `mantra-v1` wrapper script instead of `mantra`
2. Request v1.x branch from repository
3. Build v1.x from source: `git checkout v1.x && cmake -B build && cmake --build build`

**Note**: v1.x support ends 6 months after v2.0 release.

## Error Messages: What They Mean

### Error: "Expected expression after print keyword"

**Cause**: Print statement uses old "=" syntax

```mantra
dikhao = "Hello"   # Wrong
        ^
```

**Solution**: Remove the "="

```mantra
dikhao "Hello"     # Correct
```

### Error: "Print keyword cannot be used as variable"

**Cause**: Attempting to assign to a print keyword

```mantra
dikhao = 5    # Wrong - dikhao is keyword, not variable
      ^
```

**Solution**: Use different variable name

```mantra
message = 5   # Correct
```

### Error: "Expected variable name, got print keyword"

**Cause**: Using print keyword where variable expected in let binding

```mantra
rakho dikhao = 5  # Wrong - dikhao is keyword
     ^
```

**Solution**: Use different variable name

```mantra
rakho message = 5 # Correct
```

## Performance Impact

**Migration does NOT affect performance**:
- Parser: Slightly faster (simpler grammar)
- Interpreter: Identical (same AST semantics)
- Compiler: Slightly faster (fewer helper functions)

## Version Compatibility

| Version | Print Syntax | Status | Support |
|---------|--------------|--------|---------|
| v1.0-v1.9 | `print = x` | Current | Supported |
| v2.0+ | `print x` | New | Recommended |
| v1.x | `print = x` | Legacy | Support ends 2026-Q4 |

## Getting Help

If you have trouble migrating:

1. **Run auto-fixer**: `mantra-migrate --syntax v1-to-v2 program.mtr`
2. **Check examples**: See Example 1-4 above
3. **Report issues**: Create issue on GitHub with example code
4. **Community**: Post on discussion board with migration question

## Summary

**Main migration task**: Change `keyword = expression` to `keyword expression` for all print statements.

**Time required**: < 10 minutes for typical program (automated)

**Effort**: Low (single syntax change, fully auto-fixable)

**Benefits**: Clearer code, fewer ambiguities, faster parser

**Breakage risk**: Very low (only print statements affected, easily automated)

**Recommended action**: Use auto-fixer tool, review changes, commit.

---

**Questions?** See PARSER_DESIGN.md for why this change was necessary.
