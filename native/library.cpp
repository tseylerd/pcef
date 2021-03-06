#include <include/cef_app.h>
#include <include/wrapper/cef_closure_task.h>
#include <include/wrapper/cef_library_loader.h>
#include <chrono>
#include "PCefClient.h"
#include "util.h"
#import "PCefApp.h"

#if defined(OS_MACOSX)
#include "mac_util.h"
#endif

using namespace std;

void log(const char* message) {
//  mac_util::log(message);
//  LOG(VERBOSE) << message;
}

namespace browsers {
  static std::mutex mut;
  static std::map<BrowserId, CefRefPtr<CefBrowser>> id_to_browser;
  static std::map<BrowserId, BrowserId> cef_id_to_id;

  void save_browser(BrowserId id, const CefRefPtr<CefBrowser>& browser) {
    std::lock_guard<mutex> guard(mut);
    auto cef_id = (BrowserId)browser->GetIdentifier();
    id_to_browser[id] = browser;
    cef_id_to_id[cef_id] = id;
  }

  CefRefPtr<CefBrowser> get_browser(BrowserId id) {
    std::lock_guard<mutex> guard(mut);
    return id_to_browser[id];
  }

  void close_browser(BrowserId id) {
    std::lock_guard<mutex> guard(mut);
    CefRefPtr<CefBrowser> browser = id_to_browser[id];
    if (browser) {
      browser->GetHost()->CloseBrowser(true);
    }
  }

  void load_url(BrowserId id, const char* url) {
    std::lock_guard<mutex> guard(mut);
    CefRefPtr<CefBrowser> browser = id_to_browser[id];
    if (browser) {
      browser->GetMainFrame()->LoadURL(url);
    }
  }

  void erase_browser(BrowserId id) {
    log("Erasing browser...");
    std::lock_guard<mutex> guard(mut);
    CefRefPtr<CefBrowser> browser = id_to_browser[id];
    if (browser) {
      int cef_id = browser->GetIdentifier();
      id_to_browser.erase(id);
      cef_id_to_id.erase(cef_id);
    } else {
      log("Failed to erase browser");
    }
  }

  BrowserId get_id(CefRefPtr<CefBrowser>& browser) {
    std::lock_guard<mutex> guard(mut);
    return cef_id_to_id[browser->GetIdentifier()];
  }

  bool is_empty() {
    std::lock_guard<mutex> guard(mut);
    return id_to_browser.empty();
  }
}

namespace state {
  static bool terminated = false;

  bool is_terminated() {
    return terminated;
  }

  void terminate() {
    terminated = true;
  }
}

bool load_url(const LoadInfo& url_info) {
  log("Loading url...\n");
  CefRefPtr<CefClient> client(new PCefClient(url_info.context,
                                             url_info.browser_id,
                                             url_info.paint_fn,
                                             url_info.rect_fn,
                                             url_info.screen_info_fn));
  CefWindowInfo info;
  info.SetAsWindowless(nullptr);
  CefBrowserSettings browserSettings;
  bool result = CefBrowserHost::CreateBrowser(
      info, client, url_info.url, browserSettings, nullptr, nullptr);
  return result;
}

void change_url(BrowserId id, const char* url) {
  browsers::load_url(id, url);
}

void close_browser(BrowserId id) {
  browsers::close_browser(id);
}

void send_mouse_move_event(BrowserId id, const MouseMoveEvent& event) {
  auto browser = browsers::get_browser(id);
  if (browser) {
    CefMouseEvent e;
    e.x = event.info.x;
    e.y = event.info.y;
    e.modifiers = event.info.modifiers;
    browser->GetHost()->SendMouseMoveEvent(e, event.leave);
  }
}

void send_mouse_wheel_event(BrowserId id, const MouseWheelEvent& event) {
  auto browser = browsers::get_browser(id);
  if (browser) {
    CefMouseEvent e;
    e.x = event.info.x;
    e.y = event.info.y;
    e.modifiers = event.info.modifiers;
    browser->GetHost()->SendMouseWheelEvent(e, event.delta_x, event.delta_y);
  }
}

