#!/bin/bash

VERSION="0.9.94rc2"

if [ -e "../release/soundkonverter-"$VERSION".tar.gz" ]; then
  echo "release already exists; exiting"
  exit 0
fi

if [ ! -d "../release" ]; then
  mkdir "../release"
fi

cd "../release"

cp -a "../src" "./tmp"
mv "./tmp" "./soundkonverter-"$VERSION

tar --exclude="*~" -czf "./soundkonverter-"$VERSION".tar.gz" "./soundkonverter-"$VERSION
echo "new release written to: release/soundkonverter-"$VERSION".tar.gz"

rm -rf "./soundkonverter-"$VERSION

exit 0
