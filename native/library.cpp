#include <include/cef_app.h>
#include <include/wrapper/cef_library_loader.h>
#include <chrono>
#include "PCefApp.h"
#include "PCefClient.h"
#include "mac_util.h"
#include "util.h"

using namespace std;

void log(const char* message) {
//  LOG(VERBOSE) << message;
}

namespace browsers {
  void save_browser(BrowserId id, const CefRefPtr<CefBrowser>& browser) {
    auto cef_id = (BrowserId)browser->GetIdentifier();
    id_to_browser[id] = browser;
    cef_id_to_id[cef_id] = id;
  }

  CefRefPtr<CefBrowser> get_browser(BrowserId id) {
    return id_to_browser[id];
  }

  void close_browser(BrowserId id) {
    log("Closing browser");
    CefRefPtr<CefBrowser> browser = id_to_browser[id];
    if (browser) {
      log("Browser found");
      mac_util::close_browser(browser);
    }
  }

  void erase_browser(BrowserId id) {
    log("Erasing browser");
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
    return cef_id_to_id[browser->GetIdentifier()];
  }

  bool is_empty() {
    return id_to_browser.empty();
  }
}

bool load_url(const void* context, const char* url, BrowserId id,
              PaintFn paint_fn,
              RectFn rect_fn) {
  log("Loading url...\n");
  CefRefPtr<CefClient> client(new PCefClient(context, id, paint_fn, rect_fn));
  CefWindowInfo info;
  info.SetAsWindowless(nullptr);
  CefBrowserSettings browserSettings;
  bool result = CefBrowserHost::CreateBrowser(
      info, client, url, browserSettings, nullptr, nullptr);
  return result;
}

void close_browser(BrowserId id) {
  browsers::close_browser(id);
}

bool init_browser(const BrowserPaths* paths) {
  if (paths->framework_path) {
    size_t fp_len = strlen(paths->framework_path);
    size_t s_len = strlen("/Chromium Embedded Framework");
    char result[fp_len + s_len + 1];
    util::concat(result, paths->framework_path, "/Chromium Embedded Framework");
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
  std::vector<const std::string> args_vector;
  if (paths->framework_path) {
    args_vector.emplace_back("--framework-dir-path=" +std::string(paths->framework_path));
    CefString(&settings.framework_dir_path) = paths->framework_path;
  }
  if (paths->subprocess_path) {
    args_vector.emplace_back("--browser-subprocess-path=" + std::string(paths->subprocess_path));
    CefString(&settings.browser_subprocess_path) =paths->subprocess_path;
  }
  if (paths->main_bundle_path) {
    args_vector.emplace_back("--main-bundle-path=" +std::string(paths->main_bundle_path));
    CefString(&settings.main_bundle_path) =paths->main_bundle_path;
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
  CefRefPtr<CefApp> app(new PCefApp());
  return mac_util::initialize(cefArgs, settings, app);
}

void resized(BrowserId id) {
  log("Browser resized...\n");
  CefRefPtr<CefBrowser> browser = browsers::get_browser(id);
  if (browser) {
    browser->GetHost()->WasResized();
  }
}

bool stop_browser() {
  log("Stopping browser...\n");
  state::terminate();
  mac_util::finish();
  return true;
}

void run_test_loop() {
  mac_util::run_mac_loop();
}
