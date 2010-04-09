#!/bin/sh

version='20100408'

# This script makes an installable package of the camtool. Run this script
# from the root of the source (the directory that contains the camtool
# directory and the scripts directory) and it will make the package.

# Make a copy of the Icarus Camera Control application bundle, and run the
# macdeployqt program to cause all the necessary QT files to be bundled in.
cp -r camtool/camtool.app .
scripts/macdeployqt/macdeployqt camtool.app

# The basename is the name of the install application bundle.
basename='Icarus Camera Control'

# The packname is the file name to use for packages and images. This
# will have spaces removed.
packname=IcarusCameraControl-"$version"

mkdir scratch

mv camtool.app scratch/"$basename.app"

cp scripts/mac_README.rtf scratch/README.rtf

mkdir scratch/'Source Code'
cp camctl-"$version".tar.gz scratch/'Source Code'

hdiutil create "$packname".dmg -srcfolder scratch -format UDZO -volname "$packname"

rm -rf scratch
