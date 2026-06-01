# IND Architecture and Design

## Overview

IND is a production-ready package manager for the MANTRA programming language. It provides a clean abstraction over package management operations while maintaining extensibility for future features.

## Core Architecture

### Layered Design

```
┌─────────────────────────────────────────┐
│         Command Line Interface          │
│     (argparse-based CLI dispatcher)     │
└────────────────┬────────────────────────┘
                 │
┌────────────────┴────────────────────────┐
│         Command Implementation Layer     │
│  (install, search, update, etc.)        │
└────────────────┬────────────────────────┘
                 │
┌────────────────┴────────────────────────┐
│      Storage and Registry Layer         │
│  (JSON-based package registry)          │
└────────────────┬────────────────────────┘
                 │
┌────────────────┴────────────────────────┐
│      Network and Cache Layer            │
│  (URL fetching, index caching)          │
└─────────────────────────────────────────┘
```

## Component Overview

### 1. Configuration Management
- **Location**: `~/.mantra/config.json`
- **Purpose**: Stores user preferences and settings
- **Functions**: `load_config()`, `save_config()`

### 2. Registry Management
- **Location**: `~/.mantra/installed.json`
- **Purpose**: Tracks installed packages and metadata
- **Functions**: `load_registry()`, `save_registry()`
- **Data Structure**:
  ```json
  {
    "package_name": {
      "name": "display_name",
      "version": "1.0.0",
      "source": "github.com/user/repo",
      "branch": "main",
      "installed_at": "2024-01-15T10:30:00",
      "description": "Package description"
    }
  }
  ```

### 3. Package Index
- **Cache Location**: `~/.mantra/cache/package_index.json`
- **Remote Source**: Central registry on GitHub
- **Functions**: `fetch_package_index()`, `load_package_index()`, `save_package_index()`
- **Index Format**:
  ```json
  {
    "package-name": {
      "source": "github.com/user/repo",
      "latest_version": "1.0.0",
      "description": "Description",
      "author": "Author Name"
    }
  }
  ```

### 4. Package Manifest Parser
- **File**: `package.mtr` (in each package)
- **Format**: MANTRA language syntax
- **Functions**: `load_package_manifest()`
- **Supported Fields**:
  - `package_name` - Package identifier
  - `version` - Semantic version
  - `author` - Author name
  - `description` - Package description
  - `dependencies` - Array of dependencies
  - `entry_point` - Main entry file

### 5. Installation Engine
- **Functions**: `install_github()`, `install_from_registry()`
- **Process**:
  1. Validate package specification
  2. Download from source (GitHub or registry)
  3. Extract archive
  4. Validate manifest
  5. Copy to library directory
  6. Register in installed packages

### 6. Logging System
- **Location**: `~/.mantra/ind.log`
- **Function**: `setup_logging()`
- **Levels**: DEBUG, INFO, WARNING, ERROR
- **Dual Output**: File and console

## Data Flow

### Installation Flow
```
User Input
    ↓
Parse Package Spec
    ↓
Determine Source (GitHub/Registry)
    ↓
Download Package
    ↓
Extract Archive
    ↓
Load Manifest
    ↓
Validate Structure
    ↓
Copy to Library
    ↓
Update Registry
    ↓
Log Success
```

### Search Flow
```
User Query
    ↓
Fetch Package Index (with cache)
    ↓
Filter by Query (name + description)
    ↓
Format Results
    ↓
Display to User
```

## Extensibility Points

### Adding New Source Types
To support new package sources (e.g., npm, pip):

1. Create new installation function:
   ```python
   def install_from_source_type(package_spec, version=None):
       # Implementation
   ```

2. Update `install_from_registry()` to recognize source type

3. Add tests in `test_ind.py`

### Adding Configuration Options
1. Extend `load_config()` and `save_config()`
2. Define defaults in `CONFIG_FILE`
3. Reference in relevant functions

### Adding New Commands
1. Define parser in `main()`:
   ```python
   cmd_parser = sub.add_parser("command", help="Description")
   cmd_parser.add_argument("arg", help="Argument")
   ```

2. Implement command function

3. Add to command dispatcher in main loop

## Error Handling Strategy

### Error Classes and Handling
- **Network Errors**: Log warning, fall back to cache
- **File System Errors**: Log error, show user message
- **Parse Errors**: Log warning, use defaults
- **Validation Errors**: Show clear guidance to user

### User-Friendly Messages
All errors include:
- Problem description
- Suggested solution
- Link to documentation

## Security Considerations

### Current Safeguards
1. **HTTPS Only**: All remote fetches use HTTPS
2. **Directory Isolation**: Packages isolated in library directory
3. **Manifest Validation**: Packages validated before installation
4. **Dependency Checking**: Prevents circular removal

### Future Improvements
1. Package signature verification
2. Checksum validation
3. Sandboxed execution
4. Permission auditing

## Performance Characteristics

### Time Complexity
- List packages: O(n) where n = installed packages
- Search: O(m) where m = registry size
- Install: O(1) after download
- Uninstall: O(n) for dependency check

### Space Complexity
- Registry: O(n) where n = installed packages
- Cache: O(m) where m = registry size

### Optimization Techniques
- Package index caching
- Lazy loading
- Batch registry updates

## Testing Strategy

### Unit Tests (test_ind.py)
- Registry operations
- Manifest parsing
- Configuration management
- Dependency detection
- Search functionality
- Logging

### Integration Tests (future)
- End-to-end installation
- GitHub API interactions
- Registry fetching
- File system operations

### Manual Testing
- CLI command execution
- Error handling
- Multi-platform compatibility

## Future Roadmap

### Version 1.1
- [ ] Dependency resolution
- [ ] Version constraint support
- [ ] Local package development mode

### Version 1.2
- [ ] Package publishing workflow
- [ ] Registry authentication
- [ ] Lock files for reproducible installs

### Version 1.3
- [ ] Binary package support
- [ ] Platform-specific packages
- [ ] Hook system for post-install scripts

### Version 2.0
- [ ] Web interface
- [ ] Package analytics
- [ ] Community ratings
- [ ] Automated security scanning
