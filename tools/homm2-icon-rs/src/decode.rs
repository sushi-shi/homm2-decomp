//! Opcode decoding: the pure half of the pipeline.
//!
//! Decoding answers "what run comes next" and nothing else. It has no notion of
//! a surface, a direction or a clip rectangle, which is what makes it directly
//! testable and lets every blitter in the crate share exactly one copy of it.
//!
//! # Colour dialect
//!
//! ```text
//! bit  7 6 5 4 3 2 1 0
//!     +-+-------------+
//!     |0| n n n n n n n |   0x00        end of row
//!     +-+-------------+     0x01..0x7f  literal run of n bytes, payload inline
//!
//!     +-+-+-----------+
//!     |1|0| c c c c c c |   0x80        end of image
//!     +-+-+-----------+     0x81..0xbf  skip c pixels, c = op & 0x3f
//!
//!     +-+-+-----------+
//!     |1|1| c c c c c c |   0xc0        shadow run, see ShadowFlags
//!     +-+-+-----------+     0xc1        long solid: count byte, then colour byte
//!                           0xc2..0xff  solid run of c pixels, then colour byte
//! ```
//!
//! A one-pixel solid run cannot be encoded, because `0xc1` is taken as the long
//! escape; encoders emit a one-byte literal instead.
//!
//! # Mask dialect
//!
//! ```text
//! bit  7 6 5 4 3 2 1 0
//!     +-+-------------+
//!     |0| n n n n n n n |   0x00        end of row
//!     +-+-------------+     0x01..0x7f  span of n pixels, value supplied by caller
//!
//!     +-+-------------+
//!     |1| c c c c c c c |   0x80        end of image
//!     +-+-------------+     0x81..0xff  skip c pixels, c = op & 0x7f
//! ```
//!
//! Note the wider skip field: the mask dialect has no solid flag stealing bit 6,
//! so its skips reach 127 where the colour dialect's stop at 63.

use crate::error::{Error, Result};

const END_OF_ROW: u8 = 0x00;
const END_OF_IMAGE: u8 = 0x80;
const SOLID_FLAG: u8 = 0x40;
const COLOR_RUN_MASK: u8 = 0x3f;
const MASK_RUN_MASK: u8 = 0x7f;
const LONG_SOLID: u8 = 0xc1;

const SHADOW_SHORT_LEN_MASK: u8 = 0x03;
const SHADOW_LEVEL_MASK: u8 = 0x3c;
const SHADOW_LEVEL_SHIFT: u32 = 2;
const SHADOW_APPLY_FLAG: u8 = 0x40;
const SHADOW_RECOLOR_FLAG: u8 = 0x80;

/// The flags byte of a `0xc0` shadow run.
///
/// ```text
/// bit  7 6 5 4 3 2 1 0
///     +-+-+-------+---+
///     |R|A| level |len|
///     +-+-+-------+---+
///      | |    |     +--- 0x03  length 1..3, or 0 meaning "a count byte follows"
///      | |    +--------- 0x3c  dim palette level, effective index = (raw >> 2) & 0xf
///      | +-------------- 0x40  apply: 0 makes the run a plain transparent skip
///      +---------------- 0x80  recolourable by the caller's highlight colour
/// ```
///
/// The recolour bit is tested before the apply bit, so a recolourable run is
/// painted solid even when apply is clear.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct ShadowFlags(u8);

impl ShadowFlags {
    /// Wraps a raw flags byte.
    #[must_use]
    pub const fn from_raw(raw: u8) -> Self {
        Self(raw)
    }

    /// The raw byte as it appeared in the stream.
    #[must_use]
    pub const fn raw(self) -> u8 {
        self.0
    }

    /// Dim palette index this run selects, `0..=15`.
    ///
    /// Only `0..=11` address a palette in the retail table; higher levels read
    /// past its end.
    #[must_use]
    pub const fn level(self) -> u8 {
        (self.0 & SHADOW_LEVEL_MASK) >> SHADOW_LEVEL_SHIFT
    }

    /// Whether the run dims what is underneath it, as opposed to being a plain
    /// transparent skip.
    #[must_use]
    pub const fn applies(self) -> bool {
        self.0 & SHADOW_APPLY_FLAG != 0
    }

    /// Whether a non-zero highlight colour replaces this run with a solid fill.
    #[must_use]
    pub const fn recolorable(self) -> bool {
        self.0 & SHADOW_RECOLOR_FLAG != 0
    }
}

/// One decoded colour-dialect run.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Op<'a> {
    /// Raw palette indices to copy, one per pixel.
    Literal(&'a [u8]),
    /// `len` pixels of a single palette index.
    Solid {
        /// Run length in pixels.
        len: u32,
        /// Palette index to lay down.
        color: u8,
    },
    /// `len` pixels of shadow, transparency or highlight, per `flags`.
    Shadow {
        /// Run length in pixels.
        len: u32,
        /// Behaviour selector for the run.
        flags: ShadowFlags,
    },
    /// `len` transparent pixels.
    Skip(u32),
    /// Move to the next row.
    EndOfRow,
}

/// One decoded mask-dialect run.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum MaskOp {
    /// `len` covered pixels; the value comes from the caller, not the stream.
    Span(u32),
    /// `len` transparent pixels.
    Skip(u32),
    /// Move to the next row.
    EndOfRow,
}

