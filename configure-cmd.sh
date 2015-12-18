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
    CEF_VERSION_FULL="${CEF_VERSION_MAIN}.1363.gabb1158"
    CEF_MD5="7a1653d450003f8d41247b3ecd150109"
    ;;
    "linux")
    CEF_PLATFORM="linux32"
    CEF_VERSION_FULL="${CEF_VERSION_MAIN}.1363.gabb1158"
    CEF_MD5="f66f2484af5e68fd7ef735bec532a9b4"
    ;;
    "linux64")
    CEF_PLATFORM="linux64"
    CEF_VERSION_FULL="${CEF_VERSION_MAIN}.1363.gabb1158"
    CEF_MD5="9fb708dbccf9079eda254a2bf2bf5cb0"
    ;;
    "windows")
    CEF_PLATFORM="windows32"
    CEF_VERSION_FULL="${CEF_VERSION_MAIN}.1362.g26b3a7a"
    CEF_MD5="96b5ed5fd95ec95094f2215cd4a0480e"
    ;;
    "windows64")
    CEF_PLATFORM="windows64"
    CEF_VERSION_FULL="${CEF_VERSION_MAIN}.1362.g26b3a7a"
    CEF_MD5="28d115ace4c87c75d6e1cf7840516a95"
    ;;
esac
CEF_FOLDER_NAME="cef_binary_${CEF_VERSION_FULL}_${CEF_PLATFORM}"
CEF_ARCHIVE="${CEF_FOLDER_NAME}.7z"
CEF_URL="http://depot.alchemyviewer.org/pub/cef/${CEF_ARCHIVE}"

# Fetch and extract the cef archive
fetch_archive "${CEF_URL}" "${CEF_ARCHIVE}" "${CEF_MD5}"
case "${CEF_ARCHIVE}" in
    *.7z)
        7z x "$CEF_ARCHIVE"
        mv "$CEF_FOLDER_NAME" "cef"
    ;;
esac

stage="$(pwd)/stage"
echo "${CEF_VERSION_FULL}" > "${stage}/VERSION.txt"

popd
