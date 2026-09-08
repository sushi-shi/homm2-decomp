# Modernization implementation review

The [September 8 audit](modernization-audit.md) is implemented in 18 separate
feature PRs. All remain open for review; nothing has been merged to `master`.

## Feature order

| Feature | Review | Base / prerequisite |
| --- | --- | --- |
| Layout checks active in optimized builds | [#19](https://github.com/sushi-shi/homm2-decomp/pull/19) | `master` |
| Defined dim-palette indexing | [#21](https://github.com/sushi-shi/homm2-decomp/pull/21) | `master` |
| Fatal ASan/UBSan build checks | [#22](https://github.com/sushi-shi/homm2-decomp/pull/22) | #21 |
| Manual build defaults and prerequisites | [#25](https://github.com/sushi-shi/homm2-decomp/pull/25) | `master` |
| Bounded map/save text and validated headers | [#28](https://github.com/sushi-shi/homm2-decomp/pull/28) | `master` |
| Bounded legacy save filenames | [#29](https://github.com/sushi-shi/homm2-decomp/pull/29) | `master` |
| AGG directory and selected-member bounds | [#33](https://github.com/sushi-shi/homm2-decomp/pull/33) | `master` |
| Fixed-size high-score record I/O | [#34](https://github.com/sushi-shi/homm2-decomp/pull/34) | `master` |
| SDL 3.4.16 / FFmpeg 8.1.2 maintenance update | [#35](https://github.com/sushi-shi/homm2-decomp/pull/35) | `master` |
| Bounded production ICN decoders | [#37](https://github.com/sushi-shi/homm2-decomp/pull/37) | #33 |
| BMP/TIL validation and raster clipping | [#40](https://github.com/sushi-shi/homm2-decomp/pull/40) | #37 |
| Variable map records and cell-chain validation | [#43](https://github.com/sushi-shi/homm2-decomp/pull/43) | #28 |
| Transactional save replacement | [#47](https://github.com/sushi-shi/homm2-decomp/pull/47) | #29 |
| Fullscreen, scaling and VSync preferences | [#48](https://github.com/sushi-shi/homm2-decomp/pull/48) | #35 |
| Fixed-width save records / runtime separation | [#50](https://github.com/sushi-shi/homm2-decomp/pull/50) | #43 |
| Optional Linux 64-bit and ARM evaluation | [#53](https://github.com/sushi-shi/homm2-decomp/pull/53) | #50 |
| Natural alignment of runtime managers/resources | [#54](https://github.com/sushi-shi/homm2-decomp/pull/54) | #50 |
| Repeatable retail gameplay / save exchange | [#52](https://github.com/sushi-shi/homm2-decomp/pull/52) | #17; run with #47 and #50 |

The existing [#17](https://github.com/sushi-shi/homm2-decomp/pull/17) covers Web
path casing and packaging. The integration check also includes the existing
font, Sphinx, music and keypad fixes
([#11](https://github.com/sushi-shi/homm2-decomp/pull/11),
[#12](https://github.com/sushi-shi/homm2-decomp/pull/12),
[#13](https://github.com/sushi-shi/homm2-decomp/pull/13),
[#15](https://github.com/sushi-shi/homm2-decomp/pull/15)).
[#45](https://github.com/sushi-shi/homm2-decomp/pull/45) removes the obsolete
`Tracks2` startup marker; the browser package needs that cleanup to start with
the supported installed-data paths.

The unrelated UTF-8/native-save redesign in
[#42](https://github.com/sushi-shi/homm2-decomp/pull/42) changes the save format
and overlaps #50 and the 64-bit work. It is not part of this legacy-format
integration. Choosing it requires a separate integration review. Ironfist PRs
are also outside this stack.

## Evidence and limits

The local `test/portable-modernization-integration` branch combines the listed
features and records merge resolutions without changing `master`. The final
integration at `21b1dcdd0` passed all seven Nix checks: 37 CTests each on native
32-bit, native 64-bit and ASan/UBSan 32-bit; Windows and Web builds; ARM core
compilation of 107 core translation units and emulated record/dialog tests;
and Rust icon parity checks. This includes the gameplay runner and the browser
restoration follow-up. The 64-bit ASan/UBSan build also passed all 37 CTests.
The display follow-up passed its standalone full Nix matrix and the browser
regression described below.

The strongest checks beyond compilation were:

- The deliberately incorrect map layout expectation fails in both optimized
  and debug builds. Instrumented production parsers reject malformed text,
  archive, image, raster, map-record and save inputs.
- Retail validation accepted 187 map headers, 2,972 AGG entries, 15,765 ICN
  frames, 109 BMPs and 472 tiles. The ICN comparison exercised 94,552 rendering
  variants against the independent Rust model. Valid menu frames remained
  byte-identical across the resource changes.
- Save replacement fault injection covers creation, multiple write positions,
  flush, close, rename and interrupted writers. The previous destination is
  preserved before a successful commit.
- Final 64-bit and 32-bit sanitizer game processes loaded Broken Alliance,
  opened and closed the hero screen, replaced the save and quit. Both retained
  exactly 108,635 save bytes and produced 14 complete framebuffer captures;
  the save SHA-256 was
  `b0db11ea7b9cdf4cf1d20fe9d9ba72bdcdd7b11a00ccc5092a119306216a9bb6`.
  Leak checks remained enabled. The repeatable runner also rejects a process
  that exits successfully without actually loading or saving.
- Maintained SDL/FFmpeg decoded Ogg music and eight Smacker frames using the
  production media backends. Native X11 fullscreen/scaling checks passed under
  Xvfb/Openbox, including mouse mapping and composed cursor preservation.
- Chromium 151 loaded the Web game, saved through the game UI, persisted the
  exact same save to IndexedDB through the existing `autoPersist` mount, and
  restored it after a page reload. The test did not force an extra save sync.
  Browser fullscreen, nearest/linear/integer selection and VSync were exercised.
  Visual inspection found a canvas/viewport mismatch after leaving fullscreen.
  The correction in #48 reconciles the browser backing size before drawing;
  the regression's upper-border pixel check now passes, and a mouse click opens
  the correct hero screen after returning to windowed mode.

No retail assets, saves or screenshots are committed. ARM evidence covers core
translation units and emulated tests, not an SDL game running on ARM hardware.
Windows evidence is compilation/linking. These checks do not certify a full
campaign, all combat paths, browser quota/interruption durability or network
play. Browser saves retain the documented asynchronous persistence contract.

## Merge details

Most integration conflicts were adjacent CMake test registrations or additions
to documentation; retain both features' registrations and ledger entries.
The substantive resolutions were:

- Replay parsing accepts quoted names from #48 and unquoted names containing
  spaces from #15, while still rejecting extra fields after a quoted name.
- Keep #33's explicit `AggEntry` wire parser and remove runtime packing around
  `resourceManager` when applying the alignment feature. Its former packing
  directive moved during the AGG change.
- Keep explicit dialog-result assignment in high-score dialogs when combining
  #34 with the 64-bit message-layout fix.

The audit's larger UI redesign, multiplayer protocol replacement and MSVC port
remain separate feature projects, as the audit specified. The existing 32-bit
target stays available alongside the optional 64-bit evaluation.
