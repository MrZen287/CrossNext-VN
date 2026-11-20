#pragma once

#include <string>

// Saved reading percentage, or -1 when unavailable. Never builds reading caches.
int loadBookProgress(const std::string& path);
struct BookPosition {
  int section = 0;  // One-based EPUB spine item, when applicable.
  int page = 0;    // One-based page within that section or fixed-layout book.
};
BookPosition loadBookPosition(const std::string& path);
// Activity/render-owned memoization; call only after a successful progress commit.
void publishBookProgress(const std::string& path, int percent);
