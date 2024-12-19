#!/bin/bash

# Remove old plugins
rm -rf ~/Library/Audio/Plug-Ins/VST3/IKDistortion.vst3
rm -rf ~/Library/Audio/Plug-Ins/Components/IKDistortion.component

# Clear Ableton's plugin cache
rm -rf ~/Library/Preferences/Ableton/Live*/Plugin*

# Copy new plugins
cp -r Builds/MacOSX/build/Debug/IKDistortion.vst3 ~/Library/Audio/Plug-Ins/VST3/
cp -r Builds/MacOSX/build/Debug/IKDistortion.component ~/Library/Audio/Plug-Ins/Components/

echo "✨ Installation complete! Please restart Ableton Live and rescan plugins." 