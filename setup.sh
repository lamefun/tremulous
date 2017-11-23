#!/bin/sh

set -e

CMAKE_ARGS="\
  -G Ninja \
  -DCMAKE_INSTALL_PREFIX=../install \
  -DCMAKE_BUILD_TYPE=Debug \
  -DINSTALL_LAYOUT=Portable"

if [ -e build ] ; then
  if [ -d build ] ; then
    read -p "The build directory already exists, remove it [y/n]? " ANSWER
    case $ANSWER in
      [Yy]*) rm -rf build ;;
      *) exit 1 ;;
    esac
  else
    echo "The build directory exists, but is not a directory."
    exit 1
  fi
fi

set -x

git submodule update

mkdir build
cd build

eval cmake $CMAKE_ARGS ..
