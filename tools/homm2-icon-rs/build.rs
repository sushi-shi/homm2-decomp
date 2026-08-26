//! Compiles the reconstructed C++ icon decoders for the differential parity
//! tests in `tests/cxx_parity.rs`.
//!
//! The source tree is located through `HOMM2_SOURCE_DIR`, defaulting to the
//! repository root two directories above this crate. If that tree is absent,
//! the crate remains usable on its own and the parity tests are skipped at
//! compile time via the `cxx_parity` cfg.

use std::env;
use std::path::PathBuf;
use std::process::Command;

const DECODER_UNITS: &[&str] = &[
    "src/BASE/Icon2b.cpp",
    "src/BASE/Iconf2b.cpp",
    "src/BASE/icon2bc.cpp",
    "src/BASE/iconf2bc.cpp",
    "src/BASE/icon2by.cpp",
    "src/BASE/iconf2by.cpp",
    "src/BASE/Iconm2b.cpp",
    "src/BASE/Iconmf2b.cpp",
    "src/BASE/Icond2b.cpp",
    "src/BASE/Icondf2b.cpp",
];

fn main() {
    println!("cargo:rustc-check-cfg=cfg(cxx_parity)");
    let manifest = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap());
    let source_root =
        env::var("HOMM2_SOURCE_DIR").map_or_else(|_| manifest.join("../.."), PathBuf::from);
    println!("cargo:rerun-if-env-changed=HOMM2_SOURCE_DIR");
    println!("cargo:rerun-if-changed=cxx/shim.cpp");
    if !source_root.join("include/BASE/Icon2b.h").is_file() {
        println!(
            "cargo:warning=HoMM2 source tree not found at {}; C++ parity tests disabled",
            source_root.display()
        );
        return;
    }
    for unit in DECODER_UNITS {
        println!(
            "cargo:rerun-if-changed={}",
            source_root.join(unit).display()
        );
    }

    let out = PathBuf::from(env::var("OUT_DIR").unwrap());
    let compiler = env::var("CXX").unwrap_or_else(|_| "clang++".into());
    let mut objects = Vec::new();
    let mut sources: Vec<PathBuf> = vec![manifest.join("cxx/shim.cpp")];
    sources.extend(DECODER_UNITS.iter().map(|unit| source_root.join(unit)));
    for source in &sources {
        let object = out.join(format!(
            "{}.o",
            source.file_stem().unwrap().to_string_lossy()
        ));
        run(Command::new(&compiler)
            .args([
                "-std=c++20",
                "-O1",
                "-fno-strict-aliasing",
                "-fno-exceptions",
                "-fno-rtti",
                "-D__declspec(x)=",
                "-D__int64=long long",
                "-I",
            ])
            .arg(source_root.join("include"))
            .arg("-c")
            .arg(source)
            .arg("-o")
            .arg(&object));
        objects.push(object);
    }

    let archive = out.join("libh2cxxicons.a");
    let _ = std::fs::remove_file(&archive);
    let ar = env::var("AR").unwrap_or_else(|_| "ar".into());
    run(Command::new(ar).arg("rcs").arg(&archive).args(&objects));

    println!("cargo:rustc-link-search=native={}", out.display());
    println!("cargo:rustc-link-lib=static=h2cxxicons");
    println!("cargo:rustc-link-lib=stdc++");
    println!("cargo:rustc-cfg=cxx_parity");
}

fn run(command: &mut Command) {
    let rendered = format!("{command:?}");
    let status = command.status().unwrap_or_else(|error| {
        panic!("failed to spawn {rendered}: {error}");
    });
    assert!(status.success(), "command failed: {rendered}");
}