void send_mouse_click_event(BrowserId id, const MouseClickEvent& event) {
  if (event.button == Button::RIGHT) {
    return;
  }
  auto browser = browsers::get_browser(id);
  if (browser) {
    CefMouseEvent e;
    e.x = event.info.x;
    e.y = event.info.y;
    e.modifiers = event.info.modifiers;
    cef_mouse_button_type_t type = event.button == Button::LEFT ? cef_mouse_button_type_t::MBT_LEFT :
                                   event.button == Button::RIGHT ? cef_mouse_button_type_t::MBT_RIGHT :
                                   cef_mouse_button_type_t::MBT_MIDDLE;
    browser->GetHost()->SendMouseClickEvent(e, type, event.mouse_up, event.click_count);
  }
}

bool init_browser(const BrowserPaths& paths) {
  if (paths.framework_path) {
    size_t fp_len = strlen(paths.framework_path);
    size_t s_len = strlen("/Chromium Embedded Framework");
    char result[fp_len + s_len + 1];
    util::concat(result, paths.framework_path, "/Chromium Embedded Framework");
    if (!cef_load_library(result)) {
      return false;
    }
  } else {
    CefScopedLibraryLoader loader;
    if (!loader.LoadInMain()) {
      return false;
    }
  }

  CefSettings settings;
//  settings.log_severity = LOGSEVERITY_DEBUG;
  std::vector<const std::string> args_vector;
  if (paths.framework_path) {
    args_vector.emplace_back("--framework-dir-path=" +std::string(paths.framework_path));
    CefString(&settings.framework_dir_path) = paths.framework_path;
  }
  if (paths.subprocess_path) {
    args_vector.emplace_back("--browser-subprocess-path=" + std::string(paths.subprocess_path));
    CefString(&settings.browser_subprocess_path) =paths.subprocess_path;
  }
  if (paths.main_bundle_path) {
    args_vector.emplace_back("--main-bundle-path=" +std::string(paths.main_bundle_path));
    CefString(&settings.main_bundle_path) =paths.main_bundle_path;
  }
  args_vector.emplace_back("--disable-in-process-stack-traces");
  args_vector.emplace_back("--use-mock-keychain");
  args_vector.emplace_back("--disable-features=SpareRendererForSitePerProcess");
  args_vector.emplace_back("--multi_threaded_message_loop=false");
  args_vector.emplace_back("--external_message_pump=true");
  char* args[args_vector.size()];
  for (size_t i = 0; i < args_vector.size(); i++) {
    args[i] = strdup(args_vector[i].c_str());
  }
  CefMainArgs cefArgs(args_vector.size(), args);
  log("Scheduling initialization...\n");

  settings.command_line_args_disabled = false;
  settings.no_sandbox = true;
  settings.windowless_rendering_enabled = true;
  settings.multi_threaded_message_loop = false;
  settings.external_message_pump = 1;
#if defined(OS_MACOSX)
  CefRefPtr<CefApp> app = PCefApp::Create();
  return mac_util::initialize(cefArgs, settings, app);
#else
  return CefInitialize(args, settings, app, nullptr);
#endif
}

void resized(BrowserId id) {
  log("Browser resized...\n");
  CefRefPtr<CefBrowser> browser = browsers::get_browser(id);
  if (browser) {
    browser->GetHost()->WasResized();
  }
}

void screen_info_changed(BrowserId id) {
  log("Screen info changed...\n");
  CefRefPtr<CefBrowser> browser = browsers::get_browser(id);
  if (browser) {
    browser->GetHost()->NotifyScreenInfoChanged();
  }
}

bool stop_browser() {
  log("Stopping browser...\n");
  state::terminate();
#if defined(OS_MACOSX)
  mac_util::finish();
#else
  CefShutdown();
#endif
  return true;
}

void run_test_loop() {
#if defined(OS_MACOSX)
  mac_util::run_mac_loop();
#endif
}
