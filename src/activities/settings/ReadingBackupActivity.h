#pragma once

#include <string>
#include <vector>

#include "activities/UiListActivity.h"

class ReadingBackupActivity final : public UiListActivity {
 public:
  ReadingBackupActivity(GfxRenderer& renderer, MappedInputManager& input)
      : UiListActivity("ReadingBackup", renderer, input, false, true) {}
  void onEnter() override;

 private:
  int listCount() const override { return 1 + files.size(); }
  const char* headerTitle() const override;
  void buildScreen(UiScreen&) override;
  void activateIndex(int index) override;
  void refresh();
  void acknowledge(const char* body, bool restart = false);

  std::vector<std::string> files;
};

