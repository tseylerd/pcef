// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#import "mac_util.h"

#import <Cocoa/Cocoa.h>
#import "library.h"

@interface InitializeParams : NSObject {
 @public
  CefMainArgs args;
  CefSettings settings;
  CefRefPtr<CefApp> client;
  bool result;
}
@end
@implementation InitializeParams
@end

@interface NSHelper: NSObject {}
+ (void)initialize:(InitializeParams*)params;
@end

@implementation NSHelper
  + (void)initialize:(InitializeParams*)params {
    params -> result = CefInitialize(params ->args, params ->settings, params ->client, NULL);
    CefEnableHighDPISupport();
  }
@end

namespace mac_util {
  static NSTimer* timer;

  void on_main_queue(dispatch_block_t block) {
    if ([NSThread isMainThread]) {
      block();
    }
    else {
      dispatch_sync(dispatch_get_main_queue(), block);
    }
  }

  void do_message_loop_work(uint64_t delay_ms) {
    dispatch_async(dispatch_get_main_queue(), ^{
      if (state::terminated) {
        return;
      }
      if (timer) {
        [timer invalidate];
        timer = nil;
      }
      if (delay_ms <= 0) {
        CefDoMessageLoopWork();
        do_message_loop_work(time_consts::max_loop_delay);
      }
      else {
        double max_delay = (double)delay_ms > time_consts::max_loop_delay ? time_consts::max_loop_delay : delay_ms;
        timer = [NSTimer scheduledTimerWithTimeInterval:max_delay / 1000
                                                repeats:YES
                                                  block:^(NSTimer *t) {
                                                    [timer invalidate];
                                                    timer = nil;
                                                    CefDoMessageLoopWork();
                                                    do_message_loop_work(time_consts::max_loop_delay);
                                                  }];
      }
    });
  }

  void run_mac_loop() {
    CFRunLoopRun();
  }
  
  void log(const char * message) {
    NSString* str = [NSString stringWithUTF8String:message];
    NSLog(@"%@", str);
  }

  void close_browser(CefRefPtr<CefBrowser> browser) {
    dispatch_async(dispatch_get_main_queue(), ^{
       browser->GetHost()->TryCloseBrowser();
    });
  }

  bool initialize(const CefMainArgs& args,
                  const CefSettings& settings,
                  CefRefPtr<CefApp> client) {
    InitializeParams* params = [[InitializeParams alloc] init];
    params->args = args;
    params->settings = settings;
    params->client = client;

    on_main_queue(^{
      [NSHelper initialize:params];
    });

    int result = params->result;
    [params release];
    return result;
  }

  void finish() {
    on_main_queue(^{
      while (!browsers::is_empty()) {
        CefDoMessageLoopWork();
      }
      CefShutdown();
    });
  }
}
