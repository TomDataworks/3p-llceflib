#!/bin/bash

CEF_NAME="llceflib"
CEF_VERSION_MAIN="3.2526"

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
    CEF_VERSION_FULL="${CEF_VERSION_MAIN}.1364.gf6bf57b"
    CEF_PACKAGE_EXTENSION="7z"
    CEF_MD5="e46f9d00a6950bc0268a03fa24d4ec80"
    ;;
    "linux")
    CEF_PLATFORM="linux32"
    CEF_VERSION_FULL="${CEF_VERSION_MAIN}.1364.gf6bf57b"
    CEF_PACKAGE_EXTENSION="zip"
    CEF_MD5="bce206a8b7cfe555dea3457c1c71d81e"
    ;;
    "linux64")
    CEF_PLATFORM="linux64"
    CEF_VERSION_FULL="${CEF_VERSION_MAIN}.1369.ge3b2e60"
    CEF_PACKAGE_EXTENSION="zip"
    CEF_MD5="9b53485a22e979b5fd9b4ccf3c15bae8"
    ;;
    "windows")
    CEF_PLATFORM="windows32"
    CEF_VERSION_FULL="${CEF_VERSION_MAIN}.1364.gf6bf57b"
    CEF_PACKAGE_EXTENSION="7z"
    CEF_MD5="46f57f66ee9bcc03ce756e5813b88450"
    ;;
    "windows64")
    CEF_PLATFORM="windows64"
    CEF_VERSION_FULL="${CEF_VERSION_MAIN}.1364.gf6bf57b"
    CEF_PACKAGE_EXTENSION="7z"
    CEF_MD5="af4c640cb9fa386100cdc97fb5ba0f77"
    ;;
esac
CEF_FOLDER_NAME="cef_binary_${CEF_VERSION_FULL}_${CEF_PLATFORM}"
CEF_ARCHIVE="${CEF_FOLDER_NAME}.${CEF_PACKAGE_EXTENSION}"
CEF_URL="http://depot.alchemyviewer.org/pub/cef/${CEF_ARCHIVE}"

# Fetch and extract the cef archive
fetch_archive "${CEF_URL}" "${CEF_ARCHIVE}" "${CEF_MD5}"
case "${CEF_ARCHIVE}" in
    *.7z)
        7z x "$CEF_ARCHIVE"
        mv "$CEF_FOLDER_NAME" "cef"
    ;;
    *.zip)
        unzip "$CEF_ARCHIVE"
        mv "$CEF_FOLDER_NAME" "cef"
    ;;
esac

stage="$(pwd)/stage"
echo "${CEF_VERSION_FULL}" > "${stage}/VERSION.txt"

popd
