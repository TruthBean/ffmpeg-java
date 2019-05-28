#!/usr/bin/env bash

declare -r platform="$(uname -s)"

function action() {
    local machine=$1

    local make_path="$(which make)"
    local clang_path="$(which clang)"

    if [ "${clang_path}" == "" ]; then
        clang_path="$(which cc)"
    fi
    if [ "${clang_path}" == "" ]; then
        clang_path="$(which gcc)"
    fi

    local gpp_path="$(which g++)"
    if [ "${gpp_path}" == "" ]; then
        gpp_path="$(which c++)"
    fi
    if [ "${gpp_path}" == "" ]; then
        gpp_path="$(which gcc)"
    fi

    echo "-----------------"
    echo "${make_path}" "${clang_path}" ${gpp_path}
    echo "-----------------"

    if [[ -d "build" ]]; then
        echo "remove "
        rm -rf build
    fi

    mkdir build
    cd build

    # cmake -DCMAKE_MAKE_PROGRAM="${make_path}" -DCMAKE_C_COMPILER="${clang_path}" -DCMAKE_CXX_COMPILER=${gpp_path} -G "CodeBlocks - Unix Makefiles" ..
    cmake -DCMAKE_MAKE_PROGRAM="/usr/bin/make" -DCMAKE_C_COMPILER="/usr/bin/clang" -DCMAKE_CXX_COMPILER="/usr/bin/gcc" -G "CodeBlocks - Unix Makefiles" ..
    make
}

case "${platform}" in
    Linux*)
        echo "platform: Linux"
        action "linux"

        ;;

    MINGW*|MSYS*)
        echo "platform: MingW or Msys"
        action "msys"

        ;;

    CYGWIN*)
        echo "Machine: Cygwin"
        action "cygwin"
        ;;

    Darwin*)
        echo "platform: Mac"
        action "mac"
        ;;

    *)
        echo "platform: other Unix"
        action "unix"
        ;;
esac