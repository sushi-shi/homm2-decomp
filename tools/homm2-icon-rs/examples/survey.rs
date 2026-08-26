//! Decodes every icon in a `.AGG` archive and checks the format model against
//! real shipped data.
//!
//! ```sh
//! cargo run --release --example survey -- /path/to/HEROES2.AGG
//! ```
//!
//! For every frame it verifies that the stream terminates cleanly, then measures
//! the decoded geometry against the frame header. A frame whose decoded row
//! count or column extent disagrees with its declared width and height is
//! reported: either the asset is odd or the format model is.

use std::collections::BTreeMap;
use std::process::ExitCode;

use homm2_icon::agg::Archive;
use homm2_icon::{ColorOps, Icon, MaskOps, Op};

/// What one frame's stream turned out to contain.
#[derive(Default)]
struct Shape {
    rows: u32,
    widest: i64,
    literal_pixels: u64,
    solid_pixels: u64,
    shadow_pixels: u64,
    skipped_pixels: u64,
}

fn measure_color(stream: &[u8]) -> Result<Shape, homm2_icon::Error> {
    let mut shape = Shape::default();
    let mut column: i64 = 0;
    for op in ColorOps::new(stream) {
        match op? {
            Op::EndOfRow => {
                shape.widest = shape.widest.max(column);
                shape.rows += 1;
                column = 0;
            }
            Op::Skip(len) => {
                shape.skipped_pixels += u64::from(len);
                column += i64::from(len);
            }
            Op::Literal(payload) => {
                let len = i64::try_from(payload.len()).unwrap_or(i64::MAX);
                shape.literal_pixels += u64::try_from(payload.len()).unwrap_or(0);
                column += len;
            }
            Op::Solid { len, .. } => {
                shape.solid_pixels += u64::from(len);
                column += i64::from(len);
            }
            Op::Shadow { len, .. } => {
                shape.shadow_pixels += u64::from(len);
                column += i64::from(len);
            }
        }
    }
    shape.widest = shape.widest.max(column);
    shape.rows += 1; // the last row is closed by end-of-image, not end-of-row
    Ok(shape)
}

fn measure_mask(stream: &[u8]) -> Result<Shape, homm2_icon::Error> {
    let mut shape = Shape::default();
    let mut column: i64 = 0;
    for op in MaskOps::new(stream) {
        match op? {
            homm2_icon::MaskOp::EndOfRow => {
                shape.widest = shape.widest.max(column);
                shape.rows += 1;
                column = 0;
            }
            homm2_icon::MaskOp::Skip(len) => {
                shape.skipped_pixels += u64::from(len);
                column += i64::from(len);
            }
            homm2_icon::MaskOp::Span(len) => {
                shape.solid_pixels += u64::from(len);
                column += i64::from(len);
            }
        }
    }
    shape.widest = shape.widest.max(column);
    shape.rows += 1; // the last row is closed by end-of-image, not end-of-row
    Ok(shape)
}

