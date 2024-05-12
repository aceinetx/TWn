#include <vector>

#include <cstring>
#include "./window.h"

inline std::vector<THDL*> window_store;

inline void appendWindow(THDL* window);

inline void removeWindowByTitle(const char* title);

inline THDL* getWindowByTitle(const char* title);

inline THDL* getWindowByIndex(size_t idx);

inline size_t getWindowStoreLength();

inline THDL* getParentWindow(THDL* childWindow);
