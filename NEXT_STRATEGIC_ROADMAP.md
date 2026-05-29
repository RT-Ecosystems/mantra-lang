# MANTRA Strategic Roadmap - Next 12 Months

**Date:** May 29, 2026  
**Vision:** Transform MANTRA from experimental language to production-ready platform for Indian developers  
**Philosophy:** Quality over quantity; correctness before features

---

## Executive Strategic Overview

### Current State (May 2026)
- **Maturity Level:** Experimental
- **Production Readiness:** 25%
- **Key Capability:** Runs simple programs with 22 language support
- **Main Blocker:** Type system, file I/O, error handling incomplete

### Target State (May 2027)
- **Maturity Level:** Production-ready
- **Production Readiness:** 85%+
- **Key Capability:** Reliable tool for Indian developer communities
- **Enabling Feature:** Complete ecosystem with IDE, package manager, documentation

### Strategic Pillars
1. **Stabilization** - Fix architectural issues
2. **Completion** - Finish incomplete subsystems
3. **Quality** - Comprehensive testing and documentation
4. **Community** - IDE, tooling, ecosystem

---

## Phase-by-Phase Roadmap

### PHASE 1: STABILIZATION (Weeks 1-4, June 2026)

**Goal:** Fix architectural issues, reach 40% production readiness

#### 1.1: Consolidate Value Type System (Week 1)
**Problem:** Three value implementations cause duplication and maintenance burden

**Tasks:**
1. Create unified `runtime::Value` class (4 hours)
   - Merge MantraValue, vm::Value, runtime::Value
   - Single enum for ValueType
   - Unified factory methods
   
2. Update interpreter to use unified type (3 hours)
   - Replace all MantraValue with runtime::Value
   - Update environment to use new type
   
3. Update semantic analyzer (2 hours)
   - Update value type handling
   - Add semantic analysis for new types
   
4. Update all builtins (2 hours)
   - Test with new value types
   - Fix any compatibility issues
   
5. Tests for unified type (3 hours)
   - Unit tests for value type
   - Integration tests with interpreter
   
**Deliverable:** Single, unified value type system  
**Tests Required:** 50 new test cases  
**Timeline:** 1 week (14 hours)  
**Risk:** Medium - touches core execution system

---

#### 1.2: Implement Type Checking (Week 2)
**Problem:** Type checking stub, no enforcement

**Tasks:**
1. Analyze type checker design (2 hours)
   - Study current incomplete implementation
   - Design proper type checking algorithm
   
2. Implement type inference (6 hours)
   - Literal type inference
   - Expression type propagation
   - Function return type inference
   
3. Implement type checking (6 hours)
   - Binary operator type validation
   - Function call type checking
   - Assignment type checking
   
4. Add error messages (2 hours)
   - Clear type error messages
   - Suggestions for type fixes
   
5. Tests for type checking (4 hours)
   - Type error detection tests
   - Type inference tests
   
**Deliverable:** Type checking enforcement with error messages  
**Tests Required:** 100 new test cases  
**Timeline:** 1 week (20 hours)  
**Risk:** High - fundamental change to execution

---

#### 1.3: Fix Parser Grammar (Week 2-3)
**Problem:** Assignment/comparison ambiguity in if conditions

**Tasks:**
1. Analyze ambiguity (2 hours)
   - Document current behavior
   - Identify affected constructs
   
2. Choose resolution strategy (1 hour)
   - Decide on Option A/B/C
   - Document decision
   
3. Update parser (4 hours)
   - Implement chosen strategy
   - Update precedence if needed
   
4. Update semantic analysis (2 hours)
   - Add checks for disallowed patterns
   - Add helpful error messages
   
5. Tests for grammar fix (4 hours)
   - Test all conditional patterns
   - Test error cases
   
**Deliverable:** Unambiguous grammar with clear semantics  
**Tests Required:** 50 new test cases  
**Timeline:** 1 week (13 hours)  
**Risk:** Medium - could break existing programs

---

#### 1.4: Remove Dead Code (Week 3)
**Problem:** Legacy VM and old code clutters codebase

**Tasks:**
1. Identify all dead code (1 hour)
   - Legacy VM (vm/ directory)
   - Unused implementations
   - Deprecated code
   
2. Remove legacy VM (2 hours)
   - Delete vm/ directory
   - Remove from CMakeLists.txt
   - Remove legacy_vm namespace
   
3. Remove unused code (1 hour)
   - Compiler stub
   - Unused headers
   - Deprecated functions
   
4. Update documentation (1 hour)
   - Remove references to old code
   - Update architecture docs
   
5. Verify build (1 hour)
   - Full rebuild
   - Run all tests
   
