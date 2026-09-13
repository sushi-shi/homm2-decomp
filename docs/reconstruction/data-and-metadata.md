# Global data and annotation reconciliation

This completes the non-function portions of `src/SOURCE/KB.cpp`, all of
`src/SOURCE/X_GLOBAL.cpp`, and `include/va.h`. It does not add function-review
credit: all 75 physical KB definitions already have their own ten-pattern
reviews. X_GLOBAL and va.h define no physical project functions. Compiler
initialization helpers emitted for globals are not additional source bodies.

## KB storage

Read the complete preamble, all declaration spans between the reviewed bodies,
and the complete data tail (lines 5682–11510 at commit `5200b81f1`). The
inter-function complement was checked against `functions.tsv`; it contains
VA annotations, private enum constants and whitespace, not missed bodies.
The two small preamble accessors and the dialog helper are indexed separately.

The terrain/frame repetition macros expand literal initializer values. Their
two 432-entry tables are real lookup owners, not executable repetition or
artificial stack storage. Palette tables really have 256 entries; the dim
palette has separate mode/level dimensions. Zero-filled ranges are retained
as part of those real lookup tables, not split into invented padding symbols.

Other actual owners include the monster database (66 packed 26-byte records),
spell database (65 packed 22-byte records), creature scoring tables, resource
and dwelling costs, faction/skill probabilities, combat coordinate tables,
and hero animation records. `SCmbtHero` has two X/two Y coordinates, an idle
count, twelve sequence lengths and twelve nine-entry frame rows. Its unused
`-1` row tails are table sentinels. The 25 elevation overlays each contain a
terrain mask and fifteen cell entries. These are meaningful aggregates;
deleting sentinel entries or declaring neighboring owners as one array is
not justified by unused values.

Campaign choices are genuinely tagged records (two sides, twelve maps,
three choices). The per-tag resource/artifact/spell/faction/skill values and
amounts were read in full. This does not establish safety for an invalid
campaign index or fabricated saved record. Monster flag/resource alternate
views retain their separate U05 disposition; initializer nesting alone does
not prove a union removable.

Read every literal and initializer in the artifact, creature, spell, terrain,
resource, building, UI/help, campaign, settings, networking and expansion
catalogs. Views decoded CP1251 escape bytes for inspection; source bytes were
not rewritten. Literal percent signs in ordinary artifact descriptions are
not conversion directives at their direct-display consumer. Scroll entry86
is a distinct one-argument format. Other tables intentionally mix plain
text and `%s`/`%d` templates; their consumers, not a global assumption that
all strings are plain text, determine formatter safety.

The existing per-consumer evidence remains authoritative: B50 (hero), B52
(KB), B66 (town/building), B87/B88 (overview/request), B90 (drawing), B92
(setup), B93 (expansion campaign), and B94 (events), plus the army, spell and
combat reports. Their checks use hash-pinned retail bytes and actual operand
destinations. No capacity assertion here is inferred from Unicode comment
length, pointer-table size or the longest observed player input.

The 73-entry `gWinSetup` table contains window/control IDs and real text
pointers. `IMHotSpots` is five four-word rectangles. The tail globals are
separate source-annotated objects, including typed managers, configuration,
timers, mappings, music state and character buffers. The shared `gText` owner
is 768 bytes; the separate bottom-view, window, command-line, player-name and
path buffers keep their declared extents. Their initialization, length,
termination and reentrancy contracts remain attached to callers. In
particular a larger global scratch buffer cannot justify an overrun of an
unrelated 500-byte local buffer.

## X_GLOBAL

Read all 665 lines, including complete expansion help/award/scenario catalogs,
recruitment/stable/site text, all 211 password strings, the twelve-entry new
hero alignments, seven shrine costs, password indices and runtime globals.
Scenario rows have eight storage slots even for four-map campaigns; empty
entries are real initialized values. The four stable strings, six Joseph
names and six Ivan names are separate owners, as recovered by O08. The
password producer's bounded normal-domain evidence does not validate arbitrary
loaded indices. The constructed expansion campaign and spell objects retain
their actual types and initialization; no fake aggregate is introduced.

## va.h

Read all 55 lines. Despite its name, this is annotation infrastructure, not
an argument-slot or `va_list` implementation. Clang annotations record
VA/DATA/compiler-generated/vtable metadata and enable layout assertions;
the VC6 branches make metadata byte-neutral. `DATA_COMPGEN` returns its value
argument, and vtable declarations do not allocate stand-in storage. There
are no manual-varargs, owner-recovery or buffer accesses to simplify here.

## Disposition and verification

No additional reconstructed-code edit is justified by these remaining data
and metadata reads. Preserve real extents, tag-dependent values and separate
owners. Earlier per-function applied changes and open contracts remain
explicit; complete review coverage is not a claim that every retail input is
safe or that every proposed simplification has a byte-compatible spelling.

The latest full build (`build/build-events-final2.log`) passes with 1,727/1,727
exact function checks and 291,987/291,987 data bytes. Its strict before/after comparison
(`build/events-final2-native.log`) checks all 98 native objects, 1,826 emitted
functions, allocated sections and ordered relocation graphs. This final
metadata pass changes only audit documentation and review hashes.
