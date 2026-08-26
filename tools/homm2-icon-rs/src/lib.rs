#![no_std]
//! Decoder and blitter for the Heroes of Might and Magic II icon (ICN) sprite
//! format.
//!
//! The original ships eleven near-identical blitters, one per translation unit,
//! produced by cloning two ancestors and editing the copies. This crate keeps
//! the behaviour and drops the cloning: the variants are configurations of two
//! engines rather than eleven bodies. Direction is orthogonal to everything; the
//! pixel operation depends on the dialect, and is modelled that way. See
//! [`blit`] for the shape.
//!
//! ```text
//!   bytes ---> decode ---> place ---> paint ---> surface
//!              (pure)      (clip)     (pixels)
//!
//!   decode   ColorOps / MaskOps      opcode -> Op, no geometry at all
//!   place    Direction::clip         run + cursor + clip -> Span, or nothing
//!   paint    solid / dim / literal   Span -> pixels
//! ```
//!
//! Splitting it that way is what the original's macro layer reaches for and
//! misses. There, clipping is written out once per variant and the horizontal
//! test is a caller-supplied macro argument, so a handler can be invoked without
//! one. Here a [`Span`] is only obtainable from [`Direction::clip`], so a run
//! that was never tested against the clip rectangle cannot reach the surface.
//!
//! # Layers
//!
//! | Module | Responsibility |
//! | --- | --- |
//! | [`frame`] | container and frame table |
//! | [`decode`] | opcodes to runs, both dialects |
//! | [`geometry`] | direction, clipping, shear |
//! | [`paint`] | the four per-pixel operations |
//! | [`blit`] | the two engines |
//! | [`scale`] | nearest-neighbour downscale |
//! | [`retail`] | the eleven original entry points |
//! | [`agg`] | archive reader, for testing against real data |
//!
//! # Deviations from the original
//!
//! All deliberate, all in the direction of not corrupting memory:
//!
//! * Frame indices, stream offsets and stream terminators are validated instead
//!   of trusted.
//! * Drawing is always clipped to the surface. The original's unclipped path
//!   has no bounds test of any kind and relies on the caller.
//! * Shadow levels are range-checked against the palette table. A stream can
//!   encode sixteen levels where the retail table holds twelve - and the shipped
//!   assets really do use levels 12 and 13, so retail reads past `uDimPal` into
//!   the adjacent remap tables. Those 207 runs render differently here.
//! * The downscaler honours its clip rectangle. Retail forwards one only on the
//!   `scale == 32` pass-through and ignores it otherwise.
//!
//! One retail defect is reproducible on request rather than silently fixed; see
//! [`Quirks`].
//!
//! # Example
//!
//! ```
//! use homm2_icon::{Canvas, ColorBlit, Icon, Rect};
//!
//! # fn main() -> homm2_icon::Result<()> {
//! // One 4x1 frame: skip 1, then three pixels of colour 7.
//! let body = [
//!     0u8, 0, 0, 0, 4, 0, 1, 0, 0, 13, 0, 0, 0, // frame header
//!     0x81, 0xc3, 0x07, 0x00, 0x80, // skip 1, solid 3 x 7, end row, end image
//! ];
//! let icon = Icon::from_body(1, &body)?;
//!
//! let mut pixels = vec![0u8; 8];
//! let mut canvas = Canvas::new(8, 1, &mut pixels)?;
//! ColorBlit::new().draw(&mut canvas, icon.frame_data(0)?, 0, 0)?;
//! assert_eq!(canvas.pixels(), &[0, 7, 7, 7, 0, 0, 0, 0]);
//!
//! // Mirrored about the pen at x = 7.
//! let mut flipped = vec![0u8; 8];
//! let mut canvas = Canvas::new(8, 1, &mut flipped)?;
//! ColorBlit::new()
//!     .mirrored(true)
//!     .clip(Rect::new(0, 0, 8, 1))
//!     .draw(&mut canvas, icon.frame_data(0)?, 7, 0)?;
//! assert_eq!(canvas.pixels(), &[0, 0, 0, 0, 7, 7, 7, 0]);
//! # Ok(())
//! # }
//! ```

pub mod agg;
pub mod blit;
pub mod canvas;
pub mod decode;
pub mod error;
pub mod frame;
pub mod geometry;
pub mod paint;
pub mod retail;
pub mod scale;

pub use blit::{ColorBlit, MaskBlit, MaskPaint, Quirks, ShadowMode};
pub use canvas::Canvas;
pub use decode::{ColorOps, MaskOp, MaskOps, Op, ShadowFlags};
pub use error::{Error, Result};
pub use frame::{Dialect, Frame, FrameData, Icon};
pub use geometry::{Direction, Rect, RowOrigin, SHEAR_SKIP_ROW, Span};
pub use paint::{DimPalettes, PALETTE_LEN, Pixels};
pub use scale::{Scratch, draw_scaled};
