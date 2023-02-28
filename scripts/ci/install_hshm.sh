#!/bin/bash

set -x
set -e
set -o pipefail

mkdir -p "${HOME}/install"
mkdir build
pushd build
# export CXXFLAGS="${CXXFLAGS} -std=c++17 -Werror -Wall -Wextra"
spack load --only dependencies hermes_shm
cmake ../ -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$[HOME}/install
cmake --build . -- -j4
ctest -VV

popd