**Deliverable:** Clean codebase, no dead code  
**Tests Required:** None  
**Timeline:** 1 week (6 hours)  
**Risk:** Low - removes unused code

---

#### 1.5: Fix Compiler Warnings (Week 4)
**Problem:** 10+ compiler warnings (unused parameters, etc)

**Tasks:**
1. Enumerate warnings (1 hour)
   - Compile with full warnings
   - Document each warning
   
2. Fix unused parameters (3 hours)
   - Add [[maybe_unused]] where appropriate
   - Remove unused parameters where possible
   
3. Fix other warnings (2 hours)
   - Shadowed variables
   - Type conversions
   - Other issues
   
4. Verify clean build (1 hour)
   - Compile with -Wall -Wextra -Wpedantic
   - No warnings
   
**Deliverable:** Clean build with zero warnings  
**Tests Required:** None  
**Timeline:** 1 week (7 hours)  
**Risk:** Low - trivial fixes

---

#### 1.6: Expand Test Coverage (Week 4)
**Problem:** ~25% test coverage, need 50%

**Tasks:**
1. Add parser tests (6 hours)
   - Test all statement types
   - Test operator precedence
   - Test error cases
   
2. Add interpreter tests (4 hours)
   - Test all expression types
   - Test control flow
   - Test function calls
   
3. Add semantic tests (3 hours)
   - Test undefined variable detection
   - Test function arity
   - Test scope management
   
4. Add integration tests (3 hours)
   - End-to-end language tests
   - Multi-feature programs
   
**Deliverable:** 50% test coverage (1500+ test lines)  
**Tests Required:** 150+ new tests  
**Timeline:** 1 week (16 hours)  
**Risk:** Low - just adding tests

---

**Phase 1 Summary:**
- **Total Effort:** ~76 hours
- **Expected Readiness:** 40%
- **Key Achievement:** Clean, unified architecture
- **Risk Level:** Medium-High (due to type changes)

**Phase 1 Go-No-Go:** Ship if:
- ✓ Type checking implemented
- ✓ No compiler warnings
- ✓ 50% test coverage
- ✓ Parser grammar fixed

---

### PHASE 2: COMPLETION (Weeks 5-8, July 2026)

**Goal:** Implement missing features, reach 55% production readiness

#### 2.1: File I/O Implementation (Week 5)
**Problem:** File I/O functions are stubs, blocking real programs

**Tasks:**
1. Design file API (2 hours)
   - open(filename, mode) → file
   - read(file) → string
   - write(file, data) → success
   - close(file)
   
2. Implement file operations (6 hours)
   - File open/close
   - Text read/write
   - Error handling
   
3. Add file operations to stdlib (2 hours)
   - Register functions
   - Add builtins
   
4. Tests for file I/O (4 hours)
   - File creation
   - Read/write operations
   - Error cases
   
**Deliverable:** Working file I/O operations  
**Tests Required:** 30 new tests  
**Timeline:** 1 week (14 hours)  
**Risk:** Low - isolated feature

---

#### 2.2: Error Handling (try/catch) (Week 5-6)
**Problem:** No error handling primitives, programs crash on errors

**Tasks:**
1. Design error handling (2 hours)
   - try block
   - catch clause
   - finally (optional)
   - throw statement
   
2. Add AST nodes (2 hours)
   - TryNode
   - CatchNode
   - ThrowNode
   
3. Add parser support (4 hours)
   - Parse try blocks
   - Parse catch clauses
   - Parse throw statements
   
4. Add semantic analysis (2 hours)
   - Variable binding in catch
   - Type checking for exceptions
   
5. Add interpreter support (4 hours)
   - Try/catch execution
   - Exception propagation
   - Finally blocks
   
6. Tests for error handling (4 hours)
   - Try/catch execution
   - Exception propagation
   - Error cases
   
**Deliverable:** Working try/catch error handling  
**Tests Required:** 50 new tests  
**Timeline:** 2 weeks (18 hours)  
**Risk:** Medium - new language feature

---

#### 2.3: Module System Basics (Week 6-7)
**Problem:** No way to organize code into modules, everything in one file

**Tasks:**
1. Design module system (3 hours)
   - Import syntax
   - Export declarations
   - Module resolution
   - Namespace management
   
2. Add AST support (2 hours)
   - ImportNode
   - ExportNode
   
3. Add parser support (4 hours)
   - Parse import statements
   - Parse export declarations
   
4. Add semantic analysis (4 hours)
   - Module resolution
   - Symbol import/export
   - Circular dependency detection
   
5. Add interpreter support (6 hours)
   - Module loading
   - Namespace management
   - Error handling
   
