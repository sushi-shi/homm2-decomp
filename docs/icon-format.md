# Icon (ICN) resource format and parse actions

Scope: the on-disk/in-memory layout of an `icon` resource, the two run-length pixel
dialects its frames use, and the decode actions every `Icon*ToBitmap*` routine performs.
Per-function behaviour is in `icon-decoders.md`.

Evidence class. Everything in the "Format" and "Parse actions" sections below was read back
out of retail `HMM2PL.exe` with `homm2 sema disasm` (RVAs cited inline) and agrees with the
current reconstruction in `src/BASE/Icon*.cpp` / `include/BASE/IconMacro.h`. The
reconstruction itself is only partly byte-proven. The `icon` class methods and
`GetIconEntry` are exact; the eleven RLE blitters are not (check
`config/match_baseline.tsv` for current observation-only scores). Treat the blitters' C++
spelling as provisional and the retail-cited facts as solid. All ASCII diagrams below are
byte/bit exact.


## 1. Container

An icon lives in an `.AGG` aggregate as a single record addressed by a hashed name id.
`resourceManager::PointToFile(id)` seeks to it; `icon::icon` (RVA `0xc7a20`) then reads a
6-byte header and slurps the remainder into one heap block.

```
    .AGG record for one icon
    +--------+--------+---------------------------------------------+
    | u16    | u32    | body ...                                    |
    | count  | length |                                             |
    +--------+--------+---------------------------------------------+
         |       |         |
         |       |         +--> ReadBlock() into m_data, `length` bytes
         |       +------------> ReadLong(), byte length of the body
         +--------------------> ReadWord(), number of frames
```

Retail proof (`??0icon@@QAE@K@Z`, RVA `0xc7a20`):

```
    call ReadWord      ; mov word ptr [edi+0x10], ax     -> icon::m_frameCount
    call ReadLong      ; length
    call operator new  ; mov dword ptr [edi+0x12], eax   -> icon::m_data
    call ReadBlock     ; (m_data, length)
```

`class icon` (`include/BASE/icon.h`, `SIZE 0x16`, `#pragma pack(1)`):

```
    off  size  member
    0x00  0x04 vftable pointer            (resource is polymorphic; vptr first)
    0x04  0x02 resource::m_resourceType   (== RESOURCE_CATEGORY_ICON, 1)
    0x06  0x02 resource::m_refCount
    0x08  0x04 resource::m_id
    0x0c  0x04 resource::m_next
    0x10  0x02 icon::m_frameCount         i16
    0x12  0x04 icon::m_data               u8*   <- whole body, single allocation
```

Retail proof (`??0resource@@QAE@FKFPAV0@@Z`, RVA `0xd9750`) - note the vptr store to
`[ecx]`, i.e. offset 0, with its relocation masked to `0x0` by `--lite`:

```
    mov word  ptr [ecx + 0x4], ax    ; m_resourceType
    mov dword ptr [ecx], 0x0         ; vftable  <- offset 0
    mov word  ptr [ecx + 0x6], bx    ; m_refCount
    mov dword ptr [ecx + 0x8], edx   ; m_id
    mov dword ptr [ecx + 0xc], eax   ; m_next
```

Note the header count/length are *not* retained past construction beyond `m_frameCount`;
nothing in the family bounds-checks a frame index against it.


## 2. Body layout

`m_data` is the body *only* - the 6-byte header is consumed by the constructor and is not
part of the block. The body opens with the frame table, then the pixel streams.

```
    m_data
      |
      v
    +==================================+ <- +0
    | IconEntry[0]      13 bytes       |
    +----------------------------------+
    | IconEntry[1]      13 bytes       |    frame table, m_frameCount rows
    +----------------------------------+
    | ...                              |
    +----------------------------------+
    | IconEntry[n-1]    13 bytes       |
    +==================================+ <- +13*n
    | pixel stream blob                |
    |   (entries point into here via   |
    |    srcOffset, which is relative  |
    |    to m_data, i.e. it includes   |
    |    the frame table itself)       |
    +==================================+
```

`icon::Entries()` is just `reinterpret_cast<IconEntry*>(m_data)`; `GetIconEntry(icon*, i32)`
(RVA `0xc6920`) is the same thing as a free function.

Retail proof of the 13-byte stride and of `srcOffset` being m_data-relative
(`?IconToBitmap@@...`, RVA `0xd0570` prologue):

