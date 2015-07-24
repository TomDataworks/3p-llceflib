#!/bin/bash

CEF_NAME="llceflib"
CEF_VERSION="3.2357.1281.gd660177"

cd "$(dirname "$0")"

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

# Form the cef archive URL to fetch
case "$AUTOBUILD_PLATFORM" in
    "windows")
    CEF_PLATFORM="windows32"
    CEF_MD5="7096677dafbec5e4bd4b826c59b95778"
    ;;
    "windows64")
    CEF_PLATFORM="windows64"
	CEF_VERSION="3.2357.1276.gd4b589c" # override for win64 as its slightly out of date
    CEF_MD5="44542b1828e212e11549d5f8297c56ed"
    ;;
    "darwin")
    CEF_PLATFORM="macosx64"
    CEF_MD5="6916ebc14b6582760f9bf248a72a0ad3"
    ;;
    "linux")
    CEF_PLATFORM="linux32"
    CEF_MD5="822535d5d288afa8d9524fbfa72721ca"
    ;;
    "linux64")
    CEF_PLATFORM="linux32"
    CEF_MD5="0b6ea1c668ee61b2ed73e3c88264008a"
    ;;
esac
CEF_FOLDER_NAME="cef_binary_${CEF_VERSION}_${CEF_PLATFORM}"
CEF_ARCHIVE="${CEF_FOLDER_NAME}.7z"
CEF_URL="http://depot.alchemyviewer.org/pub/temp/${CEF_ARCHIVE}"

# Fetch and extract the cef archive
fetch_archive "${CEF_URL}" "${CEF_ARCHIVE}" "${CEF_MD5}"
case "${CEF_ARCHIVE}" in
    *.7z)
        7z x "$CEF_ARCHIVE"
        mv "$CEF_FOLDER_NAME" "cef"
    ;;
esac

stage="$(pwd)/stage"
stage_bin_release="$stage/lib/release"
stage_bin_debug="$stage/lib/debug"
stage_lib_release="$stage/lib/release"
stage_lib_debug="$stage/lib/debug"

# Create the staging license folder
mkdir -p "$stage/LICENSES"

# Create the staging include folders
mkdir -p "$stage/include/cef"

#Create the staging debug and release folders
mkdir -p "$stage_bin_debug"
mkdir -p "$stage_bin_release"
mkdir -p "$stage_lib_debug"
mkdir -p "$stage_lib_release"

echo "${CEF_VERSION}" > "${stage}/VERSION.txt"

case "$AUTOBUILD_PLATFORM" in
    "windows")
		pushd "cef"
            sed -i -- 's/\/MT/\/MD/' CMakeLists.txt
			sed -i -- 's/\/wd\\\"4244\\\"/\/wd\\\"4244\\\"\ \/wd\\\"4456\\\"\ \/wd\\\"4458\\\"/' CMakeLists.txt
			mkdir build
			pushd "build"
				cmake -G "Visual Studio 14" ..
                build_sln "cef.sln" "Debug|Win32" "libcef_dll_wrapper"
                build_sln "cef.sln" "Release|Win32" "libcef_dll_wrapper"
			popd
		popd
    ;;
    "windows64")
    ;;
    "darwin")
    ;;
    "linux")
     ;;
    "linux64")
    ;;
esac

    # Copy the headers
    # Copy License (extracted from the readme)

pass

