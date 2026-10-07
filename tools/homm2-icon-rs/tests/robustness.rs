//! Adversarial input: the decoders and blitters must never panic, hang or write
//! out of bounds, whatever bytes they are handed.
//!
//! This is a poor relation of `cargo fuzz` done without a dependency: a fixed
//! xorshift stream drives millions of pathological inputs deterministically, so
//! a failure reproduces from the seed alone. A real fuzzer explores far better;
//! this at least establishes that the obvious shapes are covered.

// Truncating and wrapping casts are the whole point of a value generator: the
// tests want arbitrary bit patterns, not range-checked conversions.
#![allow(
    clippy::cast_possible_truncation,
    clippy::cast_possible_wrap,
    clippy::cast_sign_loss,
    reason = "deliberate lossy casts in the random input generator"
)]

use homm2_icon::{Canvas, ColorBlit, ColorOps, DimPalettes, Icon, MaskBlit, MaskOps, Quirks, Rect};

/// Deterministic, seedable, dependency-free.
struct Rng(u64);

impl Rng {
    const fn new(seed: u64) -> Self {
        Self(seed)
    }

    fn next_u64(&mut self) -> u64 {
        let mut x = self.0;
        x ^= x << 13;
        x ^= x >> 7;
        x ^= x << 17;
        self.0 = x;
        x
    }

    fn byte(&mut self) -> u8 {
        (self.next_u64() >> 24) as u8
    }

    fn below(&mut self, bound: u32) -> u32 {
        if bound == 0 {
            0
        } else {
            (self.next_u64() >> 32) as u32 % bound
        }
    }

    fn signed(&mut self, span: i32) -> i32 {
        self.below((span * 2) as u32) as i32 - span
    }

    /// A coordinate that is usually sane but occasionally at the extremes of the
    /// type. The first version of this suite drew everything from +/-40 and so
    /// missed three reachable overflow panics in placement and clip arithmetic.
    fn extreme_or(&mut self, span: i32) -> i32 {
        match self.below(16) {
            0 => i32::MAX,
            1 => i32::MIN,
            2 => i32::MAX - self.below(1024) as i32,
            3 => i32::MIN + self.below(1024) as i32,
            4 => i32::MAX / 2,
            _ => self.signed(span),
        }
    }

    /// Frame offsets are `i16` in the file, so the full range is reachable from
    /// real assets, not just from adversarial input.
    fn offset(&mut self) -> i16 {
        match self.below(8) {
            0 => i16::MAX,
            1 => i16::MIN,
            _ => self.signed(64) as i16,
        }
    }
}

/// Bytes biased towards opcodes that carry operands, so the interesting paths
/// are reached far more often than uniform random would manage.
fn spicy_byte(rng: &mut Rng) -> u8 {
    match rng.below(24) {
        0..=3 => 0x00,                       // end of row, common in real streams
        4 => 0x80,                           // end of image, kept rare so streams run long
        5 | 6 => 0xc0,                       // shadow escape
        7 => 0xc1,                           // long solid escape
        8 | 9 => 0xc0 | 0x3f,                // max short solid
        10 | 11 => 0x7f,                     // max literal
        12 => 0x81,                          // minimum skip
        13..=17 => 0x01 + rng.byte() % 0x0f, // short literals
        _ => rng.byte(),
    }
}

fn spicy_stream(rng: &mut Rng, len: usize) -> Vec<u8> {
    (0..len).map(|_| spicy_byte(rng)).collect()
}

#[test]
fn colour_decoder_survives_arbitrary_bytes() {
    let mut rng = Rng::new(0x1234_5678_9abc_def0);
    for _ in 0..40_000 {
        let len = rng.below(64) as usize;
        let stream = spicy_stream(&mut rng, len);
        // Draining the iterator must terminate and must not panic.
        let count = ColorOps::new(&stream).count();
        assert!(count <= stream.len() + 1, "decoder invented opcodes");
    }
}

#[test]
fn mask_decoder_survives_arbitrary_bytes() {
    let mut rng = Rng::new(0x0fed_cba9_8765_4321);
    for _ in 0..40_000 {
        let len = rng.below(64) as usize;
        let stream = spicy_stream(&mut rng, len);
        let count = MaskOps::new(&stream).count();
        assert!(count <= stream.len() + 1, "decoder invented opcodes");
    }
}

/// Builds a one-frame icon body with a caller-chosen (possibly absurd) header.
fn body_with(
    width: u16,
    height: u16,
    offset_x: i16,
    offset_y: i16,
    data_offset: u32,
    stream: &[u8],
) -> Vec<u8> {
    let mut body = Vec::new();
    body.extend_from_slice(&offset_x.to_le_bytes());
    body.extend_from_slice(&offset_y.to_le_bytes());
    body.extend_from_slice(&width.to_le_bytes());
    body.extend_from_slice(&height.to_le_bytes());
    body.push(0);
    body.extend_from_slice(&data_offset.to_le_bytes());
    body.extend_from_slice(stream);
    body
}

