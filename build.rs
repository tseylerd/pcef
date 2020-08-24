use std::env;
use cmake::Config;
use std::path::Path;

fn cpp_stdlib_name() -> Option<String> {
    if let Ok(stdlib) = env::var("CXXSTDLIB") {
        if stdlib.is_empty() {
            None
        } else {
            Some(stdlib)
        }
    } else {
        let target = env::var("TARGET").unwrap();
        if target.contains("msvc") {
            None
        } else if target.contains("apple") {
            Some("c++".to_string())
        } else if target.contains("freebsd") {
            Some("c++".to_string())
        } else if target.contains("openbsd") {
            Some("c++".to_string())
        } else {
            Some("stdc++".to_string())
        }
    }
}

fn main() {
    let out_dir = std::env::var("OUT_DIR").expect("out directory");
    let install_dir_buf = Path::new(&out_dir).parent()
        .expect("parent 1").parent()
        .expect("parent 2").parent()
        .expect("final parent").join("pcef");
    std::fs::create_dir(&install_dir_buf);

    let cmake_install_dir = install_dir_buf.to_str().expect("cmake_install_dir");

    let dst = Config::new("").profile("Debug").out_dir(cmake_install_dir).build();
    println!("cargo:rustc-link-search=native={}", dst.display());
    println!("cargo:rustc-link-lib=static=cef_dll_wrapper");
    println!("cargo:rustc-link-lib=static=pcef");
    println!("cargo:rustc-link-lib=framework=Foundation");
    println!("cargo:rustc-link-lib=framework=CoreServices");

    if let Some(name) = cpp_stdlib_name() {
        println!("cargo:rustc-link-lib={}", name);
    }
}