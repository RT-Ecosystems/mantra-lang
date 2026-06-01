# IND - Package Manager for MANTRA Language

Production-ready package manager for the MANTRA programming language. IND provides seamless package installation, management, and discovery capabilities.

## Features

### Core Commands
- **install** - Install packages from GitHub or central registry
- **list** - List all installed packages with detailed information
- **uninstall** - Remove installed packages with dependency checking
- **search** - Search for packages in the central registry
- **show** - Display detailed information about installed packages
- **update** - Update installed packages to latest versions
- **init** - Initialize new MANTRA packages with manifest
- **clean** - Clean the package index cache

### Advanced Features
- **Registry Support** - Connects to central package registry for discovery
- **Dependency Tracking** - Tracks package dependencies and prevents circular removals
- **Configuration Management** - Persistent configuration storage
- **Logging System** - Comprehensive logging to `~/.mantra/ind.log`
- **Caching** - Intelligent caching of package indexes
- **Error Handling** - Robust error handling and user-friendly messages
- **Multiple Sources** - Support for GitHub and registry sources

## Installation

1. Ensure Python 3.7+ is installed
2. Copy `ind.py` to your PATH or create a symlink:

```bash
chmod +x ind.py
sudo cp ind.py /usr/local/bin/ind
# or
sudo ln -s $(pwd)/ind.py /usr/local/bin/ind
```

## Usage

### Install Packages

From GitHub:
```bash
ind install user/repo
ind install github.com/user/repo
ind install user/repo -b develop  # Install from specific branch
```

From central registry:
```bash
ind install package-name
```

### List Installed Packages

```bash
ind list              # Simple list
ind list -v           # Verbose with details
```

### Search for Packages

```bash
ind search math
ind search "data processing"
ind search ml -r      # With registry refresh
```

### View Package Details

```bash
ind show package-name
```

### Update Packages

```bash
ind update package-name
ind update package1 package2  # Multiple packages
```

### Uninstall Packages

```bash
ind uninstall package-name
ind uninstall package-name -f  # Force removal (ignore dependencies)
```

### Initialize New Package

```bash
ind init my-package
# Creates package.mtr with template manifest
```

### Clean Cache

```bash
ind clean  # Clean package index cache
```

## Package Structure

A valid MANTRA package should have a `package.mtr` manifest file:

```
// package.mtr — MANTRA package manifest
rakho package_name = "my-package"
rakho version = "1.0.0"
rakho author = "Your Name"
rakho description = "Package description"
rakho dependencies = ["other-package >= 1.0.0"]  // optional
rakho entry_point = "main.mtr"                    // optional
```

## Configuration

IND stores configuration in `~/.mantra/config.json`. Default settings:

```json
{
  "registry_url": "https://raw.githubusercontent.com/RT-Ecosystems/mantra-packages/main/index.json",
  "cache_ttl": 3600,
  "auto_update": false
}
```

## Directories

- `~/.mantra/lib` - Installed packages directory
- `~/.mantra/cache` - Package index and temporary files
- `~/.mantra/installed.json` - Registry of installed packages
- `~/.mantra/ind.log` - Activity logs

## Package Registry

The central package registry is maintained at:
`https://github.com/RT-Ecosystems/mantra-packages`

Registry index format:
```json
{
  "package-name": {
    "source": "github.com/user/repo",
    "latest_version": "1.0.0",
    "description": "Package description",
    "author": "Author Name"
  }
}
```

## Error Handling

IND provides clear error messages for common issues:

- **Network Issues** - Automatic fallback to cached registry
- **Invalid Repository** - Format guidance provided
- **Dependency Conflicts** - Clear warning with dependency list
- **Installation Failures** - Detailed logging for troubleshooting

## Logging

Detailed logs are written to `~/.mantra/ind.log`. Enable verbose output:

```bash
ind -v install package-name
```

## Development

### Testing

```bash
python3 -m pytest tests/
```

### Code Style

Follow PEP 8 conventions. Format with:

```bash
black ind.py
```

## License

Same as MANTRA Language

## Support

For issues and feature requests, visit:
https://github.com/RT-Ecosystems/mantra-lang

## Changelog

### v1.0.0 (Initial Release)
- Core install/uninstall/list functionality
- Registry search and discovery
- Package initialization
- Configuration management
- Comprehensive logging
- Dependency tracking
- Cache management
