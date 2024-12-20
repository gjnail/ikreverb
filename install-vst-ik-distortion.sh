#!/bin/bash

# Remove existing installations
rm -rf ~/Library/Audio/Plug-Ins/VST3/IKReverb.vst3
rm -rf ~/Library/Audio/Plug-Ins/Components/IKReverb.component

# Create directories if they don't exist
mkdir -p ~/Library/Audio/Plug-Ins/VST3
mkdir -p ~/Library/Audio/Plug-Ins/Components

# Copy new builds
cp -r Builds/MacOSX/build/Debug/IKReverb.vst3 ~/Library/Audio/Plug-Ins/VST3/
cp -r Builds/MacOSX/build/Debug/IKReverb.component ~/Library/Audio/Plug-Ins/Components/

echo "Installation complete!" 