```
    mov  esi, dword ptr [ecx + 0x12]   ; icon::m_data
    lea  ecx, [eax + 2*eax]            ; frame*3
    lea  ebx, [eax + 4*ecx]            ; frame*13
    lea  edi, [ebx + esi]              ; entry   = m_data + frame*13
    mov  eax, dword ptr [ebx+esi+0x9]  ; entry->srcOffset
    add  eax, esi                      ; stream  = m_data + srcOffset
```


## 3. IconEntry (frame header), 13 bytes

```
    bit   7      0 7      0
        +--------+--------+
    +0  |      x (i16)    |   signed pen-relative X offset of the frame's left edge
        +--------+--------+
    +2  |      y (i16)    |   signed pen-relative Y offset of the frame's top edge
        +--------+--------+
    +4  |      w (i16)    |   frame width  in pixels
        +--------+--------+
    +6  |      h (i16)    |   frame height in pixels
        +--------+--------+
    +8  |  type / flags   |   ONE BYTE, see below
        +--------+--------+
    +9  |                 |
    +10 |  srcOffset      |   i32, byte offset from m_data to this frame's pixel stream
    +11 |     (i32)       |
    +12 |                 |
        +-----------------+
    = 13 bytes (0x0d), pack(1)
```

`x`, `y`, `w`, `h` are all loaded with `movsx word` in retail, so all four are signed 16-bit.

Byte +8 is read by *no* decoder in this family (retail loads only +0, +2, +4, +6 and +9).
In the widely documented ICN layout this byte tags the frame's stream dialect - value 32
marks a monochrome frame. Retail instead hard-codes the dialect at the call site (see
section 5). See "Findings" below: the repository currently models this byte as `u8 flags : 5`,
which cannot hold 32.


## 4. Geometry model

`x`/`y` are offsets from the caller's pen position, not absolute coordinates. Every decoder
resolves them the same way, and the flipped decoders mirror about the pen:

```
    forward (ICON_DRAW_NORMAL)              flipped (ICON_DRAW_FLIPPED)

    left  = x_pen + entry->x                left  = x_pen - entry->x - entry->w + 1
    right = left + entry->w - 1             right = left + entry->w - 1
    top   = y_pen + entry->y                top   = y_pen + entry->y
    bot   = top + entry->h - 1              bot   = top + entry->h - 1

    decode walks X left -> right            decode walks X right -> left
    row advances +pitch each newline        row advances +pitch each newline
    (Y is never mirrored)                   (Y is never mirrored)
```

`pitch` is `bitmap::m_width` (RVA `0xd0570`: `movsx ecx, word ptr [edx+0x12]`), and the row
base is `bitmap::m_pixels + pitch * top` (`[edx+0x16]`). Only the X axis mirrors; there is no
vertical-flip decoder.

`icon::CombatClipDrawToBuffer` publishes this rectangle into a caller-supplied `SLimitData`
and folds it into the global combat extent (`giMinExtentX`..`giMaxExtentY`) - that is the
mechanism the combat screen uses to compute dirty rectangles.


## 5. Two stream dialects

There are two mutually incompatible run-length encodings. Nothing at runtime chooses between
them: the *call site* picks the decoder, and feeding a color stream to a mono decoder
desynchronizes immediately (mono treats a literal opcode as a fill and never advances past
the literal payload bytes).

```
    dialect          decoders                                used for
    ---------------  --------------------------------------  ----------------------------
    color RLE        IconToBitmap, FlipIconToBitmap,          all normal sprites, fonts,
                     IconToBitmapColorTable, Flip...,         creatures, UI, map objects
                     IconToBitmapYModify, Flip...
    mono RLE         MonoIconToBitmap, FlipMonoIconToBitmap,  combat grid (CMBTMISC), radar
                     DimIconToBitmap, FlipDimIconToBitmap
```


## 6. Color RLE opcode map

One command byte, sometimes followed by operand bytes. Read with `ReadIconRleByte` (post-
increment cursor). Dispatch is on the top two bits.

```
    command byte
    bit  7 6 5 4 3 2 1 0
        +-+-+-+-----------+
        |0| n n n n n n n |    0x00        END OF ROW
        +-+-+-------------+    0x01..0x7f  LITERAL run of n bytes (n = command)
                               payload: n raw palette indices follow inline

        +-+-+-------------+
        |1|0| c c c c c c |    0x80        END OF IMAGE  (c == 0)
        +-+-+-------------+    0x81..0xbf  SKIP c transparent pixels (c = command & 0x3f)

        +-+-+-------------+
        |1|1| c c c c c c |    0xc0        EXTENDED (shadow / transparency), see 6.1
        +-+-+-------------+    0xc1        LONG SOLID: next byte = count, then color byte
                               0xc2..0xff  SOLID run of c pixels (c = command & 0x3f),
                                           next byte = palette index
```

