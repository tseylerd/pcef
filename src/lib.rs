use std::ffi::{c_void};
use std::os::raw::c_char;

#[repr(C)]
pub struct BrowserRect {
    pub x: i32,
    pub y: i32,
    pub width: i32,
    pub height: i32
}

#[repr(C)]
pub struct BrowserPaths {
    pub framework_path: * const libc::c_char,
    pub main_bundle_path: * const libc::c_char,
    pub subprocess_path: * const libc::c_char
}

extern {
    pub fn init_browser(paths: * const BrowserPaths) -> bool;

    pub fn load_url(context: *mut c_void,
                    url: * const c_char,
                    browser_id: u64,
                    render: unsafe extern "C" fn(context: *mut c_void, u64, popup: bool, bytes: *const u8, width: i32, height: i32),
                    rect: unsafe extern "C" fn(context: *mut c_void, u64) -> BrowserRect) -> bool;

    pub fn stop_browser() -> bool;

    pub fn resized(id: u64);

    pub fn close_browser(id: u64);

    pub fn run_test_loop();
}
