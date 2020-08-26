//
//  PCefRenderHandler.hpp
//  cef-test
//
//  Created by Dmitriy Tseyler  on 20.07.2020.
//

#ifndef PCefRenderHandler_hpp
#define PCefRenderHandler_hpp

#include "library.h"
#include "include/cef_browser.h"
#include "include/cef_render_handler.h"

class PCefRenderHandler : public CefRenderHandler {
 public:
  explicit PCefRenderHandler(const void* context,PaintFn render, RectFn rect,
                             ScreenInfoFn screen_info);
  void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) OVERRIDE;
  void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height) OVERRIDE;
  bool GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) OVERRIDE;
 private:
  PaintFn _render;
  RectFn _rect;
  ScreenInfoFn _screen_info;
  const void* _context;
 IMPLEMENT_REFCOUNTING(PCefRenderHandler);
};

#endif /* PCefRenderHandler_hpp */
