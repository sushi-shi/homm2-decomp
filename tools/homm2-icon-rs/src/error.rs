//! Errors produced while parsing or decoding an icon.

use core::fmt;

/// Everything that can go wrong reading an icon or one of its pixel streams.
///
/// The retail decoders validate none of this: they trust the frame index, the
/// stream offset and the presence of a terminator, and walk off the end of the
/// destination surface when any of them is wrong. This port turns each of those
/// implicit trust assumptions into an explicit error.
/// Deliberately **not** `#[non_exhaustive]`. ICN is a shipped 1996 format: it
/// cannot grow, the API surface that reads it is fixed, and every failure mode
/// below is enumerable from the format itself. Sealing the enum would force each
/// consumer to write an unreachable `_ =>` arm and would cost them exhaustiveness
/// checking - which is exactly the signal that should fire if this set ever does
/// change. Adding a variant here is meant to be a breaking change.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Error {
    /// The buffer ended before a structure could be read in full.
    Truncated {
        /// What the parser was trying to read.
        what: &'static str,
        /// Bytes the parser needed.
        needed: usize,
        /// Bytes actually available.
        available: usize,
    },
    /// A frame index was not present in the icon.
    FrameOutOfRange {
        /// The requested index.
        index: usize,
        /// Number of frames the icon declares.
        count: usize,
    },
    /// A frame's `data_offset` points outside the icon body.
    StreamOutOfRange {
        /// The frame that owns the bad offset.
        index: usize,
        /// The offset stored in the frame header.
        offset: u32,
        /// Length of the icon body.
        body_len: usize,
    },
    /// A pixel stream ended without an end-of-image opcode.
    UnterminatedStream,
    /// The pixel buffer handed to a [`Canvas`](crate::Canvas) does not match its
    /// declared dimensions, or those dimensions are negative.
    CanvasGeometry {
        /// Requested width.
        width: i32,
        /// Requested height.
        height: i32,
        /// Length of the supplied pixel slice.
        pixels: usize,
    },
    /// A dim-palette table is too short for the level a stream selected.
    DimLevelOutOfRange {
        /// The level encoded in the shadow run.
        level: u8,
        /// Number of complete 256-entry palettes available.
        available: usize,
    },
    /// A downscale factor outside the range the sampler can express.
    ScaleOutOfRange {
        /// The requested output edge length.
        scale: u32,
    },
}

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match *self {
            Self::Truncated {
                what,
                needed,
                available,
            } => write!(f, "truncated {what}: need {needed} bytes, have {available}"),
            Self::FrameOutOfRange { index, count } => {
                write!(f, "frame {index} out of range (icon has {count})")
            }
            Self::StreamOutOfRange {
                index,
                offset,
                body_len,
            } => write!(
                f,
                "frame {index} stream offset {offset:#x} is outside a {body_len}-byte body"
            ),
            Self::UnterminatedStream => f.write_str("pixel stream ended without an end-of-image"),
            Self::CanvasGeometry {
                width,
                height,
                pixels,
            } => write!(f, "canvas {width}x{height} does not fit {pixels} pixels"),
            Self::DimLevelOutOfRange { level, available } => {
                write!(
                    f,
                    "dim level {level} requested, {available} palettes present"
                )
            }
            Self::ScaleOutOfRange { scale } => {
                write!(f, "scale {scale} outside the supported range 1..=32")
            }
        }
    }
}

impl core::error::Error for Error {}

/// Convenience alias for this crate's fallible operations.
pub type Result<T> = core::result::Result<T, Error>;
