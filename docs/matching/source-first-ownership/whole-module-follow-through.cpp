// Whole-module source/build follow-through, measured 2026-09-27.
// Parent 23515eb2098c5ef7553b487cbf737a2668268f8c, isolated matcher-4,
// matcher/native-link-recovery. Working alone; main checkout untouched.
//
// Correction to the preceding checkpoint: testing helper placement while
// freezing all surrounding module boundaries did not finish the holistic
// investigation. This pass writes complete ordinary ownership alternatives
// before compiling them, links their real callers, and judges joint code,
// storage, initializer and producer consequences. No output corrections.
//
// ACTUAL 2.0 OWNERSHIP EVIDENCE
// Independently dumped embedded NB09 records from:
// /home/sheep/Projects/homm2/investigation/extracted/img-pol/HEROES2W.EXE
// SHA256 bc8f362dd49216c9fbcee1eb2e0429b467082a93330dd84ff95757c0182fc8a3.
// scripts/archive/codeview_dump.py writes pol-codeview/ under the widget
// artifact directory below. Relevant sstModule records and public symbols:
//   .\Win32_RE\Misc.obj, basewin.lib: text 4c3d10..4c6fd0;
//   .\Win32_RE\DIMMER.OBJ, basewin.lib: text 4dd330..4dd440;
//   .\Win32_Re\REQUEST.OBJ, direct: text 48c920..48fd20;
//   ?cFRDummy@@3PADA at 4f88c4, in REQUEST's data range.
// DIMMER owns both constructors, Read, Main, Draw and deleting-wrapper symbols.
// This supports whole Misc/DIMMER TUs and a global cFRDummy as the starting
// organization. It does not prove unchanged 2.1 boundaries, and supplies no
// Audiere owner. The compiler-input count alone cannot identify a missing TU.
//
// PASS A: COMPLETE AUDIO MODULES, 12 SOURCE ARRANGEMENTS
// Cartesian product:
//   API owner: ordinary free functions / one AudiereEffects static class;
//   file owner: separate Effects and Music / one combined Audiere TU;
//   Node initialization: constructor-body assignments / member initializers /
//     implicit default construction with resource/next assignments in Play.
//
// The static-class model owns the existing state as one private static member,
// keeps Purge/Find private, and exposes the actual ten public operations as
// static members. All soundmgr call sites are changed coherently. No wrapper,
// extra call, virtual function, field, template parameter or dummy use is added.
// The combined source puts all includes first, then the complete Effects and
// Music definitions. The old Music object is omitted from that link.
// Every arrangement recompiles soundmgr and MilesSound, plus the appropriate
// audio owners: 42 successful compiles and 12 full native game links.
//
// Results:
// - Free vs static-class ownership has identical linked .text/.data payloads
//   within each construction/file-owner pair. The only .rdata differences are
//   in the debug directory timestamp and record-size fields (per-arm PDB paths).
//   These program bytes do not distinguish the two sensible API organizations.
// - Separate, body-assignment construction preserves all ordinary audio body
//   sizes/masked bytes and relocation counts against the fresh raw parent.
// - Member initialization and implicit/default construction both make Play
//   906 rather than 924 bytes, with 39 rather than 40 relocations. In the
//   separate owner they also remove the emitted OutputStream destructor copy.
//   That is a construction/unwind consequence, not merely a worse score.
// - Combining both complete modules with body-assignment construction preserves
//   their ordinary function bodies, but leaves only one ctype initializer pair
//   at the combined tail. RefPtr helpers move after Music's ordinary functions.
//   StopAudiereMusic moves from 4cd260 to 4cd180 in the linked control.
// - Every form still emits Node immediately after Purge. Whole-module merging
//   therefore does not defer that first required copy.
// - Current compiler producer count is 95; combined audio gives 94; retail is
//   96. No compensating source file was invented to equalize the count.
//
// These are diagnostic function-body/relocation-count comparisons, not claims
// of full ordered relocation closure. All raw section bytes and ordered sites,
// types, symbol identities and addends are retained in objects.json.
// The unified constructor descendants were all linked despite their regressions.
// No audio source variant is retained: the ordinary private node/header parent
// remains simpler, and these alternatives do not explain retail ordering.
//
// PASS B: REQUEST STORAGE x COMPLETE DIMMER LAYOUT, 32 GAME LINKS
// First remove all five named one-byte REQUEST arrays. Their only uses are
// strcpy source operands; write the ordinary empty literals directly.
// Independently choose the real empty-filename result owner:
//   global: existing cFRDummy initialized by "";
//   member: fileRequester::emptyFilename, a static class pointer;
//   local: static pointer inside GetFilename;
//   literal: GetFilename returns "" directly, no separate pointer object.
// No alias or separate sentinel object is introduced.
//
// Eight complete DIMMER arrangements:
//   ordinary TU, declaration-only KB include, explicit/implicit destructor;
//   ordinary TU, normal KB.h include, explicit/implicit destructor;
//   virtual Main/Draw/destructor defined in class, explicit/implicit destructor;
//   whole class defined in its header, explicit/implicit destructor.
// Both real consumers are compiled where present: DIMMER and WINDOW. The
// header-only forms remove DIMMER's standalone input; they do not retain an
// empty TU or force otherwise unused methods to emit. All alternatives use
// the real concrete widget class, fields and implementations, with no template.
// Four REQUEST compiles + fourteen widget/consumer compiles, all successful.
// All 4 x 8 full native links complete; no partial product or timeout.
//
// REQUEST raw observations (offsets within ordinary sections):
// owner    pointer in .data   pointer's empty cell   five strcpy cells in .bss
// global       004                    00c           010 014 018 01c 020
// member       004                    00c           010 014 018 01c 020
// local        244                    020           00c 010 014 018 01c
// literal      absent                return->020    00c 010 014 018 01c
// Retail requires pointer 004 -> 020, with the five calls using 00c..01c.
// Global/local forms preserve ordinary function masked bytes and relocation
// counts. Direct return changes GetFilename's instruction at the empty return:
// it materializes a literal address rather than loading a pointer object.
// Member ownership renumbers generated initializer symbols; this is not a
// missing initializer (their linked section payloads agree with global).
// The local case recovers BSS order but contradicts initialized-data placement;
// direct return contradicts the actual pointer load. Neither is retained.
//
// DIMMER raw observations:
// - All ordinary explicit-TU methods preserve raw body bytes and COMPLETE
//   ordered relocation sites/types/identities/addends when restoring KB.h.
//   It also restores the missing ctype helpers and CRT initializer cell.
// - Implicit destruction emits 19 bytes instead of retail's explicit 28-byte
//   destructor. This is a real code-shape contradiction, not an RVA preference.
// - Header-only DIMMER emits only Main, Draw, deleting wrapper and destructor
//   in WINDOW. Standalone constructors and Read disappear, while retail has
//   those bodies. The known real callers do not support a fully header-only
//   class under these build inputs.
// - Header-defined virtual methods move with vtable/constructor emission; no
//   tested arrangement supplies the retail ordinary-method/wrapper sequence.
// - WINDOW's masked function bodies remain unchanged in the retained broad-TU
//   control. Full-header alternatives are diagnostic, not retained edits.
//
// Native section mismatches for representative complete products:
//                                         .text  .rdata   .data  .rsrc
// narrow TU, explicit, global filename    115890     489  173545      0
// broad TU, explicit, global filename       7054       8    2778      0
// broad TU, explicit, local filename        7078       8    3312      0
// broad TU, explicit, literal return       22435       8   35730      0
// broad TU, implicit, global filename       7070       8    2778      0
// header virtuals, explicit, global       115904     492  173545      0
// full header, explicit, global           211494    1074  173635      0
// These one-pass links use per-arm PDB paths; they are not historical-header
// equality tests. All 32 images, MAPs, response files and results are retained.
// Equal mismatch totals do not imply equal images: removing REQUEST's named
// arrays rotates its six cells even where existing address drift hides that
// change from the total. Raw owner-relative relocation review detects it.
//
// RETAINED: restore ordinary KB.h in DIMMER and use direct empty strcpy
// literals in REQUEST, keeping the cross-version-supported global cFRDummy.
// This follows the user's source-first direction and accepts the reopened
// six-cell residual. It does not retain artificial arrays to hide that residual.
// The whole-build recovery after restoring the real header dependency is why
// the previous broad regression should not have ended the investigation.
// No destructor TU, synthetic ownership class, padding, forced root, generated
// declarations, COFF corrections or image corrections enter production.
//
// Reproducers and full source products:
//   build/holistic-modules/{probe.py,analyze.py,manifest.json,results.json,
//     baseline.json,analysis.json,<arrangement>/...}
//   build/holistic-storage-widgets/{probe.py,analyze.py,manifest.json,
//     results.json,analysis.json,retained-raw-review.log,pol-codeview/...}
// Durable copies: module_probe.py, storage_widget_probe.py in this directory.
// Reproduce with raw objects built from parent 23515eb2, in the verified Nix
// shell. These probes depend on that source snapshot and ordinary link graph.
// Initial harness setup caught a misplaced Python regex flag and a Clang
// overlay-header lookup issue; both were fixed before the complete products
// reported here. No failed setup attempt is counted as source coverage.
// Fresh canonical retained-source validation is recorded in follow-through.md.

// Representative ordinary source, before comparing its linked bytes:
// strcpy(m_fileNames[indexData].text, "");
// strcpy(m_extensions[indexData].text, "");
// strcpy(gLastFilename, "");
// strcpy(cycleNameBuffer, "");
// strcpy(filteredNameMap, "");
// H2_CONST char* cFRDummy = "";
//
// Complete alternative node constructions (mutually exclusive):
// AudiereSampleNode(sample* r, AudiereSampleNode* n) {
//     stream = NULL; sampleResource = r; next = n;
// }
// AudiereSampleNode(sample* r, AudiereSampleNode* n)
//     : stream(NULL), sampleResource(r), next(n) {}
// // No declared constructor, with this caller-owned initialization:
// AudiereSampleNode* newNode = new AudiereSampleNode;
// newNode->sampleResource = sampleResource;
// newNode->next = gAudiereEffects.sampleList;
// gAudiereEffects.sampleList = newNode;
