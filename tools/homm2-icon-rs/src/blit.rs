//! The blit engines.
//!
//! There are two, one per dialect, and between them they cover every variant the
//! original ships as a separate translation unit. The variants are not new
//! algorithms; they are points in a small product space:
//!
//! ```text
//!   dialect          direction     pixel operation        modifiers
//!   ---------------  -----------   --------------------   ----------------------
//!   Color  ------->  Forward   x   Direct | Remap(table)   shear table,
//!                    Mirrored                              highlight colour,
//!                                                          shadow mode
//!   Mask   ------->  Forward   x   Solid(colour) | Dim
//!                    Mirrored
//! ```
//!
//! Direction is genuinely orthogonal. The pixel operation is *not*: it depends on
//! the dialect, which is why `Pixels` and `MaskPaint` are two disjoint enums on
//! two engines rather than one flat set - a `MaskBlit` cannot be given a remap
//! table, and the type system says so. Eight combinations exist, not sixteen.
//!
//! Retail has a fourth axis this crate deliberately collapses: a clip mode, where
//! `ICON_DRAW_NO_CLIP` takes an entirely separate unchecked path. Here drawing is
//! always clipped, so that axis is gone rather than factored. See
//! [`crate`] for the full list of deviations.
//!
//! Each remaining axis is a type, so a variant is a configuration rather than a
//! copy. See [`crate::retail`] for the eleven original entry points expressed
//! that way.

use crate::canvas::Canvas;
use crate::decode::{ColorOps, MaskOp, MaskOps, Op};
use crate::error::Result;
use crate::frame::{Frame, FrameData};
use crate::geometry::{ClipPolicy, Direction, Rect, RowOrigin};
use crate::paint::{self, DimPalettes, Pixels};

/// Opt-in reproductions of retail behaviour that is wrong but load-bearing.
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub struct Quirks {
    /// Drop a mirrored solid, shadow or mask run that straddles a clip edge
    /// instead of clamping it.
    ///
    /// Every mirrored decoder in the original guards these runs with
    /// `clip_left <= run_left && run_right <= clip_right` and then re-tests the
    /// first half inside, leaving the clamping branch unreachable. Forward
    /// decoders clamp correctly, and mirrored *literal* runs clamp correctly,
    /// so the artefact is a mirrored sprite that loses its solid and shadow
    /// runs at a clip boundary while its literals survive.
    pub mirrored_clip_drops_partial_runs: bool,
}

impl Quirks {
    /// Behave like the shipped game.
    #[must_use]
    pub const fn retail() -> Self {
        Self {
            mirrored_clip_drops_partial_runs: true,
        }
    }

    /// Clip correctly. This is the default.
    #[must_use]
    pub const fn corrected() -> Self {
        Self {
            mirrored_clip_drops_partial_runs: false,
        }
    }
}

/// What a colour-dialect shadow run does to the surface.
#[derive(Clone, Copy, Debug, Default)]
pub enum ShadowMode<'a> {
    /// Dim the surface through the palette the run selects.
    Apply(DimPalettes<'a>),
    /// Leave the surface untouched; the run only advances the cursor.
    ///
    /// This is the original's cleared dim gate.
    #[default]
    Ignore,
}

/// What a mask-dialect span puts on the surface.
#[derive(Clone, Copy, Debug)]
pub enum MaskPaint<'a> {
    /// Fill the span with one palette index.
    Solid(u8),
    /// Remap the span through a dim palette.
    Dim {
        /// Table to select from.
        palettes: DimPalettes<'a>,
        /// Flat level, `0..=11` in the retail table.
        level: u8,
    },
}

/// Shared per-call geometry: where the frame lands and what may be painted.
struct Placement {
    /// Column each row starts from, before shear.
    base: i32,
    /// First screen row of the frame.
    top: i32,
    /// Clip rectangle already intersected with the surface.
    clip: Rect,
}

impl Placement {
    fn resolve(
        canvas: &Canvas<'_>,
        frame: Frame,
        x: i32,
        y: i32,
        direction: Direction,
        clip: Option<Rect>,
    ) -> Self {
        let width = i32::from(frame.width);
        let offset_x = i32::from(frame.offset_x);
        // Saturating throughout: offset_x is an i16 straight out of the file and
        // the pen is caller-supplied, so their sum is not bounded by anything.
        let left = match direction {
            Direction::Forward => x.saturating_add(offset_x),
            Direction::Mirrored => x
                .saturating_sub(offset_x)
                .saturating_sub(width)
                .saturating_add(1),
        };
        let base = match direction {
            Direction::Forward => left,
            Direction::Mirrored => left.saturating_add(width).saturating_sub(1),
        };
        let clip = clip
            .unwrap_or_else(|| canvas.bounds())
            .intersect(canvas.bounds())
            .unwrap_or(Rect::new(0, 0, 0, 0));
        Self {
            base,
            top: y.saturating_add(i32::from(frame.offset_y)),
            clip,
        }
    }

