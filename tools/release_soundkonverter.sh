#!/bin/bash
#
# Copyright (c) 2010 - 2014 Daniel Faust <hessijames@gmail.com>
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.

VERSION=$1

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

tar --exclude="*~" --exclude=".git" --exclude=".kdev4" --exclude="TODO" -czf "./soundkonverter-"$VERSION".tar.gz" "./soundkonverter-"$VERSION
echo "new release written to: release/soundkonverter-"$VERSION".tar.gz"

rm -rf "./soundkonverter-"$VERSION

exit 0
