//! Renders one frame of one icon from a `.AGG` archive.
//!
//! ```sh
//! cargo run --example render -- HEROES2.AGG CELLHEX.ICN 0
//! cargo run --example render -- HEROES2.AGG DRAGGREE.ICN 0 --mirrored
//! ```
//!
//! Prints the frame header, an ASCII preview, and writes a PPM alongside so the
//! result can be looked at properly. `--mask` forces the silhouette dialect,
//! `--mirrored` flips, and `--retail` enables the mirrored clipping defect.

use std::process::ExitCode;

use homm2_icon::agg::Archive;
use homm2_icon::{Canvas, ColorBlit, DimPalettes, Icon, MaskBlit, Quirks, Rect};

/// Palette entries are 6-bit VGA values.
fn expand(component: u8) -> u8 {
    component.saturating_mul(4).saturating_add(component >> 4)
}

fn ascii_preview(pixels: &[u8], width: usize) {
    const RAMP: &[u8] = b" .:-=+*#%@";
    for row in pixels.chunks(width) {
        let line: String = row
            .iter()
            .map(|&index| {
                if index == 0 {
                    ' '
                } else {
                    let bucket = usize::from(index) * (RAMP.len() - 1) / 255;
                    char::from(RAMP[bucket.max(1)])
                }
            })
            .collect();
        println!("|{line}|");
    }
}

#[allow(clippy::too_many_lines, reason = "a linear CLI reads better unsplit")]
fn main() -> ExitCode {
    let args: Vec<String> = std::env::args().skip(1).collect();
    let [path, member, frame_index, flags @ ..] = args.as_slice() else {
        eprintln!(
            "usage: render <archive.agg> <MEMBER.ICN> <frame> [--mask] [--mirrored] [--retail]"
        );
        return ExitCode::FAILURE;
    };
    let mirrored = flags.iter().any(|flag| flag == "--mirrored");
    let as_mask = flags.iter().any(|flag| flag == "--mask");
    let retail = flags.iter().any(|flag| flag == "--retail");
    let Ok(frame_index) = frame_index.parse::<usize>() else {
        eprintln!("frame index must be a number");
        return ExitCode::FAILURE;
    };

    let Ok(data) = std::fs::read(path) else {
        eprintln!("cannot read {path}");
        return ExitCode::FAILURE;
    };
    let archive = match Archive::parse(&data) {
        Ok(archive) => archive,
        Err(error) => {
            eprintln!("cannot parse archive: {error}");
            return ExitCode::FAILURE;
        }
    };
    let Some(payload) = archive.get(member) else {
        eprintln!("{member} is not in the archive");
        return ExitCode::FAILURE;
    };
    let icon = match Icon::parse(payload) {
        Ok(icon) => icon,
        Err(error) => {
            eprintln!("cannot parse {member}: {error}");
            return ExitCode::FAILURE;
        }
    };
    let frame = match icon.frame_data(frame_index) {
        Ok(frame) => frame,
        Err(error) => {
            eprintln!("{error}");
            return ExitCode::FAILURE;
        }
    };
    let header = frame.header();

    println!("{member} frame {frame_index} of {}", icon.frame_count());
    println!(
        "  size {}x{}  offset ({}, {})  kind {}  data_offset {:#x}",
        header.width,
        header.height,
        header.offset_x,
        header.offset_y,
        header.kind,
        header.data_offset
    );
    println!("  advertised dialect {:?}", header.advertised_dialect());

    let width = i32::from(header.width).max(1);
    let height = i32::from(header.height).max(1);
    let area = usize::try_from(width * height).unwrap_or(0);
    let mut pixels = vec![0u8; area];
    let mut canvas = match Canvas::new(width, height, &mut pixels) {
        Ok(canvas) => canvas,
        Err(error) => {
            eprintln!("{error}");
            return ExitCode::FAILURE;
        }
    };

    // Draw with the frame's own offsets cancelled so it lands at the origin.
    let pen_x = if mirrored {
        i32::from(header.offset_x) + width - 1
    } else {
        -i32::from(header.offset_x)
    };
    let pen_y = -i32::from(header.offset_y);
    let quirks = if retail {
        Quirks::retail()
    } else {
        Quirks::corrected()
    };
    let clip = Rect::new(0, 0, width, height);

    let flat = vec![0u8; 12 * 256];
    let result = if as_mask || header.advertised_dialect() == homm2_icon::Dialect::Mask {
        MaskBlit::solid(0xe2)
            .mirrored(mirrored)
            .clip(clip)
            .quirks(quirks)
            .draw(&mut canvas, frame, pen_x, pen_y)
    } else {
        ColorBlit::new()
            .shadows(DimPalettes::new(&flat))
            .mirrored(mirrored)
            .clip(clip)
            .quirks(quirks)
            .draw(&mut canvas, frame, pen_x, pen_y)
    };
    if let Err(error) = result {
        eprintln!("draw failed: {error}");
        return ExitCode::FAILURE;
    }

    let painted = pixels.iter().filter(|&&index| index != 0).count();
    println!("  painted {painted} of {} pixels", pixels.len());
    println!();
    ascii_preview(&pixels, usize::try_from(width).unwrap_or(1));

    // A PPM needs a palette; KB.PAL is 256 six-bit RGB triples.
    if let Some(palette) = archive.get("KB.PAL") {
        let mut ppm = format!("P6\n{width} {height}\n255\n").into_bytes();
        for &index in &pixels {
            let at = usize::from(index) * 3;
            let rgb = palette.get(at..at + 3).unwrap_or(&[0, 0, 0]);
            ppm.extend(rgb.iter().copied().map(expand));
        }
        let out = format!("{member}.{frame_index}.ppm");
        if std::fs::write(&out, ppm).is_ok() {
            println!("\nwrote {out}");
        }
    }
    ExitCode::SUCCESS
}