Retail proof (`0xd0570`): `test al,al / jge literal`; `test al,0x40 / jne solid`;
`and eax,0x3f / je end` for the skip path; `and edx,0x3f / je extended`,
`cmp eax,0xc1 / jne short_solid` for the solid path.

Consequences worth knowing:

- A 1-pixel solid run cannot be encoded, because `0xc1` is stolen as the long escape.
  Encoders emit a 1-byte literal instead.
- Maximum short solid run is 63; longer runs use `0xc1` with an 8-bit count (max 255).
- Maximum skip is 63 (6 bits), because bit 6 is the solid flag.
- `0x00` and `0x80` are the only terminators. There is no row count and no stream length:
  the decoder runs until it reads `0x80`. Row count is *not* checked against `entry->h`.
- The **last row carries no `0x00`**. A stream's final row is closed by the end-of-image
  opcode directly, so a decoder that counts rows must count `end-of-row opcodes + 1`.
  Verified on every frame in retail `HEROES2.AGG`; see section 12.

### 6.1 The 0xc0 extended command

`0xc0` introduces a shadow/transparency run. It is followed by a flags byte and, optionally,
a count byte.

```
    0xc0  <flags>  [<count>]

    flags byte
    bit  7 6 5 4 3 2 1 0
        +-+-+-------+---+
        |R|A| level |len|
        +-+-+-------+---+
         | |    |     |
         | |    |     +-- 0x03  len: run length 1..3; if 0, a further count byte follows
         | |    +-------- 0x3c  level: dim-palette selector, effective index = (flags>>2)&0xf
         | +------------- 0x40  A: apply. 0 = pure transparent skip, 1 = apply dim palette
         +--------------- 0x80  R: recolorable. If the caller passed a non-zero `color`
                                argument, this run is filled solid with `color` instead.
```

Retail proof (`0xd0570`): `and edx,0x3 / jne have_count` then a second `ReadIconRleByte`;
`test ecx,ecx` on the `color` argument then `test al,-0x80` for R; `test al,0x40` for A;
`and eax,0x3c / shl eax,0x6` for the palette address.

Palette address is

```
    palette = (u8*)uDimPal + (flags & 0x3c) * 0x40
            = (u8*)uDimPal + ((flags >> 2) & 0xf) * 256
```

`uDimPal` is `u8[3][4][256]` (`include/SOURCE/dimPalette.h`) = 12 palettes of 256 bytes.
The encoding can express levels 0..15; levels 12..15 read past the table. See Findings.

The R bit is how a creature gets its selection/spell highlight: `army::Draw` passes the
highlight color down as `icon::CombatClipDrawToBuffer(..., offset, ...)` which becomes the
`color` argument of `IconToBitmap`, and every R-tagged shadow run in the sprite is painted
that color instead of being dimmed.


## 7. Mono RLE opcode map

Simpler, one byte per command, no inline payload.

```
    command byte
    bit  7 6 5 4 3 2 1 0
        +-+---------------+
        |0| n n n n n n n |    0x00        END OF ROW
        +-+---------------+    0x01..0x7f  SPAN of n pixels (n = command)
                                           the pixels' value comes from the decoder,
                                           not from the stream

        +-+---------------+
        |1| c c c c c c c |    0x80        END OF IMAGE  (c == 0)
        +-+---------------+    0x81..0xff  SKIP c pixels (c = command & 0x7f)
```

Retail proof (`?DimIconToBitmap@@...`, RVA `0xcfd50`): `test dl,dl / jge span`;
`and edx,0x7f / je end`. Note the 7-bit skip mask - the mono dialect has no solid flag to
steal bit 6, so its skip range is 1..127 rather than the color dialect's 1..63.

Span semantics depend on the decoder:

```
    MonoIconToBitmap / FlipMonoIconToBitmap   dst[i] = color            (solid fill)
    DimIconToBitmap  / FlipDimIconToBitmap    dst[i] = pal[dst[i]]      (read-modify-write)
                                              pal = (u8*)uDimPal + color*256
```

