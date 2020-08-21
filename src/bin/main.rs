use std::ffi::{c_void, CString, CStr};
use pcef;
use pcef::{BrowserRect, BrowserPaths};
use std::thread::sleep;
use std::thread;
use std::time::Duration;
use std::ptr::null_mut;

extern "C" fn rect(context: *mut c_void, id: u64) -> BrowserRect {
    println!("Getting rect!");
    BrowserRect {
        x: 0,
        y: 0,
        width: 100,
        height: 100
    }
}

extern "C" fn render(context: *mut c_void, id: u64, popup: bool, bytes: *const u8, width: i32, height: i32) {
    println!("Performing render!");
}

fn main() {
    thread::spawn(move || {
        unsafe {
            let framework_path = "";
            let main_bundle_path = "";
            let subprocess_path = "";

            let framework_path_c = CString::new(framework_path).unwrap();
            let main_bundle_path_c = CString::new(main_bundle_path).unwrap();
            let subprocess_path_c = CString::new(subprocess_path).unwrap();

            let paths = BrowserPaths {
                framework_path: framework_path_c.as_ptr(),
                main_bundle_path: main_bundle_path_c.as_ptr(),
                subprocess_path: subprocess_path_c.as_ptr()
            };
            if !pcef::init_browser(&paths) {
                println!("Failed to initialize browser");
                return;
            }
            let path_c = CString::new("https://www.google.com").unwrap();
            pcef::load_url(null_mut(), path_c.as_ptr(), 1, render, rect);
            for i in 1..10 {
                println!("Running {}...", i);
                thread::sleep(Duration::from_secs(1));
            }
            pcef::close_browser(1);
            if !pcef::stop_browser() {
                println!("Failed to stop browser");
                return;
            }
        }
    });
    unsafe {
        pcef::run_test_loop();
    }
}