6. Tests for modules (4 hours)
   - Module loading
   - Symbol imports
   - Error cases
   
**Deliverable:** Basic module system for code organization  
**Tests Required:** 40 new tests  
**Timeline:** 2 weeks (23 hours)  
**Risk:** High - requires file I/O + semantic changes

---

#### 2.4: Expand Standard Library (Week 7-8)
**Problem:** Limited builtin functions, only 20, need 50+

**Tasks:**
1. Add type conversion functions (3 hours)
   - toString()
   - toNumber()
   - toBoolean()
   
2. Add array operations (4 hours)
   - length, push, pop
   - map, filter, reduce
   - slice, concat, join
   
3. Add object operations (3 hours)
   - keys(), values(), entries()
   - merge(), has(), delete
   
4. Add string operations (4 hours)
   - trim, replace, match
   - indexOf, lastIndexOf
   - slice, repeat, padStart/padEnd
   
5. Add utility functions (3 hours)
   - typeof, instanceof
   - assert, range
   
6. Tests for stdlib (4 hours)
   - All new functions
   - Edge cases
   
**Deliverable:** 50+ standard library functions  
**Tests Required:** 80 new tests  
**Timeline:** 2 weeks (21 hours)  
**Risk:** Low - isolated additions

---

**Phase 2 Summary:**
- **Total Effort:** ~76 hours
- **Expected Readiness:** 55%
- **Key Achievement:** Usable language for real programs
- **Risk Level:** Medium

**Phase 2 Go-No-Go:** Ship if:
- ✓ File I/O working
- ✓ Error handling working
- ✓ Module system basic but functional
- ✓ 70+ stdlib functions

---

### PHASE 3: QUALITY (Weeks 9-12, August 2026)

**Goal:** Production quality, reach 70% readiness

#### 3.1: LSP Implementation (Week 9-10)
**Problem:** No IDE support, poor developer experience

**Tasks:**
1. Design LSP implementation (3 hours)
   - Review LSP spec
   - Design server architecture
   - Plan features (autocomplete, diagnostics)
   
2. Implement LSP server (12 hours)
   - Initialize protocol
   - Document symbols
   - Hover information
   - Go to definition
   - Find references
   
3. Integrate with parser/analyzer (4 hours)
   - Extract symbols from AST
   - Generate diagnostics
   - Type information for hover
   
4. Test LSP implementation (4 hours)
   - Protocol tests
   - Feature tests
   
**Deliverable:** Working LSP server for IDE integration  
**Timeline:** 2 weeks (23 hours)  
**Risk:** High - complex protocol

---

#### 3.2: Type Inference Enhancement (Week 10-11)
**Problem:** Type checking works but inference limited

**Tasks:**
1. Improve type inference (6 hours)
   - Generic function types
   - Union type inference
   - Type constraints
   
2. Add type annotations (4 hours)
   - Optional type syntax
   - Function parameter types
   - Return type inference
   
3. Enhance error messages (2 hours)
   - Better type error diagnostics
   - Suggestion for fixes
   
4. Tests for type system (4 hours)
   - Inference edge cases
   - Type error messages
   
**Deliverable:** Advanced type system  
**Timeline:** 2 weeks (16 hours)  
**Risk:** Medium

---

#### 3.3: Performance Optimization (Week 11-12)
**Problem:** Slow execution, 100-1000x slower than native

**Tasks:**
1. Add optimization passes (6 hours)
   - Constant folding
   - Dead code elimination
   - Common subexpression elimination
   
2. Optimize interpreter (4 hours)
   - Cache global lookups
   - Optimize environment chains
   - Inline hot functions
   
3. Add peephole optimization (3 hours)
   - Instruction sequence optimization
   - Pattern matching
   
4. Benchmark and profile (3 hours)
   - Identify bottlenecks
   - Measure improvements
   - Create benchmarks
   
**Deliverable:** 2-5x performance improvement  
**Timeline:** 2 weeks (16 hours)  
**Risk:** Low

---

#### 3.4: Comprehensive Documentation (Week 12)
**Problem:** Insufficient documentation, needed for adoption

**Tasks:**
1. Language specification (4 hours)
   - Grammar (EBNF)
   - Type system
   - Semantics
   
2. Standard library reference (4 hours)
   - Function documentation
   - Examples
   - Error cases
   
3. Getting started guide (3 hours)
   - Installation
   - First program
   - Common tasks
   
4. Internals documentation (3 hours)
   - Architecture guide
   - Contributing guide
   - Development setup
   
**Deliverable:** Complete documentation set  
**Timeline:** 1 week (14 hours)  
**Risk:** Low

