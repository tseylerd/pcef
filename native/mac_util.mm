// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#import "mac_util.h"
#import <Cocoa/Cocoa.h>

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
  void on_main_queue(dispatch_block_t block) {
    if ([NSThread isMainThread]) {
      block();
    }
    else {
      dispatch_sync(dispatch_get_main_queue(), block);
    }
  }

  void run_mac_loop() {
    CFRunLoopRun();
  }
  
  void log(const char * message) {
    NSString* str = [NSString stringWithUTF8String:message];
    NSLog(@"%@", str);
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
      for (size_t i = 0; i < 10; i++) {
        CefDoMessageLoopWork();
      }
      CefShutdown();
    });
  }
}
