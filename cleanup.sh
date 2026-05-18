#!/usr/bin/env bash
if [ -d build ]; then
    git rm -r --cached build/
fi
git rm -r --cached "*.ts" 2>/dev/null
