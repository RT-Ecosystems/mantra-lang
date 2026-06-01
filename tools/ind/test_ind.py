#!/usr/bin/env python3
"""
Test suite for IND package manager.
"""

import unittest
import os
import sys
import json
import tempfile
import shutil
from pathlib import Path
from unittest.mock import patch, MagicMock

# Add parent directory to path for imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import ind

class TestINDRegistry(unittest.TestCase):
    """Test registry management functions."""

    def setUp(self):
        """Set up test fixtures."""
        self.temp_dir = tempfile.mkdtemp()
        self.original_mantra_home = ind.MANTRA_HOME
        ind.MANTRA_HOME = self.temp_dir
        ind.LIB_DIR = os.path.join(self.temp_dir, "lib")
        ind.REGISTRY_FILE = os.path.join(self.temp_dir, "installed.json")
        ind.CACHE_DIR = os.path.join(self.temp_dir, "cache")

    def tearDown(self):
        """Clean up test fixtures."""
        ind.MANTRA_HOME = self.original_mantra_home
        shutil.rmtree(self.temp_dir, ignore_errors=True)

    def test_ensure_dirs(self):
        """Test directory creation."""
        ind.ensure_dirs()
        self.assertTrue(os.path.exists(ind.LIB_DIR))
        self.assertTrue(os.path.exists(ind.CACHE_DIR))

    def test_load_empty_registry(self):
        """Test loading non-existent registry."""
        reg = ind.load_registry()
        self.assertEqual(reg, {})

    def test_save_and_load_registry(self):
        """Test saving and loading registry."""
        test_data = {
            "package1": {"version": "1.0.0", "source": "github.com/user/repo"},
            "package2": {"version": "2.0.0", "source": "registry"}
        }
        ind.save_registry(test_data)
        loaded = ind.load_registry()
        self.assertEqual(loaded, test_data)

    def test_registry_persistence(self):
        """Test that registry persists between sessions."""
        data1 = {"pkg1": {"version": "1.0"}}
        ind.save_registry(data1)

        # Simulate new session
        loaded = ind.load_registry()
        self.assertIn("pkg1", loaded)
        self.assertEqual(loaded["pkg1"]["version"], "1.0")


class TestINDManifest(unittest.TestCase):
    """Test package manifest parsing."""

    def setUp(self):
        """Set up test fixtures."""
        self.temp_dir = tempfile.mkdtemp()

    def tearDown(self):
        """Clean up test fixtures."""
        shutil.rmtree(self.temp_dir, ignore_errors=True)

    def test_load_valid_manifest(self):
        """Test loading valid package manifest."""
        manifest_content = """// package.mtr
rakho package_name = "test-pkg"
rakho version = "1.0.0"
rakho author = "Test Author"
rakho description = "Test package"
"""
        manifest_path = os.path.join(self.temp_dir, "package.mtr")
        with open(manifest_path, "w") as f:
            f.write(manifest_content)

        manifest = ind.load_package_manifest(self.temp_dir)
        self.assertIsNotNone(manifest)
        self.assertEqual(manifest.get("package_name"), "test-pkg")
        self.assertEqual(manifest.get("version"), "1.0.0")

    def test_load_missing_manifest(self):
        """Test loading missing manifest."""
        manifest = ind.load_package_manifest(self.temp_dir)
        self.assertIsNone(manifest)

    def test_parse_manifest_with_comments(self):
        """Test manifest parsing ignores comments."""
        manifest_content = """// This is a comment
rakho package_name = "test"
// Another comment
rakho version = "1.0"
"""
        manifest_path = os.path.join(self.temp_dir, "package.mtr")
        with open(manifest_path, "w") as f:
            f.write(manifest_content)

        manifest = ind.load_package_manifest(self.temp_dir)
        self.assertEqual(manifest.get("package_name"), "test")
        self.assertEqual(manifest.get("version"), "1.0")


