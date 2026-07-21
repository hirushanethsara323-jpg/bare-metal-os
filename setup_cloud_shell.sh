#!/bin/bash
# =============================================================================
# Nothing OS - Google Cloud Shell Setup & Automated Continuous Runner
# =============================================================================
# Managed by CEO & Executive AI Engineering Board
# Repo: https://github.com/hirushanethsara323-jpg/bare-metal-os
# =============================================================================

set -e

echo "=========================================================================="
echo "🚀 Nothing OS - Google Cloud Shell Automated Deployment & Sync Script"
echo "=========================================================================="

# 1. Update and install required build tools
echo "📦 Installing bare-metal toolchain dependencies (gcc, nasm, qemu, make)..."
sudo apt-get update -y
sudo apt-get install -y gcc gcc-multilib make nasm qemu-system-x86 python3 git xorriso grub-pc-bin || true

# 2. Set Git configuration & Remote PAT
echo "🔐 Configuring Git credentials..."
git config --global user.name "hirushanethsara323-jpg"
git config --global user.email "hirushanethsara323@gmail.com"

# Use GITHUB_TOKEN environment variable if set
if [ -n "$GITHUB_TOKEN" ]; then
    REPO_URL="https://${GITHUB_TOKEN}@github.com/hirushanethsara323-jpg/bare-metal-os.git"
    git remote set-url origin "$REPO_URL" || git remote add origin "$REPO_URL"
fi

# Pull latest state
echo "🔄 Syncing latest code from GitHub..."
git pull --rebase origin main || true

# 3. Clean and build Nothing OS kernel
echo "🛠️ Compiling Nothing OS Kernel v10.0 Apex Platform..."
make clean
make

# 4. Verify Executive AI Board and QA Test Suite
echo "🏢 Initializing Executive AI Board..."
python3 agents/nothing_crew.py

# 5. Start Background Continuous Auto-Builder Daemon
echo "🔄 Launching Continuous Auto-Builder Daemon in background..."
python3 agents/auto_builder.py --daemon &

echo ""
echo "=========================================================================="
echo "🎉 Nothing OS is 100% Configured and Running Continuously on Google Cloud Shell!"
echo "=========================================================================="
echo "To run Nothing OS in QEMU emulator (Command Line / Serial Mode):"
echo "   make run"
echo ""
echo "To check background Auto-Builder CI/CD Daemon status:"
echo "   python3 agents/auto_builder.py --status"
echo "=========================================================================="
