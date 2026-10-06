# Automatic precompiled headers close both tail walls

Measured 2026-10-06 on `work/exact-hash-2` with the pinned VC6 SP5 compiler.
The generators in this directory write manifests for
`python3 -m homm2.permute.emission_order` (run them from the repository root,
then pass `build/probe/<name>.json` with `--output build/probe/<name>`);
`yx-all.py` recompiles every C++ unit with an extra flag and compares each
object with the current one.

## Rule

With a precompiled header (`/YX`, or an explicit `/Yc`/`/Yu` pair), VC6 does
not emit header-inline functions, or the deleting destructor of a class
declared in the precompiled headers, where the first out-of-line use
appears. It emits them at the end of the object:

- before the ctype initializer pair when `<windows.h>` (here through
  `SOURCE/KB.h`) is parsed after the class's header;
- after the pair when the KB headers are parsed before it.

Without a precompiled header the same sources emit those functions
immediately after the first referencing function. Function bodies,
relocations and Play's inline expansion are unchanged in every variant
(`differs` is empty against the current objects).

| Target | Source form | Order |
| --- | --- | --- |
| DIMMER | in-class `virtual ~dimmerWidget() {}`, includes unchanged, `/YX` | `c0 cA R M D G X E E C` = retail |
| AudiereEffects | in-class `~AudiereSampleNode() {}`, `SOURCE/KB.h` first, `/YX` | `U F P I D S A E E C N` = retail after LINK folds D and C |

`/YX` is byte-neutral for all 93 C++ units of the current source
(`yx-all.py /YX`: 93 same order, 93 byte-identical), so it cannot be excluded
for SOURCE; the BASE tier rule now carries it because retail BASE needs it.
It is VC6's default project setting ("Automatic use of precompiled headers").
This agrees with HoMM1's Buka build, where `~dimmerWidget` also stays inline
and follows its deleting destructor.

With these forms the historical native link is byte-identical to retail
(SHA-256 `bc7e9c9320aa3e5c1ffca6d2bfa530ecedb5a3bca1b91c959501c15ad72c329a`).

## Tables

### post-init (56 variants)

