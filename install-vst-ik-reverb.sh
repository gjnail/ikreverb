#!/bin/bash

# Remove old plugins
rm -rf ~/Library/Audio/Plug-Ins/VST3/IKReverb.vst3
rm -rf ~/Library/Audio/Plug-Ins/Components/IKReverb.component

# Clear Ableton's plugin cache
rm -rf ~/Library/Preferences/Ableton/Live*/Plugin*

# Copy new plugins
cp -r Builds/MacOSX/build/Debug/IKReverb.vst3 ~/Library/Audio/Plug-Ins/VST3/
cp -r Builds/MacOSX/build/Debug/IKReverb.component ~/Library/Audio/Plug-Ins/Components/

echo "✨ Installation complete! Please restart Ableton Live and rescan plugins." 