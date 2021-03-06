#ifndef PCEF_NATIVE_MAC_UTIL_H_
#define PCEF_NATIVE_MAC_UTIL_H_

#import <include/cef_app.h>
#import <include/internal/cef_mac.h>
#import <include/internal/cef_types_wrappers.h>

namespace mac_util {
  void finish();
  bool initialize(const CefMainArgs& args,
                  const CefSettings& settings,
                  CefRefPtr<CefApp> client);
  void run_mac_loop();
  void log(const char* message);
}

#endif