/// Reads bytes out of a pixel stream, reporting truncation rather than panicking.
struct Cursor<'a> {
    bytes: &'a [u8],
    at: usize,
}

impl<'a> Cursor<'a> {
    const fn new(bytes: &'a [u8]) -> Self {
        Self { bytes, at: 0 }
    }

    /// True when the cursor sits exactly at the end of the stream, i.e. the
    /// previous opcode completed but no end-of-image ever arrived.
    const fn exhausted(&self) -> bool {
        self.at >= self.bytes.len()
    }

    fn byte(&mut self, what: &'static str) -> Result<u8> {
        let value = *self.bytes.get(self.at).ok_or(Error::Truncated {
            what,
            needed: self.at + 1,
            available: self.bytes.len(),
        })?;
        self.at += 1;
        Ok(value)
    }

    fn slice(&mut self, len: usize, what: &'static str) -> Result<&'a [u8]> {
        let end = self.at + len;
        let value = self.bytes.get(self.at..end).ok_or(Error::Truncated {
            what,
            needed: end,
            available: self.bytes.len(),
        })?;
        self.at = end;
        Ok(value)
    }
}

/// Iterator over the runs of a colour-dialect stream.
///
/// Yields until the end-of-image opcode, then stops. A malformed stream yields
/// exactly one [`Err`] and then stops.
pub struct ColorOps<'a> {
    cursor: Cursor<'a>,
    done: bool,
}

impl<'a> ColorOps<'a> {
    /// Starts decoding at the beginning of `stream`.
    #[must_use]
    pub const fn new(stream: &'a [u8]) -> Self {
        Self {
            cursor: Cursor::new(stream),
            done: false,
        }
    }

    fn step(&mut self) -> Result<Option<Op<'a>>> {
        if self.cursor.exhausted() {
            return Err(Error::UnterminatedStream);
        }
        let op = self.cursor.byte("opcode")?;

        // Literal half of the space: the top bit is clear.
        if op & END_OF_IMAGE == 0 {
            if op == END_OF_ROW {
                return Ok(Some(Op::EndOfRow));
            }
            let payload = self.cursor.slice(usize::from(op), "literal payload")?;
            return Ok(Some(Op::Literal(payload)));
        }

        // Skip half: top bit set, solid flag clear.
        if op & SOLID_FLAG == 0 {
            let len = op & COLOR_RUN_MASK;
            if len == 0 {
                return Ok(None); // 0x80, end of image
            }
            return Ok(Some(Op::Skip(u32::from(len))));
        }

        // Solid and shadow: top two bits set.
        let short_len = op & COLOR_RUN_MASK;
        if short_len != 0 {
            let len = if op == LONG_SOLID {
                u32::from(self.cursor.byte("long solid length")?)
            } else {
                u32::from(short_len)
            };
            let color = self.cursor.byte("solid colour")?;
            return Ok(Some(Op::Solid { len, color }));
        }

        let raw = self.cursor.byte("shadow flags")?;
        let short = raw & SHADOW_SHORT_LEN_MASK;
        let len = if short == 0 {
            u32::from(self.cursor.byte("shadow length")?)
        } else {
            u32::from(short)
        };
        Ok(Some(Op::Shadow {
            len,
            flags: ShadowFlags::from_raw(raw),
        }))
    }
}

impl<'a> Iterator for ColorOps<'a> {
    type Item = Result<Op<'a>>;

    fn next(&mut self) -> Option<Self::Item> {
        if self.done {
            return None;
        }
        match self.step() {
            Ok(Some(op)) => Some(Ok(op)),
            Ok(None) => {
                self.done = true;
                None
            }
            Err(error) => {
                self.done = true;
                Some(Err(error))
            }
        }
    }
}

/// Iterator over the runs of a mask-dialect stream.
pub struct MaskOps<'a> {
    cursor: Cursor<'a>,
    done: bool,
}

impl<'a> MaskOps<'a> {
    /// Starts decoding at the beginning of `stream`.
    #[must_use]
    pub const fn new(stream: &'a [u8]) -> Self {
        Self {
            cursor: Cursor::new(stream),
            done: false,
        }
    }

    fn step(&mut self) -> Result<Option<MaskOp>> {
        if self.cursor.exhausted() {
            return Err(Error::UnterminatedStream);
        }
        let op = self.cursor.byte("opcode")?;
        if op & END_OF_IMAGE == 0 {
            if op == END_OF_ROW {
                return Ok(Some(MaskOp::EndOfRow));
            }
            return Ok(Some(MaskOp::Span(u32::from(op))));
        }
        let len = op & MASK_RUN_MASK;
        if len == 0 {
            return Ok(None); // 0x80, end of image
        }
        Ok(Some(MaskOp::Skip(u32::from(len))))
    }
}

impl Iterator for MaskOps<'_> {
    type Item = Result<MaskOp>;

    fn next(&mut self) -> Option<Self::Item> {
        if self.done {
            return None;
        }
        match self.step() {
            Ok(Some(op)) => Some(Ok(op)),
            Ok(None) => {
                self.done = true;
                None
            }
            Err(error) => {
                self.done = true;
                Some(Err(error))
            }
        }
    }
}
