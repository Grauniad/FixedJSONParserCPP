#!/bin/bash

if [[ -e FixedJSONConfig.cmake ]]; then
    echo "Building dependencies..."
else
    echo "This script should be run in the home directory of the project"
    exit 1
fi

if [[ "$1" == "" ]]; then
    DEPS_ROOT="$PWD/deps"
else
    DEPS_ROOT="$1"
fi
if [[ -e $DEPS_ROOT/CMakeUtils ]]; then
    echo "Existing CMakeUtils directory, no need to clone"
else
    git clone https://github.com/Grauniad/CMakeUtils.git $DEPS_ROOT/CMakeUtils || exit 1
fi

declare -A depList

depList[Time]=https://github.com/Grauniad/NanoSecondTimestamps.git
depList[DevToolsLog]=https://github.com/Grauniad/LegacyLogger.git
depList[rapidjson]=https://github.com/Tencent/rapidjson.git

source $DEPS_ROOT/CMakeUtils/build_tools/buildDepsCommon.sh || exit 1
