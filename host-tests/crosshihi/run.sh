#!/bin/sh
set -eu
cd "$(dirname "$0")/../.."
mkdir -p .crosshihi-test
c++ -std=c++20 -Wall -Wextra -Werror -Ifreeink-sdk/libs/ui/FreeInkUI/include host-tests/crosshihi/test.cpp src/apps_local/home/ReadingDeskLayout.cpp -o .crosshihi-test/tests
.crosshihi-test/tests
