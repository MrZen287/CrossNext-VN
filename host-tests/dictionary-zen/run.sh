#!/bin/sh
set -eu
mkdir -p .crosshihi-test
c++ -std=c++20 -O1 -Isrc -Ifreeink-sdk/libs/ui/FreeInkUI/include host-tests/dictionary-zen/test.cpp -o .crosshihi-test/dictionary-zen
.crosshihi-test/dictionary-zen