Retail proof of the dim indexing (`0xcfd50`): `mov edi,[esp+0x24] / shl edi,0x8` then
`mov bl, byte ptr [ebx+edi+<uDimPal>]`. So for the mono dialect the `color` argument is a
flat dim level 0..11, not a palette index.


## 8. Parse actions

Decoding is two stages. Stage 1 (opcode -> action) is identical in all eleven routines.
Stage 2 (action -> pixels) is what distinguishes them.

### 8.1 Stage 1: opcode to action

```
    color dialect
    -------------
    ACT_END                       0x80
    ACT_NEWROW                    0x00
    ACT_SKIP(n)                   0x81..0xbf         n = cmd & 0x3f
    ACT_LITERAL(n, src)           0x01..0x7f         n = cmd,  src = n inline bytes
    ACT_SOLID(n, idx)             0xc2..0xff         n = cmd & 0x3f, idx = next byte
    ACT_SOLID(n, idx)             0xc1               n = next byte,  idx = next byte
    ACT_SHADOW(n, level)          0xc0, A=1, R=0     or R=1 with caller color == 0
    ACT_SKIP(n)                   0xc0, A=0          "transparent" extended run
    ACT_SOLID(n, callerColor)     0xc0, R=1 and caller color != 0

    mono dialect
    ------------
    ACT_END                       0x80
    ACT_NEWROW                    0x00
    ACT_SKIP(n)                   0x81..0xff         n = cmd & 0x7f
    ACT_SPAN(n)                   0x01..0x7f         n = cmd
```

### 8.2 Stage 1 state machine (forward decoders)

```
                       +-------------------------------------+
                       |                                     |
                       v                                     |
              +-----------------+                            |
              | read cmd byte   |                            |
              +--------+--------+                            |
                       |                                     |
         cmd & 0x80 == 0 ------------------> +--------------+|
                       |                     | cmd == 0 ?   ||
                       |                     +--+--------+--+|
                       |                     yes|      no|   |
                       |                        v        v   |
                       |                  X = left   copy n  |
                       |                  y += 1     bytes   |
                       |                  row+=pitch X += n  |
                       |                        |     src+=n |
                       |                        +-------+----+
                       |                                |
         cmd & 0x40 == 0 ---> c = cmd & 0x3f            |
                       |      c == 0 ? --> RETURN       |
                       |      X += c ------------------>+
                       |                                |
         cmd & 0x40 != 0 ---> c = cmd & 0x3f            |
                              c != 0 --> solid ---------+
                              c == 0 --> extended ------+
```

The reverse (flipped) decoders are the same machine with `X += n` replaced by `X -= n` and
the run's destination taken as `[X-n+1 .. X]` instead of `[X .. X+n-1]`. Literal payload is
still consumed forward from the stream but written right-to-left, so a flipped sprite is a
true horizontal mirror.

### 8.3 Stage 2: emit transforms

Every routine is (geometry) x (per-pixel transform) over the same action stream:

```
    routine                       direction  literal          solid       shadow
    ----------------------------  ---------  ---------------  ----------  ---------------
    IconToBitmap                  L->R       memcpy           memset      pal[dst]
    FlipIconToBitmap              R->L       byte loop        memset      pal[dst]
    IconToBitmapColorTable        L->R       tbl[src] loop    memset(tbl[idx])
                                                                          pal[dst] if gate
    FlipIconToBitmapColorTable    R->L       tbl[src] loop    memset(tbl[idx])
                                                                          pal[dst]
    IconToBitmapYModify           L->R+shear memcpy           memset      pal[dst]
    FlipIconToBitmapYModify       R->L+shear byte loop        memset      pal[dst]
    MonoIconToBitmap              L->R       (n/a)            memset(color)
    FlipMonoIconToBitmap          R->L       (n/a)            memset(color)
    DimIconToBitmap               L->R       (n/a)            pal[dst], pal = uDimPal+color*256
    FlipDimIconToBitmap           R->L       (n/a)            pal[dst], pal = uDimPal+color*256
```

`tbl` is the caller's 256-byte remap table (`gColorTableYellow`, `gColorTableGray`,
`gColorTableRed`, ...). The `gate` in `IconToBitmapColorTable` is its trailing `dimGate`
argument: when 0 the shadow loop still walks the span but writes nothing.

### 8.4 Clip normalization

