//
//  PCefRenderHandler.cpp
//
//  Created by Dmitriy Tseyler  on 20.07.2020.
//

#include "PCefRenderHandler.h"

void PCefRenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
  auto result = _rect(_context, browsers::get_id(browser));
  rect.x = result.x;
  rect.y = result.y;
  rect.width = result.width;
  rect.height = result.height;
}

void PCefRenderHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height) {
  log("On paint");
  BrowserId client_id = browsers::get_id(browser);
  BrowserRect rects[dirtyRects.size()];
  for (size_t i = 0; i < dirtyRects.size(); i++) {
    const CefRect& oneRect = dirtyRects.at(i);
    rects[i] = {
        oneRect.x,
        oneRect.y,
        oneRect.width,
        oneRect.height
    };
  }
  _render(_context, client_id, false, dirtyRects.size(), rects, (const uint8_t*)buffer, width, height);
}

bool PCefRenderHandler::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo &screen_info) {
  const ScreenInfo& info = _screen_info(_context, browsers::get_id(browser));
  screen_info.device_scale_factor = info.scale_factor;
  return true;
}

PCefRenderHandler::PCefRenderHandler(const void* context,
                                     PaintFn render, 
                                     RectFn rect,
                                     ScreenInfoFn screen_info_fn) {
  this->_render = render;
  this->_rect = rect;
  this->_screen_info = screen_info_fn;
  this->_context = context;
}
