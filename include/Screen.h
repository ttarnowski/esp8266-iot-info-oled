#pragma once
#include <Future.h>

class Screen {
public:
  virtual ~Screen() {}
  virtual uniuno::Future<void, void> load() = 0;
};