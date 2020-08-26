//
// Created by Dmitriy Tseyler  on 21.07.2020.
//

#ifndef CEF_PCEFCLIENT_H
#define CEF_PCEFCLIENT_H

#include "PCefRenderHandler.h"
#include "include/cef_base.h"
#include "include/cef_client.h"

class PCefClient : public CefClient, public CefLifeSpanHandler {
 public:
  PCefClient(const void* context, BrowserId id, PaintFn render, RectFn rect,
             ScreenInfoFn screen_info);
  CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE;
  CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE;
  void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
  bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
  void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

 private:
  CefRefPtr<PCefRenderHandler> _handler;
  BrowserId _id;
  IMPLEMENT_REFCOUNTING(PCefClient);
  DISALLOW_COPY_AND_ASSIGN(PCefClient);
};


#endif  // CEF_PCEFCLIENT_H
