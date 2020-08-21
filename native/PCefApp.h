//
// Created by Dmitriy Tseyler  on 03.08.2020.
//

#ifndef PCEF_PCEFAPP_H
#define PCEF_PCEFAPP_H

#include <include/cef_app.h>

class PCefApp : public CefApp, public CefBrowserProcessHandler {
 public:
  PCefApp();
  CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE;
  void OnScheduleMessagePumpWork(int64 delay_ms) OVERRIDE;
 private:
 IMPLEMENT_REFCOUNTING(PCefApp);
  DISALLOW_COPY_AND_ASSIGN(PCefApp);
};

#endif  // PCEF_PCEFAPP_H
