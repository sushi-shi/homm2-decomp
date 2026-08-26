//! Nearest-neighbour downscale, as used by the world-view map.
//!
//! Not a decoder: it renders the frame into a scratch surface and resamples.
//!
//! ```text
//!   scratch 64x64                     destination
//!   +--------+-------+                +------+
//!   | frame  |       |   sample at    |scale |
//!   | 32x32  |       |   base + n*step| x    |
//!   +--------+       |  ------------> |scale |
//!   |                |                +------+
//!   +----------------+
//! ```
//!
//! `step` is an integer division, so for a scale that does not divide 32 the
//! sampled window is narrower than the frame and the edges never reach the
//! screen. At the game's three zoom levels the loss is 0, 3 and 5 pixels per
//! edge respectively; it is a property of the original formula, reproduced here
//! deliberately.

use crate::blit::ColorBlit;
use crate::canvas::Canvas;
use crate::error::{Error, Result};
use crate::frame::FrameData;
use crate::geometry::Rect;

/// Edge length of the frame region the sampler reads.
pub const NATIVE_SIZE: u32 = 32;

/// Edge length of the scratch surface the frame is rendered into.
pub const WORK_SIZE: u32 = 64;

/// Palette index treated as transparent while resampling.
pub const TRANSPARENT_INDEX: u8 = 0;

/// The intermediate surface the frame is rendered into before resampling.
///
/// The original allocates and frees one of these per call, which on the world
/// view means once per map tile per frame. Hoisting it into a caller-owned
/// value makes that cost visible and lets it be paid once.
///
/// Backed by a fixed array rather than a `Vec`, which is what lets the whole
/// crate stay `no_std` with no allocator at all. It is 4 KiB, so a caller that
/// minds the stack should keep one in a `Box` or a static.
#[derive(Clone, Debug)]
pub struct Scratch {
    /// [`WORK_SIZE`] squared.
    pixels: [u8; 64 * 64],
}

impl Scratch {
    /// Creates the [`WORK_SIZE`] square work surface.
    #[must_use]
    pub const fn new() -> Self {
        Self {
            pixels: [TRANSPARENT_INDEX; 64 * 64],
        }
    }
}

impl Default for Scratch {
    fn default() -> Self {
        Self::new()
    }
}

/// Renders `frame` at `scale` by `scale` pixels with its top-left at `(x, y)`,
/// restricted to `clip` (or the whole surface when `clip` is `None`).
///
/// Retail forwards the caller's clip rectangle on the `scale == 32` pass-through
/// and then ignores it entirely on the scaled path, writing `scale * scale`
/// pixels unconditionally. Honouring it on both paths is a deliberate deviation:
/// the only consumer is the world-view map, which draws hundreds of tiles into a
/// viewport, and "clip to the viewport" is the one thing it needs.
///
/// # Errors
///
/// Returns [`Error::ScaleOutOfRange`] unless `scale` is in `1..=32`, and
/// propagates any failure from rendering the frame into the scratch surface.
pub fn draw_scaled(
    canvas: &mut Canvas<'_>,
    frame: FrameData<'_>,
    x: i32,
    y: i32,
    scale: u32,
    clip: Option<Rect>,
    scratch: &mut Scratch,
) -> Result<()> {
    if scale == 0 || scale > NATIVE_SIZE {
        return Err(Error::ScaleOutOfRange { scale });
    }
    if scale == NATIVE_SIZE {
        let blit = ColorBlit::new();
        return match clip {
            Some(rect) => blit.clip(rect),
            None => blit,
        }
        .draw(canvas, frame, x, y);
    }

    // The composite below writes directly, so fold the caller's rectangle into
    // the surface bounds once and test against that.
    let visible = match clip {
        Some(rect) => rect.intersect(canvas.bounds()),
        None => Some(canvas.bounds()),
    };
    let Some(visible) = visible else {
        return Ok(());
    };

    let work = usize::try_from(WORK_SIZE).unwrap_or(usize::MAX);
    let work_edge = i32::try_from(WORK_SIZE).unwrap_or(i32::MAX);
    let native_edge = i32::try_from(NATIVE_SIZE).unwrap_or(i32::MAX);
    scratch.pixels.fill(TRANSPARENT_INDEX);
    {
        let mut scratch_canvas = Canvas::new(work_edge, work_edge, &mut scratch.pixels)?;
        ColorBlit::new()
            .clip(Rect::new(0, 0, native_edge, native_edge))
            .draw(&mut scratch_canvas, frame, 0, 0)?;
    }
    let scratch = &scratch.pixels;

    let scale_i = i32::try_from(scale).unwrap_or(i32::MAX);
    let step = native_edge / scale_i;
    let base = i32::midpoint((1 - scale_i) * step, native_edge);

    for row in 0..scale_i {
        let source_y = base + row * step;
        let Ok(source_row) = usize::try_from(source_y) else {
            continue;
        };
        for column in 0..scale_i {
            let source_x = base + column * step;
            let Ok(source_column) = usize::try_from(source_x) else {
                continue;
            };
            let Some(&index) = scratch.get(source_row * work + source_column) else {
                continue;
            };
            if index == TRANSPARENT_INDEX {
                continue;
            }
            let screen_x = x.saturating_add(column);
            let screen_y = y.saturating_add(row);
            if !visible.contains_row(screen_y) || screen_x < visible.x || screen_x > visible.right()
            {
                continue;
            }
            if let Some(target) = canvas.row_mut(screen_y)
                && let Ok(destination) = usize::try_from(screen_x)
                && let Some(pixel) = target.get_mut(destination)
            {
                *pixel = index;
            }
        }
    }
    Ok(())
}
