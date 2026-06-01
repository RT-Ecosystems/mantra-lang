# IND Installation and Setup Guide

## Quick Start

### Prerequisites
- Python 3.7 or higher
- Git (for cloning packages from GitHub)
- Internet connection (for registry access)

### Installation

#### Option 1: Direct Installation (Recommended)

```bash
# Make executable
chmod +x ind.py

# Create symlink in PATH
sudo ln -s "$(pwd)/ind.py" /usr/local/bin/ind

# Verify installation
ind --help
```

#### Option 2: Add to PATH

```bash
# Add script directory to ~/.bashrc or ~/.zshrc
echo 'export PATH="$PATH:$(pwd)"' >> ~/.bashrc
source ~/.bashrc

# Or create alias
alias ind="python3 $(pwd)/ind.py"
```

#### Option 3: Virtual Environment (Development)

```bash
# Create virtual environment
python3 -m venv venv
source venv/bin/activate

# Create symlink
ln -s "$(pwd)/ind.py" venv/bin/ind
```

## First-Time Setup

### Initial Configuration

```bash
# Check installation
ind --help

# List installed packages (should be empty)
ind list

# Create initial directories
ind clean  # Creates ~/.mantra/cache
```

### Directory Structure

```
~/.mantra/
├── lib/                    # Installed packages
├── cache/                  # Temporary files and index cache
├── installed.json          # Package registry
├── config.json            # User configuration
└── ind.log                # Activity logs
```

## Usage Examples

### Basic Operations

```bash
# Install from GitHub
ind install user/repo
ind install RT-Ecosystems/mantra-ai

# Install from registry
ind install math-utils

# List packages
ind list

# Show details
ind show math-utils

# Uninstall
ind uninstall math-utils
```

### Advanced Operations

```bash
# Install specific branch
ind install user/repo -b develop

# Search registry
ind search neural

# Update package
ind update math-utils

# Force uninstall (bypass dependencies)
ind uninstall pkg --force

# Initialize new package
ind init my-package

# Clean cache
ind clean
```

## Configuration

### Custom Configuration

Edit `~/.mantra/config.json`:

```json
{
  "registry_url": "https://example.com/packages",
  "cache_ttl": 3600,
  "auto_update": false,
  "verbose": false
}
```

### Environment Variables

```bash
# Override home directory
export MANTRA_HOME=/custom/path

# Run with specific branch for all installs
export IND_DEFAULT_BRANCH=develop
```

## Troubleshooting

### Common Issues

#### 1. "Command not found: ind"

**Solution**: Ensure the script is in PATH or use full path:
```bash
/path/to/ind.py list
```

#### 2. "Permission denied"

**Solution**: Make script executable:
```bash
chmod +x ind.py
```

#### 3. "Cannot access package registry"

**Solution**: Check internet connection and cache:
```bash
# Clear cache and retry
ind clean
ind search package-name
```

#### 4. "Package already exists"

**Solution**: Uninstall first or use force update:
```bash
ind uninstall package-name
ind install package-name
```

#### 5. "HTTP 404" error

**Possible causes**:
- Repository doesn't exist
- Wrong branch name
- GitHub is unreachable

**Solution**:
```bash
# Verify repository URL
# Try with explicit branch
ind install user/repo -b main
```

### Debugging

Enable verbose logging:

```bash
# Verbose output
ind -v install package

# Check log file
tail -f ~/.mantra/ind.log

# Test connectivity
python3 -c "import urllib.request; urllib.request.urlopen('https://github.com')"
```

## Package Development

### Creating a New Package

```bash
# Create directory
mkdir my-package
cd my-package

# Initialize package.mtr
ind init my-package

# Add your code
# ... create your Mantra code ...

# Push to GitHub
git init
git add .
git commit -m "Initial commit"
git remote add origin https://github.com/user/my-package
git push -u origin main
```

### Package Manifest (package.mtr)

Required fields:
```
rakho package_name = "my-package"
rakho version = "1.0.0"
```

Optional fields:
```
rakho author = "Your Name"
rakho description = "Package description"
rakho dependencies = ["other-package >= 1.0.0"]
rakho entry_point = "main.mtr"
```

### Publishing to Registry

1. Create package on GitHub
2. Add to registry index: https://github.com/RT-Ecosystems/mantra-packages
3. Submit pull request with entry in index.json

## Integration with MANTRA

### Using Installed Packages

In your MANTRA code:

```mantra
// Import from installed package
rakho package = require("package_name")

// Use package functions
rakho result = package.function()
```

### Package Path

Packages are installed to:
```
~/.mantra/lib/package_name/
```

Set MANTRA_PATH to include this:
```bash
export MANTRA_PATH="$HOME/.mantra/lib:$MANTRA_PATH"
```

## Uninstallation

### Remove IND

```bash
# Remove symlink
sudo rm /usr/local/bin/ind

# Or if in PATH
# Edit ~/.bashrc to remove PATH entry
```

### Keep Installed Packages

```bash
# Packages remain in ~/.mantra/lib
# They can still be used manually

# To remove all packages
rm -rf ~/.mantra/lib/*
```

### Clean All IND Data

```bash
# Remove entire IND directory
rm -rf ~/.mantra

# Reinstall if needed
ind clean
```

## Upgrading IND

### Update ind.py

```bash
# From repository
cd tools/ind
git pull origin main

# Make executable
chmod +x ind.py

# Verify
ind --version
```

## Support and Help

### Getting Help

```bash
# General help
ind --help

# Command-specific help
ind install --help
ind search --help
```

### Reporting Issues

1. Check logs: `tail ~/.mantra/ind.log`
2. Enable verbose mode: `ind -v <command>`
3. Open issue on GitHub: https://github.com/RT-Ecosystems/mantra-lang/issues

### Documentation

- Architecture: `ARCHITECTURE.md`
- README: `README.md`
- Source code: `ind.py`

## Performance Tips

### Speed Up Package Installation

```bash
# Use cache for registry
ind search package  # Caches registry

# Install multiple packages
ind install user/repo1
ind install user/repo2  # Uses cached registry
```

### Reduce Disk Usage

```bash
# Clean cache periodically
ind clean

# Remove unused packages
ind uninstall old-package
```

## Security Recommendations

1. **Keep ind updated**: Get latest bug fixes and security patches
2. **Verify sources**: Only install from trusted repositories
3. **Check logs**: Monitor `~/.mantra/ind.log` for suspicious activity
4. **Use HTTPS**: Ensure all connections are encrypted
5. **Secure manifests**: Include checksums in future versions

## Next Steps

1. Install IND using one of the options above
2. Run `ind --help` to see available commands
3. Install your first package: `ind install user/repo`
4. Check your package: `ind list`
5. Read `ARCHITECTURE.md` for internals
6. Create your own package following the guide above
