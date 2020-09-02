#import <dispatch/dispatch.h>
#import "PCefAppMac.h"
#import "library.h"

PCefAppMac::PCefAppMac() {
  _timer = nil;
}

void PCefAppMac::OnScheduleMessagePumpWork(int64_t delay_ms) {
  dispatch_async(dispatch_get_main_queue(), ^{
                     OnScheduleWorkMainThread(delay_ms);
                 });
}

bool PCefAppMac::IsWorkPending() {
  return _timer != nil;
}

void PCefAppMac::Stop() {
  if (_timer != nil) {
    [_timer invalidate];
    _timer = nil;
  }
}

void PCefAppMac::DoWork() {
  CefDoMessageLoopWork();
  Schedule(time_consts::max_loop_delay);
}

void PCefAppMac::Schedule(int64_t delay) {
  DCHECK(!IsWorkPending());

  auto max_delay = (double)delay;
  _timer = [NSTimer scheduledTimerWithTimeInterval:max_delay / 1000
                                          repeats:YES
                                            block:^(NSTimer *t) {
                                              if (state::is_terminated()) {
                                                return;
                                              }
                                              Stop();
                                              DoWork();
                                            }];
}

CefRefPtr<PCefApp> PCefApp::Create() {
  return CefRefPtr<PCefApp>(new PCefAppMac());
}
