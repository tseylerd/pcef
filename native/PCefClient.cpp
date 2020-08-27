//
// Created by Dmitriy Tseyler  on 21.07.2020.
//

#include "PCefClient.h"

PCefClient::PCefClient(const void* context,
                       BrowserId id,
                       PaintFn render,
                       RectFn rect,
                       ScreenInfoFn screen_info) {
  _handler = new PCefRenderHandler(context, render, rect, screen_info);
  _id = id;
}

PCefClient::~PCefClient() {
  _handler = nullptr;
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
  log("DoClose");
  return false;
}

void PCefClient::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  log("OnBeforeClose");
  browsers::erase_browser(_id);
}