The color and mono decoders share one entry-time normalization
(`H2_ICON_NORMALIZE_CLIP`, retail `0xd0570+0x51`):

```
    if (clip != NO_CLIP) {
        if (frame rect fits entirely inside the clip rect) {
            clip = NO_CLIP;                 // take the fast unchecked path
        } else {
            clip = CLIP;
            clipR = clipX + clipW - 1;      // inclusive edges, cached in statics
            clipB = clipY + clipH - 1;
        }
    }
    // if the caller passed NO_CLIP, clipR/clipB are NOT initialized at all
```

So `NO_CLIP` means literally no bounds test anywhere in the inner loops. The two `YModify`
decoders skip this block entirely and always clip (their `clip` argument is dead, verified in
retail at `0xd9ce0` - no test of the parameter exists in the prologue).

### 8.5 Shear (YModify) decoders

`IconToBitmapYModify` / `FlipIconToBitmapYModify` take an extra `i8* shear` indexed by
*screen* row (480 entries, `gyModify`), applied as the row's X origin:

```
    forward:   X_origin(row) = left  + shear[y]
    flipped:   X_origin(row) = right - shear[y]
    shear[y] == 0x7f  ->  row is skipped entirely (ICON_SHEAR_SKIP_ROW)
```

`combatManager::RippleCreature` fills `gyModify` with a sine table to wobble a creature;
`combatManager::VaporizeCreature` writes `0x7f` into alternating stripes to dissolve it.


## 9. Worked example

A 5x2 frame: row 0 = 2 transparent, 3 solid color 0x2a; row 1 = 1 literal 0x10, then a
2-pixel level-3 shadow, then end. (Row 0 spans five columns: 2 skipped plus 3 solid.)

```
    stream bytes:  82 C3 2A 00 01 10 C0 CC 02 80
                   |  |  |  |  |  |  |  |  |  |
                   |  |  |  |  |  |  |  |  |  +-- 0x80  END OF IMAGE
                   |  |  |  |  |  |  |  |  +----- count = 2   (len field was 0)
                   |  |  |  |  |  |  |  +-------- flags 0xCC = R:1 A:1 level:3 len:0
                   |  |  |  |  |  |  |            (0x03 field must be 0 for a
                   |  |  |  |  |  |  |             count byte to follow; 0xCE
                   |  |  |  |  |  |  |             would encode len 2 inline)
                   |  |  |  |  |  |  +----------- 0xC0 extended
                   |  |  |  |  |  +-------------- literal payload, palette index 0x10
                   |  |  |  |  +----------------- 0x01 LITERAL, n = 1
                   |  |  |  +-------------------- 0x00 END OF ROW
                   |  |  +----------------------- solid palette index 0x2a
                   |  +-------------------------- 0xC3 SOLID, n = 3
                   +----------------------------- 0x82 SKIP, n = 2

    rendered (forward, '.' = untouched):
        row 0:  . . * * *          * = 0x2a
        row 1:  # s s              # = 0x10, s = uDimPal[3][dst] (or the caller's
                                              highlight color, since R is set)
```


## 10. Data-trust constraints

The decoders validate nothing. A caller or asset that violates any of these corrupts memory:

1. `frame` is never bounds-checked against `m_frameCount`.
2. `srcOffset` is never bounds-checked against the body length.
3. There is no row limit. `entry->h` is used only by the clip-normalization fast-path test;
   the decode loop terminates solely on `0x80`. A stream missing its terminator walks the
   destination bitmap downward without limit.
4. Under `ICON_DRAW_NO_CLIP` there is no horizontal bound either. `icon::DrawToBuffer`,
   `icon::FillToBuffer` and `icon::DimToBuffer` all pass `ICON_DRAW_NO_CLIP`, so those three
   entry points require the caller to guarantee the frame lands fully on-screen.
5. Shadow level *should* be 0..11 - `(flags & 0x3c) * 0x40` can address 0..15 but `uDimPal`
   holds only 12 palettes. **The shipped assets violate this**, see section 12.
6. `DimIconToBitmap`'s `color` argument must be 0..11 for the same reason (it is a flat dim
   level, not a palette index).


## 11. Findings

### F1. Flipped decoders drop partially-clipped solid and shadow runs (retail defect)

Confirmed in the shipped binary, not a reconstruction artifact.

In every reverse fill/shadow/span path - all five mirrored decoders, color and mono alike -
the visibility guard is

