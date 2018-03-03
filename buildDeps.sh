#!/bin/bash

if [[ -e FixedJSONConfig.cmake ]]; then
    echo "Building dependencies..."
else
    echo "This script should be run in the home directory of the project"
    exit 1
fi

if [[ -e deps/NSTimestamps ]]; then
    echo "Existing NSTimestamps directory, no need to clone"
else
    git clone https://github.com/Grauniad/NanoSecondTimestamps.git deps/NSTimestamps || exit 1
fi

if [[ -e deps/OSCPPTools ]]; then
    echo "Existing OSCPPTools directory, no need to clone"
else
    git clone https://github.com/Grauniad/OSCPPTools.git deps/OSCPPTools || exit 1
fi

if [[ -e deps/DevToolsLog ]]; then
    echo "Existing DevToolsLog directory, no need to clone"
else
    git clone https://github.com/Grauniad/LegacyLogger.git deps/DevToolsLog || exit 1
fi

if [[ -e deps/rapidjson ]]; then
    echo "Existing rapidjson directory, no need to clone"
else
    git clone https://github.com/Tencent/rapidjson.git deps/rapidjson || exit 1
fi

DEPS_BUILD=$PWD/deps/build
DEPS_CMAKE_DEPO=$PWD/deps/build/lib/cmake
mkdir -p $DEPS_BUILD


deps=(NSTimestamps OSCPPTools DevToolsLog rapidjson);
for dep in ${deps[@]} ; do
    mkdir -p deps/$dep/build

    pushd deps/$dep || exit 1
    git pull
    pushd build || exit 1

    cmake -DCMAKE_BUILD_TYPE=Release "-DCMAKE_PREFIX_PATH:PATH=$DEPS_CMAKE_DEPO" "-DCMAKE_INSTALL_PREFIX:PATH=$DEPS_BUILD" .. || exit 1
    make -j 3 || exit 1
    make install || exit 1

    popd || exit 1
    popd || exit 1
    done
exit 1
