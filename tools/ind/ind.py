#!/usr/bin/env python3
"""IND - Install Native Dependencies for MANTRA Language"""

import os
import sys
import json
import shutil
import tempfile
import urllib.request
import zipfile
import argparse

# ========== Configuration ==========
MANTRA_HOME = os.path.expanduser("~/.mantra")
LIB_DIR = os.path.join(MANTRA_HOME, "lib")
REGISTRY_FILE = os.path.join(MANTRA_HOME, "installed.json")
DEFAULT_INDEX = "https://raw.githubusercontent.com/RT-Ecosystems/mantra-packages/main/index.json"

# ========== Helpers ==========
def ensure_dirs():
    os.makedirs(LIB_DIR, exist_ok=True)

def load_registry():
    if not os.path.exists(REGISTRY_FILE):
        return {}
    with open(REGISTRY_FILE, "r") as f:
        return json.load(f)

def save_registry(reg):
    with open(REGISTRY_FILE, "w") as f:
        json.dump(reg, f, indent=2)

# ========== Install from GitHub ==========
def install_github(repo):
    """Install a package from a GitHub repository.
    Usage: ind install github.com/user/repo
    """
    parts = repo.split("/")
    if len(parts) < 2:
        print("❌ Invalid GitHub repository. Use format: github.com/user/repo")
        return

    user = parts[-2]
    repo_name = parts[-1]
    package_name = repo_name.lower()
    url = f"https://github.com/{user}/{repo_name}/archive/refs/heads/main.zip"

    print(f"📦 Installing {user}/{repo_name}...")

    try:
        with tempfile.TemporaryDirectory() as tmpdir:
            zip_path = os.path.join(tmpdir, "repo.zip")
            urllib.request.urlretrieve(url, zip_path)

            with zipfile.ZipFile(zip_path, "r") as zf:
                zf.extractall(tmpdir)

            # Find extracted folder (it's usually repo-main)
            extracted = os.path.join(tmpdir, f"{repo_name}-main")
            if not os.path.exists(extracted):
                print("❌ Could not find package in extracted archive.")
                return

            dest = os.path.join(LIB_DIR, package_name)
            if os.path.exists(dest):
                shutil.rmtree(dest)
            shutil.copytree(extracted, dest)

        reg = load_registry()
        reg[package_name] = {
            "version": "main",
            "source": f"github.com/{user}/{repo_name}"
        }
        save_registry(reg)
        print(f"✅ {package_name} installed successfully!")

    except Exception as e:
        print(f"❌ Installation failed: {e}")

# ========== List installed ==========
def list_packages():
    reg = load_registry()
    if not reg:
        print("📭 No packages installed.")
        return
    print("📋 Installed packages:")
    for name, info in reg.items():
        print(f"  - {name} ({info.get('source', 'unknown')})")

# ========== Uninstall ==========
def uninstall(package_name):
    reg = load_registry()
    if package_name not in reg:
        print(f"❌ {package_name} is not installed.")
        return
    dest = os.path.join(LIB_DIR, package_name)
    if os.path.exists(dest):
        shutil.rmtree(dest)
    del reg[package_name]
    save_registry(reg)
    print(f"🗑️ {package_name} uninstalled.")

# ========== CLI ==========
def main():
    parser = argparse.ArgumentParser(prog="ind", description="IND Package Manager for MANTRA")
    sub = parser.add_subparsers(dest="command")

    sub.add_parser("list", help="List installed packages")

    install_parser = sub.add_parser("install", help="Install a package")
    install_parser.add_argument("package", help="Package name or github.com/user/repo")

    uninstall_parser = sub.add_parser("uninstall", help="Uninstall a package")
    uninstall_parser.add_argument("package", help="Package name")

    args = parser.parse_args()

    if args.command == "list":
        list_packages()
    elif args.command == "install":
        if "github.com/" in args.package or "/" in args.package:
            install_github(args.package)
        else:
            print("🔍 Searching central registry (not yet implemented).")
            print("   For now, use: ind install github.com/user/repo")
    elif args.command == "uninstall":
        uninstall(args.package)
    else:
        parser.print_help()

if __name__ == "__main__":
    main()