```
    if (rowVisible && clipX <= runLeft && X <= clipR) {
        if (clipX <= runLeft) memset(full run);       // always taken
        else                  memset(clamped run);    // DEAD
    }
```

The inner test repeats the outer one, so the clamping branch is unreachable and a run that
straddles either clip edge is discarded whole instead of being clipped. Retail emits the
redundant compare and the dead block verbatim:

```
    FlipIconToBitmap            0xd1ba0   solid at +0x1a2, shadow at +0x2ca
    FlipIconToBitmapColorTable  0xd9790   solid at +0x227, shadow at +0x33d
    FlipIconToBitmapYModify     0xd9ce0   solid at +0x235, shadow at +0x314
    FlipMonoIconToBitmap        0xda800   span  at +0x1a2
    FlipDimIconToBitmap         0xdaa20   span  at +0x1ae

      cmp  ebp, ecx        ; clipX vs runLeft
      jg   skip
      ...
      cmp  ebp, ecx        ; same operands, same flags
      jg   dead_block      ; never taken
```

The forward decoders clip correctly (the forward clip helper clamps both edges), and flipped
*literal* runs clip correctly too. So the visible symptom is a mirrored sprite whose solid,
shadow and mono span runs vanish at a clip boundary while its literal runs survive - most
reachable in combat, where left-facing creatures are flipped and `gbLimitToExtent` narrows
the clip rectangle to the dirty region during partial redraws.

The defect is uniform across the mirrored half of the family: forward decoders clip, reverse
decoders reject. It is not specific to the color dialect.

Action: do not "simplify" the dead `else` in any mirrored fill/shadow/span handler. This
note is deliberately anchored to the RVAs above rather than to macro names, because the
macro layer has already been renamed once and then removed during this project's history;
locate the sites by disassembling the five functions listed. These TUs build at `/O2` (`config/units.toml`), and
retail's `/O2` still emitted both the redundant compare and the unreachable block - the
optimizer did not fold a provably redundant comparison. So the source construct that produces
that shape has to be preserved, and the empirical retail bytes, not a codegen rule of thumb,
are the reason. Those macros carry no comment today, which makes them a live trap for the
next hygiene pass.

The fact that the retail-era compiler's `/O2` (observed on the PoL line's cl 10.20) left the dead branch standing is itself a hint about the
original: a duplicated guard that survives optimization usually comes from a macro or a
nested `if` the front end had already flattened, not from hand-written straight-line C.

### F2. `IconEntry::flags` is modelled 5 bits wide but is a whole byte

`include/BASE/IconEntry.h` (and the duplicate under `HOMM2_MISC_INLINE_ICONENTRY` in
`include/BASE/Misc.h`) declare `u8 flags : 5`. Byte +8 of the frame header is a full byte,
and in the documented ICN layout the value 32 tags a monochrome frame - unrepresentable in
5 bits. The layout is unaffected (`pack(1)` puts `srcOffset` at +9 either way) and no code
reads the field, so this is a modelling defect rather than a codegen one.

Caveat before changing it: `IconEntry.h` is a shared header and this repo has repeatedly
observed MSVC re-rolling whole-TU codegen on any token change to a shared header (measured
on MSVC 4.2 cl 10.20 on the PoL line; re-validate on VC6). Any
edit needs a same-session A/B objdump across every TU that includes it, not a cached
`homm2 build` delta.

### F3. `IconToBitmapScale` silently ignores its clip arguments

`src/BASE/icon2bs.cpp` (RVA `0xd2f90`) forwards `clip`/`clipX`/`clipY`/`clipW`/`clipH` to
`IconToBitmap` only on the `scale == 32` pass-through. On every other scale it renders into a
scratch 64x64 bitmap and then composites `scale x scale` pixels straight to
`dest->m_pixels + y*pitch + x` with no bounds test of any kind. The signature promises
clipping it does not provide. Latent today only because `Viewwrld.cpp` passes
`ICON_DRAW_NO_CLIP` at all six call sites in `Viewwrld.cpp`.

### F4. `IconToBitmapScale` crops the source at the nearest zoom

`step = 32 / scale` truncates, so the sampled window is `scale * step` wide, not 32:

```
    scale   step   srcBase   samples          window   cropped per edge
      4       8       4      4,12,20,28        0..31     0   (exact)
      6       5       3      3,8,...,28        3..28     3
     12       2       5      5,7,...,27        5..27     5
```

