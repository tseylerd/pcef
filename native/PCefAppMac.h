#include "PCefApp.h"
#import <Cocoa/Cocoa.h>

#ifndef PCEF_PCEFAPPMAC_H
#define PCEF_PCEFAPPMAC_H
class PCefAppMac: public PCefApp {
 public:
  PCefAppMac();
  void OnScheduleMessagePumpWork(int64_t delay_ms) OVERRIDE;
 protected:
  bool IsWorkPending() OVERRIDE;
  void DoWork() OVERRIDE;
  void Schedule(int64_t delay) OVERRIDE;
  void Stop() OVERRIDE;
 private:
  NSTimer* _timer;
  IMPLEMENT_REFCOUNTING(PCefAppMac);
  DISALLOW_COPY_AND_ASSIGN(PCefAppMac);
};
#endif  // PCEF_PCEFAPPMAC_H
