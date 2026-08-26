//! Differential tests: the decompiled C++ decoders against this crate.
//!
//! `build.rs` compiles the ten reconstructed translation units from the
//! decomp tree and this test drives both implementations over generated
//! valid streams, comparing whole surfaces byte for byte. Retail quirks are
//! enabled on the Rust side so both reproduce the shipped behaviour,
//! including the mirrored partial-run drop.
//!
//! Deliberately outside the corpus (documented divergences):
//! * out-of-range shadow levels (retail reads past `uDimPal`);
//! * streams that overrun their advertised frame box (retail trusts them);
//! * geometry escaping the surface on the unclipped path (retail has no
//!   bounds checks there).
#![cfg(cxx_parity)]
#![allow(unsafe_code)]
#![allow(
    clippy::cast_possible_truncation,
    clippy::cast_possible_wrap,
    clippy::cast_sign_loss,
    reason = "the bounded parity-test generator deliberately selects low bits"
)]

use homm2_icon::{Canvas, DimPalettes, Icon, Quirks, Rect, retail};

const PALETTE_LEVELS: usize = 12;
const PALETTE_LEN: usize = 256;
const CANVAS_W: i32 = 48;
const CANVAS_H: i32 = 24;
const CLIP_MODE_CLIP: i32 = 1;

unsafe extern "C" {
    fn h2cxx_set_dim_palettes(flat: *const u8);
    fn h2cxx_icon_to_bitmap(
        body: *mut u8,
        pixels: *mut u8,
        width: i32,
        x: i32,
        y: i32,
        frame: i32,
        clip: i32,
        clip_x: i32,
        clip_y: i32,
        clip_w: i32,
        clip_h: i32,
        color: i32,
    );
    fn h2cxx_flip_icon_to_bitmap(
        body: *mut u8,
        pixels: *mut u8,
        width: i32,
        x: i32,
        y: i32,
        frame: i32,
        clip: i32,
        clip_x: i32,
        clip_y: i32,
        clip_w: i32,
        clip_h: i32,
        color: i32,
    );
    fn h2cxx_icon_to_bitmap_color_table(
        body: *mut u8,
        pixels: *mut u8,
        width: i32,
        x: i32,
        y: i32,
        frame: i32,
        clip: i32,
        clip_x: i32,
        clip_y: i32,
        clip_w: i32,
        clip_h: i32,
        color: i32,
        table: *mut u8,
        dim_gate: i32,
    );
    fn h2cxx_flip_icon_to_bitmap_color_table(
        body: *mut u8,
        pixels: *mut u8,
        width: i32,
        x: i32,
        y: i32,
        frame: i32,
        clip: i32,
        clip_x: i32,
        clip_y: i32,
        clip_w: i32,
        clip_h: i32,
        color: i32,
        table: *mut u8,
    );
    fn h2cxx_icon_to_bitmap_y_modify(
        body: *mut u8,
        pixels: *mut u8,
        width: i32,
        x: i32,
        y: i32,
        frame: i32,
        clip: i32,
        clip_x: i32,
        clip_y: i32,
        clip_w: i32,
        clip_h: i32,
        color: i32,
        shear: *mut i8,
    );
    fn h2cxx_flip_icon_to_bitmap_y_modify(
        body: *mut u8,
        pixels: *mut u8,
        width: i32,
        x: i32,
        y: i32,
        frame: i32,
        clip: i32,
        clip_x: i32,
        clip_y: i32,
        clip_w: i32,
        clip_h: i32,
        color: i32,
        shear: *mut i8,
    );
    fn h2cxx_mono_icon_to_bitmap(
        body: *mut u8,
        pixels: *mut u8,
        width: i32,
        x: i32,
        y: i32,
        frame: i32,
        color: i32,
        clip: i32,
        clip_x: i32,
        clip_y: i32,
        clip_w: i32,
        clip_h: i32,
    );
    fn h2cxx_flip_mono_icon_to_bitmap(
        body: *mut u8,
        pixels: *mut u8,
        width: i32,
        x: i32,
        y: i32,
        frame: i32,
        color: i32,
        clip: i32,
        clip_x: i32,
        clip_y: i32,
        clip_w: i32,
        clip_h: i32,
    );
    fn h2cxx_dim_icon_to_bitmap(
        body: *mut u8,
        pixels: *mut u8,
        width: i32,
        x: i32,
        y: i32,
        frame: i32,
        color: i32,
        clip: i32,
        clip_x: i32,
        clip_y: i32,
        clip_w: i32,
        clip_h: i32,
    );
    fn h2cxx_flip_dim_icon_to_bitmap(
        body: *mut u8,
        pixels: *mut u8,
        width: i32,
        x: i32,
        y: i32,
        frame: i32,
        color: i32,
        clip: i32,
        clip_x: i32,
        clip_y: i32,
        clip_w: i32,
        clip_h: i32,
    );
}

