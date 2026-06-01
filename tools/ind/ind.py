#!/usr/bin/env python3
"""
IND - Install Native Dependencies for MANTRA Language
Production-ready package manager with comprehensive features.
"""

import os
import sys
import json
import shutil
import tempfile
import urllib.request
import urllib.error
import zipfile
import argparse
import logging
import hashlib
from pathlib import Path
from typing import Dict, Optional, Tuple, List
from datetime import datetime

# ========== Configuration ==========
MANTRA_HOME = os.path.expanduser("~/.mantra")
LIB_DIR = os.path.join(MANTRA_HOME, "lib")
REGISTRY_FILE = os.path.join(MANTRA_HOME, "installed.json")
CONFIG_FILE = os.path.join(MANTRA_HOME, "config.json")
CACHE_DIR = os.path.join(MANTRA_HOME, "cache")
LOG_FILE = os.path.join(MANTRA_HOME, "ind.log")
DEFAULT_INDEX = "https://raw.githubusercontent.com/RT-Ecosystems/mantra-packages/main/index.json"
PACKAGE_INDEX_CACHE = os.path.join(CACHE_DIR, "package_index.json")

# ========== Logging Setup ==========
def setup_logging(verbose: bool = False) -> logging.Logger:
    """Configure logging for IND."""
    logger = logging.getLogger("ind")
    logger.setLevel(logging.DEBUG if verbose else logging.INFO)

    # File handler
    os.makedirs(MANTRA_HOME, exist_ok=True)
    fh = logging.FileHandler(LOG_FILE)
    fh.setLevel(logging.DEBUG)

    # Console handler
    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG if verbose else logging.INFO)

    formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
    fh.setFormatter(formatter)
    ch.setFormatter(formatter)

    logger.addHandler(fh)
    logger.addHandler(ch)
    return logger

logger = setup_logging()

# ========== Configuration Management ==========
def load_config() -> Dict:
    """Load IND configuration."""
    if not os.path.exists(CONFIG_FILE):
        return {}
    try:
        with open(CONFIG_FILE, "r") as f:
            return json.load(f)
    except Exception as e:
        logger.warning(f"Failed to load config: {e}")
        return {}

def save_config(config: Dict) -> None:
    """Save IND configuration."""
    try:
        os.makedirs(MANTRA_HOME, exist_ok=True)
        with open(CONFIG_FILE, "w") as f:
            json.dump(config, f, indent=2)
    except Exception as e:
        logger.error(f"Failed to save config: {e}")

# ========== Registry Management ==========
def ensure_dirs():
    """Ensure required directories exist."""
    os.makedirs(LIB_DIR, exist_ok=True)
    os.makedirs(CACHE_DIR, exist_ok=True)

def load_registry() -> Dict:
    """Load the registry of installed packages."""
    if not os.path.exists(REGISTRY_FILE):
        return {}
    try:
        with open(REGISTRY_FILE, "r") as f:
            return json.load(f)
    except Exception as e:
        logger.error(f"Failed to load registry: {e}")
        return {}

def save_registry(reg: Dict) -> None:
    """Save the registry of installed packages."""
    try:
        ensure_dirs()
        with open(REGISTRY_FILE, "w") as f:
            json.dump(reg, f, indent=2)
        logger.debug(f"Registry saved to {REGISTRY_FILE}")
    except Exception as e:
        logger.error(f"Failed to save registry: {e}")

def load_package_index() -> Dict:
    """Load cached package index from central registry."""
    if os.path.exists(PACKAGE_INDEX_CACHE):
        try:
            with open(PACKAGE_INDEX_CACHE, "r") as f:
                return json.load(f)
        except Exception as e:
            logger.warning(f"Failed to load cached index: {e}")
    return {}

def save_package_index(index: Dict) -> None:
    """Save package index to cache."""
    try:
        ensure_dirs()
        with open(PACKAGE_INDEX_CACHE, "w") as f:
            json.dump(index, f, indent=2)
    except Exception as e:
        logger.warning(f"Failed to save package index cache: {e}")

def fetch_package_index(refresh: bool = False) -> Optional[Dict]:
    """Fetch package index from central registry."""
    if not refresh:
        cached = load_package_index()
        if cached:
            return cached

    try:
        logger.info(f"Fetching package index from {DEFAULT_INDEX}...")
        with urllib.request.urlopen(DEFAULT_INDEX, timeout=10) as response:
            index = json.loads(response.read().decode('utf-8'))
            save_package_index(index)
            logger.info("Package index fetched successfully")
            return index
    except urllib.error.URLError as e:
        logger.warning(f"Failed to fetch package index: {e}")
        # Return cached version if available
        cached = load_package_index()
        return cached if cached else None
    except Exception as e:
        logger.error(f"Unexpected error fetching index: {e}")
        return None