#[test]
#[allow(
    clippy::too_many_lines,
    reason = "one linear scenario reads better unsplit"
)]
fn blitters_never_panic_or_escape_the_surface() {
    // A canary border around the surface catches any write that escapes it.
    const W: i32 = 24;
    const H: i32 = 12;
    const GUARD: usize = 64;

    let mut rng = Rng::new(0xdead_beef_cafe_f00d);
    let palettes_flat = vec![0x40u8; 12 * 256];
    let remap = [0x77u8; 256];

    let mut reached = 0u32;
    let mut painted_total = 0u64;
    for _ in 0..20_000 {
        let len = rng.below(48) as usize;
        let stream = spicy_stream(&mut rng, len);
        // One in five cases goes fully degenerate to hunt overflow; the rest stay
        // on-surface so the painting paths keep getting real exercise. Deciding
        // once per case rather than per coordinate stops the two goals from
        // cancelling each other out.
        let wild = rng.below(5) == 0;
        let body = body_with(
            rng.below(40) as u16,
            rng.below(20) as u16,
            if wild {
                rng.offset()
            } else {
                rng.signed(30) as i16
            },
            if wild {
                rng.offset()
            } else {
                rng.signed(20) as i16
            },
            13,
            &stream,
        );
        let Ok(icon) = Icon::from_body(1, &body) else {
            continue;
        };
        let Ok(frame) = icon.frame_data(0) else {
            continue;
        };

        let mut backing = vec![0u8; GUARD + (W * H) as usize + GUARD];
        backing[..GUARD].fill(0xAA);
        let tail = backing.len() - GUARD;
        backing[tail..].fill(0xAA);

        let (clip, pen_x, pen_y) = if wild {
            (
                Rect::new(
                    rng.extreme_or(30),
                    rng.extreme_or(20),
                    if rng.below(8) == 0 {
                        i32::MAX
                    } else {
                        rng.below(40) as i32
                    },
                    if rng.below(8) == 0 {
                        i32::MAX
                    } else {
                        rng.below(20) as i32
                    },
                ),
                rng.extreme_or(40),
                rng.extreme_or(30),
            )
        } else {
            (
                Rect::new(
                    rng.below(6) as i32,
                    rng.below(4) as i32,
                    W - rng.below(6) as i32,
                    H - rng.below(4) as i32,
                ),
                rng.below(30) as i32 - 6,
                rng.below(16) as i32 - 3,
            )
        };
        let mirrored = rng.below(2) == 1;
        let retail = rng.below(2) == 1;
        let quirks = if retail {
            Quirks::retail()
        } else {
            Quirks::corrected()
        };

        {
            let mut canvas = Canvas::new(W, H, &mut backing[GUARD..tail]).expect("canvas fits");
            let _ = ColorBlit::new()
                .mirrored(mirrored)
                .clip(clip)
                .remap(&remap)
                .shadows(DimPalettes::new(&palettes_flat))
                .highlight(rng.byte())
                .quirks(quirks)
                .draw(&mut canvas, frame, pen_x, pen_y);
            let _ = MaskBlit::solid(rng.byte())
                .mirrored(mirrored)
                .clip(clip)
                .quirks(quirks)
                .draw(&mut canvas, frame, pen_x, pen_y);
            let _ = MaskBlit::dim(DimPalettes::new(&palettes_flat), rng.byte())
                .mirrored(mirrored)
                .clip(clip)
                .quirks(quirks)
                .draw(&mut canvas, frame, pen_x, pen_y);
        }

        reached += 1;
        painted_total += backing[GUARD..tail].iter().filter(|&&b| b != 0).count() as u64;
        assert!(
            backing[..GUARD].iter().all(|&b| b == 0xAA),
            "wrote before the surface"
        );
        assert!(
            backing[tail..].iter().all(|&b| b == 0xAA),
            "wrote past the surface"
        );
    }
    // A green test that skipped everything would be worthless, so assert it did
    // real work. These are vacuity guards, not coverage targets: the bar is one
    // painted pixel per case on average, well under the ~2.2 actually observed.
    assert!(
        reached > 10_000,
        "only {reached} of 20000 cases reached the blitter"
    );
    assert!(
        painted_total > 20_000,
        "only {painted_total} pixels painted across {reached} cases; test is vacuous"
    );
    println!("reached the blitter {reached} times, painted {painted_total} pixels");
}

#[test]
fn container_parsing_survives_arbitrary_bytes() {
    let mut rng = Rng::new(0x5eed_0000_0000_0001);
    for _ in 0..40_000 {
        let len = rng.below(80) as usize;
        let bytes: Vec<u8> = (0..len).map(|_| rng.byte()).collect();
        if let Ok(icon) = Icon::parse(&bytes) {
            for index in 0..icon.frame_count().min(8) {
                let _ = icon.frame(index);
                let _ = icon.frame_data(index);
            }
            let _ = icon.frames().count();
        }
    }
}

#[test]
fn shear_tables_shorter_than_the_frame_do_not_panic() {
    let mut rng = Rng::new(0xabcd_0000_1234_5678);
    for _ in 0..5_000 {
        let len = rng.below(32) as usize;
        let stream = spicy_stream(&mut rng, len);
        let body = body_with(8, 40, 0, 0, 13, &stream);
        let Ok(icon) = Icon::from_body(1, &body) else {
            continue;
        };
        let Ok(frame) = icon.frame_data(0) else {
            continue;
        };
        // Deliberately far shorter than the frame is tall.
        let shear = [0i8, 1, -1, homm2_icon::SHEAR_SKIP_ROW];
        let mut pixels = vec![0u8; 8 * 8];
        let mut canvas = Canvas::new(8, 8, &mut pixels).unwrap();
        let _ = ColorBlit::new()
            .shear(&shear)
            .clip(Rect::new(0, 0, 8, 8))
            .draw(&mut canvas, frame, rng.signed(10), rng.signed(10));
    }
}
