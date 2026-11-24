#!/bin/sh
set -eu
cd "$(dirname "$0")/../.."
mkdir -p .crosshihi-test
c++ -std=c++20 -Wall -Wextra -Werror -Ifreeink-sdk/libs/ui/FreeInkUI/include host-tests/crosshihi/test.cpp src/apps_local/home/ReadingDeskLayout.cpp -o .crosshihi-test/tests
.crosshihi-test/tests
c++ -std=c++20 -Wall -Wextra -Werror -Wno-comment -Ifreeink-sdk/libs/ui/FreeInkUI/include -Ifreeink-sdk/libs/assets/Icons/include -Ilib/I18n -Ilib/BlePageTurner/include -Ifreeink-sdk/libs/network/BleKeyboardHost/include host-tests/crosshihi/chrome.cpp src/apps_local/ui/PixelMenu.cpp freeink-sdk/libs/ui/FreeInkUI/src/FreeInkUI.cpp -o .crosshihi-test/chrome
.crosshihi-test/chrome
c++ -std=c++20 -Wall -Wextra -Werror -Wno-comment -Wno-unused-function -DZEN_SCREENS_TEST -Isrc -Ifreeink-sdk/libs/ui/FreeInkUI/include -Ifreeink-sdk/libs/assets/Icons/include -Ilib/I18n -Ilib/BlePageTurner/include -Ifreeink-sdk/libs/network/BleKeyboardHost/include host-tests/crosshihi/chrome.cpp src/apps_local/ui/PixelMenu.cpp src/apps_local/tools/ToolsScreens.cpp src/apps_local/tools/VietnameseLunar.cpp src/apps_local/tools/Almanac.cpp src/apps_local/reading/ReadingStatsCore.cpp src/apps_local/reading/ReadingStatsScreens.cpp src/apps_local/reading/ReadingGoalScreens.cpp freeink-sdk/libs/ui/FreeInkUI/src/FreeInkUI.cpp -o .crosshihi-test/screens
.crosshihi-test/screens
c++ -std=c++20 -Wall -Wextra -Werror -Wno-comment -Ifreeink-sdk/libs/ui/FreeInkUI/include -Ifreeink-sdk/libs/assets/Icons/include host-tests/crosshihi/launcher.cpp -o .crosshihi-test/launcher
.crosshihi-test/launcher