    /// Inclusive horizontal clip range; empty when nothing is visible.
    const fn horizontal(&self) -> (i32, i32) {
        if self.clip.is_empty() {
            (0, -1)
        } else {
            (self.clip.x, self.clip.right())
        }
    }
}

/// Blitter for the colour dialect: sprites, fonts, creatures, map objects.
#[derive(Clone, Copy, Debug, Default)]
pub struct ColorBlit<'a> {
    direction: Direction,
    clip: Option<Rect>,
    pixels: Pixels<'a>,
    shadow: ShadowMode<'a>,
    highlight: u8,
    rows: RowOrigin<'a>,
    quirks: Quirks,
}

impl<'a> ColorBlit<'a> {
    /// A forward, unclipped, verbatim blit.
    #[must_use]
    pub fn new() -> Self {
        Self::default()
    }

    /// Mirrors the frame about the pen.
    #[must_use]
    pub const fn mirrored(mut self, mirrored: bool) -> Self {
        self.direction = if mirrored {
            Direction::Mirrored
        } else {
            Direction::Forward
        };
        self
    }

    /// Restricts drawing to `clip`, which is intersected with the surface.
    #[must_use]
    pub const fn clip(mut self, clip: Rect) -> Self {
        self.clip = Some(clip);
        self
    }

    /// Sends every literal and solid index through a translation table.
    #[must_use]
    pub const fn remap(mut self, table: &'a [u8; paint::PALETTE_LEN]) -> Self {
        self.pixels = Pixels::Remap(table);
        self
    }

    /// Makes shadow runs dim the surface through `palettes`.
    #[must_use]
    pub const fn shadows(mut self, palettes: DimPalettes<'a>) -> Self {
        self.shadow = ShadowMode::Apply(palettes);
        self
    }

    /// Replaces recolourable shadow runs with a solid colour.
    ///
    /// Zero means "no highlight", exactly as in the original.
    #[must_use]
    pub const fn highlight(mut self, color: u8) -> Self {
        self.highlight = color;
        self
    }

    /// Applies a per-screen-row horizontal displacement table.
    #[must_use]
    pub const fn shear(mut self, table: &'a [i8]) -> Self {
        self.rows = RowOrigin::Sheared(table);
        self
    }

    /// Selects which retail defects to reproduce.
    #[must_use]
    pub const fn quirks(mut self, quirks: Quirks) -> Self {
        self.quirks = quirks;
        self
    }

    const fn run_policy(self) -> ClipPolicy {
        if self.quirks.mirrored_clip_drops_partial_runs
            && matches!(self.direction, Direction::Mirrored)
        {
            ClipPolicy::WholeRunOnly
        } else {
            ClipPolicy::Clamp
        }
    }

    /// Draws `frame` with its pen at `(x, y)`.
    ///
    /// # Errors
    ///
    /// Only decode and dim-level failures remain: resolving the frame already
    /// happened when the [`FrameData`] was built.
    pub fn draw(self, canvas: &mut Canvas<'_>, frame: FrameData<'_>, x: i32, y: i32) -> Result<()> {
        let place = Placement::resolve(canvas, frame.header(), x, y, self.direction, self.clip);
        let (clip_left, clip_right) = place.horizontal();
        let run_policy = self.run_policy();

        let mut row = place.top;
        let mut cursor = place
            .base
            .saturating_add(self.rows.displacement(row, self.direction));

        for op in ColorOps::new(frame.stream()) {
            let op = op?;
            let visible = place.clip.contains_row(row) && self.rows.row_enabled(row);

            match op {
                Op::EndOfRow => {
                    // Retail reads shear[y] and only then increments y, so the
                    // next row's origin comes from the row just finished. That
                    // one-row lag is visible behaviour, not an accident: keep it.
                    let displacement = self.rows.displacement(row, self.direction);
                    row = row.saturating_add(1);
                    cursor = place.base.saturating_add(displacement);
                }
                Op::Skip(len) => {
                    cursor = self.direction.advance(cursor, len);
                }
                Op::Literal(payload) => {
                    let len = u32::try_from(payload.len()).unwrap_or(u32::MAX);
                    if visible {
                        // Literal runs clamp in every retail decoder, mirrored
                        // included, so they never take the quirk policy.
                        if let Some(span) = self.direction.clip(
                            cursor,
                            len,
                            clip_left,
                            clip_right,
                            ClipPolicy::Clamp,
                        ) && let Some(target) = canvas.row_mut(row)
                        {
                            paint::literal(target, span, payload, self.pixels);
                        }
                    }
                    cursor = self.direction.advance(cursor, len);
                }
                Op::Solid { len, color } => {
                    if visible
                        && let Some(span) = self
                            .direction
                            .clip(cursor, len, clip_left, clip_right, run_policy)
                        && let Some(target) = canvas.row_mut(row)
                    {
                        paint::solid(target, span, self.pixels.map(color));
                    }
                    cursor = self.direction.advance(cursor, len);
                }
                Op::Shadow { len, flags } => {
                    if visible {
                        if flags.recolorable() && self.highlight != 0 {
                            if let Some(span) = self
                                .direction
                                .clip(cursor, len, clip_left, clip_right, run_policy)
                                && let Some(target) = canvas.row_mut(row)
                            {
                                paint::solid(target, span, self.highlight);
                            }
                        } else if let ShadowMode::Apply(palettes) = self.shadow
                            && flags.applies()
                        {
                            let palette = palettes.for_shadow(flags)?;
                            if let Some(span) = self
                                .direction
                                .clip(cursor, len, clip_left, clip_right, run_policy)
                                && let Some(target) = canvas.row_mut(row)
                            {
                                paint::dim(target, span, palette);
                            }
                        }
                    }
                    cursor = self.direction.advance(cursor, len);
                }
            }
        }
        Ok(())
    }
}

