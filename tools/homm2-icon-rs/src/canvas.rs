//! The destination surface.

use crate::error::{Error, Result};
use crate::geometry::Rect;

/// A borrowed 8-bit indexed surface with a one-byte-per-pixel row stride.
///
/// Borrowing rather than owning keeps the blitters usable against a caller's
/// framebuffer without a copy, which is how the original is always used.
#[derive(Debug)]
pub struct Canvas<'a> {
    width: i32,
    height: i32,
    pixels: &'a mut [u8],
}

impl<'a> Canvas<'a> {
    /// Wraps `pixels` as a `width` by `height` surface.
    ///
    /// # Errors
    ///
    /// Returns [`Error::CanvasGeometry`] when either extent is negative or
    /// `pixels` is shorter than `width * height`.
    pub fn new(width: i32, height: i32, pixels: &'a mut [u8]) -> Result<Self> {
        let fits = usize::try_from(width)
            .ok()
            .zip(usize::try_from(height).ok())
            .and_then(|(w, h)| w.checked_mul(h))
            .is_some_and(|needed| needed <= pixels.len());
        if !fits {
            return Err(Error::CanvasGeometry {
                width,
                height,
                pixels: pixels.len(),
            });
        }
        Ok(Self {
            width,
            height,
            pixels,
        })
    }

    /// Surface width in pixels.
    #[must_use]
    pub const fn width(&self) -> i32 {
        self.width
    }

    /// Surface height in pixels.
    #[must_use]
    pub const fn height(&self) -> i32 {
        self.height
    }

    /// The whole surface as a rectangle.
    #[must_use]
    pub const fn bounds(&self) -> Rect {
        Rect::new(0, 0, self.width, self.height)
    }

    /// Read-only view of the pixels.
    #[must_use]
    pub fn pixels(&self) -> &[u8] {
        self.pixels
    }

    /// One row, or `None` when `y` is off-surface.
    pub fn row_mut(&mut self, y: i32) -> Option<&mut [u8]> {
        if y < 0 || y >= self.height {
            return None;
        }
        let width = usize::try_from(self.width).ok()?;
        let start = usize::try_from(y).ok()?.checked_mul(width)?;
        self.pixels.get_mut(start..start.checked_add(width)?)
    }
}
