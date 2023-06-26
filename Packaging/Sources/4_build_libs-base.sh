#!/bin/bash

. ./versions.inc.sh

. /Developer/Makefiles/GNUstep.sh

git apply extra/icu68.patch --directory nextspace-base-${gnustep_base_version}/Source/

cd ./nextspace-base-${gnustep_base_version} || exit 1

$MAKE_CMD clean
./configure || exit 1

$MAKE_CMD install
