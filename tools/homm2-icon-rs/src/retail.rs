//! The original's eleven blitters, expressed as configurations.
//!
//! The game ships one translation unit per variant, each a near-copy of its
//! neighbour with the differences edited in by hand. Laid out against the axes
//! in [`crate::blit`], the whole family collapses to a table:
//!
//! ```text
//!   retail symbol                 RVA      dialect  direction  configuration
//!   ----------------------------  -------  -------  ---------  ------------------------
//!   IconToBitmap                  0xd0570  Color    Forward    shadows
//!   FlipIconToBitmap              0xd1ba0  Color    Mirrored   shadows
//!   IconToBitmapColorTable        0xd32a0  Color    Forward    shadows?, remap
//!   FlipIconToBitmapColorTable    0xd9790  Color    Mirrored   shadows, remap
//!   IconToBitmapYModify           0xda270  Color    Forward    shadows, shear
//!   FlipIconToBitmapYModify       0xd9ce0  Color    Mirrored   shadows, shear
//!   MonoIconToBitmap              0xcfae0  Mask     Forward    solid
//!   FlipMonoIconToBitmap          0xda800  Mask     Mirrored   solid
//!   DimIconToBitmap               0xcfd50  Mask     Forward    dim
//!   FlipDimIconToBitmap           0xdaa20  Mask     Mirrored   dim
//!   IconToBitmapScale             0xd2f90  Color    Forward    see crate::scale
//! ```
//!
//! Only `IconToBitmapColorTable` takes a runtime switch for whether shadow runs
//! render; its mirrored twin lost that parameter, which is visible in the
//! mangled names as one fewer argument.
//!
//! Every constructor here returns a builder, so a call site adds its own clip
//! rectangle and quirk selection and then draws.
//!
//! ```
//! # use homm2_icon::{Canvas, DimPalettes, Icon, Quirks, Rect, retail};
//! # fn demo(icon: &Icon<'_>, palettes: DimPalettes<'_>) -> homm2_icon::Result<()> {
//! let mut pixels = vec![0u8; 640 * 480];
//! let mut canvas = Canvas::new(640, 480, &mut pixels)?;
//! retail::flip_icon_to_bitmap(palettes, 0)
//!     .clip(Rect::new(0, 0, 640, 444))
//!     .quirks(Quirks::retail())
//!     .draw(&mut canvas, icon.frame_data(0)?, 320, 200)?;
//! # Ok(())
//! # }
//! ```

use crate::blit::{ColorBlit, MaskBlit};
use crate::paint::{DimPalettes, PALETTE_LEN};

/// `IconToBitmap`, RVA `0xd0570`.
///
/// `highlight` is the original's `color` argument: non-zero replaces every
/// recolourable shadow run with that palette index.
#[must_use]
pub fn icon_to_bitmap(palettes: DimPalettes<'_>, highlight: u8) -> ColorBlit<'_> {
    ColorBlit::new().shadows(palettes).highlight(highlight)
}

/// `FlipIconToBitmap`, RVA `0xd1ba0`.
#[must_use]
pub fn flip_icon_to_bitmap(palettes: DimPalettes<'_>, highlight: u8) -> ColorBlit<'_> {
    icon_to_bitmap(palettes, highlight).mirrored(true)
}

/// `IconToBitmapColorTable`, RVA `0xd32a0`.
///
/// `shadows` is the original's trailing `dimGate`: when clear, shadow runs
/// advance the cursor and touch nothing.
#[must_use]
pub fn icon_to_bitmap_color_table<'a>(
    table: &'a [u8; PALETTE_LEN],
    palettes: DimPalettes<'a>,
    highlight: u8,
    shadows: bool,
) -> ColorBlit<'a> {
    let blit = ColorBlit::new().remap(table).highlight(highlight);
    if shadows {
        blit.shadows(palettes)
    } else {
        blit
    }
}

/// `FlipIconToBitmapColorTable`, RVA `0xd9790`.
///
/// Has no shadow switch: the mirrored variant always applies the dim palette.
#[must_use]
pub fn flip_icon_to_bitmap_color_table<'a>(
    table: &'a [u8; PALETTE_LEN],
    palettes: DimPalettes<'a>,
    highlight: u8,
) -> ColorBlit<'a> {
    icon_to_bitmap_color_table(table, palettes, highlight, true).mirrored(true)
}

/// `IconToBitmapYModify`, RVA `0xda270`.
///
/// `shear` is indexed by absolute screen row; [`SHEAR_SKIP_ROW`] suppresses a
/// row entirely. The original ignores its clip-mode argument here and always
/// clips.
///
/// [`SHEAR_SKIP_ROW`]: crate::SHEAR_SKIP_ROW
#[must_use]
pub fn icon_to_bitmap_y_modify<'a>(
    palettes: DimPalettes<'a>,
    highlight: u8,
    shear: &'a [i8],
) -> ColorBlit<'a> {
    icon_to_bitmap(palettes, highlight).shear(shear)
}

/// `FlipIconToBitmapYModify`, RVA `0xd9ce0`.
#[must_use]
pub fn flip_icon_to_bitmap_y_modify<'a>(
    palettes: DimPalettes<'a>,
    highlight: u8,
    shear: &'a [i8],
) -> ColorBlit<'a> {
    icon_to_bitmap_y_modify(palettes, highlight, shear).mirrored(true)
}

/// `MonoIconToBitmap`, RVA `0xcfae0`.
#[must_use]
pub const fn mono_icon_to_bitmap(color: u8) -> MaskBlit<'static> {
    MaskBlit::solid(color)
}

/// `FlipMonoIconToBitmap`, RVA `0xda800`.
#[must_use]
pub const fn flip_mono_icon_to_bitmap(color: u8) -> MaskBlit<'static> {
    MaskBlit::solid(color).mirrored(true)
}

/// `DimIconToBitmap`, RVA `0xcfd50`.
///
/// `level` is the original's `color` argument, which is a flat dim level rather
/// than a palette index.
#[must_use]
pub const fn dim_icon_to_bitmap(palettes: DimPalettes<'_>, level: u8) -> MaskBlit<'_> {
    MaskBlit::dim(palettes, level)
}

/// `FlipDimIconToBitmap`, RVA `0xdaa20`.
#[must_use]
pub const fn flip_dim_icon_to_bitmap(palettes: DimPalettes<'_>, level: u8) -> MaskBlit<'_> {
    MaskBlit::dim(palettes, level).mirrored(true)
}

/// `IconToBitmapScale`, RVA `0xd2f90`.
///
/// The odd one out: not a decoder but a wrapper that renders into a scratch
/// surface and resamples, so it is a function rather than a builder. Re-exported
/// here so the retail table above maps all eleven entry points rather than ten.
///
/// # Errors
///
/// See [`crate::scale::draw_scaled`].
pub use crate::scale::draw_scaled as icon_to_bitmap_scale;
