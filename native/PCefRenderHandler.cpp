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
  _render(_context, client_id, false, (const uint8_t*)buffer, width, height);
}

bool PCefRenderHandler::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo &screen_info) {
  screen_info.device_scale_factor = 2;
  return true;
}

PCefRenderHandler::PCefRenderHandler(const void* context,
                                               PaintFn render, RectFn rect) {
  this->_render = render;
  this->_rect = rect;
  this->_context = context;
}
