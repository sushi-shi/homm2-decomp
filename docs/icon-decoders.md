# Icon function catalog

Per-function reference for the `icon` class and the eleven `Icon*ToBitmap*` blitters.
The format they consume is documented in `icon-format.md`; read that first for the RLE
opcode maps, the shear table, and the confirmed retail defects.

RVAs are relative to image base `0x400000`. Sizes are the `VA(...)` extents recorded in
source. Fidelity is mixed: `GetIconEntry`, the `icon` constructor and destructor, and
`DrawToBuffer` / `ClipFillToBuffer` / `FillToBuffer` / `DimToBuffer` are exact; the eleven
RLE blitters and `CombatClipDrawToBuffer` are not. `config/match_baseline.tsv` holds the
current observation-only scores. Behaviour claims below that cite an RVA were read out of
retail with `homm2 sema disasm`; the rest are read off the reconstruction and inherit its
provisional status.

**Stale-RVA warning.** The index and per-function RVAs below are the PoL 2.0
addresses carried over with this file; they are NOT this image's. Read the
current addresses off the `VA(...)` markers in `src/BASE/*.cpp` (for example
`IconToBitmap` is 0xc14c0 here, not 0xd0570). The fidelity sentence above is
also historical: all eleven RLE blitters plus the four `BASE/bmap2` blitters are
now byte-exact, and the retail bodies keep every working value in file statics
rather than locals (see `docs/patterns/no-local-frame-file-statics.md`).


## 1. Index

```
    RVA      size   symbol                            TU
    -------  -----  --------------------------------  ---------------------
    0xc6920  0x00c  GetIconEntry                      BASE/Misc.cpp
    0xc7a20  0x067  icon::icon(u32l)                  BASE/ICON.cpp
    0xc7ad0  0x021  icon::~icon()                     BASE/ICON.cpp
    0xc7b00  0x044  icon::DrawToBuffer                BASE/ICON.cpp
    0xc7b50  0x2bb  icon::CombatClipDrawToBuffer      BASE/ICON.cpp
    0xc7e10  0x03d  icon::ClipFillToBuffer            BASE/ICON.cpp
    0xc7e50  0x103  icon::FillToBuffer                BASE/ICON.cpp
    0xc7f60  0x03e  icon::DimToBuffer                 BASE/ICON.cpp

    0xcfae0  0x266  MonoIconToBitmap                  BASE/Iconm2b.cpp
    0xcfd50  0x26e  DimIconToBitmap                   BASE/Icond2b.cpp
    0xd0570  0x4ed  IconToBitmap                      BASE/Icon2b.cpp
    0xd1ba0  0x4f1  FlipIconToBitmap                  BASE/Iconf2b.cpp
    0xd2f90  0x179  IconToBitmapScale                 BASE/icon2bs.cpp
    0xd32a0  0x5af  IconToBitmapColorTable            BASE/icon2bc.cpp
    0xd9790  0x54d  FlipIconToBitmapColorTable        BASE/iconf2bc.cpp
    0xd9ce0  0x58d  FlipIconToBitmapYModify           BASE/iconf2by.cpp
    0xda270  0x588  IconToBitmapYModify               BASE/icon2by.cpp
    0xda800  0x212  FlipMonoIconToBitmap              BASE/Iconmf2b.cpp
    0xdaa20  0x23b  FlipDimIconToBitmap               BASE/Icondf2b.cpp

    0xd0f70  0x0e5  iconWidget::Draw                  BASE/ICONWDGT.cpp
```

Dispatch shape - the class methods are thin selectors, all real work is in the free
functions:

```
    icon::DrawToBuffer ----+-- normal --> IconToBitmap
                           +-- flipped -> FlipIconToBitmap

    icon::CombatClipDrawToBuffer
        |
        +-- extent bookkeeping (SLimitData + giMin/giMax globals)
        +-- yModify != NULL ---+-- normal --> IconToBitmapYModify
        |                      +-- flipped -> FlipIconToBitmapYModify
        +-- colorTable != NULL +-- normal --> IconToBitmapColorTable
        |                      +-- flipped -> FlipIconToBitmapColorTable
        +-- otherwise ---------+-- normal --> IconToBitmap
                               +-- flipped -> FlipIconToBitmap

    icon::FillToBuffer ----+-- normal --> MonoIconToBitmap
                           +-- flipped -> FlipMonoIconToBitmap
    icon::ClipFillToBuffer ------------> MonoIconToBitmap   (orientation ignored, see F6)
    icon::DimToBuffer -----+-- normal --> DimIconToBitmap
                           +-- flipped -> FlipDimIconToBitmap

    IconToBitmapScale -----+-- scale 32 -> IconToBitmap (pass-through)
                           +-- otherwise -> IconToBitmap into scratch, then manual resample
```