/// Small deterministic generator so failures replay from a seed.
struct Lcg(u64);

impl Lcg {
    fn next(&mut self) -> u32 {
        self.0 = self
            .0
            .wrapping_mul(6_364_136_223_846_793_005)
            .wrapping_add(1_442_695_040_888_963_407);
        (self.0 >> 33) as u32
    }

    fn below(&mut self, bound: u32) -> u32 {
        self.next() % bound
    }
}

fn palettes_flat() -> Vec<u8> {
    (0..PALETTE_LEVELS * PALETTE_LEN)
        .map(|i| ((i * 37) ^ (i >> 3)) as u8)
        .collect()
}

fn remap_table() -> Vec<u8> {
    (0..PALETTE_LEN).map(|i| (255 - i) as u8).collect()
}

fn base_pixels() -> Vec<u8> {
    (0..(CANVAS_W * CANVAS_H) as usize)
        .map(|i| ((i * 7) % 251) as u8)
        .collect()
}

fn icon_body(width: u16, height: u16, offset_x: i16, offset_y: i16, stream: &[u8]) -> Vec<u8> {
    let mut body = Vec::new();
    body.extend_from_slice(&offset_x.to_le_bytes());
    body.extend_from_slice(&offset_y.to_le_bytes());
    body.extend_from_slice(&width.to_le_bytes());
    body.extend_from_slice(&height.to_le_bytes());
    body.push(0);
    body.extend_from_slice(&13u32.to_le_bytes());
    body.extend_from_slice(stream);
    body
}

/// One colour-dialect row worth of runs staying within `width` columns.
fn color_row(rng: &mut Lcg, width: u32, stream: &mut Vec<u8>) {
    let mut remaining = width;
    while remaining > 0 && rng.below(5) != 0 {
        match rng.below(5) {
            // literal run with payload bytes
            0 => {
                let n = 1 + rng.below(remaining.min(0x3f));
                stream.push(n as u8);
                for _ in 0..n {
                    stream.push(rng.next() as u8);
                }
                remaining -= n;
            }
            // skip
            1 => {
                let n = 1 + rng.below(remaining.min(0x3f));
                stream.push(0x80 | n as u8);
                remaining -= n;
            }
            // short solid; 0xc1 is the long-solid escape, so a one-pixel
            // run must use the long spelling
            2 => {
                let n = 1 + rng.below(remaining.min(0x3e));
                if n == 1 {
                    stream.push(0xc1);
                    stream.push(1);
                } else {
                    stream.push(0xc0 | n as u8);
                }
                stream.push(rng.next() as u8);
                remaining -= n;
            }
            // long solid
            3 => {
                let n = 1 + rng.below(remaining);
                stream.push(0xc1);
                stream.push(n as u8);
                stream.push(rng.next() as u8);
                remaining -= n;
            }
            // shadow run: level kept inside the twelve real palettes
            _ => {
                let n = 1 + rng.below(remaining);
                stream.push(0xc0);
                let level = rng.below(PALETTE_LEVELS as u32) as u8;
                let recolor = if rng.below(3) == 0 { 0x80 } else { 0 };
                if n <= 3 {
                    stream.push(0x40 | recolor | (level << 2) | n as u8);
                } else {
                    stream.push(0x40 | recolor | (level << 2));
                    stream.push(n as u8);
                }
                remaining -= n;
            }
        }
    }
}

fn color_stream(rng: &mut Lcg, width: u32, height: u32) -> Vec<u8> {
    let mut stream = Vec::new();
    for row in 0..height {
        color_row(rng, width, &mut stream);
        if row + 1 < height {
            stream.push(0);
        }
    }
    stream.push(0x80);
    stream
}

/// One mask-dialect row: solid runs carry no payload.
fn mono_stream(rng: &mut Lcg, width: u32, height: u32) -> Vec<u8> {
    let mut stream = Vec::new();
    for row in 0..height {
        let mut remaining = width;
        while remaining > 0 && rng.below(4) != 0 {
            let n = 1 + rng.below(remaining.min(0x7e));
            if rng.below(2) == 0 {
                stream.push(n as u8); // solid mask run
            } else {
                stream.push(0x80 | n as u8); // skip
            }
            remaining -= n;
        }
        if row + 1 < height {
            stream.push(0);
        }
    }
    stream.push(0x80);
    stream
}

