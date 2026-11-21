#include "ReadingBackupActivity.h"

#include <HalStorage.h>
#include <I18n.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <string_view>

#include "activities/util/ConfirmationActivity.h"
#include "apps_local/power/PowerMenuActivity.h"
#include "apps_local/ui/PixelMenuTheme.h"

namespace fui = freeink::ui;

namespace {
constexpr char STORE[] = "/.crosspoint/";
constexpr char ARCHIVES[] = "/Sao-luu-CrossHihi";
constexpr char FORMAT[8] = {'C', 'N', 'B', 'K', '0', '1', '\r', '\n'};
constexpr const char* TOP_FILES[] = {"settings.json", "state.json", "recent.json", "chicken-a.bin", "chicken-b.bin"};
constexpr const char* DATA_DIRS[] = {"bookmarks", "clippings", "reading-history", "pet-v2"};
constexpr size_t MAX_PATH = 800;
constexpr uint32_t MAX_FILE = 128 * 1024 * 1024;

bool safeName(std::string_view name) {
  return !name.empty() && name.front() != '.' && name.back() != ' ' && name.back() != '.' &&
         std::none_of(name.begin(), name.end(), [](unsigned char c) {
           return c < 0x20 || c == 0x7f || std::string_view("/\\:\"*?<>|").find(char(c)) != std::string_view::npos;
         });
}

bool bookDir(std::string_view dir) {
  if (!dir.starts_with("epub_") && !dir.starts_with("xtc_")) return false;
  const auto suffix = dir.substr(dir.find('_') + 1);
  return !suffix.empty() && std::all_of(suffix.begin(), suffix.end(), [](char c) { return c >= '0' && c <= '9'; });
}

bool allowed(std::string_view path) {
  const auto slash = path.find('/');
  if (slash == path.npos) return std::any_of(std::begin(TOP_FILES), std::end(TOP_FILES), [path](auto name) {
    return path == name;
  });
  const auto dir = path.substr(0, slash), file = path.substr(slash + 1);
  if (!safeName(file)) return false;
  if (dir == "bookmarks") return file.ends_with(".json");
  if (dir == "clippings") return file.ends_with(".bin") || file.ends_with(".bin.deleted");
  if (dir == "reading-history") return file == "sessions.bin";
  if (dir == "pet-v2") return file.ends_with(".bin");
  return bookDir(dir) && file == "progress.bin";
}

uint32_t recordHash(uint32_t hash, const void* data, size_t size) {
  const auto* bytes = static_cast<const uint8_t*>(data);
  for (size_t i = 0; i < size; ++i) hash = (hash ^ bytes[i]) * UINT32_C(16777619);
  return hash;
}

// Fixed integer layout. The record checksum covers both the destination and its bytes.
struct Record {
  uint32_t pathSize;
  uint32_t fileSize;
};
static_assert(sizeof(Record) == 8);

bool transfer(HalFile& source, HalFile* destination, uint32_t length, uint32_t& hash) {
  uint8_t buffer[1024];
  while (length) {
    const auto chunk = std::min<size_t>(sizeof(buffer), length);
    if (source.read(buffer, chunk) != int(chunk) || (destination && destination->write(buffer, chunk) != chunk))
      return false;
    hash = recordHash(hash, buffer, chunk);
    length -= chunk;
    if ((length & 0x3fff) < chunk) delay(1);
  }
  return true;
}

bool writeRecord(HalFile& archive, HalFile& source, const std::string& path) {
  if (path.size() >= MAX_PATH || source.fileSize64() > MAX_FILE) return false;
  const Record record{uint32_t(path.size()), uint32_t(source.fileSize64())};
  uint32_t hash = recordHash(2166136261u, &record, sizeof(record));
  hash = recordHash(hash, path.data(), path.size());
  return archive.write(&record, sizeof(record)) == sizeof(record) &&
         archive.write(path.data(), path.size()) == path.size() &&
         transfer(source, &archive, record.fileSize, hash) && archive.write(&hash, sizeof(hash)) == sizeof(hash);
}

bool writePath(HalFile& archive, const std::string& path) {
  const auto full = STORE + path;
  if (!Storage.exists(full.c_str())) return true;
  HalFile file;
  return Storage.openFileForRead("BACKUP", full, file) && !file.isDirectory() && writeRecord(archive, file, path);
}

bool writeData(HalFile& archive) {
  for (const auto* name : TOP_FILES)
    if (!writePath(archive, name)) return false;
  for (const auto* name : DATA_DIRS) {
    const auto folder = std::string(STORE) + name;
    if (!Storage.exists(folder.c_str())) continue;
    auto dir = Storage.open(folder.c_str());
    if (!dir || !dir.isDirectory()) return false;
    bool io = false;
    HalFile file;
    while (dir.openNextFile(file, io)) {
      char leaf[MAX_PATH]{};
      if (!file.getName(leaf, sizeof(leaf)) || file.isDirectory()) continue;
      const auto relative = std::string(name) + "/" + leaf;
      if (!allowed(relative) || relative.ends_with(".bin.deleted")) continue;
      if (!writeRecord(archive, file, relative)) return false;
      if (std::string_view(name) == "clippings" && !writePath(archive, relative + ".deleted")) return false;
    }
    if (io) return false;
  }

  auto dir = Storage.open("/.crosspoint");
  if (!dir) return false;
  bool io = false;
  HalFile entry;
  while (dir.openNextFile(entry, io)) {
    char leaf[40]{};
    if (entry.isDirectory() && entry.getName(leaf, sizeof(leaf)) && bookDir(leaf) &&
        !writePath(archive, std::string(leaf) + "/progress.bin"))
      return false;
  }
  return !io;
}

bool readArchive(const std::string& source, bool restore) {
  HalFile archive;
  char magic[sizeof(FORMAT)];
  if (!Storage.openFileForRead("BACKUP", source, archive) || archive.read(magic, sizeof(magic)) != sizeof(magic) ||
      std::memcmp(magic, FORMAT, sizeof(magic)) != 0)
    return false;

  for (;;) {
    Record record{};
    if (archive.read(&record, sizeof(record)) != sizeof(record)) return false;
    if (!record.pathSize) return record.fileSize == 0 && archive.position() == archive.size();
    if (record.pathSize >= MAX_PATH || record.fileSize > MAX_FILE) return false;
    char relative[MAX_PATH]{};
    if (archive.read(relative, record.pathSize) != int(record.pathSize) ||
        !allowed(std::string_view(relative, record.pathSize)))
      return false;
    uint32_t hash = recordHash(2166136261u, &record, sizeof(record));
    hash = recordHash(hash, relative, record.pathSize);

    HalFile out;
    const auto path = restore ? std::string(STORE) + relative : std::string();
    constexpr char tmp[] = "/.crosspoint/restore-part.tmp";
    if (restore && (!Storage.ensureDirectoryExists(path.substr(0, path.rfind('/')).c_str()) ||
                    !Storage.openFileForWrite("BACKUP", tmp, out))) return false;
    uint32_t expected = 0;
    const bool valid = transfer(archive, restore ? &out : nullptr, record.fileSize, hash) &&
                       archive.read(&expected, sizeof(expected)) == sizeof(expected) && hash == expected;
    if (restore) {
      const bool closed = out.close();
      if (!valid || !closed || !Storage.replaceFile(tmp, path.c_str())) {
        Storage.remove(tmp);
        return false;
      }
      // The primary clipping DB and its deletion journal are a pair. The
      // archive publishes the primary first, followed by its journal if any.
      if (std::string_view(relative).starts_with("clippings/") && std::string_view(relative).ends_with(".bin") &&
          Storage.exists((path + ".deleted").c_str()) && !Storage.remove((path + ".deleted").c_str()))
        return false;
    } else if (!valid) {
      return false;
    }
  }
}

std::string createArchive() {
  if (!Storage.ensureDirectoryExists(ARCHIVES) || !Storage.ensureDirectoryExists("/.crosspoint"))
    return {};
  const auto now = std::time(nullptr);
  std::tm clock{};
  char stamp[24] = "Chua-dat-gio";
  if (now >= 1577836800 && localtime_r(&now, &clock))
    std::strftime(stamp, sizeof(stamp), "%Y-%m-%d_%H%M%S", &clock);
  std::string path;
  for (unsigned n = 0; n < 1000; ++n) {
    char name[64];
    std::snprintf(name, sizeof(name), "%s/%s-%03u.cnbak", ARCHIVES, stamp, n);
    if (!Storage.exists(name)) {
      path = name;
      break;
    }
  }
  if (path.empty()) return {};
  const auto tmp = path + ".tmp";
  HalFile archive;
  if (!Storage.openFileForWrite("BACKUP", tmp, archive)) return {};
  const bool wrote = archive.write(FORMAT, sizeof(FORMAT)) == sizeof(FORMAT) && writeData(archive);
  // The zero path length is a required end marker. Read-back below checks every record.
  const Record end{};
  const bool finished = wrote && archive.write(&end, sizeof(end)) == sizeof(end);
  const bool closed = archive.close();
  if (!finished || !closed || !readArchive(tmp, false) || !Storage.rename(tmp.c_str(), path.c_str())) {
    Storage.remove(tmp.c_str());
    return {};
  }
  return path;
}

void invalidateDerivedStats() {
  auto dir = Storage.open("/.crosspoint/reading-history");
  if (!dir) return;
  bool io = false;
  HalFile file;
  while (dir.openNextFile(file, io)) {
    char name[100]{};
    if (!file.getName(name, sizeof(name)) || file.isDirectory()) continue;
    const std::string_view leaf(name);
    if (!safeName(leaf) || (!leaf.ends_with(".idx") && leaf != "goal-days.bin")) continue;
    file.close();
    Storage.remove((std::string("/.crosspoint/reading-history/") + name).c_str());
  }
}
}