## 2. The shared static-state idiom

Every blitter mirrors its loop variables into function-scope statics (`s_row`, `s_src`,
`s_x`, `s_y`, `s_run`, `s_clipR`, ... - carried in source as `DATA(0x005...)` declarations).
Retail writes these on essentially every iteration; they are not debug aids, they are how the
original was written. Three consequences:

1. **Not reentrant, not thread-safe.** Two overlapping blits of the same routine corrupt each
   other. This is fine in the retail single-threaded frame loop and is a hard constraint on
   any reuse.
2. **State leaks between calls.** Under `ICON_DRAW_NO_CLIP` the normalization block is
   skipped, so `s_clipR` / `s_clipB` still hold values from a *previous* invocation. Nothing
   in the no-clip path reads them, so this is latent, but it is why those statics must not be
   turned into locals.
3. **Each TU owns its own set.** The statics are per-function, at distinct addresses per TU
   (e.g. `IconToBitmap` at `0x00534c20..0x00534c5c`, `FlipIconToBitmap` at
   `0x00534c60..0x00534ca4`), so the forward and flipped variants do not share state.

The redundant stores are load-bearing for matching. Removing one changes bytes.


## 3. `icon` class

### `icon::icon(u32l id)` - RVA `0xc7a20`, size `0x67`

```
    icon::icon(u32l id) : resource(RESOURCE_CATEGORY_ICON, id, 1, NULL)
```

Seeks the aggregate to `id`, reads the 2-byte frame count into `m_frameCount`, reads the
4-byte body length, allocates exactly that many bytes and reads the body into `m_data`.
No validation: a short or truncated record produces a live object with a garbage frame table.
The allocation goes through the instrumented `H2_ALLOC_AT` wrapper, which is why the TU
carries a `static char allocationSourceFile[] = "I:\\Projects\\Heroes\\Prog\\BASE\\ICON.CPP"`
at `0x0051e94c`.

### `icon::~icon()` - RVA `0xc7ad0`, size `0x21`

Frees `m_data`. Declared `inline` and `__declspec(dllexport)`; retail leaves an
inline-accessor continuation inside the body. (`BASE/ICON` builds at `/O2`, so the `/Ob1`
attribution in the source comment there does not apply to this TU.)

### `icon::Entries()` - source-level inline

`reinterpret_cast<IconEntry*>(m_data)`. Exists so call sites stop re-casting the blob. Not a
retail symbol; `GetIconEntry` is the retail-visible equivalent.

### `GetIconEntry(icon*, i32)` - RVA `0xc6920`, size `0xc`, in `BASE/Misc.cpp`

`&icon->Entries()[index]`. Twelve bytes: the 13x multiply plus an add. Used all over
`SOURCE/` for layout maths (centring, extent computation) without drawing anything.

### `icon::DrawToBuffer(x, y, frame, orientation)` - RVA `0xc7b00`, size `0x44`

Unconditional blit to `gpWindowManager->m_screen` with `ICON_DRAW_NO_CLIP` and a nominal
640x480 rect. Because the clip mode is `NO_CLIP`, the rect arguments are inert and **no
bounds checking happens at all** - the caller must guarantee the frame lands on-screen.
Selects `IconToBitmap` or `FlipIconToBitmap` on `orientation`.

### `icon::CombatClipDrawToBuffer(x, y, frame, limits, orientation, offset, colorTable, yModify)` - RVA `0xc7b50`, size `0x2bb`

The combat workhorse. Three responsibilities, in order:

1. **Extent publication.** When `gbComputeExtent`, writes the frame's screen rectangle into
   `*limits` (mirroring the X axis for flipped orientation) and, when `gbSaveBiggestExtent`,
   widens the `giMinExtentX / giMinExtentY / giMaxExtentX / giMaxExtentY` globals. If
   `gbReturnAfterComputeExtent` it returns `ICON_DRAW_SKIPPED` without drawing - this is the
   "measure only" pass the combat renderer uses to compute a dirty rectangle.
