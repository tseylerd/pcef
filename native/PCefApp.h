//
// Created by Dmitriy Tseyler  on 03.08.2020.
//

#ifndef PCEF_PCEFAPP_H
#define PCEF_PCEFAPP_H

#include <include/cef_app.h>

class PCefApp : public CefApp, public CefBrowserProcessHandler {
 public:
  PCefApp();
  ~PCefApp();
  CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE;
  void OnScheduleMessagePumpWork(int64 delay_ms) OVERRIDE = 0;

  static CefRefPtr<PCefApp> Create();

 protected:
  void OnScheduleWorkMainThread(int64 delay_ms);

  virtual bool IsWorkPending() = 0;
  virtual void Schedule(int64_t delay) = 0;
  virtual void Stop() = 0;
  virtual void DoWork() = 0;

 private:
  IMPLEMENT_REFCOUNTING(PCefApp);
  DISALLOW_COPY_AND_ASSIGN(PCefApp);
};

#endif  // PCEF_PCEFAPP_H