struct Case {
    body: Vec<u8>,
    width: u16,
    height: u16,
    x: i32,
    y: i32,
    clip: Rect,
    mirrored: bool,
}

fn geometry(rng: &mut Lcg, mirrored: bool, body_stream: &[u8], width: u16, height: u16) -> Case {
    let (offset_x, offset_y) = (rng.below(3) as i16 - 1, rng.below(3) as i16 - 1);
    let body = icon_body(width, height, offset_x, offset_y, body_stream);
    // Keep the whole sprite box inside the canvas so the unclipped retail
    // path cannot write out of bounds; the clip rectangle still cuts it.
    let w = i32::from(width);
    let h = i32::from(height);
    let x = if mirrored {
        w - i32::from(offset_x) + rng.below((CANVAS_W - w - 2).max(1) as u32) as i32
    } else {
        -i32::from(offset_x) + 1 + rng.below((CANVAS_W - w - 2).max(1) as u32) as i32
    };
    let y = -i32::from(offset_y) + 1 + rng.below((CANVAS_H - h - 2).max(1) as u32) as i32;
    let raw_clip = match rng.below(4) {
        0 => Rect::new(0, 0, CANVAS_W, CANVAS_H),
        1 => Rect::new(x - w / 2, 0, w, CANVAS_H),
        2 => Rect::new(0, y + h / 3, CANVAS_W, h.max(2) / 2),
        _ => Rect::new(x - 2, y - 1, w / 2 + 3, h / 2 + 2),
    };
    // The retail decoders trust the clip rectangle; keep it on the surface.
    let clip = raw_clip
        .intersect(Rect::new(0, 0, CANVAS_W, CANVAS_H))
        .unwrap_or(Rect::new(0, 0, 1, 1));
    Case {
        body,
        width,
        height,
        x,
        y,
        clip,
        mirrored,
    }
}

fn assert_surfaces(label: &str, seed: u64, case: &Case, cxx: &[u8], rust: &[u8]) {
    if cxx != rust {
        let index = cxx.iter().zip(rust).position(|(a, b)| a != b).unwrap();
        panic!(
            "{label} diverged (seed {seed}): first at ({}, {}) cxx={} rust={} \
             sprite {}x{} at ({}, {}) clip {:?} mirrored {}",
            index as i32 % CANVAS_W,
            index as i32 / CANVAS_W,
            cxx[index],
            rust[index],
            case.width,
            case.height,
            case.x,
            case.y,
            case.clip,
            case.mirrored,
        );
    }
}

fn rust_draw(blit: homm2_icon::ColorBlit<'_>, case: &Case, pixels: &mut [u8]) {
    let icon = Icon::from_body(1, &case.body).expect("body parses");
    let mut canvas = Canvas::new(CANVAS_W, CANVAS_H, pixels).expect("canvas");
    blit.clip(case.clip)
        .quirks(Quirks::retail())
        .draw(
            &mut canvas,
            icon.frame_data(0).expect("frame"),
            case.x,
            case.y,
        )
        .expect("rust blit succeeds");
}

fn rust_draw_mask(blit: homm2_icon::MaskBlit<'_>, case: &Case, pixels: &mut [u8]) {
    let icon = Icon::from_body(1, &case.body).expect("body parses");
    let mut canvas = Canvas::new(CANVAS_W, CANVAS_H, pixels).expect("canvas");
    blit.clip(case.clip)
        .quirks(Quirks::retail())
        .draw(
            &mut canvas,
            icon.frame_data(0).expect("frame"),
            case.x,
            case.y,
        )
        .expect("rust blit succeeds");
}

fn clip_args(case: &Case) -> (i32, i32, i32, i32) {
    (case.clip.x, case.clip.y, case.clip.width, case.clip.height)
}