class TestINDConfiguration(unittest.TestCase):
    """Test configuration management."""

    def setUp(self):
        """Set up test fixtures."""
        self.temp_dir = tempfile.mkdtemp()
        self.original_config_file = ind.CONFIG_FILE
        ind.CONFIG_FILE = os.path.join(self.temp_dir, "config.json")
        ind.MANTRA_HOME = self.temp_dir

    def tearDown(self):
        """Clean up test fixtures."""
        ind.CONFIG_FILE = self.original_config_file
        shutil.rmtree(self.temp_dir, ignore_errors=True)

    def test_load_empty_config(self):
        """Test loading non-existent config."""
        config = ind.load_config()
        self.assertEqual(config, {})

    def test_save_and_load_config(self):
        """Test saving and loading configuration."""
        test_config = {
            "registry_url": "https://example.com/index.json",
            "cache_ttl": 7200
        }
        ind.save_config(test_config)
        loaded = ind.load_config()
        self.assertEqual(loaded, test_config)


class TestINDPackageName(unittest.TestCase):
    """Test package name handling."""

    def test_github_package_name_extraction(self):
        """Test extracting package name from GitHub URL."""
        # This tests the logic in install_github
        repo_spec = "user/my-repo"
        parts = repo_spec.replace("github.com/", "").split("/")
        package_name = parts[-1].lower().replace("-", "_")
        self.assertEqual(package_name, "my_repo")

    def test_github_url_format(self):
        """Test GitHub URL format parsing."""
        repo_spec = "github.com/user/repo"
        parts = repo_spec.replace("github.com/", "").split("/")
        self.assertEqual(len(parts), 2)
        self.assertEqual(parts[0], "user")
        self.assertEqual(parts[1], "repo")


class TestINDSearch(unittest.TestCase):
    """Test package search functionality."""

    def test_search_query_formatting(self):
        """Test search query case insensitivity."""
        query = "MATH"
        package_names = ["math-utils", "advanced_math", "matrix"]
        
        matches = [name for name in package_names if query.lower() in name.lower()]
        self.assertIn("math-utils", matches)
        self.assertIn("advanced_math", matches)


class TestINDDependencies(unittest.TestCase):
    """Test dependency management."""

    def setUp(self):
        """Set up test fixtures."""
        self.temp_dir = tempfile.mkdtemp()
        self.original_mantra_home = ind.MANTRA_HOME
        ind.MANTRA_HOME = self.temp_dir
        ind.LIB_DIR = os.path.join(self.temp_dir, "lib")
        ind.REGISTRY_FILE = os.path.join(self.temp_dir, "installed.json")

    def tearDown(self):
        """Clean up test fixtures."""
        ind.MANTRA_HOME = self.original_mantra_home
        shutil.rmtree(self.temp_dir, ignore_errors=True)

    def test_detect_dependent_packages(self):
        """Test detecting packages that depend on others."""
        registry = {
            "pkg1": {"dependencies": ["pkg2"]},
            "pkg2": {"dependencies": []},
            "pkg3": {"dependencies": ["pkg2"]}
        }
        
        # Find packages that depend on pkg2
        dependents = []
        for pkg, pkg_info in registry.items():
            if pkg == "pkg2":
                continue
            deps = pkg_info.get("dependencies", [])
            if "pkg2" in deps:
                dependents.append(pkg)
        
        self.assertIn("pkg1", dependents)
        self.assertIn("pkg3", dependents)
        self.assertNotIn("pkg2", dependents)


class TestINDLogging(unittest.TestCase):
    """Test logging functionality."""

    def setUp(self):
        """Set up test fixtures."""
        self.temp_dir = tempfile.mkdtemp()
        self.original_log_file = ind.LOG_FILE
        ind.LOG_FILE = os.path.join(self.temp_dir, "test.log")
        ind.MANTRA_HOME = self.temp_dir

    def tearDown(self):
        """Clean up test fixtures."""
        ind.LOG_FILE = self.original_log_file
        shutil.rmtree(self.temp_dir, ignore_errors=True)

    def test_logging_setup(self):
        """Test logging initialization."""
        logger = ind.setup_logging()
        self.assertIsNotNone(logger)
        self.assertEqual(logger.name, "ind")


if __name__ == "__main__":
    unittest.main()
