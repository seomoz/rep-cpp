#! /usr/bin/env bash

set -e

sudo apt-get install -y g++ libgtest-dev cmake python-pip

sudo pip install gcovr==3.2

pushd /tmp
    mkdir -p gtest-build
    pushd gtest-build
        cmake -DCMAKE_BUILD_TYPE=RELEASE /usr/src/gtest/
        make
        find . -name 'libg*.a' | xargs sudo cp -f --target-directory=/usr/lib/
    popd
popd
