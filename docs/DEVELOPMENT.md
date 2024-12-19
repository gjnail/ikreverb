# IK Reverb Development Guide

This guide covers the development setup and workflow for the IK Reverb plugin.

## Development Setup

1. Install JUCE (7.0.0 or higher)
2. Open `IKReverb.jucer` in Projucer
3. Configure your development environment:
   - Set C++ Language Standard to C++17
   - Enable JUCE_DSP module
   - Configure VST3/AU paths as needed

## Project Structure

```
IKReverb/
├── Source/                 # Plugin source code
├── Builds/                # Build files for different platforms
├── JuceLibraryCode/       # JUCE library code
├── scripts/               # Build and deployment scripts
└── docs/                  # Documentation
```

## Building

### macOS
1. Open Projucer
2. Load `IKReverb.jucer`
3. Click "Save and Open in Xcode"
4. Build in Xcode

### Windows
1. Open Projucer
2. Load `IKReverb.jucer`
3. Click "Save and Open in Visual Studio"
4. Build in Visual Studio

## Adding Features

### New Reverb Algorithm
1. Add enum value to `ReverbType`
2. Implement processing in `processReverb()`
3. Update parameter handling in `processBlock()`
4. Add UI controls in `PluginEditor`

### UI Customization
1. Modify `IKReverbLookAndFeel` for custom styling
2. Update layout in `PluginEditor::resized()`
3. Add new controls in `PluginEditor`

## Testing

1. Build both Debug and Release configurations
2. Test in multiple DAWs
3. Verify CPU usage is reasonable
4. Check for audio glitches
5. Validate parameter automation

## Release Process

1. Update version number in:
   - `IKReverb.jucer`
   - Installer scripts
2. Build for all platforms
3. Create installers
4. Test installation process
5. Create release notes
6. Tag release in git