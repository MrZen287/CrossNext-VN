#!/bin/sh
set -eu
cd "$(dirname "$0")/../.."
mkdir -p .crosshihi-test
c++ -std=c++20 -Wall -Wextra -Werror -Ihost-tests/bookpet/stubs host-tests/bookpet/test.cpp src/apps_local/bookpet/PetState.cpp -o .crosshihi-test/bookpet
.crosshihi-test/bookpet
