use std::env;
use cmake::Config;

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
    let dst = Config::new("").profile("Debug").out_dir(".").build();
    println!("cargo:rustc-link-search=native={}/build/install/", dst.display());
    println!("cargo:rustc-link-lib=static=cef_dll_wrapper");
    println!("cargo:rustc-link-lib=static=pcef");
    println!("cargo:rustc-link-lib=framework=Foundation");
    println!("cargo:rustc-link-lib=framework=CoreServices");

    if let Some(name) = cpp_stdlib_name() {
        println!("cargo:rustc-link-lib={}", name);
    }
}