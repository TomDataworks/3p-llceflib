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
            export GYP_GENERATORS=xcode GYP_DEFINES=mac_sdk=10.8
            XCODE_FLAGS="-sdk macosx10.11 -mmacosx-version-min=10.8 -configuration Release"
            #sh ./cef_create_projects.sh 
            cmake -G "Xcode" . -DPROJECT_ARCH="x86_64" \
                -DCMAKE_BUILD_TYPE=Release 
            xcodebuild -target libcef_dll_wrapper ${XCODE_FLAGS}
            xcodebuild -target "cefclient Helper" ${XCODE_FLAGS}

            cp -R cefclient/resources/* "${stage}/resources"
            cp libcef_dll/Release/libcef_dll_wrapper.a "${stage_lib_release}"
            cp -R Release/"Chromium Embedded Framework.framework" "${stage_lib_release}"
            cp -R cefclient/Release/"cefclient Helper.app" "${stage_bin_release}"
        popd
        pushd "llceflib"
            cmake -G "Xcode" . -DPROJECT_ARCH="x86_64" -DCMAKE_BUILD_TYPE=Release
            xcodebuild -target llceflib ${XCODE_FLAGS}
            xcodebuild -target llceflib_host ${XCODE_FLAGS}

            cp "lib/Release/libllceflib.a" "${stage_lib_release}"
            cp -R "bin/Release/LLCefLib Helper.app" "${stage_lib_release}"
        popd
    ;;
    "linux")
        echo "Not implemented"
        fail
     ;;
    "linux64")
        pushd "cef"
            mkdir -p build_debug
            pushd "build_debug"
                cmake -G "Unix Makefiles" ../ -DCMAKE_BUILD_TYPE=Debug
                make -j4 libcef_dll_wrapper
                cp libcef_dll/libcef_dll_wrapper.a "$stage_lib_debug"
            popd
            mkdir -p build_release
            pushd "build_release"
                cmake -G "Unix Makefiles" ../ -DCMAKE_BUILD_TYPE=Release
                make -j4 libcef_dll_wrapper
                cp libcef_dll/libcef_dll_wrapper.a "$stage_lib_release"
            popd
            cp -a Debug/chrome-sandbox "$stage_bin_debug"
            cp -a Debug/*.bin "$stage_bin_debug"
            cp -a Debug/*.so "$stage_lib_debug"
            cp -a Release/chrome-sandbox "$stage_bin_release"
            cp -a Release/*.bin "$stage_bin_release"
            cp -a Release/*.so "$stage_lib_release"
            cp -a Resources/* "$stage/resources"
        popd
        pushd "llceflib"
            mkdir -p build_debug
            pushd "build_debug"
                cmake -G "Unix Makefiles" ../ -DCMAKE_BUILD_TYPE=Debug
                make -j4
                cp bin/* "$stage_bin_debug"
                cp lib/* "$stage_lib_debug"
            popd
            mkdir -p build_release
            pushd "build_release"
                cmake -G "Unix Makefiles" ../ -DCMAKE_BUILD_TYPE=Release
                make -j4
                cp bin/* "$stage_bin_release"
                cp lib/* "$stage_lib_release"
            popd
        popd
    ;;
    "windows")
        pushd "cef"
            pushd "cmake"
                sed -i -- 's/\/MT/\/MD/' cef_variables.cmake
            popd
            mkdir -p build
            pushd "build"
                cmake -G "Visual Studio 14" ..
                #build_sln "cef.sln" "Debug" "Win32" "libcef_dll_wrapper"
                build_sln "cef.sln" "Release" "Win32" "libcef_dll_wrapper"
                
                #cp libcef_dll_wrapper/Debug/libcef_dll_wrapper.* "$stage_lib_debug"
                cp libcef_dll_wrapper/Release/libcef_dll_wrapper.* "$stage_lib_release"
            popd
            #cp Debug/*.bin "$stage_bin_debug"
            #cp Debug/*.dll "$stage_bin_debug"
            #cp Debug/*.exe "$stage_bin_debug"
            #cp Debug/libcef.lib "$stage_lib_debug"
            cp Release/*.bin "$stage_bin_release"
            cp Release/*.dll "$stage_bin_release"
            cp Release/*.exe "$stage_bin_release"
            cp Release/libcef.lib "$stage_lib_release"
            cp -R Resources/* "$stage/resources"
        popd
        pushd "llceflib"
            mkdir -p build
            pushd "build"
                cmake -G "Visual Studio 14" ..
                #build_sln "llceflib.sln" "Debug" "Win32"
                build_sln "llceflib.sln" "Release" "Win32"
                
                #cp "lib/Debug/llceflib.lib" "$stage_lib_debug"
                #cp "lib/Debug/llceflib.pdb" "$stage_lib_debug"
                cp "lib/Release/llceflib.lib" "$stage_lib_release"
                
                #cp "bin/Debug/llceflib_host.exe" "$stage_bin_debug"
                cp "bin/Release/llceflib_host.exe" "$stage_bin_release"
            popd
        popd
    ;;
    "windows64")
        pushd "cef"
            pushd "cmake"
                sed -i -- 's/\/MT/\/MD/' cef_variables.cmake
            popd
            mkdir -p build
            pushd "build"
                cmake -G "Visual Studio 14 Win64" ..
                #build_sln "cef.sln" "Debug" "x64" "libcef_dll_wrapper"
                build_sln "cef.sln" "Release" "x64" "libcef_dll_wrapper"
                
                #cp libcef_dll_wrapper/Debug/libcef_dll_wrapper.* "$stage_lib_debug"
                cp libcef_dll_wrapper/Release/libcef_dll_wrapper.* "$stage_lib_release"
            popd
            #cp Debug/*.dll "$stage_bin_debug"
            #cp Debug/*.bin "$stage_bin_debug"
            #cp Debug/libcef.lib "$stage_lib_debug"
            cp Release/*.dll "$stage_bin_release"
            cp Release/*.bin "$stage_bin_release"
            cp Release/libcef.lib "$stage_lib_release"
            cp -R Resources/* "$stage/resources"
        popd
        pushd "llceflib"
            mkdir -p build
            pushd "build"
                cmake -G "Visual Studio 14 Win64" ..
                #build_sln "llceflib.sln" "Debug" "x64"
                build_sln "llceflib.sln" "Release" "x64"
                
                #cp "lib/Debug/llceflib.lib" "$stage_lib_debug"
                #cp "lib/Debug/llceflib.pdb" "$stage_lib_debug"
                cp "lib/Release/llceflib.lib" "$stage_lib_release"
                
                #cp "bin/Debug/llceflib_host.exe" "$stage_bin_debug"
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