| variant | order | differs from current object |
|---|---|---|
| plain_inclass+str_top+nopch+gy | U N P S E E C | - |
| plain_inclass+str_top+nopch+nogy | U P E E N S C | - |
| plain_inclass+str_top+pch+gy | U P S E E C N | - |
| plain_inclass+str_top+pch+nogy | U P E E S C N | - |
| plain_inclass+str_end+nopch+gy | U N P S E E C | - |
| plain_inclass+str_end+nopch+nogy | U P E E N S C | - |
| plain_inclass+str_end+pch+gy | U P N S E E C | - |
| plain_inclass+str_end+pch+nogy | U P E E N S C | - |
| plain_implicit+str_top+nopch+gy | U N P S E E C | - |
| plain_implicit+str_top+nopch+nogy | U P E E N S C | - |
| plain_implicit+str_top+pch+gy | U N P S E E C | - |
| plain_implicit+str_top+pch+nogy | U P E E N S C | - |
| plain_implicit+str_end+nopch+gy | U N P S E E C | - |
| plain_implicit+str_end+nopch+nogy | U P E E N S C | - |
| plain_implicit+str_end+pch+gy | U N P S E E C | - |
| plain_implicit+str_end+pch+nogy | U P E E N S C | - |
| plain_eof+str_top+nopch+gy | U P N S E E C | - |
| plain_eof+str_top+nopch+nogy | U P E E N S C | - |
| plain_eof+str_top+pch+gy | U P N S E E C | - |
| plain_eof+str_top+pch+nogy | U P E E N S C | - |
| plain_eof+str_end+nopch+gy | U P N S E E C | - |
| plain_eof+str_end+nopch+nogy | U P E E N S C | - |
| plain_eof+str_end+pch+gy | U P N S E E C | - |
| plain_eof+str_end+pch+nogy | U P E E N S C | - |
| plain_hdr+str_top+nopch+gy | U N P S E E C | - |
| plain_hdr+str_top+nopch+nogy | U P E E N S C | - |
| plain_hdr+str_top+pch+gy | U P S E E C N | - |
| plain_hdr+str_top+pch+nogy | U P E E S C N | - |
| plain_hdr+str_end+nopch+gy | U N P S E E C | - |
| plain_hdr+str_end+nopch+nogy | U P E E N S C | - |
| plain_hdr+str_end+pch+gy | U P N S E E C | - |
| plain_hdr+str_end+pch+nogy | U P E E N S C | - |
| tmpl_inclass+str_top+nopch+gy | U P N S E E C | - |
| tmpl_inclass+str_top+nopch+nogy | U P E E N S C | - |
| tmpl_inclass+str_top+pch+gy | U P N S E E C | - |
| tmpl_inclass+str_top+pch+nogy | U P E E N S C | - |
| tmpl_inclass+str_end+nopch+gy | U P N S E E C | - |
| tmpl_inclass+str_end+nopch+nogy | U P E E N S C | - |
| tmpl_inclass+str_end+pch+gy | U P N S E E C | - |
| tmpl_inclass+str_end+pch+nogy | U P E E N S C | - |
| tmpl_implicit+str_top+nopch+gy | U N P S E E C | - |
| tmpl_implicit+str_top+nopch+nogy | U P E E N S C | - |
| tmpl_implicit+str_top+pch+gy | U N P S E E C | - |
| tmpl_implicit+str_top+pch+nogy | U P E E N S C | - |
| tmpl_implicit+str_end+nopch+gy | U N P S E E C | - |
| tmpl_implicit+str_end+nopch+nogy | U P E E N S C | - |
| tmpl_implicit+str_end+pch+gy | U N P S E E C | - |
| tmpl_implicit+str_end+pch+nogy | U P E E N S C | - |
| tmpl_eof+str_top+nopch+gy | U P S N E E C | - |
| tmpl_eof+str_top+nopch+nogy | U P E E S N C | - |
| tmpl_eof+str_top+pch+gy | U P S N E E C | - |
| tmpl_eof+str_top+pch+nogy | U P E E S N C | - |
| tmpl_eof+str_end+nopch+gy | U P S N E E C | - |
| tmpl_eof+str_end+nopch+nogy | U P E E S N C | - |
| tmpl_eof+str_end+pch+gy | U P S N E E C | - |
| tmpl_eof+str_end+pch+nogy | U P E E S N C | - |

### post-init4 (12 variants)

| variant | order | differs from current object |
|---|---|---|
| audiere+hdr_inline+no_va+no_sample+none | U P S E E C N | - |
| audiere+hdr_inline+no_va+no_sample+kb | U P S N E E C | - |
| audiere+hdr_inline+no_va+no_sample+smgr | U P S E E C N | - |
| audiere+hdr_inline+no_va+sample+none | U P S E E C N | - |
| audiere+hdr_inline+no_va+sample+kb | U P S N E E C | - |
| audiere+hdr_inline+no_va+sample+smgr | U P S E E C N | - |
| audiere+hdr_inline+va+no_sample+none | U P S E E C N | - |
| audiere+hdr_inline+va+no_sample+kb | U P S N E E C | - |
| audiere+hdr_inline+va+no_sample+smgr | U P S E E C N | - |
| audiere+hdr_inline+va+sample+none | U P S E E C N | - |
| audiere+hdr_inline+va+sample+kb | U P S N E E C | - |
| audiere+hdr_inline+va+sample+smgr | U P S E E C N | - |

### post-init5 (14 variants)

