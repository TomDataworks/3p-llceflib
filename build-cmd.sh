#!/bin/bash

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

stage="$(pwd)/stage"
stage_bin_release="$stage/bin/release"
stage_bin_debug="$stage/bin/debug"
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

#Create the staging resource dir
mkdir -p "$stage/resources"

case "$AUTOBUILD_PLATFORM" in
    "darwin")
        pushd "cef"
            export GYP_GENERATORS=ninja
            #sh ./cef_create_projects.sh 
            cmake -G "Ninja" . -DPROJECT_ARCH="x86_64" -DCMAKE_BUILD_TYPE=Release
            ninja cefclient
        popd
        pushd "llceflib"
            cmake -G "Ninja" . -DPROJECT_ARCH="x86_64" -DCMAKE_BUILD_TYPE=Release
            ninja llceflib
        popd
    ;;
    "linux")
        echo "Not implemented"
        fail
     ;;
    "linux64")
        echo "Not implemented"
        fail
    ;;
    "windows")
        pushd "cef"
            sed -i -- 's/\/MT/\/MD/' CMakeLists.txt
            sed -i -- 's/\/wd\\\"4244\\\"/\/wd\\\"4244\\\"\ \/wd\\\"4456\\\"\ \/wd\\\"4458\\\"/' CMakeLists.txt
            mkdir build
            pushd "build"
                cmake -G "Visual Studio 14" ..
                build_sln "cef.sln" "Debug" "Win32" "libcef_dll_wrapper"
                build_sln "cef.sln" "Release" "Win32" "libcef_dll_wrapper"
                
                cp libcef_dll/Debug/libcef_dll_wrapper.* "$stage_lib_debug"
                cp libcef_dll/Release/libcef_dll_wrapper.* "$stage_lib_release"
            popd
            cp Debug/*.dll "$stage_bin_debug"
            cp Debug/*.bin "$stage_bin_debug"
            cp Debug/libcef.lib "$stage_lib_debug"
            cp Release/*.dll "$stage_bin_release"
            cp Release/*.bin "$stage_bin_release"
            cp Release/libcef.lib "$stage_lib_release"
            cp -R Resources/* "$stage/resources"
        popd
        pushd "llceflib"
            mkdir build
            pushd "build"
                cmake -G "Visual Studio 14" ..
                build_sln "llceflib.sln" "Debug" "Win32"
                build_sln "llceflib.sln" "Release" "Win32"
                
                cp "lib/Debug/llceflib.lib" "$stage_lib_debug"
                cp "lib/Debug/llceflib.pdb" "$stage_lib_debug"
                cp "lib/Release/llceflib.lib" "$stage_lib_release"
                
                cp "bin/Debug/llceflib_host.exe" "$stage_bin_debug"
                cp "bin/Release/llceflib_host.exe" "$stage_bin_release"
            popd
        popd
    ;;
    "windows64")
        pushd "cef"
            sed -i -- 's/\/MT/\/MD/' CMakeLists.txt
            sed -i -- 's/\/wd\\\"4244\\\"/\/wd\\\"4244\\\"\ \/wd\\\"4456\\\"\ \/wd\\\"4458\\\"/' CMakeLists.txt
            mkdir build
            pushd "build"
                cmake -G "Visual Studio 14 Win64" ..
                build_sln "cef.sln" "Debug" "x64" "libcef_dll_wrapper"
                build_sln "cef.sln" "Release" "x64" "libcef_dll_wrapper"
                
                cp libcef_dll/Debug/libcef_dll_wrapper.* "$stage_lib_debug"
                cp libcef_dll/Release/libcef_dll_wrapper.* "$stage_lib_release"
            popd
            cp Debug/*.dll "$stage_bin_debug"
            cp Debug/*.bin "$stage_bin_debug"
            cp Debug/libcef.lib "$stage_lib_debug"
            cp Release/*.dll "$stage_bin_release"
            cp Release/*.bin "$stage_bin_release"
            cp Release/libcef.lib "$stage_lib_release"
            cp -R Resources/* "$stage/resources"
        popd
        pushd "llceflib"
            mkdir build
            pushd "build"
                cmake -G "Visual Studio 14 Win64" ..
                build_sln "llceflib.sln" "Debug" "x64"
                build_sln "llceflib.sln" "Release" "x64"
                
                cp "lib/Debug/llceflib.lib" "$stage_lib_debug"
                cp "lib/Debug/llceflib.pdb" "$stage_lib_debug"
                cp "lib/Release/llceflib.lib" "$stage_lib_release"
                
                cp "bin/Debug/llceflib_host.exe" "$stage_bin_debug"
                cp "bin/Release/llceflib_host.exe" "$stage_bin_release"
            popd
        popd
    ;;
esac

# Copy the headers
cp "llceflib/llceflib.h" "$stage/include/cef"

# Copy License (extracted from the readme)
cp LICENSES/*.txt "$stage/LICENSES"

pass

