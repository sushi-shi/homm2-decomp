# Localization architecture

Status: implementation and known-limitations record, 2026-08-24.

The native port has three localization inputs:

1. engine messages compiled with the executable;
2. text stored in original AGG widget files and maps;
3. glyphs and labels baked into original bitmap resources.

No original game asset is stored in this repository. Project-owned catalogs
are packaged with the game; localized retail resources remain user-supplied.

## UTF-8 invariant

All text inside the engine is UTF-8. This includes:

- English source messages and PO/MO translations;
- localized legacy tables after startup binding;
- SDL committed text input and editable buffers;
- text passed to measurement, wrapping, and bitmap-font drawing;
- original widget/map/save text after it crosses a decoding boundary.

The renderer decodes UTF-8 to Unicode code points before selecting bitmap
glyphs. Cursor movement, deletion, truncation, case conversion, comparison,
and wrapping operate on code-point boundaries and never split an encoded
character. Gameplay shortcuts remain physical-key events, separate from text
input, so a keyboard layout does not insert duplicate characters or alter
hotkeys.

Original files are not assumed to be UTF-8. `X-Homm2-Resource-Encoding` in a
catalog describes the separately obtained edition associated with that
catalog. Serialized UI strings are decoded exactly once when read. Authored
map and old-save fields are preserved when already valid UTF-8 and otherwise
decoded with that resource profile. The latter is necessarily a heuristic
because the retail formats contain no encoding tag.

The currently implemented resource decoders are Windows-1252, Windows-1251,
and UTF-8. A future legacy edition using another encoding needs a decoder at
this boundary; it does not change the UTF-8 runtime.

## Catalogs and the English source of truth

Translator-facing files use GNU PO and compiled GNU MO. Runtime lookups use
stable semantic IDs:

```cpp
localization::Tr("adventure.confirm.quit");
localization::TrPlural("requester.map.size_mismatch", count);
```

[`locales/messages.def`](../locales/messages.def) is the only authoritative
copy of English engine messages. Call sites contain IDs, not English fallback
text.

A PO entry also contains the English `msgid`. This apparent duplication is
intentional and required by the PO format: translators need the source text
and translation tools use it to track changes. It is only a validated
snapshot. The build regenerates the POT from `messages.def` and
`tools/validate_catalog.py` rejects any PO entry if its context, singular, or
plural English source no longer matches, including entries whose translation
is still empty. Therefore changing English in the registry without updating a
catalog cannot silently leave two different English strings; the build fails
until the PO is merged.

Large recovered global tables use IDs such as `table.<symbol>.<index>`.
`tools/localization_tables.py` extracts their authoritative English
initializers, builds the POT portion, and can import format-compatible Russian
entries from the reconstructed Buka branch. The bridge binds 1,999 entries
across 114 tables to catalog-owned UTF-8 storage. Together with 512 directly
registered messages, the Russian catalog currently covers all 2,511 generated
entries. New or structurally different messages should use semantic IDs
directly.

GNU gettext requires a translation to retain a source string's leading
newline. Some fixed-size legacy widgets instead need that line for English but
must reclaim it for a longer translation. In a legacy-table entry only, a
translation beginning with `\n\b` preserves gettext's structural invariant;
the binding bridge removes both control characters before the label reaches
the widget. The Buka `Map Size` label is the current audited use of this escape.

Catalogs may be incomplete. A missing, empty, stale, or format-incompatible
translation falls back to the authoritative English string. Placeholder
signatures are checked both during the build and defensively at runtime.

Plural selection is not keyed to a language name. The runtime parses and
validates each catalog's standard gettext `Plural-Forms` expression, so Polish,
Russian, one-form languages, and future grammars do not require branches in
game logic. Grammatical case and gender still require whole-message templates
or explicitly contextual noun forms; concatenating translated fragments is
not sufficient.

## Language selection

Language tags are normalized BCP-47-style values such as `en`, `ru`, `pl`, or
`pt-br`. Region tags fall back to their base language. Selection precedence is:

1. `--language=<tag>`;
2. `HOMM2_LANGUAGE`;
3. `language.cfg` in the platform user-data directory;
4. the host locale;
5. built-in English.

The retail `HEROES2.CFG` layout is not changed. Language settings belong in a
port-owned file. Language selection occurs before preferences and default
names are initialized. Live language switching is deliberately deferred until
resource caches, active windows, and generated names can be invalidated
atomically.

Catalog discovery checks `HOMM2_LOCALE_PATH`, the executable's `lang`
directory, the installed shared-data directory, and development-tree
`lang`/`locales` directories. Catalogs must declare UTF-8 content.

## Bitmap fonts and future languages

UTF-8 storage does not make the original bitmap fonts Unicode fonts. A catalog
selects a language-independent `X-Homm2-Font-Profile`, which maps Unicode code
points to frames in the edition's `FONT.ICN` and `SMALFONT.ICN` files.

Implemented profiles are:

- `latin`: the original 96-frame English layout (ASCII, with the recovered
  lower-case folding behavior);
- `buka-cyrillic`: the Buka Cyrillic layout, including `Ё`/`ё`, requiring at
  least 161 frames.

