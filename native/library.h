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

namespace time_consts {
  static uint64_t _fps = 30;
  static uint64_t max_loop_delay = 1000 / _fps;
}

struct BrowserRect {
  int32_t x;
  int32_t y;
  int32_t width;
  int32_t height;
};

struct BrowserPaths {
 public:
  const char* framework_path;
  const char* main_bundle_path;
  const char* subprocess_path;
};

typedef uint64_t BrowserId;
typedef void (*PaintFn)(const void*, BrowserId, bool, const uint8_t*, int, int);
typedef BrowserRect (*RectFn)(const void*, BrowserId);

void log(const char* message);

extern "C" EXPORT_API bool load_url(const void* context,
                                    const char* url,
                                    BrowserId id,
                                    PaintFn paint_fn,
                                    RectFn rect_fn);

extern "C" EXPORT_API bool init_browser(const BrowserPaths* configuration);

extern "C" EXPORT_API void resized(BrowserId id);
extern "C" EXPORT_API bool stop_browser();
extern "C" EXPORT_API void close_browser(BrowserId id);
extern "C" EXPORT_API void run_test_loop();

namespace browsers {
  static std::map<BrowserId, CefRefPtr<CefBrowser>> id_to_browser;
  static std::map<BrowserId, BrowserId> cef_id_to_id;
  static base::Lock lock_;

  CefRefPtr<CefBrowser> get_browser(BrowserId id);
  void close_browser(BrowserId id);
  void erase_browser(BrowserId id);
  void save_browser(BrowserId  id, const CefRefPtr<CefBrowser>& browser);
  BrowserId get_id(CefRefPtr<CefBrowser>& browser);
  bool is_empty();
}

namespace state {
  static bool terminated = false;

  void terminate() {
    terminated = true;
  }
}
#endif
