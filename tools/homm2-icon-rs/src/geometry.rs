//! Placement: turning a decoded run into the pixels it actually covers.
//!
//! This is the piece the original spreads across a dozen near-identical
//! clipping blocks. Everything a run needs to know about where it lands is
//! computed here, once, for both directions:
//!
//! ```text
//!   forward run, cursor at C, length n        mirrored run, cursor at C
//!
//!   C   C+1 C+2 ... C+n-1                     C-n+1 ... C-2 C-1  C
//!   +---+---+---+---+                         +---+---+---+---+
//!   | 0 | 1 | 2 |...|   <- source order       |...| 2 | 1 | 0 |   <- source order
//!   +---+---+---+---+                         +---+---+---+---+
//!         |<-- clip -->|                      |<-- clip -->|
//!         span.left                           span.left
//!         span.source_skip = 2                span.source_skip = 1
//! ```
//!
//! `source_skip` counts pixels consumed *in iteration order* before the first
//! visible one, which is the left end for a forward run and the right end for a
//! mirrored one. Getting that asymmetry wrong is what desynchronises a literal
//! run against its inline payload.

/// Shear-table value marking a row that must not be drawn.
pub const SHEAR_SKIP_ROW: i8 = 0x7f;

/// Horizontal direction a decoder walks while laying down runs.
#[derive(Clone, Copy, Debug, PartialEq, Eq, Default)]
pub enum Direction {
    /// Left to right; the frame is drawn as authored.
    #[default]
    Forward,
    /// Right to left; the frame is mirrored about the pen.
    Mirrored,
}

/// How a run that only partly overlaps the clip rectangle is treated.
#[derive(Clone, Copy, Debug, PartialEq, Eq, Default)]
pub(crate) enum ClipPolicy {
    /// Clamp the run to the visible part. This is what the code means to do.
    #[default]
    Clamp,
    /// Draw the run only when it lies entirely inside the clip rectangle, and
    /// drop it otherwise.
    ///
    /// Reproduces the guard retail uses for mirrored solid, shadow and mask
    /// runs, where the clamping branch is written but unreachable. Enable it
    /// through [`Quirks`](crate::Quirks) when byte-level agreement with the
    /// original matters more than correct output.
    WholeRunOnly,
}

/// An axis-aligned rectangle with a non-negative extent.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct Rect {
    /// Left edge, inclusive.
    pub x: i32,
    /// Top edge, inclusive.
    pub y: i32,
    /// Width in pixels; clamped to zero if negative.
    pub width: i32,
    /// Height in pixels; clamped to zero if negative.
    pub height: i32,
}

impl Rect {
    /// Builds a rectangle, clamping a negative extent to zero.
    #[must_use]
    pub const fn new(x: i32, y: i32, width: i32, height: i32) -> Self {
        Self {
            x,
            y,
            width: if width < 0 { 0 } else { width },
            height: if height < 0 { 0 } else { height },
        }
    }

    /// Right edge, inclusive.
    ///
    /// Saturates rather than overflowing: `Rect::new(1, 0, i32::MAX, 1)` is how a
    /// caller naturally spells "clip to everything", and it must not panic.
    #[must_use]
    pub const fn right(self) -> i32 {
        self.x.saturating_add(self.width).saturating_sub(1)
    }

    /// Bottom edge, inclusive. Saturates; see [`Rect::right`].
    #[must_use]
    pub const fn bottom(self) -> i32 {
        self.y.saturating_add(self.height).saturating_sub(1)
    }

    /// Whether the rectangle covers any pixels.
    #[must_use]
    pub const fn is_empty(self) -> bool {
        self.width == 0 || self.height == 0
    }

    /// Whether `y` falls inside the rectangle's rows.
    #[must_use]
    pub const fn contains_row(self, y: i32) -> bool {
        !self.is_empty() && y >= self.y && y <= self.bottom()
    }

    /// The overlap of two rectangles, or `None` when they are disjoint.
    #[must_use]
    pub fn intersect(self, other: Self) -> Option<Self> {
        let x = self.x.max(other.x);
        let y = self.y.max(other.y);
        let right = self.right().min(other.right());
        let bottom = self.bottom().min(other.bottom());
        if x > right || y > bottom {
            return None;
        }
        Some(Self {
            x,
            y,
            width: right - x + 1,
            height: bottom - y + 1,
        })
    }

    /// Whether the rectangle fully contains `other`.
    #[must_use]
    pub fn contains(self, other: Self) -> bool {
        other.is_empty()
            || (!self.is_empty()
                && self.x <= other.x
                && self.y <= other.y
                && other.right() <= self.right()
                && other.bottom() <= self.bottom())
    }
}

/// The visible part of a run, resolved against the clip rectangle.
///
/// Only constructible through [`Direction::clip`], so a span always carries a
/// horizontal test. There is no way to paint a run without one.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct Span {
    left: usize,
    len: usize,
    source_skip: usize,
    direction: Direction,
}

