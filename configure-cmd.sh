#!/bin/bash

CEF_NAME="llceflib"
CEF_VERSION_MAIN="3.2883"

# turn on verbose debugging output for parabuild logs.
set -x
# make errors fatal
set -e

# Check autobuild is around or fail
if [ -z "$AUTOBUILD" ] ; then
    fail
fi

if [ "$OSTYPE" = "cygwin" ] ; then
    export AUTOBUILD="$(cygpath -u $AUTOBUILD)"
fi

# Load autobuild provided shell functions and variables
set +x
eval "$("$AUTOBUILD" source_environment)"
set -x

TOP="$(dirname "$0")"
pushd "${TOP}"

# Form the cef archive URL to fetch
case "$AUTOBUILD_PLATFORM" in
    "darwin")
    CEF_PLATFORM="macosx64"
    CEF_VERSION_FULL="${CEF_VERSION_MAIN}.1540.gedbfb20"
    CEF_PACKAGE_EXTENSION="tar.bz2"
    CEF_MD5="81a9b86b1b7df09575fba80bb600e3e1"
    CEF_FOLDER_NAME="cef_binary_${CEF_VERSION_FULL}_${CEF_PLATFORM}"
    CEF_ARCHIVE="${CEF_FOLDER_NAME}.${CEF_PACKAGE_EXTENSION}"
    CEF_URL="http://opensource.spotify.com/cefbuilds/${CEF_ARCHIVE}"
    ;;
    "linux")
    CEF_PLATFORM="linux32"
    CEF_VERSION_FULL="${CEF_VERSION_MAIN}.1540.gedbfb20"
    CEF_PACKAGE_EXTENSION="zip"
    CEF_MD5="bce206a8b7cfe555dea3457c1c71d81e"
    CEF_FOLDER_NAME="cef_binary_${CEF_VERSION_FULL}_${CEF_PLATFORM}"
    CEF_ARCHIVE="${CEF_FOLDER_NAME}.${CEF_PACKAGE_EXTENSION}"
    CEF_URL="http://depot.alchemyviewer.org/pub/cef/${CEF_ARCHIVE}"
    ;;
    "linux64")
    CEF_PLATFORM="linux64"
    CEF_VERSION_FULL="3.2526.1373.gb660893"
    CEF_PACKAGE_EXTENSION="zip"
    CEF_MD5="e460d5f628e231f349be4a9521c2786b"
    CEF_FOLDER_NAME="cef_binary_${CEF_VERSION_FULL}_${CEF_PLATFORM}"
    CEF_ARCHIVE="${CEF_FOLDER_NAME}.${CEF_PACKAGE_EXTENSION}"
    CEF_URL="http://depot.alchemyviewer.org/pub/cef/${CEF_ARCHIVE}"
    ;;
    "windows")
    CEF_PLATFORM="windows32"
    CEF_VERSION_FULL="${CEF_VERSION_MAIN}.1540.gedbfb20"
    CEF_PACKAGE_EXTENSION="tar.bz2"
    CEF_MD5="d51f889d539072f78aaf0316d2734ca7"
    CEF_FOLDER_NAME="cef_binary_${CEF_VERSION_FULL}_${CEF_PLATFORM}"
    CEF_ARCHIVE="${CEF_FOLDER_NAME}.${CEF_PACKAGE_EXTENSION}"
    CEF_URL="http://opensource.spotify.com/cefbuilds/${CEF_ARCHIVE}"
    ;;
    "windows64")
    CEF_PLATFORM="windows64"
    CEF_VERSION_FULL="${CEF_VERSION_MAIN}.1540.gedbfb20"
    CEF_PACKAGE_EXTENSION="tar.bz2"
    CEF_MD5="241e4317eaba9c8a8bdb2dd01791bcb9"
    CEF_FOLDER_NAME="cef_binary_${CEF_VERSION_FULL}_${CEF_PLATFORM}"
    CEF_ARCHIVE="${CEF_FOLDER_NAME}.${CEF_PACKAGE_EXTENSION}"
    CEF_URL="http://opensource.spotify.com/cefbuilds/${CEF_ARCHIVE}"
    ;;
esac

# Fetch and extract the cef archive
fetch_archive "${CEF_URL}" "${CEF_ARCHIVE}" "${CEF_MD5}"
extract ${CEF_ARCHIVE}
mv "$CEF_FOLDER_NAME" "cef"

stage="$(pwd)/stage"
echo "${CEF_VERSION_FULL}" > "${stage}/VERSION.txt"

popd