#[allow(
    clippy::too_many_lines,
    reason = "a linear report reads better unsplit"
)]
fn main() -> ExitCode {
    let Some(path) = std::env::args().nth(1) else {
        eprintln!("usage: survey <HEROES2.AGG>");
        return ExitCode::FAILURE;
    };
    let data = match std::fs::read(&path) {
        Ok(data) => data,
        Err(error) => {
            eprintln!("cannot read {path}: {error}");
            return ExitCode::FAILURE;
        }
    };
    let archive = match Archive::parse(&data) {
        Ok(archive) => archive,
        Err(error) => {
            eprintln!("cannot parse {path}: {error}");
            return ExitCode::FAILURE;
        }
    };

    let mut icons = 0u32;
    let mut frames = 0u32;
    let mut decode_failures = Vec::new();
    let mut row_mismatch = Vec::new();
    let mut column_overflow = Vec::new();
    let mut kinds: BTreeMap<u8, u32> = BTreeMap::new();
    let mut mask_icons: BTreeMap<String, u32> = BTreeMap::new();
    let mut both_dialects_decode = 0u32;
    let mut totals = Shape::default();

    for entry in archive.icons() {
        let name = entry.name_str().unwrap_or("<non-utf8>").to_owned();
        let Some(payload) = archive.payload(&entry) else {
            decode_failures.push((name, 0usize, "payload outside archive".to_owned()));
            continue;
        };
        let icon = match Icon::parse(payload) {
            Ok(icon) => icon,
            Err(error) => {
                decode_failures.push((name, 0, error.to_string()));
                continue;
            }
        };
        icons += 1;

        for index in 0..icon.frame_count() {
            frames += 1;
            let frame = match icon.frame_data(index) {
                Ok(frame) => frame,
                Err(error) => {
                    decode_failures.push((name.clone(), index, error.to_string()));
                    continue;
                }
            };
            let header = frame.header();
            let stream = frame.stream();
            *kinds.entry(header.kind).or_default() += 1;

            // Every frame is tried with the dialect its kind byte advertises.
            let measured = if header.kind == homm2_icon::frame::KIND_MASK_FRAME {
                measure_mask(stream)
            } else {
                measure_color(stream)
            };
            let shape = match measured {
                Ok(shape) => shape,
                Err(error) => {
                    decode_failures.push((name.clone(), index, error.to_string()));
                    continue;
                }
            };

            if header.kind == homm2_icon::frame::KIND_MASK_FRAME {
                *mask_icons.entry(name.clone()).or_default() += 1;
            }
            // Do the two dialects actually distinguish anything, or would either
            // decode any stream?
            let other_ok = if header.kind == homm2_icon::frame::KIND_MASK_FRAME {
                measure_color(stream).is_ok()
            } else {
                measure_mask(stream).is_ok()
            };
            if other_ok {
                both_dialects_decode += 1;
            }

            if shape.rows != u32::from(header.height) {
                row_mismatch.push((name.clone(), index, shape.rows, header.height));
            }
            if shape.widest > i64::from(header.width) {
                column_overflow.push((name.clone(), index, shape.widest, header.width));
            }
            totals.rows += shape.rows;
            totals.literal_pixels += shape.literal_pixels;
            totals.solid_pixels += shape.solid_pixels;
            totals.shadow_pixels += shape.shadow_pixels;
            totals.skipped_pixels += shape.skipped_pixels;
        }
    }

    println!("archive        {path}");
    println!("members        {}", archive.len());
    println!("icons          {icons}");
    println!("frames         {frames}");
    println!("decoded rows   {}", totals.rows);
    println!();
    println!("frame kind byte histogram");
    for (kind, count) in &kinds {
        let tag = if *kind == homm2_icon::frame::KIND_MASK_FRAME {
            " (mask)"
        } else {
            ""
        };
        println!("  {kind:>3}{tag:<7} {count}");
    }
    println!();
    println!("pixels by opcode class");
    println!("  literal      {}", totals.literal_pixels);
    println!("  solid/span   {}", totals.solid_pixels);
    println!("  shadow       {}", totals.shadow_pixels);
    println!("  transparent  {}", totals.skipped_pixels);
    println!();

    println!("icons containing mask frames");
    for (name, count) in &mask_icons {
        println!("  {name:<16} {count} frame(s)");
    }
    println!();
    println!("frames the OTHER dialect also accepts   {both_dialects_decode} of {frames}");
    println!();

    let report = |label: &str, count: usize| println!("{label:<28} {count}");
    report("streams that failed", decode_failures.len());
    report("row count != height", row_mismatch.len());
    report("column extent > width", column_overflow.len());

    for (name, index, message) in decode_failures.iter().take(10) {
        println!("  FAIL  {name} frame {index}: {message}");
    }
    for (name, index, rows, height) in row_mismatch.iter().take(10) {
        println!("  ROWS  {name} frame {index}: decoded {rows}, header {height}");
    }
    for (name, index, widest, width) in column_overflow.iter().take(10) {
        println!("  COLS  {name} frame {index}: reached {widest}, header {width}");
    }

    if decode_failures.is_empty() {
        ExitCode::SUCCESS
    } else {
        ExitCode::FAILURE
    }
}
