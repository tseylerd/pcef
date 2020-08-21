//
// Created by Dmitriy Tseyler  on 03.08.2020.
//

#include "PCefApp.h"
#include "mac_util.h"

PCefApp::PCefApp() {
}

CefRefPtr<CefBrowserProcessHandler> PCefApp::GetBrowserProcessHandler() {
  return this;
}

void PCefApp::OnScheduleMessagePumpWork(int64 delay_ms) {
  mac_util::do_message_loop_work(uint64_t(delay_ms));
}