---

**Phase 3 Summary:**
- **Total Effort:** ~69 hours
- **Expected Readiness:** 70%
- **Key Achievement:** Production quality, IDE support
- **Risk Level:** Medium

**Phase 3 Go-No-Go:** Ship if:
- ✓ LSP server functional
- ✓ Type system solid
- ✓ Performance improved
- ✓ Documentation complete

---

### PHASE 4: ECOSYSTEM (Weeks 13-16, September 2026)

**Goal:** Community readiness, reach 80%+ readiness

#### 4.1: Package Manager (Week 13-14)
**Problem:** No package management, can't share code

**Tasks:**
1. Design package format (3 hours)
   - Package structure
   - Manifest format
   - Version semantics
   
2. Implement package registry (8 hours)
   - GitHub-based (simplest)
   - Package search
   - Version resolution
   
3. Implement package manager (8 hours)
   - Install command
   - Update command
   - Remove command
   - Dependency resolution
   
4. Tests for package manager (4 hours)
   
**Deliverable:** Working package manager  
**Timeline:** 2 weeks (23 hours)  
**Risk:** High - complex system

---

#### 4.2: Formatter & Linter (Week 14-15)
**Problem:** No code style enforcement

**Tasks:**
1. Implement formatter (6 hours)
   - AST-based formatting
   - Configurable style
   
2. Implement linter (6 hours)
   - Unused variables
   - Type errors
   - Style violations
   
3. Integration (2 hours)
   - CLI tools
   - IDE integration
   
4. Tests (2 hours)
   
**Deliverable:** Code formatter and linter  
**Timeline:** 2 weeks (16 hours)  
**Risk:** Low

---

#### 4.3: Debugger (Week 15-16)
**Problem:** Hard to debug programs

**Tasks:**
1. Design debugger (2 hours)
   - Architecture
   - Protocol
   
2. Implement debugger (10 hours)
   - Breakpoints
   - Step execution
   - Variable inspection
   
3. Integrate with IDE (4 hours)
   - DAP protocol
   - IDE integration
   
4. Tests (2 hours)
   
**Deliverable:** Working debugger with IDE support  
**Timeline:** 2 weeks (18 hours)  
**Risk:** High

---

#### 4.4: Community Engagement (Week 16)
**Problem:** No community, adoption difficult

**Tasks:**
1. Create community resources (3 hours)
   - GitHub discussions
   - Contributing guide
   - Code of conduct
   
2. Build examples (3 hours)
   - Example programs
   - Tutorials
   - Case studies
   
3. Marketing (2 hours)
   - Blog posts
   - Social media
   - Talks/demos
   
**Deliverable:** Community ready for adoption  
**Timeline:** 1 week (8 hours)  
**Risk:** Low

---

**Phase 4 Summary:**
- **Total Effort:** ~65 hours
- **Expected Readiness:** 80%+
- **Key Achievement:** Ecosystem complete, community ready
- **Risk Level:** High

---

## Overall Roadmap Timeline

```
June 2026 (Weeks 1-4)    PHASE 1: STABILIZATION
├─ Consolidate value types
├─ Type checking
├─ Grammar fixes
├─ Clean code
└─ Readiness: 40%

July 2026 (Weeks 5-8)    PHASE 2: COMPLETION
├─ File I/O
├─ Error handling
├─ Modules
├─ Stdlib expansion
└─ Readiness: 55%

August 2026 (Weeks 9-12) PHASE 3: QUALITY
├─ LSP server
├─ Type enhancement
├─ Performance
├─ Documentation
└─ Readiness: 70%

September 2026 (Weeks 13-16) PHASE 4: ECOSYSTEM
├─ Package manager
├─ Formatter/Linter
├─ Debugger
├─ Community
└─ Readiness: 80%+

TOTAL: 16 weeks (4 months), ~280 hours
```

---

## Resource Requirements

### Phase 1
- **Team Size:** 1-2 engineers
- **Skill Level:** Senior C++ developer (compiler knowledge)
- **Effort:** 76 hours (~2 weeks at 40 hrs/week)

### Phase 2
- **Team Size:** 2 engineers
- **Skill Level:** Senior + Mid-level
- **Effort:** 76 hours (~2 weeks at 40 hrs/week)

### Phase 3
- **Team Size:** 2 engineers
- **Skill Level:** Senior + Senior
- **Effort:** 69 hours (~2 weeks at 40 hrs/week)

### Phase 4
- **Team Size:** 2-3 engineers
- **Skill Level:** Senior + Mid-level + Community
- **Effort:** 65 hours (~2 weeks at 40 hrs/week)

