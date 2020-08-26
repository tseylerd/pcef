use std::ffi::{c_void};
use std::os::raw::c_char;

pub type BrowserId = u64;
pub type PaintFn = unsafe extern "C" fn(context: *const c_void, brwsr_id: BrowserId, popup: bool, bytes: *const u8, width: i32, height: i32);
pub type RectFn = unsafe extern "C" fn(context: *mut c_void, u64) -> BrowserRect;
pub type ScreenInfoFn = unsafe extern "C" fn(context: *mut c_void, u64) -> ScreenInfo;

#[repr(C)]
#[derive(Default, Debug)]
pub struct BrowserRect {
    pub x: i32,
    pub y: i32,
    pub width: i32,
    pub height: i32
}

#[repr(C)]
#[derive(Default, Debug)]
pub struct ScreenInfo {
    pub scale_factor: f32
}

#[repr(C)]
pub struct BrowserPaths {
    pub framework_path: * const c_char,
    pub main_bundle_path: * const c_char,
    pub subprocess_path: * const c_char
}

#[repr(C)]
pub struct LoadInfo {
    pub context: *const c_void,
    pub data: *const c_char,
    pub browser_id: BrowserId,
    pub paint_fn: PaintFn,
    pub rect_fn: RectFn,
    pub screen_info_fn: ScreenInfoFn
}

extern {
    pub fn init_browser(paths: &BrowserPaths) -> bool;

    pub fn load_url(info: &LoadInfo) -> bool;

    pub fn change_url(id: BrowserId, url: *const c_char);

    pub fn stop_browser() -> bool;

    pub fn resized(id: BrowserId);

    pub fn screen_info_changed(id: BrowserId);

    pub fn close_browser(id: BrowserId);

    pub fn run_test_loop();
}
