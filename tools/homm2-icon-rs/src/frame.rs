//! Icon container and frame table.
//!
//! An icon record is a six-byte header followed by a body. The body opens with
//! one 13-byte [`Frame`] per frame and continues with the pixel streams those
//! frames point at.
//!
//! ```text
//! record
//! +--------+--------+-------------------------------------------+
//! | u16    | u32    | body                                      |
//! | count  | length |                                           |
//! +--------+--------+-------------------------------------------+
//!                    |
//!                    v
//!                   +===================+ <- +0
//!                   | Frame[0]  13 B    |
//!                   | Frame[1]  13 B    |   frame table
//!                   | ...               |
//!                   +===================+ <- +13*count
//!                   | pixel streams     |   reached via Frame::data_offset,
//!                   |                   |   which is relative to the body
//!                   +===================+
//! ```

use crate::error::{Error, Result};

/// Bytes in the record header that precedes the body.
pub const RECORD_HEADER_LEN: usize = 6;

/// Bytes in one serialized frame header.
pub const FRAME_LEN: usize = 13;

/// Frame-header byte that the published format documentation uses to tag a
/// monochrome frame.
///
/// The retail decoders never read this byte; the call site picks the dialect
/// instead. It is exposed because it is the only in-band hint that a frame's
/// stream is [`Dialect::Mask`] rather than [`Dialect::Color`].
pub const KIND_MASK_FRAME: u8 = 32;

/// Which run-length encoding a pixel stream uses.
///
/// The two are mutually incompatible: feeding a colour stream to the mask
/// decoder desynchronises immediately, because the mask dialect treats a
/// literal opcode as a span and never steps over the inline payload.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Dialect {
    /// Full-colour sprites: literals, solid runs and shadow runs.
    Color,
    /// Silhouettes: spans and skips only, with the pixel value supplied by the
    /// caller rather than the stream.
    Mask,
}

/// One frame header from the frame table.
///
/// ```text
///  +0  offset_x    i16   signed, pen-relative
///  +2  offset_y    i16   signed, pen-relative
///  +4  width       i16   signed in the file; retail loads it with movsx
///  +6  height      i16   signed in the file; retail loads it with movsx
///  +8  kind        u8    advisory dialect tag, unread by retail
///  +9  data_offset u32   byte offset from the body start
/// ```
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct Frame {
    /// Signed horizontal offset of the frame's left edge from the pen.
    pub offset_x: i16,
    /// Signed vertical offset of the frame's top edge from the pen.
    pub offset_y: i16,
    /// Frame width in pixels.
    ///
    /// Retail loads this with `movsx`, so it is signed on the wire even though no
    /// shipped frame is negative. Modelled unsigned because every consumer here
    /// treats it as an extent; the distinction is recorded rather than erased.
    pub width: u16,
    /// Frame height in pixels. Signed on the wire; see [`Frame::width`].
    pub height: u16,
    /// Raw dialect tag. See [`KIND_MASK_FRAME`].
    ///
    /// This is a whole byte in the file. Modelling it any narrower loses the
    /// documented value 32.
    pub kind: u8,
    /// Byte offset of this frame's pixel stream, relative to the body start.
    pub data_offset: u32,
}

impl Frame {
    /// Parses one frame header from exactly [`FRAME_LEN`] bytes.
    fn parse(bytes: &[u8; FRAME_LEN]) -> Self {
        let word = |lo: usize| u16::from_le_bytes([bytes[lo], bytes[lo + 1]]);
        Self {
            offset_x: word(0).cast_signed(),
            offset_y: word(2).cast_signed(),
            width: word(4),
            height: word(6),
            kind: bytes[8],
            data_offset: u32::from_le_bytes([bytes[9], bytes[10], bytes[11], bytes[12]]),
        }
    }

    /// The dialect this frame's `kind` byte advertises.
    ///
    /// Advisory only: retail ignores it, so a frame may well be drawn with the
    /// other dialect's decoder.
    #[must_use]
    pub fn advertised_dialect(&self) -> Dialect {
        if self.kind == KIND_MASK_FRAME {
            Dialect::Mask
        } else {
            Dialect::Color
        }
    }
}

/// A frame header paired with the stream it points at.
///
/// Constructing one proves the stream offset is inside the body, so the blitters
/// that consume a `FrameData` cannot fail a lookup. That is the whole point of
/// the type: it moves two error cases out of the drawing path into one place
/// where they are checked once. The fields are private precisely so that claim
/// holds - [`Icon::frame_data`] is the only way to obtain one.
#[derive(Clone, Copy, Debug)]
pub struct FrameData<'a> {
    header: Frame,
    stream: &'a [u8],
}