#[test]
fn color_pair_agrees() {
    let flat = palettes_flat();
    unsafe { h2cxx_set_dim_palettes(flat.as_ptr()) };
    let palettes = DimPalettes::new(&flat);
    for seed in 0..400u64 {
        let rng = &mut Lcg(seed.wrapping_mul(0x9e37_79b9_7f4a_7c15) + 1);
        let width = 4 + rng.below(20) as u16;
        let height = 2 + rng.below(10) as u16;
        let stream = color_stream(rng, u32::from(width), u32::from(height));
        let mirrored = seed % 2 == 1;
        let case = geometry(rng, mirrored, &stream, width, height);
        let highlight = if rng.below(2) == 0 {
            0
        } else {
            1 + rng.next() as u8 % 254
        };
        let (cx, cy, cw, ch) = clip_args(&case);
        let mut cxx = base_pixels();
        unsafe {
            if mirrored {
                h2cxx_flip_icon_to_bitmap(
                    case.body.as_ptr().cast_mut(),
                    cxx.as_mut_ptr(),
                    CANVAS_W,
                    case.x,
                    case.y,
                    0,
                    CLIP_MODE_CLIP,
                    cx,
                    cy,
                    cw,
                    ch,
                    i32::from(highlight),
                );
            } else {
                h2cxx_icon_to_bitmap(
                    case.body.as_ptr().cast_mut(),
                    cxx.as_mut_ptr(),
                    CANVAS_W,
                    case.x,
                    case.y,
                    0,
                    CLIP_MODE_CLIP,
                    cx,
                    cy,
                    cw,
                    ch,
                    i32::from(highlight),
                );
            }
        }
        let mut rust = base_pixels();
        let blit = if mirrored {
            retail::flip_icon_to_bitmap(palettes, highlight)
        } else {
            retail::icon_to_bitmap(palettes, highlight)
        };
        rust_draw(blit, &case, &mut rust);
        assert_surfaces("IconToBitmap", seed, &case, &cxx, &rust);
    }
}

#[test]
fn color_table_pair_agrees() {
    let flat = palettes_flat();
    unsafe { h2cxx_set_dim_palettes(flat.as_ptr()) };
    let palettes = DimPalettes::new(&flat);
    let table_bytes = remap_table();
    let table: &[u8; PALETTE_LEN] = table_bytes.as_slice().try_into().unwrap();
    for seed in 0..300u64 {
        let rng = &mut Lcg(seed.wrapping_mul(0x2545_f491_4f6c_dd1d) + 7);
        let width = 4 + rng.below(20) as u16;
        let height = 2 + rng.below(10) as u16;
        let stream = color_stream(rng, u32::from(width), u32::from(height));
        let mirrored = seed % 2 == 1;
        let case = geometry(rng, mirrored, &stream, width, height);
        let highlight = if rng.below(2) == 0 {
            0
        } else {
            1 + rng.next() as u8 % 254
        };
        let dim_gate = if mirrored { 1 } else { rng.below(2) as i32 };
        let (cx, cy, cw, ch) = clip_args(&case);
        let mut cxx = base_pixels();
        unsafe {
            if mirrored {
                h2cxx_flip_icon_to_bitmap_color_table(
                    case.body.as_ptr().cast_mut(),
                    cxx.as_mut_ptr(),
                    CANVAS_W,
                    case.x,
                    case.y,
                    0,
                    CLIP_MODE_CLIP,
                    cx,
                    cy,
                    cw,
                    ch,
                    i32::from(highlight),
                    table_bytes.as_ptr().cast_mut(),
                );
            } else {
                h2cxx_icon_to_bitmap_color_table(
                    case.body.as_ptr().cast_mut(),
                    cxx.as_mut_ptr(),
                    CANVAS_W,
                    case.x,
                    case.y,
                    0,
                    CLIP_MODE_CLIP,
                    cx,
                    cy,
                    cw,
                    ch,
                    i32::from(highlight),
                    table_bytes.as_ptr().cast_mut(),
                    dim_gate,
                );
            }
        }
        let mut rust = base_pixels();
        let blit = if mirrored {
            retail::flip_icon_to_bitmap_color_table(table, palettes, highlight)
        } else {
            retail::icon_to_bitmap_color_table(table, palettes, highlight, dim_gate != 0)
        };
        rust_draw(blit, &case, &mut rust);
        assert_surfaces("IconToBitmapColorTable", seed, &case, &cxx, &rust);
    }
}