# ========== Package Detection ==========
def load_package_manifest(package_dir: str) -> Optional[Dict]:
    """Load package.mtr manifest file."""
    manifest_path = os.path.join(package_dir, "package.mtr")
    if not os.path.exists(manifest_path):
        return None

    try:
        manifest = {}
        with open(manifest_path, "r") as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith("//"):
                    continue
                # Simple parser for rakho key = "value" format
                if "=" in line:
                    parts = line.split("=", 1)
                    if len(parts) == 2:
                        key = parts[0].replace("rakho", "").strip()
                        val = parts[1].strip().strip('"')
                        manifest[key] = val
        return manifest if manifest else None
    except Exception as e:
        logger.warning(f"Failed to parse manifest: {e}")
        return None

# ========== Installation Functions ==========
def install_github(repo_spec: str, branch: str = "main") -> bool:
    """Install a package from a GitHub repository.
    
    Args:
        repo_spec: GitHub repository in format user/repo or github.com/user/repo
        branch: Git branch to install from (default: main)
    
    Returns:
        True if successful, False otherwise
    """
    # Parse repo spec
    parts = repo_spec.replace("github.com/", "").split("/")
    if len(parts) < 2:
        logger.error("Invalid GitHub repository. Use format: user/repo or github.com/user/repo")
        return False

    user = parts[-2]
    repo_name = parts[-1]
    package_name = repo_name.lower().replace("-", "_")

    url = f"https://github.com/{user}/{repo_name}/archive/refs/heads/{branch}.zip"
    logger.info(f"📦 Installing from {user}/{repo_name}@{branch}...")

    try:
        with tempfile.TemporaryDirectory() as tmpdir:
            zip_path = os.path.join(tmpdir, "repo.zip")
            
            # Download with progress
            logger.debug(f"Downloading from {url}")
            try:
                urllib.request.urlretrieve(url, zip_path)
            except urllib.error.HTTPError as e:
                logger.error(f"Failed to download: HTTP {e.code} - {e.reason}")
                return False

            # Extract
            logger.debug("Extracting archive...")
            with zipfile.ZipFile(zip_path, "r") as zf:
                zf.extractall(tmpdir)

            # Find extracted folder
            extracted = os.path.join(tmpdir, f"{repo_name}-{branch}")
            if not os.path.exists(extracted):
                logger.error(f"Could not find package in extracted archive (expected: {repo_name}-{branch})")
                return False

            # Validate package structure (check for package.mtr or main files)
            manifest = load_package_manifest(extracted)
            
            # Install to library
            dest = os.path.join(LIB_DIR, package_name)
            if os.path.exists(dest):
                logger.warning(f"Package {package_name} already exists, overwriting...")
                shutil.rmtree(dest)
            
            logger.debug(f"Copying to {dest}")
            shutil.copytree(extracted, dest)

            # Record in registry
            reg = load_registry()
            reg[package_name] = {
                "name": manifest.get("package_name", package_name) if manifest else package_name,
                "version": manifest.get("version", branch) if manifest else branch,
                "source": f"github.com/{user}/{repo_name}",
                "branch": branch,
                "installed_at": datetime.now().isoformat(),
                "description": manifest.get("description", "") if manifest else ""
            }
            save_registry(reg)

            logger.info(f"✅ {package_name} installed successfully!")
            print(f"✅ {package_name} installed successfully!")
            return True

    except Exception as e:
        logger.error(f"Installation failed: {e}")
        print(f"❌ Installation failed: {e}")
        return False

def install_from_registry(package_name: str, version: Optional[str] = None) -> bool:
    """Install a package from central registry.
    
    Args:
        package_name: Name of the package
        version: Specific version to install (default: latest)
    
    Returns:
        True if successful, False otherwise
    """
    index = fetch_package_index()
    if not index:
        logger.error("Cannot access package registry. Please check your internet connection.")
        print("❌ Cannot access package registry. Please check your internet connection.")
        return False

    if package_name not in index:
        logger.error(f"Package '{package_name}' not found in registry")
        print(f"❌ Package '{package_name}' not found in registry")
        return False

    pkg_info = index[package_name]
    source = pkg_info.get("source")
    
    if not source:
        logger.error(f"Package '{package_name}' has no source information")
        return False

    if source.startswith("github.com/"):
        version_to_use = version or pkg_info.get("latest_version", "main")
        return install_github(source, branch=version_to_use)
    else:
        logger.error(f"Unsupported package source: {source}")
        return False

