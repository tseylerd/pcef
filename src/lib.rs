use std::ffi::{c_void};
use std::os::raw::c_char;

pub static LEFT_MOUSE_BUTTON: u32 = 1 << 4;
pub static MIDDLE_MOUSE_BUTTON: u32 = 1 << 5;
pub static RIGHT_MOUSE_BUTTON: u32 = 1 << 6;

pub type BrowserId = u64;
pub type PaintFn = unsafe extern "C" fn(context: *const c_void,
                                        brwsr_id: BrowserId,
                                        popup: bool,
                                        dirty_count: usize,
                                        dirty_rects: *const BrowserRect,
                                        bytes: *const u8,
                                        width: i32,
                                        height: i32);
pub type RectFn = unsafe extern "C" fn(context: *mut c_void, u64) -> BrowserRect;
pub type ScreenInfoFn = unsafe extern "C" fn(context: *mut c_void, u64) -> ScreenInfo;

pub trait MouseEvent {
    unsafe fn send_me(&self, id: BrowserId);
}

#[repr(C)]
#[derive(Default, Debug, Copy, Clone)]
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

#[repr(C)]
pub enum Button {
    LEFT,
    RIGHT,
    MIDDLE
}

#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct MouseInfo {
    pub x: i32,
    pub y: i32,
    pub modifiers: u32
}

#[repr(C)]
pub struct MouseClickEvent {
    pub info: MouseInfo,
    pub button: Button,
    pub click_count: i32,
    pub mouse_up: bool
}

#[repr(C)]
pub struct MouseMoveEvent {
    pub info: MouseInfo,
    pub leave: bool
}

#[repr(C)]
pub struct MouseWheelEvent {
    pub info: MouseInfo,
    pub delta_x: i32,
    pub delta_y: i32
}

impl MouseEvent for MouseWheelEvent {
    unsafe fn send_me(&self, id: BrowserId) {
        send_mouse_wheel_event(id, self);
    }
}
impl MouseEvent for MouseMoveEvent {
    unsafe fn send_me(&self, id: BrowserId) {
        send_mouse_move_event(id, self);
    }
}
impl MouseEvent for MouseClickEvent {
    unsafe fn send_me(&self, id: BrowserId) {
        send_mouse_click_event(id, self);
    }
}

extern {
    pub fn init_browser(paths: &BrowserPaths) -> bool;

    pub fn load_url(info: &LoadInfo) -> bool;

    pub fn change_url(id: BrowserId, url: *const c_char);

    pub fn stop_browser() -> bool;

    pub fn resized(id: BrowserId);

    pub fn screen_info_changed(id: BrowserId);

    pub fn close_browser(id: BrowserId);

    pub fn send_mouse_move_event(id: BrowserId, event: &MouseMoveEvent);

    pub fn send_mouse_click_event(id: BrowserId, event: &MouseClickEvent);

    pub fn send_mouse_wheel_event(id: BrowserId, event: &MouseWheelEvent);

    pub fn run_test_loop();
}
