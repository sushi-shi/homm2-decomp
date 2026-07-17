// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Iconf2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Iconf2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/IconEntry.h>
#include <string.h>
// Per-call decoder scratch — its own file-static block (0x534c60+).
DATA(0x00534c78) static IconEntry *gFlipEntry;
DATA(0x00534c68) static i32 gFlipX0;
DATA(0x00534ca4) static i32 gFlipXEnd;
DATA(0x00534c74) static i32 gFlipY;
DATA(0x00534c7c) static i32 gFlipX;
DATA(0x00534c80) static u8 *gFlipSrc;
DATA(0x00534c9c) static i32 gFlipClipR;
DATA(0x00534c8c) static i32 gFlipClipB;
DATA(0x00534c90) static i32 gFlipRow;
DATA(0x00534c64) static u32 gFlipRun;
DATA(0x00534c98) static u8 gFlipColor;
DATA(0x00534c94) static u32 gFlipCnt2;
DATA(0x00534c70) static u32 gFlipDimLen;
DATA(0x00534c84) static u8 *gFlipDimPal;
DATA(0x00534c6c) static u32 gFlipCnt;
DATA(0x00534c88) static u8 *gFlipDimDst;
DATA(0x00534ca0) static u8 *gFlipDst;
DATA(0x00534c60) static i32 gFlipSkip;

