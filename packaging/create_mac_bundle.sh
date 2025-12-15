#!/bin/bash
set -e

APP_NAME="Amphitude"
APP_DIR="packaging/mac/$APP_NAME.app"
ICON_SOURCE="assets/Logo.png"
ICONSET_DIR="packaging/mac/Amphitude.iconset"

echo "📦 Creating macOS Bundle..."

# 1. Clean & Prepare
rm -rf "$APP_DIR"
mkdir -p "$APP_DIR/Contents/MacOS"
mkdir -p "$APP_DIR/Contents/Resources"

# 2. Copy Binary
if [ ! -f "amphitude" ]; then
    echo "❌ Binary 'amphitude' not found! Build it first."
    exit 1
fi
cp amphitude "$APP_DIR/Contents/MacOS/$APP_NAME"
chmod +x "$APP_DIR/Contents/MacOS/$APP_NAME"

# 3. Copy Assets
cp -R assets "$APP_DIR/Contents/Resources/"

# 4. Create Launcher Script
cat <<EOF > "$APP_DIR/Contents/MacOS/launcher.sh"
#!/bin/bash
DIR="\$(cd "\$(dirname "\$0")" && pwd)"
export RESOURCE_PATH="\$DIR/../Resources"
cd "\$RESOURCE_PATH"
# Execute the binary located adjacent to this script, but while valid CWD is Resources
"\$DIR/$APP_NAME"
EOF
chmod +x "$APP_DIR/Contents/MacOS/launcher.sh"

# 5. Create Info.plist
cat <<EOF > "$APP_DIR/Contents/Info.plist"
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>launcher.sh</string>
    <key>CFBundleIconFile</key>
    <string>AppIcon</string>
    <key>CFBundleIdentifier</key>
    <string>com.amphitainments.amphitude</string>
    <key>CFBundleName</key>
    <string>$APP_NAME</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0</string>
    <key>CFBundleVersion</key>
    <string>1</string>
    <key>NSHighResolutionCapable</key>
    <true/>
</dict>
</plist>
EOF

# 6. Generate Icons
echo "🎨 Generating Icons..."
mkdir -p "$ICONSET_DIR"
sips -z 16 16     "$ICON_SOURCE" --out "$ICONSET_DIR/icon_16x16.png" > /dev/null
sips -z 32 32     "$ICON_SOURCE" --out "$ICONSET_DIR/icon_16x16@2x.png" > /dev/null
sips -z 32 32     "$ICON_SOURCE" --out "$ICONSET_DIR/icon_32x32.png" > /dev/null
sips -z 64 64     "$ICON_SOURCE" --out "$ICONSET_DIR/icon_32x32@2x.png" > /dev/null
sips -z 128 128   "$ICON_SOURCE" --out "$ICONSET_DIR/icon_128x128.png" > /dev/null
sips -z 256 256   "$ICON_SOURCE" --out "$ICONSET_DIR/icon_128x128@2x.png" > /dev/null
sips -z 256 256   "$ICON_SOURCE" --out "$ICONSET_DIR/icon_256x256.png" > /dev/null
sips -z 512 512   "$ICON_SOURCE" --out "$ICONSET_DIR/icon_256x256@2x.png" > /dev/null
sips -z 512 512   "$ICON_SOURCE" --out "$ICONSET_DIR/icon_512x512.png" > /dev/null
sips -z 1024 1024 "$ICON_SOURCE" --out "$ICONSET_DIR/icon_512x512@2x.png" > /dev/null

iconutil -c icns "$ICONSET_DIR" -o "$APP_DIR/Contents/Resources/AppIcon.icns"
rm -rf "$ICONSET_DIR"

echo "✅ App Bundle Created: $APP_DIR"

# 7. Create DMG
echo "💿 Creating DMG..."
if [ -f "Amphitude_Installer_Mac.dmg" ]; then
    rm "Amphitude_Installer_Mac.dmg"
fi
hdiutil create -volname "$APP_NAME" -srcfolder "$APP_DIR" -ov -format UDZO "Amphitude_Installer_Mac.dmg"

echo "🎉 DONE! Installer: Amphitude_Installer_Mac.dmg"
