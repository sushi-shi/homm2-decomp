//! Blitter tests: geometry, clipping, pixel operations and the retail quirk.

use homm2_icon::{Canvas, ColorBlit, DimPalettes, Icon, MaskBlit, Quirks, Rect};

/// Builds a one-frame icon body around a hand-written stream.
fn icon_body(width: u16, height: u16, offset_x: i16, offset_y: i16, stream: &[u8]) -> Vec<u8> {
    let mut body = Vec::new();
    body.extend_from_slice(&offset_x.to_le_bytes());
    body.extend_from_slice(&offset_y.to_le_bytes());
    body.extend_from_slice(&width.to_le_bytes());
    body.extend_from_slice(&height.to_le_bytes());
    body.push(0);
    body.extend_from_slice(&13u32.to_le_bytes());
    body.extend_from_slice(stream);
    body
}

fn render(body: &[u8], width: i32, blit: ColorBlit<'_>, x: i32) -> Vec<u8> {
    let icon = Icon::from_body(1, body).expect("body parses");
    let mut pixels = vec![0u8; usize::try_from(width).unwrap()];
    let mut canvas = Canvas::new(width, 1, &mut pixels).expect("canvas fits");
    blit.draw(
        &mut canvas,
        icon.frame_data(0).expect("frame 0 resolves"),
        x,
        0,
    )
    .expect("blit succeeds");
    pixels
}

/// Literal run of 1..=4 followed by end-of-row and end-of-image.
const LITERAL_ROW: &[u8] = &[0x04, 1, 2, 3, 4, 0x00, 0x80];

#[test]
fn forward_and_mirrored_are_reflections_of_each_other() {
    let body = icon_body(4, 1, 0, 0, LITERAL_ROW);
    let forward = render(&body, 8, ColorBlit::new(), 0);
    assert_eq!(forward, [1, 2, 3, 4, 0, 0, 0, 0]);

    // Pen at 7 puts the mirrored frame's right edge on column 7.
    let mirrored = render(&body, 8, ColorBlit::new().mirrored(true), 7);
    assert_eq!(mirrored, [0, 0, 0, 0, 4, 3, 2, 1]);
}

#[test]
fn frame_offsets_displace_the_frame_and_mirror_correctly() {
    let body = icon_body(4, 1, 2, 0, LITERAL_ROW);

    // Forward: left = pen + offset_x = 2.
    assert_eq!(
        render(&body, 8, ColorBlit::new(), 0),
        [0, 0, 1, 2, 3, 4, 0, 0]
    );

    // Mirrored: left = pen - offset_x - width + 1 = 7 - 2 - 4 + 1 = 2, so the
    // frame occupies columns 2..=5 and is written right to left.
    assert_eq!(
        render(&body, 8, ColorBlit::new().mirrored(true), 7),
        [0, 0, 4, 3, 2, 1, 0, 0]
    );
}

#[test]
fn literal_runs_clamp_at_both_clip_edges_in_both_directions() {
    let body = icon_body(4, 1, 0, 0, LITERAL_ROW);
    // Clip keeps columns 1..=2 only. Both pens put the frame on columns 0..=3,
    // so both edges are clipped in both directions.
    let clip = Rect::new(1, 0, 2, 1);
    assert_eq!(
        render(&body, 8, ColorBlit::new().clip(clip), 0),
        [0, 2, 3, 0, 0, 0, 0, 0]
    );
    assert_eq!(
        render(&body, 8, ColorBlit::new().mirrored(true).clip(clip), 3),
        [0, 3, 2, 0, 0, 0, 0, 0]
    );
}

#[test]
fn solid_runs_clamp_when_corrected_and_vanish_under_the_retail_quirk() {
    // Solid run of four, straddling the clip's left edge once mirrored.
    let stream = [0xc4, 9, 0x00, 0x80];
    let body = icon_body(4, 1, 0, 0, &stream);
    let clip = Rect::new(2, 0, 6, 1);

    // Forward decoders clamp, with or without the quirk.
    let forward = render(
        &body,
        8,
        ColorBlit::new().clip(clip).quirks(Quirks::retail()),
        0,
    );
    assert_eq!(forward, [0, 0, 9, 9, 0, 0, 0, 0]);

    // Mirrored: correct behaviour clamps to the visible part.
    let corrected = render(&body, 8, ColorBlit::new().mirrored(true).clip(clip), 4);
    assert_eq!(corrected, [0, 0, 9, 9, 9, 0, 0, 0]);

    // Retail drops the whole run because its left end is outside the clip.
    let retail = render(
        &body,
        8,
        ColorBlit::new()
            .mirrored(true)
            .clip(clip)
            .quirks(Quirks::retail()),
        4,
    );
    assert_eq!(retail, [0; 8], "retail loses the entire straddling run");
}

