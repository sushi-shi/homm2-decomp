//! Per-pixel operations.
//!
//! The original passes these as macro arguments spelled `pixelCode`, `spanCode`
//! and `literalCode`. They are a closed set of four, so they are a closed set of
//! four here, and the two that do not care about direction do not take one.

use crate::decode::ShadowFlags;
use crate::error::{Error, Result};
use crate::geometry::{Direction, Span};

/// Entries in one palette.
pub const PALETTE_LEN: usize = 256;

/// How literal payload bytes reach the surface.
#[derive(Clone, Copy, Debug, Default)]
pub enum Pixels<'a> {
    /// Copy palette indices verbatim.
    #[default]
    Direct,
    /// Send every index through a 256-entry translation table first.
    ///
    /// This is the creature palette swap and the coloured font path.
    Remap(&'a [u8; PALETTE_LEN]),
}

impl Pixels<'_> {
    /// Applies the mapping to a single palette index.
    #[must_use]
    pub const fn map(self, index: u8) -> u8 {
        match self {
            Self::Direct => index,
            Self::Remap(table) => table[index as usize],
        }
    }
}

/// A flat table of 256-entry dim palettes, indexed by shadow level.
///
/// The retail table holds twelve. A stream can encode levels up to fifteen, so
/// a level is checked here rather than trusted.
#[derive(Clone, Copy, Debug)]
pub struct DimPalettes<'a> {
    flat: &'a [u8],
}

impl<'a> DimPalettes<'a> {
    /// Wraps a flat `level * 256` table.
    #[must_use]
    pub const fn new(flat: &'a [u8]) -> Self {
        Self { flat }
    }

    /// Number of complete palettes available.
    #[must_use]
    pub const fn count(&self) -> usize {
        self.flat.len() / PALETTE_LEN
    }

    /// The palette for `level`.
    ///
    /// # Errors
    ///
    /// Returns [`Error::DimLevelOutOfRange`] when the table is too short.
    pub fn level(&self, level: u8) -> Result<&'a [u8]> {
        let start = usize::from(level) * PALETTE_LEN;
        self.flat
            .get(start..start + PALETTE_LEN)
            .ok_or(Error::DimLevelOutOfRange {
                level,
                available: self.count(),
            })
    }

    /// The palette a shadow run selects.
    ///
    /// # Errors
    ///
    /// Returns [`Error::DimLevelOutOfRange`] when the table is too short.
    pub fn for_shadow(&self, flags: ShadowFlags) -> Result<&'a [u8]> {
        self.level(flags.level())
    }
}

/// Lays down one palette index across the span. Order-independent.
pub(crate) fn solid(row: &mut [u8], span: Span, color: u8) {
    let end = span.left() + span.width();
    if let Some(target) = row.get_mut(span.left()..end) {
        target.fill(color);
    }
}

/// Remaps what is already on the surface through `palette`. Order-independent.
pub(crate) fn dim(row: &mut [u8], span: Span, palette: &[u8]) {
    let end = span.left() + span.width();
    if let Some(target) = row.get_mut(span.left()..end) {
        for pixel in target {
            if let Some(&mapped) = palette.get(*pixel as usize) {
                *pixel = mapped;
            }
        }
    }
}

/// Copies literal payload onto the surface.
///
/// This is the one operation where direction matters: the payload always runs
/// forwards while the destination runs backwards for a mirrored frame. The
/// direction comes from the [`Span`], which is the only thing that knows it.
pub(crate) fn literal(row: &mut [u8], span: Span, payload: &[u8], pixels: Pixels) {
    let end = span.left() + span.width();
    let Some(target) = row.get_mut(span.left()..end) else {
        return;
    };
    let source = payload.iter().skip(span.source_skip()).copied();
    // The span is already resolved against this row, so the bounds check belongs
    // outside the loop - and hoisting it also makes literal agree with solid and
    // dim about what an out-of-range span means: paint nothing.
    match span.direction() {
        Direction::Forward => {
            for (pixel, index) in target.iter_mut().zip(source) {
                *pixel = pixels.map(index);
            }
        }
        Direction::Mirrored => {
            for (pixel, index) in target.iter_mut().rev().zip(source) {
                *pixel = pixels.map(index);
            }
        }
    }
}
