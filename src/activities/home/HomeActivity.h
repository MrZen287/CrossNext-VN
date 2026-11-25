#pragma once
#include <Bitmap.h>

#include <array>
#include <atomic>
#include <functional>
#include <vector>

#include "../../apps_local/home/HomeDashboardUi.h"
#include "../../apps_local/home/HomeMenuInput.h"
#include "../../apps_local/home/HomeMenuIntegration.h"
#include "../../apps_local/home/ReadingDesk.h"
#include "../../apps_local/home/HomeLauncher.h"
#include "../../apps_local/library/PreparedCoverCache.h"
#include "./FileBrowserActivity.h"
#include "CrossPointSettings.h"
#include "RecentBooksStore.h"
#include "activities/Activity.h"
// Not a forward declaration: the std::vector<RecentBook> member below needs
// the complete type wherever this header's implicit destructor instantiates,
// and the host simulator build (libc++) instantiates it in every including TU.
#include "RecentBooksStore.h"
#include "components/CoverGridHomeUi.h"
#include "util/ButtonNavigator.h"

struct Rect;

class HomeActivity final : public Activity {
  libraryui::PreparedCoverCache libraryWarmup;
  HalFile libraryWarmFile;
  Bitmap libraryWarmBitmap{libraryWarmFile};
  std::atomic<bool> libraryWarmInterrupted{true};
  std::atomic<unsigned> libraryWarmIndex{0};
  unsigned long libraryWarmInputAt = 0;
  size_t gridPreparationIndex = 0, shelfNativeIndex = 0;
  uint32_t libraryWarmMediaRevision = 0;
  bool prepareGridCover(RecentBook& book, int height);
  bool nativeShelfPending() const {
    return coverGridUi && SETTINGS.homeLayout == CrossPointSettings::HOME_LICORICE &&
           shelfNativeIndex < recentBooks.size();
  }
  uint32_t libraryWarmRevision = 0, libraryWarmArtRevision = 0;
  uint8_t libraryWarmOrientation = 0;
  char libraryWarmPath[112]{};
  freeink::ui::Rect libraryWarmArea{};
  int libraryWarmFallbackHeight = 0;
  void configureLibraryWarmup();
  static bool continueLibraryWarmup(void* context);
  std::unique_ptr<CoverGridHomeUi> coverGridUi;
  ButtonNavigator buttonNavigator{140, 400};
  homeui::MenuInput menuInput;
  HomeDashboardUi dashboard;
  readingdesk::Layout deskLayout{};
  readingdesk::Action deskFocus = readingdesk::None;
  std::vector<RecentBook> deskShelf;
  std::vector<int> deskPageStarts;
  struct DeskPage {
    std::vector<RecentBook> books;
    int start = 0, next = -1;
    bool valid = false;
  };
  DeskPage deskPrefetch;
  int deskPrefetchCover = 0;
  bool deskLibraryPending = true;
  int deskPage = 0, deskNextPageStart = -1;
  uint32_t deskShelfRevision = 0;
  int deskFeatured = -1, deskPrepared = -2, deskCoverHeight = 0;
  std::array<int, readingdesk::shelfSize> deskShelfHeights{};
  bool deskMenuOpen = false;
  unsigned deskCatPose = 0;
  unsigned deskWormPose = 0;
  const char* deskQuote = "";
  uint32_t deskMinute = 0;
  static bool readingDeskEnabled() { return true; }
  bool launcherOpen = false, launcherStatsPending = false;
  int launcherPage = 0, launcherDay = 0;
  homelauncher::Layout launcherLayout{};
  homelauncher::Model launcherModel{};
  std::array<int, 24> launcherItems{};
  int launcherCount = 0;
  void openLauncher();
  void loopLauncher();
  void renderLauncher();
  void loopReadingDesk();
  DeskPage collectDeskPage(int start, bool useLibrary);
  void loadDeskShelfPage(int page, bool useLibrary = true);
  RecentBook* deskBook();
  int nextDeskShelfCover() const;
  void activateReadingDesk(readingdesk::Action action);
  int selectorIndex = 0;
  int recentBookIndex = 0;
  int recentBookCount = 0;
  int shelfPage = 0, shelfPages = 1, shelfPageSize = 0;
  int recentSourceIndices[RecentBooksStore::MAX_RECENT_BOOKS]{};
  int displayBookCount() const {
    return coverGridUi ? static_cast<int>(recentBooks.size()) : recentBooks.empty() ? 0 : 1;
  }
  enum class EntryPhase : uint8_t { Blocking, Shell, Preparing, Painting, Ready };
  std::atomic<EntryPhase> entryPhase{EntryPhase::Blocking};
  void initializeContent();
  bool menuCursorVisible = false;
  int paintedSelection() const {
    return SETTINGS.homeLayout == CrossPointSettings::HOME_BOOX && !menuCursorVisible ? -1 : selectorIndex;
  }
  bool recentsLoading = false;
  bool recentsLoaded = false;
  bool firstRenderDone = false;
  bool dashboardDirty = true;
  uint32_t lastHomePaint = 0;
  bool hasOpdsServers = false;
  bool hasPlugins = false;
  // The optional services row opens installed Plugins, otherwise configured OPDS.
  bool hasLibrarySlot() const { return hasPlugins || hasOpdsServers; }
  bool hasContinueReading = false;
  bool coverBufferStored = false;          // Track if cover buffer is stored
  std::unique_ptr<uint8_t[]> coverBuffer;  // HomeActivity's own buffer for cover image
  size_t coverBufferSize = 0;              // Bytes allocated to coverBuffer
  // Cover art is reused on full redraws; focus updates repaint only menu rows.
  int coverRectX = 0;
  int coverRectY = 0;
  int coverRectW = 0;
  int coverRectH = 0;
  std::vector<RecentBook> recentBooks;
  const HomeMenuItem initialMenuItem;
  const bool cleanInitialRefresh;

