#!/usr/bin/env bash

# This script will download and build the leveldb library
# It shouldn't need to be run again

# compile

mkdir -p "$HOME/tmp/tar_gz"
cd "$HOME/tmp/tar_gz"
if [[ ! -f "$HOME/tmp/tar_gz/leveldb-1.20.tar.gz" ]]; then
  wget -O "$HOME/tmp/tar_gz/leveldb-1.20.tar.gz" "https://github.com/google/leveldb/archive/v1.20.tar.gz"
fi
tar xf "$HOME/tmp/tar_gz/leveldb-1.20.tar.gz"
cd "$HOME/tmp/tar_gz/leveldb-1.20"
make clean
make distclean
make

# install

cd "$HOME/tmp/tar_gz/leveldb-1.20"
sudo scp -r out-static/lib* out-shared/lib* "/usr/src/projects/bigbear/lib"
cd "$HOME/tmp/tar_gz/leveldb-1.20/include"
sudo scp -r leveldb "/usr/src/projects/bigbear/include"
sudo ldconfig