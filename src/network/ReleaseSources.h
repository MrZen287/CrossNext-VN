#pragma once

namespace release_sources {
constexpr const char* const kUrls[] = {
    "https://raw.githubusercontent.com/MrZen287/CrossNext-VN/crosshihi/ota.json",
    "https://api.github.com/repos/MrZen287/CrossNext-VN/releases/latest",
};
constexpr int kCount = static_cast<int>(sizeof(kUrls) / sizeof(kUrls[0]));
}  // namespace release_sources