  // Convert HomeMenuItem to menu index (used in onEnter)
  static int menuItemToIndex(HomeMenuItem item, bool hasOpdsUrl) {
    if (SETTINGS.homeLayout == CrossPointSettings::HOME_BOOX) {
      for (int n = 0; n < homeui::kPiccoActions; ++n)
        if (indexToMenuItem(n, hasOpdsUrl) == item) return n;
      return 0;
    }
    const int index = homemenu::indexOf(homemenu::config(), homemenu::idFor(item), hasOpdsUrl);
    return index < 0 ? 0 : index;
  }

  // Convert menu index to HomeMenuItem (used in loop)
  static HomeMenuItem indexToMenuItem(int idx, bool hasOpdsUrl) {
    if (SETTINGS.homeLayout == CrossPointSettings::HOME_BOOX) {
      static constexpr HomeMenuItem actions[] = {
          HomeMenuItem::FILE_BROWSER, HomeMenuItem::LIBRARY, HomeMenuItem::FILE_TRANSFER, HomeMenuItem::READING_STATS,
          HomeMenuItem::APPS,         HomeMenuItem::GAMES,   HomeMenuItem::SETTINGS_MENU};
      return idx >= 0 && idx < homeui::kPiccoActions ? actions[idx] : HomeMenuItem::NONE;
    }
    return homemenu::item(homemenu::at(homemenu::config(), idx, hasOpdsUrl));
  }
  void onSelectBook(const std::string& path);
  void onFileBrowserOpen();
  void onLibraryOpen();
  void onSettingsOpen();
  void onFileTransferOpen();
  void onOpdsBrowserOpen();
  void onPluginsOpen();

  int getMenuItemCount() const;
  bool storeCoverBuffer();    // Store frame buffer for cover image
  bool restoreCoverBuffer();  // Restore frame buffer from stored cover
  void freeCoverBuffer();     // Free the stored cover buffer
  void loadRecentBooks(int maxBooks);
  bool loadRecentCovers(int coverHeight);
  void fillCoverGridFromLibrary();
  void loadShelfPage();
  void resolveGridCoverPaths();
  void loadGridCover(RecentBook& book, int height, bool& showingLoading, Rect& popupRect);

 public:
  explicit HomeActivity(GfxRenderer& renderer, MappedInputManager& mappedInput,
                        HomeMenuItem initialMenuItemValue = HomeMenuItem::NONE, bool cleanInitialRefresh = false)
      : Activity("Home", renderer, mappedInput),
        dashboard(renderer),
        initialMenuItem(initialMenuItemValue),
        cleanInitialRefresh(cleanInitialRefresh) {
    setInterfaceFontSection(menufont::Section::Home);
  }
  void onEnter() override;
  void onReturnFromChild() override;
  void onExit() override;
  void loop() override;
  std::optional<uint32_t> nextLoopWakeDelayMs(uint32_t) const override {
    if (readingDeskEnabled()) return entryPhase == EntryPhase::Ready ? 1000 : 20;
    return entryPhase != EntryPhase::Ready || recentsLoading || libraryWarmIndex.load() < 9 || nativeShelfPending()
               ? 20
               : 1000;
  }
  void runIdleWork() override;
  void render(RenderLock&&) override;
  bool isHomeActivity() const override { return true; }
};