impl<'a> FrameData<'a> {
    /// The frame's header.
    #[must_use]
    pub const fn header(&self) -> Frame {
        self.header
    }

    /// The frame's pixel stream, running to the end of the body.
    #[must_use]
    pub const fn stream(&self) -> &'a [u8] {
        self.stream
    }
}

/// A parsed icon: a validated frame table plus the body it indexes into.
///
/// Allocation-free and [`Copy`]. Building one proves the frame table is present
/// in full, so [`Icon::frame`] cannot fail on truncation; the only way to miss
/// is to ask for an index that does not exist, which is a caller mistake rather
/// than a data defect and is reported as [`None`].
#[derive(Clone, Copy, Debug)]
pub struct Icon<'a> {
    /// Exactly `count * FRAME_LEN` bytes, proven present at construction.
    table: &'a [u8],
    body: &'a [u8],
}

impl<'a> Icon<'a> {
    /// Parses a complete icon record, header included.
    ///
    /// # Errors
    ///
    /// Returns [`Error::Truncated`] if the header, the declared body length or
    /// the frame table runs past the end of `bytes`.
    pub fn parse(bytes: &'a [u8]) -> Result<Self> {
        let header = bytes
            .first_chunk::<RECORD_HEADER_LEN>()
            .ok_or(Error::Truncated {
                what: "icon record header",
                needed: RECORD_HEADER_LEN,
                available: bytes.len(),
            })?;

        let count = u16::from_le_bytes([header[0], header[1]]);
        let length = u32::from_le_bytes([header[2], header[3], header[4], header[5]]);
        let length = usize::try_from(length).unwrap_or(usize::MAX);

        let rest = &bytes[RECORD_HEADER_LEN..];
        let body = rest.get(..length).ok_or(Error::Truncated {
            what: "icon body",
            needed: length,
            available: rest.len(),
        })?;

        Self::from_body(count, body)
    }

    /// Builds an icon from an already-extracted body.
    ///
    /// # Errors
    ///
    /// Returns [`Error::Truncated`] if `body` is too short to hold `count`
    /// frame headers.
    pub fn from_body(count: u16, body: &'a [u8]) -> Result<Self> {
        let table_len = usize::from(count) * FRAME_LEN;
        let table = body.get(..table_len).ok_or(Error::Truncated {
            what: "frame table",
            needed: table_len,
            available: body.len(),
        })?;
        Ok(Self { table, body })
    }

    /// Number of frames the icon declares.
    #[must_use]
    pub const fn frame_count(&self) -> usize {
        self.table.len() / FRAME_LEN
    }

    /// The whole icon body, frame table included.
    #[must_use]
    pub const fn body(&self) -> &'a [u8] {
        self.body
    }

    /// Looks up one frame header.
    ///
    /// Infallible except for the index: the constructor already proved every
    /// header in the table is present.
    #[must_use]
    pub fn frame(&self, index: usize) -> Option<Frame> {
        self.table
            .get(index.checked_mul(FRAME_LEN)?..)
            .and_then(<[u8]>::first_chunk::<FRAME_LEN>)
            .map(Frame::parse)
    }

    /// Every frame header, in file order.
    ///
    /// The iterator borrows the icon's body, not the icon, so it outlives the
    /// `&self` it was created from - which matters because [`Icon`] is [`Copy`]
    /// and a caller has no allocator to `collect()` into.
    #[must_use = "iterators are lazy"]
    pub fn frames(&self) -> impl ExactSizeIterator<Item = Frame> + use<'a> {
        let table: &'a [u8] = self.table;
        let (headers, _) = table.as_chunks::<FRAME_LEN>();
        headers.iter().map(Frame::parse)
    }

    /// Resolves a frame's header and stream together.
    ///
    /// The stream deliberately runs to the end of the body rather than to the
    /// next frame's offset: streams are self-terminating, frames may share
    /// suffixes, and they need not appear in offset order.
    ///
    /// # Errors
    ///
    /// Returns [`Error::FrameOutOfRange`] for a bad index, or
    /// [`Error::StreamOutOfRange`] when the frame's offset falls outside the
    /// body.
    pub fn frame_data(&self, index: usize) -> Result<FrameData<'a>> {
        let header = self.frame(index).ok_or(Error::FrameOutOfRange {
            index,
            count: self.frame_count(),
        })?;
        let offset = usize::try_from(header.data_offset).unwrap_or(usize::MAX);
        let stream = self.body.get(offset..).ok_or(Error::StreamOutOfRange {
            index,
            offset: header.data_offset,
            body_len: self.body.len(),
        })?;
        Ok(FrameData { header, stream })
    }
}