2. **Extent rejection.** When `gbLimitToExtent`, drops the draw (returns `ICON_DRAW_SKIPPED`)
   if the frame does not intersect the accumulated extent rectangle.
3. **Decoder selection**, in priority order: `yModify` (shear) beats `colorTable` (remap)
   beats plain. Under `gbLimitToExtent` the plain path narrows the clip rect to the extent
   rectangle instead of the full screen. Clip mode is always `ICON_DRAW_CLIP`, and the
   vertical extent is `DRAW_COMBAT_HEIGHT` (444), not 480 - the bottom 36 rows are the
   combat status bar.

`offset` is forwarded as the decoders' `color` argument, i.e. it is the solid highlight color
substituted for R-tagged shadow runs (`icon-format.md` section 6.1). `army::Draw` passes the
spell-effect or selected-creature color there and `m_palette` (a `gyModify` shear table) as
`yModify`.

Returns `ICON_DRAW_COMPLETED` (1) if it drew, `ICON_DRAW_SKIPPED` (0) if it bailed. Callers
use the return value to decide whether a region needs a screen update.

### `icon::ClipFillToBuffer(x, y, frame, color, orientation, clipX, clipY, clipW, clipH)` - RVA `0xc7e10`, size `0x3d`

Silhouette fill with clipping. Forwards straight to `MonoIconToBitmap` with
`ICON_DRAW_CLIP` and the caller's rect.

**F6: `orientation` is accepted and never read.** Retail (`0xc7e10`) contains no test of the
parameter - the whole body is nine pushes and one call. Requesting `ICON_DRAW_FLIPPED` gets
you an unflipped fill. Latent: both call sites (`advManager` radar viewport frame,
`ADVMGR.cpp`) pass `ICON_DRAW_NORMAL`.

### `icon::FillToBuffer(x, y, frame, color, orientation, limits)` - RVA `0xc7e50`, size `0x103`

Silhouette fill without clipping (`ICON_DRAW_NO_CLIP`, zero rect). Flipped orientation goes
straight to `FlipMonoIconToBitmap`. For normal orientation, if `gbLimitToExtent` and `limits`
is non-NULL it first fills `*limits` with the frame rectangle and drops the draw when that
rectangle misses the accumulated extent - the same dirty-rectangle culling as
`CombatClipDrawToBuffer`, minus the extent-widening.

Note the asymmetry: the flipped path performs neither the limits computation nor the extent
test.

### `icon::DimToBuffer(x, y, frame, orientation)` - RVA `0xc7f60`, size `0x3e`

Applies dim level 0 over the frame's silhouette, unclipped. Selects `DimIconToBitmap` or
`FlipDimIconToBitmap`. The `color` argument is hard-wired to 0, so this always uses
`uDimPal[0][0]`.


## 4. Color-dialect blitters

All six share the stage-1 decoder of `icon-format.md` section 8 and the
`H2_ICON_NORMALIZE_CLIP` entry block (except the two `YModify` variants). They differ only in
direction, per-pixel transform, and geometry.

### `IconToBitmap(icon, dest, x, y, frame, clip, clipX, clipY, clipW, clipH, color)` - RVA `0xd0570`, size `0x4ed`

The reference implementation. Left-to-right, literal runs via inline `memcpy`
(`rep movsd` + `rep movsb`), solid runs via inline `memset` (`rep stosd` + `rep stosb`),
shadow runs via a byte loop through the dim palette. Clipping clamps both edges correctly on
every run type. `color` is the R-bit highlight substitute.

### `FlipIconToBitmap(...)` - RVA `0xd1ba0`, size `0x4f1`

Mirror of the above. Literal runs become a descending byte loop (no `rep movs`, since the
destination walks backwards while the source walks forwards). Solid runs still use `rep stos`
because the destination is computed as `row + X - count + 1`.

**Carries F1** (`icon-format.md`): solid and shadow runs that straddle a clip edge are
dropped entirely rather than clipped, because the guard's inner test duplicates its outer
test. Confirmed in retail at `+0x1a2` (solid) and `+0x2ca` (shadow). Flipped *literal* runs
clip correctly, so the artifact is selective.

