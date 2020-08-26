//
//  library.h
//
//  Created by Dmitriy Tseyler  on 20.07.2020.
//

#include <objc/objc.h>
#include <cstdint>
#include <cstdio>
#include "include/cef_browser.h"
#include "include/base/cef_lock.h"

#ifndef LIBRARY_H
#define LIBRARY_H

#ifndef EXPORT_API
#define EXPORT_API __attribute__ ((visibility("default")))
#endif

struct BrowserRect {
  int32_t x;
  int32_t y;
  int32_t width;
  int32_t height;
};

typedef uint64_t BrowserId;
typedef void (*PaintFn)(const void*, BrowserId, bool, const uint8_t*, int, int);
typedef BrowserRect (*RectFn)(const void*, BrowserId);

namespace time_consts {
  static uint64_t _fps = 30;
  static uint64_t max_loop_delay = 1000 / _fps;
}

struct LoadInfo {
  const void* context;
  const char* url;
  BrowserId browser_id;
  const PaintFn paint_fn;
  const RectFn rect_fn;
};

struct BrowserPaths {
 public:
  const char* framework_path;
  const char* main_bundle_path;
  const char* subprocess_path;
};


void log(const char* message);

extern "C" EXPORT_API bool load_url(const LoadInfo& url_info);
extern "C" EXPORT_API void change_url(BrowserId id, const char* text);
extern "C" EXPORT_API bool init_browser(const BrowserPaths& configuration);
extern "C" EXPORT_API void resized(BrowserId id);
extern "C" EXPORT_API bool stop_browser();
extern "C" EXPORT_API void close_browser(BrowserId id);
extern "C" EXPORT_API void run_test_loop();

namespace browsers {
  CefRefPtr<CefBrowser> get_browser(BrowserId id);
  void close_browser(BrowserId id);
  void load_url(BrowserId id, const char* url);
  void erase_browser(BrowserId id);
  void save_browser(BrowserId  id, const CefRefPtr<CefBrowser>& browser);
  BrowserId get_id(CefRefPtr<CefBrowser>& browser);
  bool is_empty();
}

namespace state {
  void terminate();
  bool is_terminated();
}
#endif