#[test]
fn y_modify_pair_agrees() {
    let flat = palettes_flat();
    unsafe { h2cxx_set_dim_palettes(flat.as_ptr()) };
    let palettes = DimPalettes::new(&flat);
    for seed in 0..300u64 {
        let rng = &mut Lcg(seed.wrapping_mul(0xda94_2042_e4dd_58b5) + 3);
        let width = 4 + rng.below(16) as u16;
        let height = 2 + rng.below(8) as u16;
        let stream = color_stream(rng, u32::from(width), u32::from(height));
        let mirrored = seed % 2 == 1;
        let case = geometry(rng, mirrored, &stream, width, height);
        // Absolute-row shear; ±1 keeps the sheared sprite inside the box the
        // geometry helper reserved. A skipped row exercises 0x7f.
        let shear: Vec<i8> = (0..CANVAS_H)
            .map(|_| match rng.below(8) {
                0 => 0x7f,
                v => (v as i8 % 3) - 1,
            })
            .collect();
        let highlight = if rng.below(2) == 0 {
            0
        } else {
            1 + rng.next() as u8 % 254
        };
        let (cx, cy, cw, ch) = clip_args(&case);
        let mut cxx = base_pixels();
        unsafe {
            if mirrored {
                h2cxx_flip_icon_to_bitmap_y_modify(
                    case.body.as_ptr().cast_mut(),
                    cxx.as_mut_ptr(),
                    CANVAS_W,
                    case.x,
                    case.y,
                    0,
                    CLIP_MODE_CLIP,
                    cx,
                    cy,
                    cw,
                    ch,
                    i32::from(highlight),
                    shear.as_ptr().cast_mut(),
                );
            } else {
                h2cxx_icon_to_bitmap_y_modify(
                    case.body.as_ptr().cast_mut(),
                    cxx.as_mut_ptr(),
                    CANVAS_W,
                    case.x,
                    case.y,
                    0,
                    CLIP_MODE_CLIP,
                    cx,
                    cy,
                    cw,
                    ch,
                    i32::from(highlight),
                    shear.as_ptr().cast_mut(),
                );
            }
        }
        let mut rust = base_pixels();
        let blit = if mirrored {
            retail::flip_icon_to_bitmap_y_modify(palettes, highlight, &shear)
        } else {
            retail::icon_to_bitmap_y_modify(palettes, highlight, &shear)
        };
        rust_draw(blit, &case, &mut rust);
        assert_surfaces("IconToBitmapYModify", seed, &case, &cxx, &rust);
    }
}

#[test]
fn mask_pair_agrees() {
    let flat = palettes_flat();
    unsafe { h2cxx_set_dim_palettes(flat.as_ptr()) };
    let palettes = DimPalettes::new(&flat);
    for seed in 0..400u64 {
        let rng = &mut Lcg(seed.wrapping_mul(0xd134_2543_de82_ef95) + 11);
        let width = 4 + rng.below(20) as u16;
        let height = 2 + rng.below(10) as u16;
        let stream = mono_stream(rng, u32::from(width), u32::from(height));
        let mirrored = seed % 2 == 1;
        let dim = seed % 4 >= 2;
        let case = geometry(rng, mirrored, &stream, width, height);
        let color = if dim {
            rng.below(PALETTE_LEVELS as u32) as u8
        } else {
            rng.next() as u8
        };
        let (cx, cy, cw, ch) = clip_args(&case);
        let mut cxx = base_pixels();
        unsafe {
            match (dim, mirrored) {
                (false, false) => h2cxx_mono_icon_to_bitmap(
                    case.body.as_ptr().cast_mut(),
                    cxx.as_mut_ptr(),
                    CANVAS_W,
                    case.x,
                    case.y,
                    0,
                    i32::from(color),
                    CLIP_MODE_CLIP,
                    cx,
                    cy,
                    cw,
                    ch,
                ),
                (false, true) => h2cxx_flip_mono_icon_to_bitmap(
                    case.body.as_ptr().cast_mut(),
                    cxx.as_mut_ptr(),
                    CANVAS_W,
                    case.x,
                    case.y,
                    0,
                    i32::from(color),
                    CLIP_MODE_CLIP,
                    cx,
                    cy,
                    cw,
                    ch,
                ),
                (true, false) => h2cxx_dim_icon_to_bitmap(
                    case.body.as_ptr().cast_mut(),
                    cxx.as_mut_ptr(),
                    CANVAS_W,
                    case.x,
                    case.y,
                    0,
                    i32::from(color),
                    CLIP_MODE_CLIP,
                    cx,
                    cy,
                    cw,
                    ch,
                ),
                (true, true) => h2cxx_flip_dim_icon_to_bitmap(
                    case.body.as_ptr().cast_mut(),
                    cxx.as_mut_ptr(),
                    CANVAS_W,
                    case.x,
                    case.y,
                    0,
                    i32::from(color),
                    CLIP_MODE_CLIP,
                    cx,
                    cy,
                    cw,
                    ch,
                ),
            }
        }
        let mut rust = base_pixels();
        let blit = match (dim, mirrored) {
            (false, false) => retail::mono_icon_to_bitmap(color),
            (false, true) => retail::flip_mono_icon_to_bitmap(color),
            (true, false) => retail::dim_icon_to_bitmap(palettes, color),
            (true, true) => retail::flip_dim_icon_to_bitmap(palettes, color),
        };
        rust_draw_mask(blit, &case, &mut rust);
        assert_surfaces("MonoDimIconToBitmap", seed, &case, &cxx, &rust);
    }
}