`giViewWorldScale` takes exactly these three values (4 / 6 / 12). At `VIEW_WORLD_SCALE_NEAR`
the sampled columns run 5..27, so 5 pixels are lost on the left and 4 on the right - 9 of 32,
about 28 percent of the width. Retail behaviour; recorded so it is not mistaken for a decode
bug.

### F5. Minor: the scratch clear in `IconToBitmapScale` uses the wrong stride

The prologue does `memset(px + off, 0, 32); off += 32;` while `off < 32*64`, which clears
2048 contiguous bytes = rows 0..31 of the 64-wide scratch bitmap. The evident intent was the
top-left 32x32 (stride 64). The actual clear is a superset of the region that is later read,
so it is harmless - but it over-clears columns 32..63 and never touches rows 32..63.

## 12. Validation against shipped assets

The model in this document was executed against retail `HEROES2.AGG` by an
independent reimplementation (`../homm2-icon-rs`, `cargo run --example survey`).
Every icon in the archive was decoded and its geometry compared with its frame header.

```
    archive        HEROES2.AGG
    members        1434
    icons           846
    frames        14444

    streams that failed to decode           0
    decoded row count != declared height    0
    decoded column extent > declared width  0
```

Three things this settles.

**Row termination.** The row count matches `entry->h` exactly for all 14,444 frames only
when the final row is counted as closed by `0x80` rather than by a `0x00`. That confirms the
last row carries no end-of-row opcode.

**Width discipline.** No frame's decoded run extent ever exceeds its declared `entry->w`,
so the width field is honoured by the data even though no decoder enforces it.

**The frame `kind` byte is not a boolean.** Byte +8 takes eighteen distinct values across the
archive, which is consistent with an animation-frame count rather than a pure dialect tag,
with 32 reserved as the monochrome marker:

```
    kind   frames   kind   frames   kind   frames
    ----   ------   ----   ------   ----   ------
       0    12049      9      130     22       23
       3      144     10      165     25       26
       5      306     11       56     26       27
       6      980     14      240     29       30
       7       64     15       64     30       31
       8       27     20       63     32       19  <- mask
```

This quantifies F2 exactly: **19 frames carry `kind = 32`, which the repository's
`u8 flags : 5` model cannot represent** (5 bits saturate at 31). The other seventeen values all
fit, so those 19 frames are the entire blast radius of the narrow bitfield.

The mask-tagged frames live in `RADAR.ICN` (10), `OVERLAY.ICN` (3), `CMBTMISC.ICN` (2),
`SWAPBTN.ICN` (2), `LOCATORE.ICN` (1) and `LOCATORS.ICN` (1). `RADAR.ICN` is the icon
`advManager` draws through `icon::ClipFillToBuffer`, which routes to `MonoIconToBitmap` -
the mask decoder - so the tag and the call site agree there.

**The dialect cannot be detected by trial decoding.** 14,429 of the 14,444 streams also
decode without error under the *other* dialect, producing different pixels. Only 15 fail.
So a decoder cannot recover the dialect by attempting both and keeping the one that parses:
the call site must know, which is exactly what retail does by hard-coding the decoder per
icon and never reading byte +8.

### Shipped assets exceed the dim-palette table

Section 10 lists "shadow level must be 0..11" as a data-trust rule. Retail data breaks it.
Counting only runs with apply set and recolour clear - the branch that actually dereferences
`uDimPal + level * 256`:

```
    level    runs   level    runs
    -----  ------   -----  ------
        0  175266       7    4966
        1   66997       8    1980
        2   30911       9    6136
        3   28033      10   11345
        4    6911      11   15080
        5   11718      12      19   <- past the end of uDimPal
        6    8779      13     188   <- past the end of uDimPal
```

207 runs across 24 frames select levels 12 and 13, in `PORT0013.ICN`, `BRCREST.ICN`,
`MTNSNOW.ICN` (frames 82, 83), `PORTMEDI.ICN` and others. `uDimPal` is 0xc00 bytes, so
level 12 reads the 256 bytes immediately after it and level 13 the 256 after that. Those are
adjacent remap tables, not unmapped memory, so the reads are deterministic rather than
crashes - but they are out of bounds with respect to `uDimPal`, and any reimplementation that
range-checks the level will render these frames differently from retail.

This was missed by the first pass of the survey above, which checked stream termination and
geometry but never inspected shadow levels. Recorded as a caution: a clean survey result only
covers the invariants the survey actually tested.