/// Blitter for the mask dialect: silhouettes filled or dimmed by the caller.
#[derive(Clone, Copy, Debug)]
pub struct MaskBlit<'a> {
    direction: Direction,
    clip: Option<Rect>,
    paint: MaskPaint<'a>,
    quirks: Quirks,
}

impl<'a> MaskBlit<'a> {
    /// A forward, unclipped silhouette blit.
    #[must_use]
    pub const fn new(paint: MaskPaint<'a>) -> Self {
        Self {
            direction: Direction::Forward,
            clip: None,
            paint,
            quirks: Quirks::corrected(),
        }
    }

    /// Fills the silhouette with one palette index.
    #[must_use]
    pub const fn solid(color: u8) -> Self {
        Self::new(MaskPaint::Solid(color))
    }

    /// Dims the silhouette through `palettes` at `level`.
    #[must_use]
    pub const fn dim(palettes: DimPalettes<'a>, level: u8) -> Self {
        Self::new(MaskPaint::Dim { palettes, level })
    }

    /// Mirrors the frame about the pen.
    #[must_use]
    pub const fn mirrored(mut self, mirrored: bool) -> Self {
        self.direction = if mirrored {
            Direction::Mirrored
        } else {
            Direction::Forward
        };
        self
    }

    /// Restricts drawing to `clip`, which is intersected with the surface.
    #[must_use]
    pub const fn clip(mut self, clip: Rect) -> Self {
        self.clip = Some(clip);
        self
    }

    /// Selects which retail defects to reproduce.
    #[must_use]
    pub const fn quirks(mut self, quirks: Quirks) -> Self {
        self.quirks = quirks;
        self
    }

    /// Draws `frame` with its pen at `(x, y)`.
    ///
    /// # Errors
    ///
    /// Only decode and dim-level failures remain: resolving the frame already
    /// happened when the [`FrameData`] was built.
    pub fn draw(self, canvas: &mut Canvas<'_>, frame: FrameData<'_>, x: i32, y: i32) -> Result<()> {
        let place = Placement::resolve(canvas, frame.header(), x, y, self.direction, self.clip);
        let (clip_left, clip_right) = place.horizontal();
        let policy = if self.quirks.mirrored_clip_drops_partial_runs
            && matches!(self.direction, Direction::Mirrored)
        {
            ClipPolicy::WholeRunOnly
        } else {
            ClipPolicy::Clamp
        };

        // Resolve the palette up front. Doing it inside the span loop made the
        // same misconfiguration error or not depending on whether any span
        // happened to be visible.
        let palette = match self.paint {
            MaskPaint::Solid(_) => None,
            MaskPaint::Dim { palettes, level } => Some(palettes.level(level)?),
        };

        let mut row = place.top;
        let mut cursor = place.base;

        for op in MaskOps::new(frame.stream()) {
            match op? {
                MaskOp::EndOfRow => {
                    row = row.saturating_add(1);
                    cursor = place.base;
                }
                MaskOp::Skip(len) => cursor = self.direction.advance(cursor, len),
                MaskOp::Span(len) => {
                    if place.clip.contains_row(row)
                        && let Some(span) = self
                            .direction
                            .clip(cursor, len, clip_left, clip_right, policy)
                        && let Some(target) = canvas.row_mut(row)
                    {
                        match (self.paint, palette) {
                            (MaskPaint::Solid(color), _) => {
                                paint::solid(target, span, color);
                            }
                            (MaskPaint::Dim { .. }, Some(palette)) => {
                                paint::dim(target, span, palette);
                            }
                            (MaskPaint::Dim { .. }, None) => {}
                        }
                    }
                    cursor = self.direction.advance(cursor, len);
                }
            }
        }
        Ok(())
    }
}
