//
// Created by Dmitriy Tseyler  on 21.07.2020.
//

#include "PCefClient.h"

PCefClient::PCefClient(const void* context,
                                 BrowserId id,
                                 PaintFn render,
                                 RectFn rect) {
  _handler = new PCefRenderHandler(context, render, rect);
  _id = id;
}

CefRefPtr<CefRenderHandler> PCefClient::GetRenderHandler() {
  return _handler;
}

CefRefPtr<CefLifeSpanHandler> PCefClient::GetLifeSpanHandler() {
  return this;
}

void PCefClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  browsers::save_browser(_id, browser);
}

bool PCefClient::DoClose(CefRefPtr<CefBrowser> browser) {
  browsers::erase_browser(_id);
  _handler = nullptr;
  return false;
}