# ========== Package Information ==========
def show_package_info(package_name: str) -> None:
    """Display information about a package."""
    reg = load_registry()
    
    if package_name not in reg:
        print(f"❌ Package '{package_name}' is not installed.")
        return

    info = reg[package_name]
    print(f"\n📦 Package: {info.get('name', package_name)}")
    print(f"   Version: {info.get('version', 'unknown')}")
    print(f"   Source: {info.get('source', 'unknown')}")
    print(f"   Installed: {info.get('installed_at', 'unknown')}")
    if info.get('description'):
        print(f"   Description: {info['description']}")
    
    package_dir = os.path.join(LIB_DIR, package_name)
    if os.path.exists(package_dir):
        size = sum(f.stat().st_size for f in Path(package_dir).rglob('*') if f.is_file())
        print(f"   Size: {size / 1024 / 1024:.2f} MB")
        print(f"   Location: {package_dir}")
    print()

# ========== Search ==========
def search_packages(query: str) -> None:
    """Search for packages in the registry."""
    index = fetch_package_index()
    
    if not index:
        print("❌ Cannot access package registry.")
        return

    query = query.lower()
    results = []

    for pkg_name, pkg_info in index.items():
        if query in pkg_name.lower() or query in pkg_info.get("description", "").lower():
            results.append((pkg_name, pkg_info))

    if not results:
        print(f"❌ No packages found matching '{query}'")
        return

    print(f"\n🔍 Found {len(results)} package(s) matching '{query}':\n")
    for pkg_name, pkg_info in results:
        version = pkg_info.get("latest_version", "main")
        description = pkg_info.get("description", "No description")
        print(f"  {pkg_name} ({version})")
        print(f"    {description}")
        print()

# ========== Update ==========
def update_package(package_name: str) -> bool:
    """Update an installed package."""
    reg = load_registry()

    if package_name not in reg:
        logger.error(f"Package '{package_name}' is not installed")
        print(f"❌ Package '{package_name}' is not installed")
        return False

    pkg_info = reg[package_name]
    source = pkg_info.get("source")

    if not source:
        logger.error(f"Cannot determine source for package '{package_name}'")
        return False

    print(f"🔄 Updating {package_name}...")

    if source.startswith("github.com/"):
        branch = pkg_info.get("branch", "main")
        # Uninstall first
        package_dir = os.path.join(LIB_DIR, package_name)
        if os.path.exists(package_dir):
            shutil.rmtree(package_dir)
        del reg[package_name]
        save_registry(reg)
        # Reinstall
        return install_github(source, branch=branch)

    logger.error(f"Cannot update package from source: {source}")
    return False

# ========== Uninstall ==========
def uninstall_package(package_name: str, force: bool = False) -> bool:
    """Uninstall a package."""
    reg = load_registry()

    if package_name not in reg:
        print(f"❌ Package '{package_name}' is not installed.")
        return False

    # Check for dependencies (if any other packages depend on this)
    dependents = []
    for pkg, pkg_info in reg.items():
        if pkg == package_name:
            continue
        deps = pkg_info.get("dependencies", [])
        if package_name in deps:
            dependents.append(pkg)

    if dependents and not force:
        print(f"⚠️  The following packages depend on '{package_name}':")
        for dep in dependents:
            print(f"   - {dep}")
        print(f"   Use --force to uninstall anyway")
        return False

    package_dir = os.path.join(LIB_DIR, package_name)
    if os.path.exists(package_dir):
        logger.info(f"Removing package directory: {package_dir}")
        shutil.rmtree(package_dir)

    del reg[package_name]
    save_registry(reg)

    print(f"🗑️  {package_name} uninstalled.")
    logger.info(f"Package '{package_name}' uninstalled")
    return True

# ========== List Packages ==========
def list_packages(verbose: bool = False) -> None:
    """List installed packages."""
    reg = load_registry()

    if not reg:
        print("📭 No packages installed.")
        return

    print("\n📋 Installed packages:\n")
    for name, info in sorted(reg.items()):
        if verbose:
            print(f"  {name}")
            print(f"    Version: {info.get('version', 'unknown')}")
            print(f"    Source: {info.get('source', 'unknown')}")
            print(f"    Installed: {info.get('installed_at', 'unknown')}")
            if info.get('description'):
                print(f"    {info['description']}")
        else:
            version = info.get('version', 'unknown')
            source = info.get('source', 'unknown')
            print(f"  {name:20} {version:15} ({source})")
    print()