The profile is metadata, not an `if (language == ...)` decision. A future
language can reuse a compatible profile. A language needing different glyphs
must add a code-point-to-frame profile and a minimum-frame capability test,
then name that profile in its catalog. Unknown profiles are rejected safely.
This is necessary because an arbitrary Unicode character cannot be rendered
by a 96-frame retail bitmap font.

If a selected profile cannot be rendered by the localized archives, the game
logs a precise warning, restores English catalog bindings, disables localized
archive overrides, and continues with the primary English assets. It never
indexes beyond the available font frames.

## External localized resources

Set a primary installation and, optionally, a localized installation:

```text
HOMM2_DATA=/games/homm2-english
HOMM2_LOCALE_DATA=/games/homm2-buka
HOMM2_LANGUAGE=ru
```

The resource manager opens archives in this lookup order:

1. localized `HEROES2X.AGG`;
2. localized `HEROES2.AGG`;
3. primary `HEROES2X.AGG`;
4. primary `HEROES2.AGG`.

The first matching resource ID wins. Directory metadata and payload offsets
always come from the same archive, so a partial localized installation can
override only changed entries while everything else falls back to the primary
data. AGG, ICN, BIN, SMK, and map payloads are never copied into the source or
install package.

`tools/agg_manifest.py` records only names, IDs, sizes, and SHA-256 hashes. It
can compare independently obtained editions without extracting copyrighted
payloads:

```sh
python3 tools/agg_manifest.py manifest \
  /games/homm2/DATA/HEROES2.AGG english.json
python3 tools/agg_manifest.py compare \
  /games/homm2/DATA/HEROES2.AGG \
  /games/homm2-localized/DATA/HEROES2.AGG changes.json
```

The inspected English base archive has 1,434 entries and 96-frame font icons.
No Buka archive is available in this workspace, so the exact Russian changed
resource set is intentionally not claimed. The reconstructed Buka renderer
proves the different font layout and its 161-frame requirement.

## Serialized and authored text

Widget records in entries such as `MAGEWIND.BIN`, `THIEFWIN.BIN`,
`RECRUIT0.BIN`, and `REQUESTS.BIN` contain initial labels outside the
executable. The resource overlay supplies an edition's localized versions;
`textWidget` and `textEntryWidget` decode those fields to UTF-8 when reading
them. A future catalog overlay keyed by resource name, widget kind, and widget
ID could support translations without a localized retail BIN.

MP2/MX2 maps contain fixed-size names, descriptions, rumours, riddles, signs,
events, hero names, and town names. They have no reliable locale metadata.
Display paths now normalize those strings at the boundary without changing
the raw map layout. Official-map translations can later be optional sidecars
keyed by map checksum; selecting a UI language must not replace user-authored
prose automatically.

Retail saves and network packets also contain small fixed byte arrays. UTF-8
is copied without splitting a code point, but those legacy capacities can hold
fewer non-ASCII characters and old retail executables will not render the new
UTF-8 bytes. Full-length Unicode names with strict retail interoperability need
a versioned extension or sidecar that separates runtime strings from the
legacy serialized fields. This remains an explicit compatibility limitation.

## Adding a language

To add a new language:

1. create `locales/<tag>.po` as UTF-8 and merge it with the generated POT;
2. set `Language`, standard `Plural-Forms`, `X-Homm2-Font-Profile`, and
   `X-Homm2-Resource-Encoding` headers;
3. translate semantic IDs and compatible legacy-table entries;
4. reuse an existing font profile or implement and test a new glyph mapping;
5. compare any separately obtained localized archives with the metadata-only
   manifest tool and test partial-resource fallback;
6. test startup, menus, scenario selection, hero/town screens, combat, long
   wrapping, text entry, maps, saves, and missing-resource fallback.

No new language should require changing catalog lookup, UTF-8 editing,
wrapping, or plural selection. The shared case utility currently covers the
Latin, Greek, and Cyrillic ranges used by known editions; a script with other
case rules needs shared Unicode mappings rather than language-specific game
logic. Genuinely edition-specific legacy decoding and bitmap-glyph mapping
belong in new profiles.

## Current status and known limitations

Implemented:

- startup language selection, English fallback, UTF-8 MO loading, catalog
  packaging, stale-source and placeholder validation;
- a complete 1,668-entry Russian catalog: 1,204 recovered table entries and
  464 directly registered messages;
- a source comparison audit whose only remaining edition differences are the
  internal map name `Chaos` and save-file prefix `WIN_`, neither displayed as
  translatable prose;
- UTF-8 rendering, measuring, wrapping, committed input, cursor editing,
  truncation, comparison, and common case conversion;
- catalog-driven gettext plural expressions;
- localized four-archive overlay, metadata-only AGG comparison, and safe font
  capability fallback;
- decoding boundaries for serialized widgets and major map/save display paths.

Known follow-up work that does not change the catalog architecture:

- replace unsafe fixed-buffer formatting where longer UTF-8 translations can
  overflow recovered storage;
- add screenshot/replay coverage in English and Russian with real localized
  resources;
- design a versioned compatibility extension for full-length Unicode player,
  hero, and town names;
- optionally add checksum-keyed official-map translations and widget catalog
  overlays.

The acceptance bar is that English remains the reliable fallback, translations
can be changed without rebuilding code, malformed catalogs/resources fail
safely, original assets stay external, and no UTF-8 operation splits or
misinterprets a character.