// @semantic
// 214bd52 fresh structural pass: complete CFG, sub esp,8 frame, and all external relocation
// targets; candidate .text remains 0x4e5 versus retail 0x4f1, with 82/81 relocations. The sole
// count excess is the setup gFlipY reload (9/8): retail keeps the +0x73 load through the fourth
// clip clause, while this build reloads it at +0x89. The first unmasked divergence remains +0x1d:
// candidate loads entry Y before forming the entry pointer; retail forms EDI, loads/subtracts
// entry X, then loads entry Y. The decoder begins at +0xe9 versus +0xe3.
// A retail/adjacent-decoder-evidenced scratch declaration sequence raised 85.63395 to 86.5756;
// the literal path now has the real shared nonzero-skip publication and split right-arm skip
// lifetime, but VC4.2 still keeps that lifetime in ECX where retail spills three intermediates
// through [esp+0x10]. Mutating parameter x regressed, while clause-local Y caching and split cursor
// loads were byte-identical. This is not a wall or permitted stop. Continue from the setup Y/X
// allocation and right-skip spill after a newly proved lifetime/header/compiler-state change;
// do not replay either older matrix or iconf2b-fresh-214bd52.tsv.
// A later consolidated 239-candidate audit forced the best safe `srcIcon->m_data` inline accessor
// across balanced declaration/predecessor states. Every cross regressed from the accessor-only
// 84.771880% maximum; the best combined state was 84.692310%, candidate 0x4f4 versus retail 0x4f1,
// with the same exact 81/81 relocation multiplicity. Predecessor noise is therefore closed for
// this helper; no generated source/state was retained. Continue from structural setup/spill evidence.
// The 032a2b3 retry exhausted a new typed-alias/right-skip ownership family. Whole-entry
// references, a separate right-arm temporary, semantic skip-name buckets, cn/skip declaration
// order, setup-width slot reuse, compound updates, and the new icon2bc decoder-lifetime
// declaration surface were byte-identical. A field-Y reference regressed to 85.52%; volatile
// diagnostics did force memory ownership but regressed to 81.44%/80.44%, so they are not source
// evidence and were discarded. See iconf2b-fresh-032a2b3.tsv; revisit only with a new real
// aliasing/declaration surface that explains the retail spill without volatile semantics.
// The 8116876 sibling-template differential aligned the retail decoder with iconf2bc/iconf2by
// and recovered the exact scratch owner VAs: the earlier DATA annotations had assigned most
// semantic roles to adjacent four-byte cells. The shared clipped-fill current-Y lifetime and
// clipped-literal copy-count snapshot are retained, although VC4.2 emits the same 0x4e5 bytes.
// Nested/early-reject gates were also byte-identical; a shared selected-destination lifetime
// regressed to 85.78% and was discarded. See iconf2b-template-8116876.tsv. The remaining setup
// and [esp+0x10] spill residual is still unresolved and is not a wall.
// The 6a86651 corrected-owner follow-up tested the newly derivable declaration surfaces. Retail
// physical order, reverse physical order, sibling symbol-table order, sibling BSS-offset order,
// function-local ownership, and one packed scratch record all regressed (84.33-85.87%). The exact
// first-use and sibling source orders were already covered by the earlier matrices and were not
// replayed. The retained file-static order is still 86.5756%; see the appended corrected-owner
// family in iconf2b-template-8116876.tsv.
// The 708de32 post-dimPalette pass tested the new real header/lifetime evidence. Isolating the
// narrow dimPalette owner (as icon2bc now does) restored the desired ESI-source/EBX-X allocation
// but regressed to 84.60%, grew relocation occurrences to 84/81, and left the +0x1d Y-load
// divergence. A const-correct entry owner was byte-identical; chained transformed-X spellings,
// the sibling final-Y lifetime, and an int pitch all regressed. The broad-header source was
// restored; a clean live rebuild is 86.05%/0x4e5/82:81, while the retained source-hash maximum
// remains 86.5756%. See the appended post-dimPalette family. This remains unresolved, not a wall.
// The 5645ed8 hybrid follow-up localized the narrow owner's three excess occurrences: every
// scratch owner except gFlipY exactly matches retail, while gFlipY is 11/8 because setup, clipped
// fill, and clipped dim each reload it once. CURSOR.h or hero.h alone restore the broad 82/81
// allocation, proving those unrelated X_GLOBAL dependencies dominate the palette declaration;
// declaring dimPalette before X_GLOBAL is byte-identical to broad canonical. Nested vertical
// gates improve the narrow state to 85.36% but regress broad canonical, and dependency-after-
// scratch, Misc-only, const/register lifetime, and include-order hybrids do not beat 86.05% live.
// The integrated broad source remains; see the appended 5645ed8 hybrid family. This is unresolved.
// The fresh7 typed-view pass recovered a stronger source ownership without changing the broad
// dependency surface: retain an IconEntry array root and a separate byte cursor derived from it.
// That restores the retail ESI source cursor and the [edi+9] srcOffset add, rebuilding at the
// retained 86.5756%, 0x4e5 bytes, and 82/81 relocations. Pointer and reference entry views are
// byte-identical. CodeView contains only a synthetic line-1 record for this optimized compiland,
// so it provides no local/accessor boundary to transfer. Delayed-Y, early entry-publication,
// sibling-setup, field-pointer, and split entry-X lifetime combinations all regressed; see the
// appended fresh7 typed-view family. The unresolved first divergence remains the +0x1d Y load
// versus retail's +0x1d entry LEA, and the sole excess occurrence remains gFlipY. This is not a wall.
// The fd9229b follow-up tested only new retail-backed boundaries. The real 0xc-byte GetIconEntry
// helper has out-of-line retail callers, but forcing its exact body inline here regressed to 83.33%
// and moved the decoder cursor back to EBX. Assigning or dereferencing through the real gFlipEntry
// owner either added three owner reloads (85/81 relocations, 84.58%) or reproduced the rejected
// early-publication 86.13% shape. A named 13-byte entry-offset lifetime regressed to 84.55%; early
// pitch declaration regressed to 86.05%, while split X/Y declarations and source-before-X order
// were byte-identical. The typed-cursor source is restored unchanged; see the appended fd9229b
// family. The +0x1d/Y occurrence residual remains unresolved and is still not a wall.
// The fresh8 normalized-sibling pass compared retail Icon2b, Iconf2b, and iconf2bc setup roles.
// All three read one or more indexed IconEntry fields before materializing the selected entry,
// but the exact boundary differs: Icon2b reads x/srcOffset before its +0x19 LEA, Iconf2b reads
// width before its +0x1d LEA, and iconf2bc reads width/x before its +0x21 LEA. Expressing the
// Iconf2b width read before entry binding, or ending the indexed-root scope immediately after
// producing entry/src, is byte-identical at 86.5756% and keeps ESI/[edi+9] plus 82/81 relocs.
// A two-stage final-Y lifetime instead grows the frame to 0x10 and regresses to 83.40%. Thus the
// shared semantic selection phase is not the missing allocator trigger; the canonical typed-root
// source is restored. See the appended fresh8 family. The residual is unresolved, not a wall.
// The fresh9 ordered-relocation pass proved the sole excess occurrence precisely. Retail loads
// gFlipY once at +0x73 (relocation +0x75) and keeps ECX live for both `clipY > y` and
// `entry->h + y`; candidate loads at +0x78 (relocation +0x7d) and again at +0x89 (relocation
// +0x8b). Mutating the caller's y lifetime reproduces retail's `add ebp,ecx` accumulator direction
// and removes the extra reload, but it spills the result to the y parameter home and substitutes
// that home for two required global reads, leaving only 79/81 relocations. Tight final-Y scopes,
// split global publication, and a nested horizontal/vertical clip CFG do not retain that direction;
// the nested CFG hashes to the canonical text. Direct field-to-global accumulation regresses to
// 84.98% and 83/81. Canonical 86.5756%, 0x4e5, 82/81 source is restored; see the appended fresh9
// family in iconf2b-template-8116876.tsv. This is still an unresolved residual, not a wall.
// The fresh10 consumer-ownership pass retained that proved formal-y accumulator and reconstructed
// the two later retail global reads at their actual short-circuit consumers. A plain value,
// reference, pointer, or assignment snapshot is value-numbered back to the y home (79/81). A
// four-byte semantic snapshot into the incoming clip home only after both horizontal tests defeats
// that substitution; the following bottom test reuses the snapshot, and the row-base expression
// again reads gFlipY. This raises the live score to 86.92%, keeps the 0x8 frame, ESI/[edi+9], and
// reaches 81/81 relocations with no wrong owner; candidate .text is 0x4e7 versus retail 0x4f1.
// Reusing entryY/w/x0/y instead, copying at publication, or snapshotting before the horizontal
// short-circuit regresses. The first divergence is still the premature entry-Y load before retail's
// +0x1d entry LEA. See the appended fresh10 family; this remains unresolved and is not a wall.
// The fresh11 ownership pass removed the promoted entryY local and accumulates the packed field
// directly into the caller's y parameter at the source point after the X/source-offset reads. This
// is semantically identical, preserves the fresh10 clip-home snapshot, 0x8 frame, ESI/[edi+9], and
// 81/81 relocation stream, and raises the live score again to 87.01% at 0x4e9. Register/const
// promoted locals and delayed source-cursor declaration return to 86.92%; a late pointer copy,
// dead-parameter copies, and an address-taken packed-short snapshot regress. The latter reaches
// 87.28% only by widening the frame to 0xc and is structurally rejected. The setup's first raw
// divergence remains unchanged at +0x1d (early Y load versus retail entry LEA), so this is a better
// executable checkpoint rather than a wall proof. See the appended fresh11 family.
// The fresh12 selected-frame pass tested the remaining address-formation boundary without replaying
// fresh1-11. Advancing the typed root materializes EDI before packed Y, but either reads width after
// the LEA (84.89%) or preloads srcOffset into EBP before it (86.60%). Reading width through a separate
// selection cursor is byte-identical to this checkpoint. A no-symbol inline typed selector regresses
// to 83.71%/0x4f8 and moves the decoder cursor to EBX; immediate gFlipEntry publication aligns the
// width/LEA/spill prefix but emits the observable store at +0x24 and regresses to 86.71%. Reusing the
// dead frame parameter for entry X is erased, while an explicit base-data owner plus delayed cursor
// returns to the fresh10 86.92%/0x4e7 allocation. Every variant preserves the 0x8 frame and 81/81
// relocation targets. Retail has no helper call/continuation and this TU has no predecessor, so there
// is no supported accessor or predecessor-state lever left in this family. This remains an unresolved
// residual, not a byte-proven wall; see the appended fresh12 rows.
// The fresh13 CFG-first pass normalized the decoder's ESI/EBX source/X register swap and checked
// every command block independently. Dispatch order, condition polarity, clipped/unclipped arm
// order, shared fill/dim/literal tails, newline, and all loop backedges agree with retail; unlike
// icon2bc, no reversed subtree remains to repair. A typed byte-row owner regresses to 86.14%, a
// signed literal count to 85.63%/82:81, and a combined source/destination copy expression to
// 86.23%/82:81. Explicit retail instruction-order cursor ownership, unsigned skip ownership, and
// removal of the dead flags initializer are byte-identical to the 86.39% live f24566c object
// (0x4e9, 81:81; disassembly SHA-256 a2235f0d...). See iconf2b-cfg-f24566c.tsv. This closes only
// the fresh decoder-CFG/type family at that hash. Rebuilding the identical body after a237782 made
// SIZE declarations neutral changes the live result to 82.77% and 83/81 relocations, proving that
// this closure is TU-state-local and triggering a new macro-head structural/relocation audit. The
// setup allocation residual remains unresolved, not a wall.
// The 71462a5 macro-neutral audit closes that trigger structurally. With SIZE erased under MSVC,
// the unchanged body emits 0x4fe bytes at 82.76923% and 83/81 relocations. Both excess occurrences
// are gFlipY: candidate clipped-fill +0x1b2/+0x1bf/+0x1c4 loads Y/clipB/Y versus retail
// +0x1a2/+0x1b4 loads Y/clipB, and clipped-dim +0x2e7/+0x2f6/+0x2fb repeats the same allocation
// versus retail +0x2ca/+0x2de. All other 17 scratch owners and uDimPal agree occurrence-for-
// occurrence, and there is no base-only target. The explicit per-arm currentY snapshots are already
// the retail semantic lifetime: retail keeps each Y value in ECX while loading clipB into EAX;
// macro-neutral VC4.2 instead loads Y into EAX, overwrites it with clipB, and reloads Y into ECX.
// Candidate and retail still have the same 0x8 frame, the same two local homes, 76 basic blocks,
// 59 branches, and an identical ordered successor graph for every command arm and backedge.
// `register` on both snapshots and retail-facing comparison operand spelling emit byte-identical
// candidate text. Conversely, the same body under the former SIZE-typedef compiler state emitted
// the proved 81/81 f24566c object. Thus the new two-occurrence residual is compiler-state/register
// allocation, not missing CFG, ownership, type, or relocation structure. Do not restore fake SIZE
// typedefs or tune fuzzy here; revisit after a real header/TU-state change. See the macro-head rows
// in iconf2b-cfg-f24566c.tsv. This remains a coverage checkpoint, not a byte-proven wall.
VA(0x004d1ba0, 0x4f1)
void FlipIconToBitmap(class icon *srcIcon, class bitmap *dest, i32 x, i32 y, i32 frame,
                      i32 clip, i32 clipX, i32 clipY, i32 clipW, i32 clipH, i32 color)
{
    IconEntry *entries = reinterpret_cast<IconEntry *>(srcIcon->m_data);
    u8 *src = reinterpret_cast<u8 *>(entries);
    i32 x0 = x;
    i32 w;
    IconEntry *entry = &entries[frame];
    w = entry->w;
    x0 = x0 - entry->x;
    src += entry->srcOffset;
    y += entry->y;
    x0 = x0 - w;
    gFlipEntry = entry;
    x0++;
    gFlipX0 = x0;
    i32 X = w + x0 - 1;
    gFlipXEnd = X;
    gFlipY = y;
    if (clip != 0) {
        if (gFlipX0 < clipX || clipW + clipX < gFlipX0 + w ||
            (memcpy(&clip, &gFlipY, sizeof(clip)), clip < clipY) ||
            clipY + clipH < entry->h + clip) {
            clip = 1;
            gFlipClipR = clipX + clipW - 1;
            gFlipClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    i16 pitch = dest->m_width;
    gFlipRow = gFlipY * pitch + reinterpret_cast<i32>(dest->m_pixels);
    for (;;) {
        i32 cmd = *src++;
        if (static_cast<i8>(cmd) < 0) {
            if ((cmd & 0x40) == 0) {
                // skip run / end-of-sprite
                gFlipRun = cmd;
                i32 n = cmd & 0x3f;
                gFlipX = X;
                gFlipSrc = src;
                if (n == 0)
                    return;
                X = X - n;
                continue;
            }
            // 0xc0 - 0xff
            gFlipRun = cmd;
            u32 count = cmd & 0x3f;
            i32 flags = 0;
            if (count != 0) {
                // 0xc1 - 0xff : solid colour run
                if (cmd == 0xc1)
                    count = *src++;
                gFlipColor = *src++;
                goto do_fill;
            }
            // 0xc0 : shadow / dim run
            flags = *src++;
            count = flags & 3;
            if (count == 0)
                count = *src++;
            gFlipCnt2 = count;
            if (color != 0) {
                gFlipRun = flags;
                if (flags & 0x80) {
                    gFlipDimLen = count;
                    gFlipColor = static_cast<u8>(color);
                    goto do_fill;
                }
            }
            goto do_dim;
        do_fill:
            if (clip == 0) {
                memset(reinterpret_cast<u8 *>((gFlipRow - count) + 1 + X), gFlipColor,
                       count);
            } else {
                i32 currentY = gFlipY;
                i32 left;
                if (currentY >= clipY && currentY <= gFlipClipB &&
                    (left = (X - count) + 1, clipX <= left) && X <= gFlipClipR) {
                    if (clipX <= left) {
                        memset(reinterpret_cast<u8 *>((gFlipRow - count) + 1 + X),
                               gFlipColor, count);
                    } else {
                        memset(reinterpret_cast<u8 *>(gFlipRow + clipX), gFlipColor,
                               (X - clipX) + 1);
                    }
                }
            }
            X = X - count;
            gFlipRun = count;
            continue;
        do_dim:
            gFlipRun = flags;
            gFlipDimLen = count;
            if (flags & 0x40) {
                u8 *palette =
                    reinterpret_cast<u8 *>(uDimPal) + (flags & 0x3c) * 0x40;
                gFlipDimPal = palette;
                if (clip == 0) {
                    u8 *dp =
                        reinterpret_cast<u8 *>((gFlipRow - count) + 1 + X);
                    gFlipCnt = 0;
                    i32 dimCount = count;
                    gFlipDimDst = dp;
                    if (dimCount > 0) {
                        gFlipCnt = dimCount;
                        do {
                            u8 *dimPalette = gFlipDimPal;
                            i32 px = *dp++;
                            count--;
                            gFlipDimDst = dp;
                            dp[-1] = dimPalette[px];
                        } while (count != 0);
                    }
                } else {
                    const i32 currentY = gFlipY;
                    gFlipDimLen = count;
                    if (clipY <= currentY && currentY <= gFlipClipB &&
                        clipX <= static_cast<i32>((X - count) + 1) && X <= gFlipClipR) {
                        i32 left = (X - count) + 1;
                        u8 *dp;
                        if (clipX <= left) {
                            dp = reinterpret_cast<u8 *>((gFlipRow - count) + 1 + X);
                        } else {
                            count = (X - clipX) + 1;
                            dp = reinterpret_cast<u8 *>(gFlipRow + clipX);
                        }
                        i32 dimCount = count;
                        gFlipDimLen = count;
                        gFlipDimDst = dp;
                        gFlipCnt = 0;
                        if (dimCount > 0) {
                            gFlipCnt = dimCount;
                            do {
                                u8 *dimPalette = gFlipDimPal;
                                i32 px = *dp++;
                                count--;
                                gFlipDimDst = dp;
                                dp[-1] = dimPalette[px];
                            } while (count != 0);
                        }
                    }
                }
            }
            X = X - gFlipCnt2;
            continue;
        }
        // ---- positive command : literal copy / newline ----
        gFlipRun = cmd;
        gFlipX = X;
        if (cmd != 0) {
            if (clip == 0) {
                gFlipCnt = 0;
                u8 *dst = reinterpret_cast<u8 *>(gFlipRow + X);
                gFlipDst = dst;
                if (cmd > 0) {
                    gFlipCnt = cmd;
                    i32 k = cmd;
                    do {
                        u8 c = *src++;
                        *dst-- = c;
                        gFlipDst = dst;
                        k--;
                    } while (k != 0);
                }
            } else {
                i32 currentY = gFlipY;
                if (currentY >= clipY && currentY <= gFlipClipB) {
                    i32 left = (X - cmd) + 1;
                    if (left <= gFlipClipR && clipX <= X) {
                        u32 cn;
                        i32 skip;
                        if (X <= gFlipClipR) {
                            gFlipDst = reinterpret_cast<u8 *>(gFlipRow + X);
                            if (clipX <= left) {
                                gFlipSkip = 0;
                                cn = cmd;
                            } else {
                                cn = (X - clipX) + 1;
                                skip = cmd - cn;
                                goto set_skip;
                            }
                        } else {
                            cn = gFlipClipR;
                            src = src + (X - cn);
                            gFlipDst =
                                reinterpret_cast<u8 *>(gFlipRow + cn);
                            if (clipX <= (X - cmd)) {
                                gFlipSkip = 0;
                                cn = (cmd - X) + cn;
                            } else {
                                cn = clipW;
                                skip = cmd;
                                skip = skip - X;
                                skip = skip - clipW;
                                skip = gFlipClipR + skip;
                                goto set_skip;
                            }
                        }
                        goto skip_set;
                    set_skip:
                        gFlipSkip = skip;
                    skip_set:
                        i32 copyCount = cn;
                        gFlipDimLen = cn;
                        gFlipCnt = 0;
                        if (copyCount > 0) {
                            gFlipCnt = copyCount;
                            do {
                                u8 c = *src++;
                                (gFlipDst = gFlipDst - 1)[1] = c;
                                cn--;
                            } while (cn != 0);
                        }
                        src = src + gFlipSkip;
                    } else {
                        src = src + cmd;
                    }
                } else {
                    src = src + cmd;
                }
            }
            X = X - cmd;
            gFlipRun = cmd;
            continue;
        }
        // newline
        X = gFlipXEnd;
        gFlipY = gFlipY + 1;
        gFlipRow = gFlipRow + pitch;
    }
}