**Total:** ~280 hours / 70 person-days / 4 months with 1-2 FTE

---

## Success Metrics

### Phase 1 Success
- ✓ Zero compiler warnings
- ✓ 50% test coverage
- ✓ Type checking works
- ✓ All tests pass

### Phase 2 Success
- ✓ 70% test coverage
- ✓ File I/O working
- ✓ Error handling works
- ✓ Module system functional

### Phase 3 Success
- ✓ 80% test coverage
- ✓ LSP server working
- ✓ IDE integration working
- ✓ Documentation complete

### Phase 4 Success
- ✓ Package manager working
- ✓ 100+ packages published
- ✓ Community growing
- ✓ 85%+ production readiness

---

## Risk Mitigation

### High Risk Items
1. **Type System Changes (Phase 1)**
   - Risk: Could break existing code
   - Mitigation: Comprehensive testing before merge
   - Contingency: Version control, easy rollback

2. **Module System (Phase 2)**
   - Risk: Complex feature, could have bugs
   - Mitigation: Incremental implementation, heavy testing
   - Contingency: Ship Phase 2 without modules, delay to Phase 3

3. **Package Manager (Phase 4)**
   - Risk: Complex distributed system
   - Mitigation: Start with GitHub-based approach (simpler)
   - Contingency: Use existing package manager initially

### Mitigation Strategy
- **Continuous Integration:** All changes tested immediately
- **Incremental Releases:** Ship after each phase
- **Community Testing:** Beta period for each phase
- **Documentation:** Keep docs updated with changes
- **Rollback Plan:** Easy version control for quick fixes

---

## Delivery Checkpoints

### End of June 2026 (Phase 1)
- [ ] v0.2.0 Released
- [ ] Production Readiness: 40%
- [ ] Type checking working
- [ ] Clean build, no warnings

### End of July 2026 (Phase 2)
- [ ] v0.3.0 Released
- [ ] Production Readiness: 55%
- [ ] File I/O working
- [ ] Error handling working

### End of August 2026 (Phase 3)
- [ ] v0.4.0 Released
- [ ] Production Readiness: 70%
- [ ] IDE support via LSP
- [ ] Performance improved

### End of September 2026 (Phase 4)
- [ ] v1.0.0 Released
- [ ] Production Readiness: 85%+
- [ ] Package manager working
- [ ] Community engaged

---

## Long-term Vision (12+ Months)

### Post-1.0 Initiatives

**Months 5-6:** Advanced Features
- Pattern matching
- Generics/Parametric types
- Async/await
- Macros

**Months 7-8:** Performance
- VM implementation (if not done)
- JIT compilation
- LLVM backend (optional)

**Months 9-10:** Ecosystem
- Web framework
- Database driver
- Game engine bindings
- Data science library

**Months 11-12:** Community
- Conference talks
- Tutorial series
- Case studies
- Corporate adoption

---

## Strategic Decisions

### Decision 1: Keep Interpreter or Implement VM?
**Decision:** Start with interpreter (fast to stabilize), add VM later
**Rationale:** 
- Simpler to debug
- Faster to get features working
- Can optimize later if needed
- VM is orthogonal to core features

### Decision 2: Single Language or Multi-language Support?
**Decision:** Keep all 22 languages supported
**Rationale:**
- Core differentiator
- Minimal added complexity
- Already working well
- Important for Indian developers

### Decision 3: Static or Dynamic Typing?
**Decision:** Dynamic typing with optional static annotations
**Rationale:**
- Easier to learn (important for beginners)
- Faster to develop
- Can add static typing later
- Matches other popular languages

### Decision 4: Which Platforms to Support?
**Decision:** Linux, macOS, Windows (all via C++ cross-platform)
**Rationale:**
- Pure C++17 is portable
- CMake handles platform differences
- No external dependencies = easier cross-platform

### Decision 5: Package Manager - Build or Reuse?
**Decision:** Build custom package manager using GitHub
**Rationale:**
- Simple GitHub-based approach
- No external dependency
- Easy to implement
- Community contribution friendly

---

## Conclusion

This 16-week roadmap transforms MANTRA from experimental to production-ready. The strategy focuses on:

1. **Stabilization First** - Fix architectural issues
2. **Completion Second** - Finish missing features
3. **Quality Third** - Testing and documentation
4. **Community Fourth** - Ecosystem and tooling

With disciplined execution of this plan, MANTRA can become a viable language for Indian developer communities by end of September 2026.

**Key Success Factor:** Maintain focus on quality over quantity. Shipping production-ready features matters more than shipping many features.

**Next Step:** Start Phase 1 immediately with consolidating value types.
