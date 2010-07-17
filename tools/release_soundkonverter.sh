#!/bin/bash

VERSION="0.9.91"

if [ -e "../release/soundkonverter-"$VERSION".tar.bz2" ]; then
  echo "release already exists; exiting"
  exit 0
fi

if [ ! -d "../release" ]; then
  mkdir "../release"
fi

cd "../release"

cp -a "../src" "./tmp"
mv "./tmp" "./soundkonverter-"$VERSION

tar --exclude="*~" -cjf "./soundkonverter-"$VERSION".tar.bz2" "./soundkonverter-"$VERSION
echo "new release written to: release/soundkonverter-"$VERSION".tar.bz2"

rm -rf "./soundkonverter-"$VERSION

exit 0