void ReadingBackupActivity::onEnter() {
  refresh();
  UiListActivity::onEnter();
}

const char* ReadingBackupActivity::headerTitle() const { return tr(STR_READING_BACKUP); }

void ReadingBackupActivity::refresh() {
  files.clear();
  auto dir = Storage.open(ARCHIVES);
  if (!dir) return;
  bool io = false;
  HalFile file;
  while (dir.openNextFile(file, io)) {
    char name[70]{};
    if (!file.getName(name, sizeof(name)) || file.isDirectory() || !safeName(name) ||
        !std::string_view(name).ends_with(".cnbak"))
      continue;
    const auto pos = std::lower_bound(files.begin(), files.end(), name, std::greater<>{});
    if (files.size() < 32 || pos != files.end()) {
      files.insert(pos, name);
      if (files.size() > 32) files.pop_back();
    }
  }
}

void ReadingBackupActivity::buildScreen(UiScreen& screen) {
  pixelui::FontScope fonts(uiTarget, renderer, true);
  pixelui::begin(screen, 44);
  fui::ListProps props{};
  props.count = listCount();
  props.action = ACTION_ROW;
  props.inputMask = fui::InputTouch;
  props.rowHeight = 54;
  props.labelText.font = pixelui::kSmall;
  props.valueText.font = pixelui::kSmall;
  props.rowProviderCtx = this;
  props.rowProvider = [](void* user, uint16_t index, fui::ListItem& item) {
    const auto& self = *static_cast<ReadingBackupActivity*>(user);
    item.label = index == 0 ? tr(STR_READING_BACKUP_CREATE) : self.files[index - 1].c_str();
    item.value = index == 0 ? "" : tr(STR_READING_BACKUP_RESTORE);
    item.actionValue = index;
  };
  syncListViewport(screen, props);
  screen.list(props);
}

