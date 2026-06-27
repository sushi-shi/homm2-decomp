# Recovering packed records & bitfields from /Od accesses

**tags:** `topic:layout` `cpp:bitfield` `cpp:pack` `toolchain:vc42`

## symptom
- An array is indexed with a non-power-of-two stride, e.g. retail computes
  `extras - i` then addresses `off(eax, ecx, 8)` (= `extras + 7*i + off`) or does
  `mov edx,ecx; shl ecx,3; sub edx` (= `i*7`). The element size is **7** — a
  *packed* record (a `u16` + five `u8` rounds to 8 unless `#pragma pack(1)`).
- A field read is `movb m; shrb $k; andw $mask; andl $0xffff` and a write is
  `andb ~m; ... orb`. That is **bitfield** extraction/insertion, with the
  allocation unit equal to the *declared base type* (`unsigned char` => 1 byte).

## cause / fix
Model the real struct, don't cast raw bytes — it both matches better and the
`SIZE(T, n)` assert (active under clang AND MSVC 4.2) locks the layout:

```cpp
#pragma pack(push,1)
struct mapCellExtra {        // 7 bytes; stride proven by *7 indexing
    u16 index;               // +0   (movw / cmp 0xFFFF)
    u8  objFlag    : 1;      // +2 bit0     order within a byte is LSB-first,
    u8  objTileset : 7;      // +2 bits1-7  so declare bit0's field first
    u8  objIndex;            // +3   (plain byte, cmp 0xFF)
    u8  f4a:1,f4b:1,f4c:1,f4hi:5;   // +4
    u8  ovlFlag0:1,ovlFlag1:1,ovlTileset:6; // +5
    u8  ovlIndex;            // +6
};
#pragma pack(pop)
SIZE(mapCellExtra, 7);
```

Rules learned for VC4.2 /Od:
- **Allocation unit = declared bitfield base type.** `u8 x:7` occupies one byte;
  use `u8` (not `int`) so offsets stay byte-granular. A `u16` bitfield container
  (`andw` on the field) is a *different* field width — mapCell's +4 flags are
  `u16 :1` (cleared with `andw $0xFFFE`), mapCellExtra's +4 are `u8 :1` (`andb`).
- **LSB-first within a unit:** the field touching bit 0 must be declared first.
- **`field = 0`** lowers to a bare `and mem, ~mask` (no OR); **`field = v`** to
  `and mem, ~mask` + `(v<<pos)` + `or`. `<<1` shows up as `add cl,cl`.
- The retail statement *order* in a clear/init routine (e.g. ClearCellExtra) is
  the *source* order — read it straight off the andb/movb sequence.
- `&arr[i]` for an odd stride S is emitted two ways by /Od: byte ops with a
  displacement use `(arr - i)` as base + `(,i,8)` (works when S = 8-1); a bare
  pointer/`movw` precomputes `i*S` via `shl/sub`. Both fall out of writing
  ordinary `arr[i].field` — don't hand-roll the addressing.