#[test]
fn mirrored_literals_survive_the_quirk_that_kills_mirrored_solids() {
    let body = icon_body(4, 1, 0, 0, LITERAL_ROW);
    let clip = Rect::new(2, 0, 6, 1);
    let retail = render(
        &body,
        8,
        ColorBlit::new()
            .mirrored(true)
            .clip(clip)
            .quirks(Quirks::retail()),
        4,
    );
    assert_eq!(
        retail,
        [0, 0, 3, 2, 1, 0, 0, 0],
        "literal clipping is correct even in retail"
    );
}

#[test]
fn remap_table_is_applied_to_literals_and_solids() {
    let mut table = [0u8; 256];
    for (index, entry) in table.iter_mut().enumerate() {
        *entry = u8::try_from(index).unwrap_or(0).wrapping_add(100);
    }
    let body = icon_body(4, 1, 0, 0, LITERAL_ROW);
    assert_eq!(
        render(&body, 8, ColorBlit::new().remap(&table), 0),
        [101, 102, 103, 104, 0, 0, 0, 0]
    );

    let solid = icon_body(2, 1, 0, 0, &[0xc2, 7, 0x00, 0x80]);
    assert_eq!(
        render(&solid, 8, ColorBlit::new().remap(&table), 0),
        [107, 107, 0, 0, 0, 0, 0, 0]
    );
}

#[test]
fn shadow_runs_dim_apply_and_recolour_in_the_right_precedence() {
    // 0xc0 with flags 0xc6: recolourable, applies, level 1, length 2.
    let stream = [0xc0, 0xc6, 0x00, 0x80];
    let body = icon_body(2, 1, 0, 0, &stream);
    let icon = Icon::from_body(1, &body).unwrap();

    // Level 1 palette maps everything to 0x55.
    let mut flat = vec![0u8; 2 * 256];
    flat[256..].fill(0x55);
    let palettes = DimPalettes::new(&flat);

    let draw = |blit: ColorBlit<'_>| {
        let mut pixels = vec![3u8; 4];
        let mut canvas = Canvas::new(4, 1, &mut pixels).unwrap();
        blit.draw(&mut canvas, icon.frame_data(0).unwrap(), 0, 0)
            .unwrap();
        pixels
    };

    // No shadow table configured: the run only advances.
    assert_eq!(draw(ColorBlit::new()), [3, 3, 3, 3]);
    // Shadow table configured: the run dims.
    assert_eq!(draw(ColorBlit::new().shadows(palettes)), [0x55, 0x55, 3, 3]);
    // Highlight beats dimming, even though the apply bit is also set.
    assert_eq!(
        draw(ColorBlit::new().shadows(palettes).highlight(0x21)),
        [0x21, 0x21, 3, 3]
    );
}

#[test]
fn a_cleared_apply_bit_makes_a_shadow_run_transparent() {
    // flags 0x86: recolourable, apply clear, length 2.
    let body = icon_body(2, 1, 0, 0, &[0xc0, 0x86, 0x00, 0x80]);
    let icon = Icon::from_body(1, &body).unwrap();
    let flat = vec![0x55u8; 4 * 256];
    let palettes = DimPalettes::new(&flat);

    let mut pixels = vec![3u8; 4];
    let mut canvas = Canvas::new(4, 1, &mut pixels).unwrap();
    ColorBlit::new()
        .shadows(palettes)
        .draw(&mut canvas, icon.frame_data(0).unwrap(), 0, 0)
        .unwrap();
    assert_eq!(pixels, [3, 3, 3, 3]);
}

#[test]
fn shear_displaces_rows_and_suppresses_marked_ones() {
    // Two rows of two literal pixels each.
    let stream = [0x02, 1, 2, 0x00, 0x02, 3, 4, 0x00, 0x80];
    let body = icon_body(2, 2, 0, 0, &stream);
    let icon = Icon::from_body(1, &body).unwrap();

    let shear = [1i8, homm2_icon::SHEAR_SKIP_ROW];
    let mut pixels = vec![0u8; 8];
    let mut canvas = Canvas::new(4, 2, &mut pixels).unwrap();
    ColorBlit::new()
        .shear(&shear)
        .clip(Rect::new(0, 0, 4, 2))
        .draw(&mut canvas, icon.frame_data(0).unwrap(), 0, 0)
        .unwrap();

    assert_eq!(&pixels[0..4], [0, 1, 2, 0], "row 0 displaced right by one");
    assert_eq!(&pixels[4..8], [0, 0, 0, 0], "row 1 suppressed");
}