| variant | order | differs from current object |
|---|---|---|
| audiere+hdr_inline+none | U P S E E C N | - |
| audiere+hdr_inline+windows.h | U P S N E E C | - |
| audiere+hdr_inline+SOURCE_armyGroup.h | U P S E E C N | - |
| audiere+hdr_inline+SOURCE_hero.h | U P S E E C N | - |
| audiere+hdr_inline+SOURCE_REMOTE_TYPES.h | U P S E E C N | - |
| audiere+hdr_inline+SOURCE_KB_TYPES.h | U P S E E C N | - |
| audiere+hdr_inline+BASE_message.h | U P S E E C N | - |
| audiere+hdr_inline+SOURCE_KBForward.h | U P S N E E C | - |
| audiere+hdr_inline+BASE_WINMGR.h | U P S E E C N | - |
| audiere+hdr_inline+BASE_dialog.h | U P S E E C N | - |
| audiere+hdr_inline+SOURCE_GAME.h | U P S E E C N | - |
| audiere+hdr_inline+SOURCE_town.h | U P S E E C N | - |
| audiere+hdr_inline+SOURCE_KBDeclarations.h | U P S N E E C | - |
| audiere+hdr_inline+SOURCE_KB.h | U P S N E E C | - |

### post-init6 (12 variants)

| variant | order | differs from current object |
|---|---|---|
| audiere+hdr_inline+none+pch | U P S E E C N | - |
| audiere+hdr_inline+none+nopch | U N P S E E C | - |
| audiere+hdr_inline+pch_before+pch | U P S E E C N | - |
| audiere+hdr_inline+pch_before+nopch | U N P S E E C | - |
| audiere+hdr_inline+pch_after+pch | U P S N E E C | - |
| audiere+hdr_inline+pch_after+nopch | U N P S E E C | - |
| audiere+hdr_inline+tu_after+pch | U P S N E E C | - |
| audiere+hdr_inline+tu_after+nopch | U N P S E E C | - |
| audiere+hdr_inline+pch_before_tu_again+pch | U P S E E C N | - |
| audiere+hdr_inline+pch_before_tu_again+nopch | U N P S E E C | - |
| audiere+hdr_inline+winbase_pch+pch | compile error | |
| audiere+hdr_inline+winbase_pch+nopch | compile error | |

### audiere-pch (24 variants)

| variant | order | differs from current object |
|---|---|---|
| eof_inline+nopch | U F P I N S A D E E C | - |
| eof_inline+pch_sb_sep | U F P I N S A D E E C | - |
| eof_inline+pch_kb_sep | U F P I N S A D E E C | - |
| eof_inline+pch_sb_self | U F P I N S A D E E C | - |
| eof_inline+pch_kb_self | U F P I N S A D E E C | - |
| eof_inline+yx | U F P I N S A D E E C | - |
| inclass+nopch | U N F P I S A D E E C | - |
| inclass+pch_sb_sep | U F P I S A D N E E C | - |
| inclass+pch_kb_sep | U F P I S A D N E E C | - |
| inclass+pch_sb_self | U F P I S A D N E E C | - |
| inclass+pch_kb_self | U F P I S A D N E E C | - |
| inclass+yx | U F P I S A D N E E C | - |
| hdr_inline+nopch | U N F P I S A D E E C | - |
| hdr_inline+pch_sb_sep | U F P I S A D N E E C | - |
| hdr_inline+pch_kb_sep | U F P I S A D N E E C | - |
| hdr_inline+pch_sb_self | U F P I S A D N E E C | - |
| hdr_inline+pch_kb_self | U F P I S A D N E E C | - |
| hdr_inline+yx | U F P I S A D N E E C | - |
| implicit+nopch | U N F P I S A D E E C | - |
| implicit+pch_sb_sep | U N F P I S A D E E C | - |
| implicit+pch_kb_sep | U N F P I S A D E E C | - |
| implicit+pch_sb_self | U N F P I S A D E E C | - |
| implicit+pch_kb_self | U N F P I S A D E E C | - |
| implicit+yx | U N F P I S A D E E C | - |

### audiere-yx (30 variants)