# ========== Init Package ==========
def init_package(package_name: str) -> None:
    """Initialize a new package in current directory."""
    if os.path.exists("package.mtr"):
        print("❌ package.mtr already exists in current directory")
        return

    template = f"""// package.mtr — MANTRA package manifest
rakho package_name = "{package_name}"
rakho version = "1.0.0"
rakho author = "Your Name"
rakho description = "A short description of your package"
// rakho dependencies = ["package1 >= 1.0.0", "package2"]  // optional
// rakho entry_point = "main.mtr"  // optional, default = package_name.mtr
"""

    try:
        with open("package.mtr", "w") as f:
            f.write(template)
        print(f"✅ Initialized new package: {package_name}")
        print("   Created package.mtr")
    except Exception as e:
        logger.error(f"Failed to create package.mtr: {e}")
        print(f"❌ Failed to create package.mtr: {e}")

# ========== Clean Cache ==========
def clean_cache() -> None:
    """Clean the package index cache."""
    if os.path.exists(PACKAGE_INDEX_CACHE):
        try:
            os.remove(PACKAGE_INDEX_CACHE)
            print("✅ Cache cleaned")
            logger.info("Package index cache cleaned")
        except Exception as e:
            logger.error(f"Failed to clean cache: {e}")
            print(f"❌ Failed to clean cache: {e}")
    else:
        print("📭 Cache is already empty")

# ========== CLI ==========
def main():
    """Main entry point for IND CLI."""
    parser = argparse.ArgumentParser(
        prog="ind",
        description="IND - Package Manager for MANTRA Language",
        epilog="Examples:\n"
               "  ind install user/repo              # Install from GitHub\n"
               "  ind install package_name           # Install from registry\n"
               "  ind search math                    # Search for packages\n"
               "  ind list -v                        # List packages verbosely\n"
               "  ind show package_name              # Show package info\n"
               "  ind update package_name            # Update a package\n"
               "  ind init my-package                # Initialize new package\n",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )

    parser.add_argument("-v", "--verbose", action="store_true", help="Verbose output")
    sub = parser.add_subparsers(dest="command", help="Command to execute")

    # list command
    list_parser = sub.add_parser("list", help="List installed packages")
    list_parser.add_argument("-v", "--verbose", action="store_true", help="Show detailed info")

    # install command
    install_parser = sub.add_parser("install", help="Install a package")
    install_parser.add_argument("package", help="Package name or github.com/user/repo")
    install_parser.add_argument("-b", "--branch", default="main", help="Git branch to install (default: main)")

    # uninstall command
    uninstall_parser = sub.add_parser("uninstall", help="Uninstall a package")
    uninstall_parser.add_argument("package", help="Package name")
    uninstall_parser.add_argument("-f", "--force", action="store_true", help="Force removal even if other packages depend on it")

    # search command
    search_parser = sub.add_parser("search", help="Search for packages in registry")
    search_parser.add_argument("query", help="Search query")
    search_parser.add_argument("-r", "--refresh", action="store_true", help="Refresh package index")

    # show command
    show_parser = sub.add_parser("show", help="Show package information")
    show_parser.add_argument("package", help="Package name")

    # update command
    update_parser = sub.add_parser("update", help="Update an installed package")
    update_parser.add_argument("package", help="Package name")

    # init command
    init_parser = sub.add_parser("init", help="Initialize a new package")
    init_parser.add_argument("package", help="Package name")

    # clean command
    clean_parser = sub.add_parser("clean", help="Clean package cache")

    args = parser.parse_args()

    ensure_dirs()

    if not args.command:
        parser.print_help()
        return

    try:
        if args.command == "list":
            list_packages(verbose=args.verbose)
        elif args.command == "install":
            if "github.com/" in args.package or "/" in args.package:
                install_github(args.package, branch=args.branch)
            else:
                install_from_registry(args.package)
        elif args.command == "uninstall":
            uninstall_package(args.package, force=args.force)
        elif args.command == "search":
            search_packages(args.query)
        elif args.command == "show":
            show_package_info(args.package)
        elif args.command == "update":
            update_package(args.package)
        elif args.command == "init":
            init_package(args.package)
        elif args.command == "clean":
            clean_cache()
        else:
            parser.print_help()
    except KeyboardInterrupt:
        print("\n❌ Operation cancelled by user")
        sys.exit(1)
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        print(f"❌ Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
