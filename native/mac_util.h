#ifndef PCEF_NATIVE_MAC_UTIL_H_
#define PCEF_NATIVE_MAC_UTIL_H_

#import <include/cef_app.h>
#import <include/internal/cef_mac.h>
#import <include/internal/cef_types_wrappers.h>

namespace mac_util {
  void do_message_loop_work(uint64_t delay);
  void finish();
  bool initialize(const CefMainArgs& args,
                  const CefSettings& settings,
                  CefRefPtr<CefApp> client);
  void close_browser(CefRefPtr<CefBrowser> browser);
  void run_mac_loop();
  void log(const char* message);
}

#endif