void ReadingBackupActivity::acknowledge(const char* body, bool restart) {
  startActivityForResult(std::make_unique<ConfirmationActivity>(renderer, mappedInput, headerTitle(), body, true),
                         [this, restart](const ActivityResult&) {
    if (restart) powermenu::request(powermenu::Action::Restart);
    else requestUpdate();
  });
}

void ReadingBackupActivity::activateIndex(int index) {
  app.clearTapFlash();
  if (index == 0) {
    const auto path = createArchive();
    { RenderLock lock; refresh(); }
    acknowledge(path.empty() ? tr(STR_READING_BACKUP_FAILED) : tr(STR_READING_BACKUP_SAVED));
    return;
  }
  const auto path = std::string(ARCHIVES) + "/" + files[index - 1];
  startActivityForResult(
      std::make_unique<ConfirmationActivity>(renderer, mappedInput, tr(STR_READING_BACKUP_RESTORE),
                                             tr(STR_READING_BACKUP_CONFIRM)),
      [this, path](const ActivityResult& result) {
        if (result.isCancelled) return;
        if (!readArchive(path, false) || createArchive().empty()) {
          acknowledge(tr(STR_READING_BACKUP_FAILED));
          return;
        }
        const bool restored = readArchive(path, true);
        invalidateDerivedStats();
        acknowledge(restored ? tr(STR_READING_BACKUP_DONE) : tr(STR_READING_BACKUP_INCOMPLETE), true);
      });
}