| variant | order | differs from current object |
|---|---|---|
| inclass+current+yx | U F P I S A D N E E C | [] |
| inclass+current+nopch | U N F P I S A D E E C | [] |
| inclass+hdr_windows+yx | U F P I S A D N E E C | [] |
| inclass+hdr_windows+nopch | U N F P I S A D E E C | [] |
| inclass+hdr_mss+yx | U F P I S A D N E E C | [] |
| inclass+hdr_mss+nopch | U N F P I S A D E E C | [] |
| inclass+eff_kb_first+yx | U F P I D S A E E C N | [] |
| inclass+eff_kb_first+nopch | U N F P I D S A E E C | [] |
| inclass+eff_windows_first+yx | U F P I S A D N E E C | [] |
| inclass+eff_windows_first+nopch | U N F P I S A D E E C | [] |
| hdr_inline+current+yx | U F P I S A D N E E C | [] |
| hdr_inline+current+nopch | U N F P I S A D E E C | [] |
| hdr_inline+hdr_windows+yx | U F P I S A D N E E C | [] |
| hdr_inline+hdr_windows+nopch | U N F P I S A D E E C | [] |
| hdr_inline+hdr_mss+yx | U F P I S A D N E E C | [] |
| hdr_inline+hdr_mss+nopch | U N F P I S A D E E C | [] |
| hdr_inline+eff_kb_first+yx | U F P I D S A E E C N | [] |
| hdr_inline+eff_kb_first+nopch | U N F P I D S A E E C | [] |
| hdr_inline+eff_windows_first+yx | U F P I S A D N E E C | [] |
| hdr_inline+eff_windows_first+nopch | U N F P I S A D E E C | [] |
| eof_inline+current+yx | U F P I N S A D E E C | [] |
| eof_inline+current+nopch | U F P I N S A D E E C | [] |
| eof_inline+hdr_windows+yx | U F P I N S A D E E C | [] |
| eof_inline+hdr_windows+nopch | U F P I N S A D E E C | [] |
| eof_inline+hdr_mss+yx | U F P I N S A D E E C | [] |
| eof_inline+hdr_mss+nopch | U F P I N S A D E E C | [] |
| eof_inline+eff_kb_first+yx | U F P I N D S A E E C | [] |
| eof_inline+eff_kb_first+nopch | U F P I N D S A E E C | [] |
| eof_inline+eff_windows_first+yx | U F P I N S A D E E C | [] |
| eof_inline+eff_windows_first+nopch | U F P I N S A D E E C | [] |

### dimmer-yx (24 variants)

| variant | order | differs from current object |
|---|---|---|
| ool_src+current+yx | c0 G cA R M D X E E C | [] |
| ool_src+current+nopch | c0 G cA R M D X E E C | [] |
| ool_src+kb_first+yx | c0 G cA R M D X E E C | [] |
| ool_src+kb_first+nopch | c0 G cA R M D X E E C | [] |
| ool_src+windows_first+yx | c0 G cA R M D X E E C | [] |
| ool_src+windows_first+nopch | c0 G cA R M D X E E C | [] |
| inline_src+current+yx | c0 G cA R M D X E E C | [] |
| inline_src+current+nopch | c0 G cA R M D X E E C | [] |
| inline_src+kb_first+yx | c0 G cA R M D X E E C | [] |
| inline_src+kb_first+nopch | c0 G cA R M D X E E C | [] |
| inline_src+windows_first+yx | c0 G cA R M D X E E C | [] |
| inline_src+windows_first+nopch | c0 G cA R M D X E E C | [] |
| inline_hdr+current+yx | c0 cA R M D G X E E C | [] |
| inline_hdr+current+nopch | c0 G X cA R M D E E C | [] |
| inline_hdr+kb_first+yx | c0 cA R M D E E C G X | [] |
| inline_hdr+kb_first+nopch | c0 G X cA R M D E E C | [] |
| inline_hdr+windows_first+yx | c0 cA R M D G X E E C | [] |
| inline_hdr+windows_first+nopch | c0 G X cA R M D E E C | [] |
| inclass+current+yx | c0 cA R M D G X E E C | [] |
| inclass+current+nopch | c0 G X cA R M D E E C | [] |
| inclass+kb_first+yx | c0 cA R M D E E C G X | [] |
| inclass+kb_first+nopch | c0 G X cA R M D E E C | [] |
| inclass+windows_first+yx | c0 cA R M D G X E E C | [] |
| inclass+windows_first+nopch | c0 G X cA R M D E E C | [] |