impl Span {
    /// Leftmost destination column covered.
    #[must_use]
    pub const fn left(self) -> usize {
        self.left
    }

    /// Number of visible pixels; never zero, by construction.
    #[must_use]
    pub const fn width(self) -> usize {
        self.len
    }

    /// Source pixels consumed before the first visible one, in iteration order.
    #[must_use]
    pub const fn source_skip(self) -> usize {
        self.source_skip
    }

    /// The direction this span was resolved for.
    ///
    /// Carried rather than re-supplied by the caller: `source_skip` is computed
    /// *from* the direction, so the two must agree or a literal run desynchronises
    /// against its payload. Keeping them in one value makes disagreement
    /// unrepresentable.
    #[must_use]
    pub const fn direction(self) -> Direction {
        self.direction
    }

    /// Destination columns in the order the source is consumed.
    ///
    /// Ascending for [`Direction::Forward`], descending for
    /// [`Direction::Mirrored`], so callers can always zip this against the
    /// source running forwards.
    pub fn columns(self) -> impl Iterator<Item = usize> + Clone {
        let last = self.left + self.len - 1;
        (0..self.len).map(move |step| match self.direction {
            Direction::Forward => self.left + step,
            Direction::Mirrored => last - step,
        })
    }
}

impl Direction {
    /// Where the cursor sits after laying down `len` pixels.
    #[must_use]
    pub const fn advance(self, cursor: i32, len: u32) -> i32 {
        match self {
            Self::Forward => cursor.saturating_add_unsigned(len),
            Self::Mirrored => cursor.saturating_sub_unsigned(len),
        }
    }

    /// The columns a run occupies before clipping, as an inclusive range.
    #[must_use]
    pub const fn extent(self, cursor: i32, len: u32) -> (i32, i32) {
        match self {
            Self::Forward => (
                cursor,
                cursor.saturating_add_unsigned(len).saturating_sub(1),
            ),
            Self::Mirrored => (
                cursor.saturating_sub_unsigned(len).saturating_add(1),
                cursor,
            ),
        }
    }

    /// Resolves a run against an inclusive horizontal clip range.
    ///
    /// Returns `None` when nothing is visible, which is also what
    /// [`ClipPolicy::WholeRunOnly`] returns for a partially visible run.
    ///
    /// Crate-internal: the clip range must already be intersected with the
    /// destination surface, which only the blitters know how to do. Exposing it
    /// would let a caller mint a [`Span`] that satisfies *some* horizontal test
    /// but not the one belonging to the row it is about to paint.
    #[must_use]
    pub(crate) fn clip(
        self,
        cursor: i32,
        len: u32,
        clip_left: i32,
        clip_right: i32,
        policy: ClipPolicy,
    ) -> Option<Span> {
        if len == 0 || clip_left > clip_right {
            return None;
        }
        let (run_left, run_right) = self.extent(cursor, len);

        if policy == ClipPolicy::WholeRunOnly && (run_left < clip_left || run_right > clip_right) {
            return None;
        }

        let visible_left = run_left.max(clip_left);
        let visible_right = run_right.min(clip_right);
        if visible_left > visible_right {
            return None;
        }

        let source_skip = match self {
            Self::Forward => visible_left.abs_diff(run_left),
            Self::Mirrored => run_right.abs_diff(visible_right),
        };

        Some(Span {
            left: usize::try_from(visible_left).ok()?,
            len: usize::try_from(visible_right.abs_diff(visible_left)).ok()? + 1,
            source_skip: usize::try_from(source_skip).ok()?,
            direction: self,
        })
    }
}

/// Where each row of a frame starts horizontally.
///
/// The sheared variant reproduces the per-screen-row displacement table the
/// combat ripple and vaporise effects drive.
#[derive(Clone, Copy, Debug, Default)]
pub enum RowOrigin<'a> {
    /// Every row starts at the frame's own left or right edge.
    #[default]
    Fixed,
    /// Row `y` is displaced by `table[y]`; [`SHEAR_SKIP_ROW`] suppresses it.
    Sheared(&'a [i8]),
}

impl RowOrigin<'_> {
    /// Horizontal displacement applied to row `y`, oriented for `direction`.
    ///
    /// Out-of-range rows displace by zero, matching a table read that never
    /// happens because the row is off-surface anyway.
    #[must_use]
    pub fn displacement(self, y: i32, direction: Direction) -> i32 {
        let Self::Sheared(table) = self else {
            return 0;
        };
        let shear = usize::try_from(y)
            .ok()
            .and_then(|index| table.get(index).copied())
            .unwrap_or(0);
        match direction {
            Direction::Forward => i32::from(shear),
            Direction::Mirrored => -i32::from(shear),
        }
    }

    /// Whether row `y` may be painted at all.
    #[must_use]
    pub fn row_enabled(self, y: i32) -> bool {
        let Self::Sheared(table) = self else {
            return true;
        };
        usize::try_from(y)
            .ok()
            .and_then(|index| table.get(index).copied())
            .is_none_or(|shear| shear != SHEAR_SKIP_ROW)
    }
}
