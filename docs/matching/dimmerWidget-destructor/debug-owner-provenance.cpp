// Independent ownership-evidence audit, 2026-09-27.
// Purpose: distinguish what the earlier PoL CodeView image actually proves
// from reconstruction assumptions about Buka's remaining destructor placement.
// No source, compiler input, object, or executable changed by this audit.
//
// Primary local artifact, read directly (not inferred from a reconstructed TU):
// /home/sheep/Projects/homm2/investigation/extracted/img-pol/HEROES2W.EXE
// SHA256 bc8f362dd49216c9fbcee1eb2e0429b467082a93330dd84ff95757c0182fc8a3
// CodeView NB09 directory: DIMMER module index267, .\Win32_RE\DIMMER.OBJ.
// It has exactly one attributed subsection: sstModule 0x120, 54 bytes.
// Its module ranges include VA 0x004dd330..0x004dd43f and vtable storage.
//
// Raw public symbols are independently decoded from sstGlobalPub, preserving
// actual decorated names rather than trusting prior demangler display text:
//   004dd330 ??0dimmerWidget@@QAE@XZ
//   004dd350 ??0dimmerWidget@@QAE@FFFFFF@Z
//   004dd390 ?Read@dimmerWidget@@QAEXXZ
//   004dd3f0 ?Main@dimmerWidget@@UAEHAAUtag_message@@@Z
//   004dd400 ?Draw@dimmerWidget@@UAEXXZ
//   004dd410 ??_EdimmerWidget@@UAEPAXI@Z
//   004dd410 ??_GdimmerWidget@@UAEPAXI@Z
//   004ebae0 ??_7dimmerWidget@@6B@
// Thus PoL's actual owner is the concrete, unnested dimmerWidget class, with
// the scalar/vector deleting aliases after the five ordinary methods. There
// is no distinct ordinary-destructor public symbol in this eight-symbol set.
// Its absence from publics does not prove absence of unnamed/private code.
//
// Limits of the debug evidence:
// - The whole NB09 image has177 S_COMPILE records, but none belongs to DIMMER.
// - DIMMER has no sstAlignSym/sstSymbols body from which compile flags could
//   be recovered; records from another compiland are not substitutes.
// - The global type payload is0100000000000000: zero type records.
// - No source-line subsections occur in the entire NB09 directory.
// Consequently this artifact cannot resolve destructor declaration/definition
// spelling, inline ownership, per-unit flags, or source-line placement.
//
// Implication for the successful Buka template-split experiment:
// It proves a sufficient native local layout, not historical type identity.
// The actual earlier-build names support retaining the concrete owner as an
// active hypothesis. They neither establish that Buka's source was unchanged
// nor prove that Buka used the experimental nested template. Do not promote
// that experimental type into an original-source claim merely because its
// bytes/layout fit; its extra compiler-input contradiction is also unresolved.
//
// Artifacts: build/dimmer-debug-provenance/{audit.py,results.json}.
// JSON retains the exact54-byte module record, all eight raw names/addresses,
// complete177-record compile inventory with attributed module names, and type
// payload. The earlier reports under investigation/reports remain read-only.
// This is an evidence-bound audit; it adds no source variant or build setting.