### `IconToBitmapColorTable(..., color, colorTable, dimGate)` - RVA `0xd32a0`, size `0x5af`

`IconToBitmap` plus a 256-entry remap applied to every emitted pixel:

```
    literal:  dst[i] = colorTable[src[i]]      (byte loop, no memcpy)
    solid:    fill with colorTable[idx]        (remap once, then memset)
    shadow:   dst[i] = dimPal[dst[i]] if dimGate != 0, else walk and write nothing
```

`dimGate` is the caller's switch for whether shadow runs render at all. `BASE/FONT.cpp`
passes 1 for the yellow font and 0 for the scenario-win font; `icon::CombatClipDrawToBuffer`
passes `COLOR_TABLE_APPLY_DIM` (1). When the gate is 0 the loop still iterates the full span
doing nothing, which is a small but real per-glyph cost.

The remap tables are the creature palette swaps (`gColorTableGray`, `gColorTableRed`,
`gColorTableDarkBrown`, `gColorTableLighten`) and the font colors
(`gColorTableYellow`, `gColorTableScenWin`).

### `FlipIconToBitmapColorTable(..., color, colorTable)` - RVA `0xd9790`, size `0x54d`

Mirrored remapping blit. **No `dimGate` parameter** - the flipped variant always applies the
dim palette. That asymmetry is real, not a reconstruction slip - the PoL line's debug names
established the arity difference (this branch's image is stripped and has no such names):

```
    ?IconToBitmapColorTable@@YIXPAVicon@@PAVbitmap@@HHHHHHHHHPAEH@Z    13 params
    ?FlipIconToBitmapColorTable@@YIXPAVicon@@PAVbitmap@@HHHHHHHHHPAE@Z 12 params
                                                                  ^^^ no trailing H
```

**Carries F1** at `+0x227` (solid) and `+0x33d` (shadow).

### `IconToBitmapYModify(..., color, shear)` - RVA `0xda270`, size `0x588`

`IconToBitmap` with a per-screen-row horizontal displacement. Each new row sets
`X = shear[y] + left`; rows whose `shear[y] == 0x7f` are decoded but not drawn
(`ICON_SHEAR_SKIP_ROW`). `shear` is `gyModify`, a 480-entry `i8` array indexed by absolute
screen Y and filled by `combatManager::RippleCreature` (sine wobble) or
`combatManager::VaporizeCreature` (stripe dissolve).

**The `clip` parameter is dead.** Retail never tests it (`0xd9ce0` prologue for the flipped
twin shows the same); the routine always computes `s_clipR`/`s_clipB` from the rect arguments
and always clips. Passing `ICON_DRAW_NO_CLIP` does not get you an unclipped blit.

Also unlike its siblings, this pair does not run `H2_ICON_NORMALIZE_CLIP`, so it never takes
the "frame fits entirely, drop to the fast path" shortcut.

### `FlipIconToBitmapYModify(..., color, shear)` - RVA `0xd9ce0`, size `0x58d`

Mirrored shear blit: `X = right - shear[y]`. Same dead `clip` parameter.

**Carries F1** at `+0x235` (solid) and `+0x314` (shadow).


## 5. Mono-dialect blitters

These consume the second RLE dialect (`icon-format.md` section 7). They never read pixel data
from the stream - the stream only describes *which* pixels are covered.

### `MonoIconToBitmap(icon, dest, x, y, frame, color, clip, clipX, clipY, clipW, clipH)` - RVA `0xcfae0`, size `0x266`

Fills every covered span with the flat palette index `color`. Clipping is correct on both
edges (this is a forward decoder, so F1 does not apply). Used for the combat grid outline (`COMBAT_ICON_GRID` frame 0, color `0xe2`) and, via
`icon::ClipFillToBuffer`, for the adventure-map radar viewport frame.

### `FlipMonoIconToBitmap(...)` - RVA `0xda800`, size `0x212`

Mirrored solid fill. **Carries F1** at `+0x1a2`: like every other reverse decoder, its span
guard tests `clipX <= spanLeft` twice and drops any span straddling a clip edge instead of
clamping it. Its skip mask is 7 bits (`and edx, 0x7f` at `+0x83`), matching the mono dialect
rather than the color dialect's 6.

### `DimIconToBitmap(icon, dest, x, y, frame, color, clip, clipX, clipY, clipW, clipH)` - RVA `0xcfd50`, size `0x26e`

Read-modify-write: `dst[i] = uDimPal_flat[color*256 + dst[i]]` over every covered span.
`color` is a flat dim level 0..11, not a palette index (retail: `shl edi,0x8` then
`[ebx+edi+uDimPal]`). Used to shade combat hexes: `combatManager` passes
`gridState - 1` for reachable/blocked shading and `COMBAT_GRID_MOUSE_FRAME` (2) for the
hex under the cursor.

Clipping is correct on both edges.

### `FlipDimIconToBitmap(...)` - RVA `0xdaa20`, size `0x23b`

Mirrored dim. **Carries F1** at `+0x1ae`, same span guard as `FlipMonoIconToBitmap`, and the
same 7-bit skip mask (`and edx, 0x7f` at `+0x78`).


## 6. Composite

### `IconToBitmapScale(icon, dest, x, y, frame, clip, clipX, clipY, clipW, clipH, scale)` - RVA `0xd2f90`, size `0x179`

Nearest-neighbour downscale for the View World map. Not a decoder - it wraps `IconToBitmap`.

```
    scale == 32 ?  yes -> IconToBitmap(dest, ..., clip args honoured); return
                   no  |
                       v
    step    = 32 / scale                     (integer, truncates)
    srcBase = ((1 - scale) * step + 32) >> 1 (centres the sample window)
    srcAdv  = step * 64

    tmp = new bitmap(NONE, 64, 64)
    clear tmp rows 0..31
    IconToBitmap(icon, tmp, 0, 0, frame, CLIP, 0, 0, 32, 32, 0)

    for row in 0..scale-1:
        for col in 0..scale-1:
            s = tmp[srcBase + row*step][srcBase + col*step]
            if s != 0: dest[y+row][x+col] = s      <- index 0 is transparent
    delete tmp
```

Three notes, all in `icon-format.md`:

- **F3**: the clip arguments are ignored on the scaled path. The final composite writes
  `scale x scale` pixels at `(x, y)` with no bounds test.
- **F4**: `step` truncation crops the source window. At `giViewWorldScale ==
  VIEW_WORLD_SCALE_NEAR` (12) the sampled window is only source columns/rows 5..27, so five
  pixels are lost from each edge of every tile.
- **F5**: the scratch clear uses stride 32 over a 64-wide bitmap. Harmless (it clears a
  superset of the region later read) but not what it looks like.

Also: one 64x64 bitmap is allocated and freed per call, i.e. per tile per frame of the
View World screen.


## 7. Widget

### `iconWidget::Draw()` - RVA `0xd0f70`, size `0xe5`, in `BASE/ICONWDGT.cpp`

Resolves the widget's position against its owning `heroWindow` and dispatches on
`WidgetKind`:

```
    WIDGET_KIND_ICON_DIRECT    icon->DrawToBuffer(x, y, frame, orientation)
    WIDGET_KIND_ICON_CENTERED  cancel the frame's x/y offsets, centre horizontally in the
                               widget, bottom-align with padding, then DrawToBuffer
    WIDGET_KIND_ICON_FILL      icon->FillToBuffer(x, y, frame, fillColor, orientation, NULL)
```

`WIDGET_KIND_ICON_FILL` routes into the mono dialect, so a widget configured with that kind
must be bound to a mono-encoded icon. Nothing checks this - see `icon-format.md` section 5.


## 8. Quick reference: which routine for which effect

```
    want                                        use
    ------------------------------------------  --------------------------------------
    plain sprite, guaranteed on-screen          icon::DrawToBuffer
    plain sprite, clipped                       IconToBitmap with ICON_DRAW_CLIP
    combat sprite with extents + dirty-rect     icon::CombatClipDrawToBuffer
    palette-swapped sprite (petrified, font)    IconToBitmapColorTable
    highlight outline on a creature             CombatClipDrawToBuffer with offset != 0
    wobbling / dissolving creature              CombatClipDrawToBuffer with yModify != NULL
    solid silhouette (grid lines, radar frame)  MonoIconToBitmap / icon::ClipFillToBuffer
    darkened silhouette (hex shading)           DimIconToBitmap / icon::DimToBuffer
    downscaled map tile                         IconToBitmapScale
```
