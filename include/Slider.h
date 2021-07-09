#pragma once

#include <Future.h>
#include <Screen.h>
#include <optional>
#include <vector>

using namespace uniuno;

class Slider {
public:
  Slider(std::vector<std::reference_wrapper<Screen>> s) : screens(s) {}

  std::optional<Future<void, void>> next_screen() {
    if (this->is_loading) {
      return {};
    }
    this->is_loading = true;

    if (this->index > this->screens.size() - 1) {
      this->index = 0;
    }

    return this->screens[this->index++].get().load().and_then(
        [this]() { this->is_loading = false; });
  }

private:
  bool is_loading = false;
  uint8_t index = 0;
  std::vector<std::reference_wrapper<Screen>> screens;
};
