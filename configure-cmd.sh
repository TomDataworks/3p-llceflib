#!/bin/bash

CEF_NAME="llceflib"
CEF_VERSION_MAIN="3.2454"

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
    CEF_VERSION_FULL="${CEF_VERSION_MAIN}.1344.g2782fb8"
    CEF_MD5="1f36440edd3b3330671d1a9d77735a6f"
    ;;
    "linux")
    CEF_PLATFORM="linux32"
    CEF_VERSION_FULL="${CEF_VERSION_MAIN}.1329.g29055f6"
    CEF_MD5="bd2600a175d6c035cef87a96ae7234ad"
    ;;
    "linux64")
    CEF_PLATFORM="linux64"
    CEF_VERSION_FULL="${CEF_VERSION_MAIN}.1333.ga594fe2"
    CEF_MD5="7d776de38a86917684058bf1834b8efc"
    ;;
    "windows")
    CEF_PLATFORM="windows32"
    CEF_VERSION_FULL="${CEF_VERSION_MAIN}.1344.g2782fb8"
    CEF_MD5="56390a070ad371b604f5d16685e0877e"
    ;;
    "windows64")
    CEF_PLATFORM="windows64"
    CEF_VERSION_FULL="${CEF_VERSION_MAIN}.1344.g2782fb8"
    CEF_MD5="1a77ac0cef64d5ea3555f0e1f2aff866"
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
