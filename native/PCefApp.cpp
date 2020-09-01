//
// Created by Dmitriy Tseyler  on 03.08.2020.
//

#include "PCefApp.h"
#include "library.h"

PCefApp::PCefApp() {
}

CefRefPtr<CefBrowserProcessHandler> PCefApp::GetBrowserProcessHandler() {
  return this;
}

void PCefApp::OnScheduleWorkMainThread(int64 delay_ms) {
  if (IsWorkPending()) {
    Stop();
  }
  if (state::is_terminated()) {
    return;
  }
  if (delay_ms <= 0) {
    DoWork();
    return;
  }
  auto max_delay = (uint64_t)delay_ms > time_consts::max_loop_delay ? time_consts::max_loop_delay : delay_ms;
  Schedule(max_delay);
}