#[test]
fn mask_dialect_fills_and_dims_spans() {
    // Span of three, end of row, end of image.
    let body = icon_body(3, 1, 0, 0, &[0x03, 0x00, 0x80]);
    let icon = Icon::from_body(1, &body).unwrap();

    let mut pixels = vec![1u8; 6];
    let mut canvas = Canvas::new(6, 1, &mut pixels).unwrap();
    MaskBlit::solid(0xe2)
        .draw(&mut canvas, icon.frame_data(0).unwrap(), 0, 0)
        .unwrap();
    assert_eq!(pixels, [0xe2, 0xe2, 0xe2, 1, 1, 1]);

    let flat = vec![0x40u8; 256];
    let mut pixels = vec![1u8; 6];
    let mut canvas = Canvas::new(6, 1, &mut pixels).unwrap();
    MaskBlit::dim(DimPalettes::new(&flat), 0)
        .draw(&mut canvas, icon.frame_data(0).unwrap(), 0, 0)
        .unwrap();
    assert_eq!(pixels, [0x40, 0x40, 0x40, 1, 1, 1]);
}

#[test]
fn drawing_never_writes_outside_the_surface() {
    // A frame deliberately placed half off each edge.
    let body = icon_body(4, 1, 0, 0, LITERAL_ROW);
    let icon = Icon::from_body(1, &body).unwrap();

    for x in [-3, -1, 0, 2, 6] {
        let mut pixels = vec![0u8; 4];
        let mut canvas = Canvas::new(4, 1, &mut pixels).unwrap();
        ColorBlit::new()
            .draw(&mut canvas, icon.frame_data(0).unwrap(), x, 0)
            .expect("blit succeeds regardless of placement");
    }
}

#[test]
fn a_bad_frame_index_is_an_error_not_a_read() {
    let body = icon_body(4, 1, 0, 0, LITERAL_ROW);
    let icon = Icon::from_body(1, &body).unwrap();
    let mut pixels = vec![0u8; 4];
    let mut canvas = Canvas::new(4, 1, &mut pixels).unwrap();
    let _ = &mut canvas;
    assert!(
        icon.frame_data(9).is_err(),
        "the lookup fails, not the blit"
    );
}

#[test]
fn shear_lags_one_row_behind_as_retail_does() {
    // Retail's sheared new-row block reads shear[y] and increments y afterwards
    // (IconToBitmapYModify 0xda270), so the row about to be drawn is displaced by
    // the entry belonging to the row just finished. Row 0 uses shear[0]; row 1
    // also uses shear[0]; row 2 uses shear[1].
    let stream = [0x02, 1, 2, 0x00, 0x02, 3, 4, 0x00, 0x02, 5, 6, 0x00, 0x80];
    let body = icon_body(2, 3, 0, 0, &stream);
    let icon = Icon::from_body(1, &body).unwrap();

    let shear = [0i8, 5, 0];
    let mut pixels = vec![0u8; 8 * 3];
    let mut canvas = Canvas::new(8, 3, &mut pixels).unwrap();
    ColorBlit::new()
        .shear(&shear)
        .clip(Rect::new(0, 0, 8, 3))
        .draw(&mut canvas, icon.frame_data(0).unwrap(), 0, 0)
        .unwrap();

    assert_eq!(
        &pixels[0..8],
        [1, 2, 0, 0, 0, 0, 0, 0],
        "row 0 uses shear[0]=0"
    );
    assert_eq!(
        &pixels[8..16],
        [3, 4, 0, 0, 0, 0, 0, 0],
        "row 1 uses shear[0]=0 too, not shear[1]=5"
    );
    assert_eq!(
        &pixels[16..24],
        [0, 0, 0, 0, 0, 5, 6, 0],
        "row 2 uses shear[1]=5"
    );
}

#[test]
fn scaled_drawing_honours_its_clip_rectangle() {
    // A 32x32 solid frame, drawn at scale 8 into a 16x16 surface with a clip that
    // keeps only the top-left 4x4. Retail ignores the clip here; we do not.
    let mut stream = Vec::new();
    for _ in 0..32 {
        stream.extend_from_slice(&[0xe0, 9, 0x00]);
    }
    stream.push(0x80);
    let body = icon_body(32, 32, 0, 0, &stream);
    let icon = Icon::from_body(1, &body).unwrap();
    let mut scratch = homm2_icon::Scratch::new();

    let mut painted = |clip: Option<Rect>| {
        let mut pixels = vec![0u8; 16 * 16];
        let mut canvas = Canvas::new(16, 16, &mut pixels).unwrap();
        homm2_icon::draw_scaled(
            &mut canvas,
            icon.frame_data(0).unwrap(),
            0,
            0,
            8,
            clip,
            &mut scratch,
        )
        .unwrap();
        pixels.iter().filter(|&&p| p != 0).count()
    };

    assert_eq!(painted(None), 64, "unclipped: the whole 8x8 lands");
    assert_eq!(
        painted(Some(Rect::new(0, 0, 4, 4))),
        16,
        "clipped to 4x4: only 16 pixels survive"
    );
    assert_eq!(
        painted(Some(Rect::new(100, 100, 4, 4))),
        0,
        "clip disjoint from the surface paints nothing"
    );
}
