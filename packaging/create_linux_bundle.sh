#!/bin/bash
set -e

APP_NAME="Amphitude"
BUILD_DIR="packaging/linux/$APP_NAME"
ICON_SOURCE="assets/Logo.png"

echo "📦 Creating Linux Bundle..."

# 1. Clean & Prepare
rm -rf "packaging/linux"
mkdir -p "$BUILD_DIR"

# 2. Copy Binary & Assets
if [ ! -f "amphitude" ]; then
    echo "❌ Binary 'amphitude' not found! Build it first."
    exit 1
fi
cp amphitude "$BUILD_DIR/"
cp -R assets "$BUILD_DIR/"
cp "$ICON_SOURCE" "$BUILD_DIR/icon.png"

# 3. Create Launcher Script
cat <<EOF > "$BUILD_DIR/run.sh"
#!/bin/bash
cd "\$(dirname "\$0")"
chmod +x amphitude
./amphitude
EOF
chmod +x "$BUILD_DIR/run.sh"

# 4. Create .desktop file
cat <<EOF > "$BUILD_DIR/amphitude.desktop"
[Desktop Entry]
Version=1.0
Name=Amphitude
Comment=Serverless P2P Platform Fighter
Exec=$(pwd)/$APP_NAME/run.sh
Icon=$(pwd)/$APP_NAME/icon.png
Terminal=false
Type=Application
Categories=Game;ArcadeGame;
EOF

echo "⚠️  Note: The .desktop file paths are relative to where you extract it."
echo "   Users usually install to /opt or ~/.local/share."

# 5. Tarball
echo "🗜️  Compressing..."
cd packaging/linux
tar -czvf "../../Amphitude_Linux.tar.gz" "$APP_NAME"

echo "🎉 DONE! Installer: Amphitude_Linux.tar.gz"
