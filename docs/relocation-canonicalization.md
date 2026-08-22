# Reviewed relocation spellings

A linked PE preserves the relocated 32-bit value, not the original COFF symbol and
implicit addend. The same value may be represented as `earlier_symbol + positive`,
`later_symbol + negative`, or many other equivalent pairs. Vostok's ordinary
last-preceding-symbol rule is therefore only a fallback representation, not evidence
of the original source owner.

Canonical target generation accepts reviewed disambiguation rows from
`config/delink_reloc_aliases.tsv`:

```text
function_rva  target_rva  site_rva  owner  addend  occurrences  provenance
```

The normal key is the containing retail function plus the encoded target RVA, with
`site_rva=*`. When multiple relocations in that function encode the same address but
need different COFF spellings, `site_rva` is the absolute retail RVA of the four-byte
relocation field. An exact-site row overrides a matching wildcard row and must have
one occurrence. Vostok checks that every numeric site lies inside the containing
function, that the named owner exists, `owner RVA + addend == target RVA` with 32-bit
wrapping, and that each selected row's configured occurrence count closes. Reviewed
aliases take precedence over data-manifest ownership and nearest-symbol selection.

The same manifest can resolve ambiguous code aliases for decoded `REL32` calls and
jumps. For a code target, Vostok requires a zero addend and requires the selected name
to be one of the public symbols at that exact target RVA. Do not use this to compensate
for a wrong CRT source declaration: for example, MSVC's `_write` and legacy `write`
declarations naturally emit different C++ COFF spellings. Reconstruct the declaration
that emits retail's spelling instead of forcing the target to match the candidate.

These rows are reconstruction evidence. They are added whenever owner/addend identity
is certain; fuzzy percentage is not an acceptance criterion. Exact same-site code can
provide evidence, but semantic array indexing, TU ownership, serialization layout, or
other independent evidence can also prove a row below 100%.

The disposable paired-target pass also handles two identities which the linked PE
proves without a hand-authored alias row. A candidate ``__imp_`` DIR32 spelling is
retained only when the retail import directory maps that exact decorated name to the
encoded IAT slot. For x86 ``__stdcall`` imports whose PE export name is undecorated,
the pass recognizes only the mechanical ``__imp__Name@N`` COFF form and still
requires the same exact slot. The corresponding x86 ``__cdecl`` rule recognizes only
``__imp__Name`` and has the same slot and zero-addend requirements. A candidate REL32
spelling is retained across an identical-COMDAT fold only when the candidate object
defines both spellings with non-empty, byte-identical bodies and retail names the
folded partner at that same relocation site. An ordinal-only PE import supplies no
name by itself, so it is eligible only when a reviewed DLL-and-ordinal table supplies
the exact decorated COFF identity; unknown DLLs and ordinals remain unresolved. These
rules recover lost COFF spelling; none changes linked bytes or treats same-address
symbols as generally interchangeable.

The Gold 2.1 audit that introduced these rules closed ten previously identity-only
residuals without changing reconstructed source: ``PlayAudiereMusic``, all six
``BASE/Midi`` API functions, ``PlayMilesSample``, ``soundManager::CDStartup``, and
``soundManager::ShutdownSoundBackends``. Each retained result has identical
instruction bytes and complete ordered relocation identity after pairing. The import
directory independently identifies Audiere's ``0xea324`` IAT slot as
``_AdrOpenSampleSource@4``; the candidate object independently proves the local
``RefPtr<SampleSource>::operator=`` and ``RefPtr<OutputStream>::operator=`` COMDAT
bodies are identical before the linker selects one address.

The x86 ``__stdcall`` extension was then audited across 109 relocation sites, 41
distinct imports, and 23 newly exact functions. Those functions are ``com_rcv``,
``DPSD``, ``InitVars``, ``AppWndProc``, ``Process1WindowsMessage``, ``ResizeWindow``,
``AppCommand``, ``SetMenus``, ``nb_init``, ``fileRequester::InitializeFiles``,
``game::SetupNetworkGame2``, ``DDAppPaint``, ``DDSD``, ``WGUpdatePalette``,
``WGInitializePalette``, ``soundManager::StartupMilesBackend``,
``soundManager::Open``, ``mouseManager::SetPointer(int)``, ``MouseMessageHandler``,
``WritePrefsToRegistry``, ``IsCDDrive``, ``SetupCDDrive``, and
``BlitBitmapToScreenVesa``. Every site resolves to the exact import-directory IAT
slot, and every normalized function has a complete, equal ordered relocation stream.
The executable spans are byte-exact; the disassembler's apparent suffix differences
for ``DPSD``, ``AppCommand``, and ``DDSD`` begin after their real return in embedded
switch-table or adjacent compiler-data bytes covered by the enclosing source marker.

The candidate-wide x86 ``__cdecl`` census contained exactly two applicable sites,
both for ``__imp__wsprintfA``. The retail import directory fixes that identity at IAT
RVA ``0xea218``. Applying the rule closed ``com_init`` and
``DriveSupportsFreeSpaceQuery`` with byte-exact reviewed spans, exact CFGs, and equal
ordered relocation streams.

The Smacker ordinal rule reuses the independently reviewed 3.0g export mapping that
also generates the vendor import library. The current retail PE contains nine mapped
``smackw32.DLL`` IAT slots and the candidate contains 20 Smacker import relocation
sites across ``DoAdvance``, ``SmackManagerMain``, and ``ShutDownSmacker``. Every site
now resolves to its exact DLL-and-ordinal slot. The only newly exact function was
``SmackManagerMain``: its reviewed ``0x117d``-byte span is byte-identical, all 207 CFG
blocks are exact, and its complete ordered relocation stream is equal at 352 of 352
sites including imports (334 of 334 non-import sites).

`python3 -m homm2.build.gen_reloc_aliases` writes candidate-derived proposals to
`build/gen/reloc_alias_proposals.tsv` and unresolved groups to
`build/gen/reloc_alias_diagnostics.json`. Proposals never enter canonical targets
automatically. Multiple candidate spellings for one function/address are inherently
ambiguous after instruction movement and remain diagnostics until independently
resolved.
