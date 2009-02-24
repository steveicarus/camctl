#!/bin/sh

# This script makes an installable package of the camtool. Run this script
# from the root of the source (the directory that contains the camtool
# directory and the scripts directory) and it will make the package.

cp -r camtool/camtool.app .

scripts/macdeployqt/macdeployqt camtool.app

basename='Icarus Camera Control'
mv camtool.app "$basename.app"

hdiutil create "$basename".dmg -srcfolder "$basename".app -format UDZO -volname "$basename"

rm -rf "$basename".app
