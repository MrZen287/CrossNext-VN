#!/bin/sh
set -eu
cd "$(dirname "$0")/../.."
mkdir -p .crosshihi-test
c++ -std=c++20 -Wall -Wextra -Werror -Ifreeink-sdk/libs/ui/FreeInkUI/include host-tests/dragon-home/test.cpp src/apps_local/home/ReadingDeskLayout.cpp -o .crosshihi-test/dragon-home
.crosshihi-test/dragon-home
