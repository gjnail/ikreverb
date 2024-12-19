#!/bin/bash

# Exit on error
set -e

# Configuration
PLUGIN_NAME="IKDistortion"
VERSION="1.0.0"
DEVELOPER="Internet Kids"
IDENTIFIER="com.internetkids.ikdistortion"

# Directory setup
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/Builds/MacOSX/build/Release"
INSTALLER_DIR="$PROJECT_DIR/Installer"
PACKAGE_DIR="$INSTALLER_DIR/package"
COMPONENTS_DIR="$PACKAGE_DIR/Components"
VST3_DIR="$COMPONENTS_DIR/VST3"
AU_DIR="$COMPONENTS_DIR/AU"
RESOURCES_DIR="$INSTALLER_DIR/Resources"

# Create necessary directories
mkdir -p "$INSTALLER_DIR"
mkdir -p "$VST3_DIR"
mkdir -p "$AU_DIR"
mkdir -p "$RESOURCES_DIR"

# Build the plugin (assumes Xcode project exists)
echo "Building plugin..."
xcodebuild -project "$PROJECT_DIR/Builds/MacOSX/$PLUGIN_NAME.xcodeproj" -configuration Release clean build

# Copy built plugins to staging area
echo "Copying built plugins..."
cp -R "$BUILD_DIR/$PLUGIN_NAME.vst3" "$VST3_DIR/"
cp -R "$BUILD_DIR/$PLUGIN_NAME.component" "$AU_DIR/"

# Create distribution XML
cat > "$INSTALLER_DIR/distribution.xml" << EOF
<?xml version="1.0" encoding="utf-8"?>
<installer-gui-script minSpecVersion="1">
    <title>$PLUGIN_NAME</title>
    <organization>$DEVELOPER</organization>
    <domains enable_localSystem="true"/>
    <options customize="true" require-scripts="true" allow-external-scripts="no"/>
    <welcome file="welcome.txt"/>
    <license file="license.txt"/>
    <conclusion file="conclusion.txt"/>
    
    <choices-outline>
        <line choice="vst3"/>
        <line choice="au"/>
    </choices-outline>
    
    <choice id="vst3" title="VST3 Plugin" description="Install VST3 plugin">
        <pkg-ref id="$IDENTIFIER.vst3"/>
    </choice>
    
    <choice id="au" title="Audio Unit Plugin" description="Install Audio Unit plugin">
        <pkg-ref id="$IDENTIFIER.au"/>
    </choice>
    
    <pkg-ref id="$IDENTIFIER.vst3" auth="root">vst3.pkg</pkg-ref>
    <pkg-ref id="$IDENTIFIER.au" auth="root">au.pkg</pkg-ref>
</installer-gui-script>
EOF

# Create welcome text
cat > "$RESOURCES_DIR/welcome.txt" << EOF
Welcome to the $PLUGIN_NAME Installer

This will install $PLUGIN_NAME version $VERSION on your computer.
EOF

# Create license text
cat > "$RESOURCES_DIR/license.txt" << EOF
Copyright (c) $(date +%Y) $DEVELOPER

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
EOF

# Create conclusion text
cat > "$RESOURCES_DIR/conclusion.txt" << EOF
$PLUGIN_NAME has been successfully installed!

VST3 plugins are installed in: /Library/Audio/Plug-Ins/VST3
Audio Unit plugins are installed in: /Library/Audio/Plug-Ins/Components
EOF

# Create VST3 component package
pkgbuild --root "$VST3_DIR" \
         --install-location "/Library/Audio/Plug-Ins/VST3" \
         --identifier "$IDENTIFIER.vst3" \
         --version "$VERSION" \
         --ownership recommended \
         "$INSTALLER_DIR/vst3.pkg"

# Create AU component package
pkgbuild --root "$AU_DIR" \
         --install-location "/Library/Audio/Plug-Ins/Components" \
         --identifier "$IDENTIFIER.au" \
         --version "$VERSION" \
         --ownership recommended \
         "$INSTALLER_DIR/au.pkg"

# Create final installer package
productbuild --distribution "$INSTALLER_DIR/distribution.xml" \
             --resources "$RESOURCES_DIR" \
             --package-path "$INSTALLER_DIR" \
             "$PROJECT_DIR/$PLUGIN_NAME-$VERSION-macOS.pkg"

echo "Installer package created: $PROJECT_DIR/$PLUGIN_NAME-$VERSION-macOS.pkg" 