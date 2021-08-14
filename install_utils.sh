#!/usr/bin/env bash

INSTALL_DIR=/usr/local

install() {
    cmake -H. -Bdist -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR
    cmake --build dist --clean-first --target install
}

pushd png2pnm
install
popd

pushd pnm2png
install
popd
