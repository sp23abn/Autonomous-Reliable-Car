#!/bin/bash

# Remote Deployment Script for Autonomous-Reliable-Car
# Deploys code to Raspberry Pi and runs it from GitHub Codespaces

set -e

# ============ CONFIGURATION ============
RASPI_IP="${1:-192.168.1.50}"      # Default: 192.168.1.50, override with arg 1
RASPI_USER="${2:-e300user}"        # Default: e300user, override with arg 2
RASPI_HOME="/home/$RASPI_USER"

echo "🤖 Autonomous Car - Remote Deployment Script"
echo "============================================"
echo "Target Raspberry Pi: $RASPI_USER@$RASPI_IP"
echo ""

# ============ STEP 1: SETUP ============
echo "📦 Step 1: Installing dependencies on Raspberry Pi..."
ssh -t $RASPI_USER@$RASPI_IP << 'SETUP'
echo "Installing build tools, git, ncurses, wiringpi, pkgconfig..."
sudo apt-get update
sudo apt-get install -y build-essential git libncurses-dev libwiringpi-dev wiringpi pkgconfig
echo "✅ Dependencies installed!"
SETUP

# ============ STEP 2: INSTALL INITIO LIBRARY ============
echo ""
echo "📚 Step 2: Installing initio library..."
ssh -t $RASPI_USER@$RASPI_IP << 'INITIO'
if [ -d /tmp/initio_lib ]; then
    echo "initio_lib already exists, skipping clone..."
else
    cd /tmp
    git clone https://github.com/rkirner/initio_lib.git
fi
cd /tmp/initio_lib
make clean && make compile && make link
sudo make install
sudo ldconfig
echo "✅ initio library installed!"
INITIO

# ============ STEP 3: COPY CODE ============
echo ""
echo "📤 Step 3: Copying code to Raspberry Pi..."
scp -r /workspaces/Autonomous-Reliable-Car/line_follower $RASPI_USER@$RASPI_IP:~/
echo "✅ Code copied!"

# ============ STEP 4: COMPILE ============
echo ""
echo "🔨 Step 4: Compiling on Raspberry Pi..."
ssh -t $RASPI_USER@$RASPI_IP << 'COMPILE'
cd ~/line_follower
make clean
make
echo "✅ Compilation successful!"
COMPILE

# ============ STEP 5: RUN ============
echo ""
echo "🚀 Step 5: Starting program..."
echo ""
echo "Program is running on the Raspberry Pi!"
echo "Press Ctrl+C to stop"
echo ""

ssh -t $RASPI_USER@$RASPI_IP << 'RUN'
cd ~/line_follower
sudo ./line_follower
RUN

echo ""
echo "✅ Program finished!"
