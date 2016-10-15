#!/bin/bash

CEF_NAME="llceflib"
CEF_VERSION_MAIN="3.2785"

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
    CEF_VERSION_FULL="${CEF_VERSION_MAIN}.1478.gaab5543"
    CEF_PACKAGE_EXTENSION="tar.bz2"
    CEF_MD5="67d40653b5502482b42e48ca8008936a"
    ;;
    "linux")
    CEF_PLATFORM="linux32"
    CEF_VERSION_FULL="${CEF_VERSION_MAIN}.1364.gf6bf57b"
    CEF_PACKAGE_EXTENSION="zip"
    CEF_MD5="bce206a8b7cfe555dea3457c1c71d81e"
    ;;
    "linux64")
    CEF_PLATFORM="linux64"
    CEF_VERSION_FULL="3.2526.1373.gb660893"
    CEF_PACKAGE_EXTENSION="zip"
    CEF_MD5="e460d5f628e231f349be4a9521c2786b"
    ;;
    "windows")
    CEF_PLATFORM="windows32"
    CEF_VERSION_FULL="${CEF_VERSION_MAIN}.1478.gaab5543"
    CEF_PACKAGE_EXTENSION="tar.bz2"
    CEF_MD5="bb65f4382f8614683c1d6e7af27be9ae"
    ;;
    "windows64")
    CEF_PLATFORM="windows64"
    CEF_VERSION_FULL="${CEF_VERSION_MAIN}.1478.gaab5543"
    CEF_PACKAGE_EXTENSION="tar.bz2"
    CEF_MD5="d60bb74329860497d0b41d6454a92d15"
    ;;
esac
CEF_FOLDER_NAME="cef_binary_${CEF_VERSION_FULL}_${CEF_PLATFORM}"
CEF_ARCHIVE="${CEF_FOLDER_NAME}.${CEF_PACKAGE_EXTENSION}"
CEF_URL="http://depot.alchemyviewer.org/pub/cef/${CEF_ARCHIVE}"

# Fetch and extract the cef archive
fetch_archive "${CEF_URL}" "${CEF_ARCHIVE}" "${CEF_MD5}"
extract ${CEF_ARCHIVE}
mv "$CEF_FOLDER_NAME" "cef"

stage="$(pwd)/stage"
echo "${CEF_VERSION_FULL}" > "${stage}/VERSION.txt"

popd
