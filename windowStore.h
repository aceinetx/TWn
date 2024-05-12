#pragma once
#include <vector>

#include <cstring>
#include "./window.h"

inline std::vector<THDL*> window_store;

void appendWindow(THDL* window);

void removeWindowByTitle(const char* title);

void removeWindowByUID(int uid);

THDL* getWindowByTitle(const char* title);

THDL* getWindowByIndex(size_t idx);

size_t getWindowStoreLength();
