// Target: cFRDummy backing byte at retail VA 0x00533d98.
// Units: SOURCE/REQUEST followed by SOURCE/SEARCH in the final link.
// Artifacts:
//   build/link/request-cross-tu-comdat/results.json
//   build/link/request-cross-tu-comdat/HMM2PL.exe
//   build/link/request-cross-tu-comdat/HMM2PL.rsp
//   build/link/request-bss-state/results.json
//
// Retail relocation identity proves six distinct writable empty-string cells.
// The five literals used by InitializeFiles, Open, and Main occupy REQUEST BSS
// offsets 0x0c, 0x10, 0x14, 0x18, and 0x1c.  cFRDummy's initializer points to
// the sixth cell at offset 0x20.  The clean VC6 SP5 object emits the same six
// cells but reserves the global-initializer literal first at offset 0x0c.
// This is a true identity rotation, not coalescible padding.
//
// Unchanged-source state census:
//
//   50/50 declaration-forest trials compiled; every trial kept cFRDummy's
//   literal at offset 0x0c.  VC6 RTM, VC6 SP3, VC6 SP5 /YX, and the ordinary
//   SP5 build all produced the same rotation.  Moving the definition, making
//   the pointer file-static, and replacing the literal with initialized or
//   uninitialized named storage also failed to put the backing cell last.
//   /Gi and omitted /Gi- preserve the same six-cell rotation.  /Gf instead
//   coalesces/removes the required writable local cells, so it contradicts the
//   six distinct retail relocation identities rather than exposing a new
//   placement orbit.

// Two further same-TU ownership arms forward-declared an external or class-static
// one-byte cell, initialized cFRDummy from it, and defined the cell after all
// functions and named REQUEST data.  VC6 still allocates either named cell at
// BSS offset 0x0c.  Source definition order therefore does not reproduce the
// rotation.  The measured arms are in build/link/request-cfr-scope/results.json.

// Historical source/include/output path matrices contribute another 29 exact-
// flag objects:
//
//   build/link/source-path-spelling/results.json
//   build/link/include-path-spelling/results.json
//   build/link/output-path-spelling/results.json
//
// Every repository-relative, basename, historical E:, installed-VC98, /Fo, /Fd,
// /Fp, case, and short-name arm keeps cFRDummy's target at offset 0x0c.  The
// retail VA 0x00533d98 also occurs as a little-endian dword exactly once in the
// entire PE, at file offset 0x116ae0: cFRDummy's initializer.  No code or other
// data provides evidence that the backing byte belongs to SEARCH or another TU.

// Historical language-selection matrix:
//
//   build/link/source-language-spelling/results.json
//
// VC6 compiled all eight ordinary C++ selections: .cpp default, explicit /TP,
// .c/.C/.cc/.cxx with /TP, and .c/.C passed through the per-file /Tp option.
// Every object retains the same six offsets and points cFRDummy at offset 0x0c.
// The reconstructed .cpp suffix versus the inventory's historical .c spelling
// therefore does not explain the retail rotation.
//
// Exact topology proof (measured, but deliberately not retained):
//
//   // REQUEST.cpp
//   struct cFRDummyStorageOwner { static char storage[1]; };
//   char* cFRDummy = cFRDummyStorageOwner::storage;
//
//   // SEARCH.cpp
//   struct cFRDummyStorageOwner { static char storage[1]; };
//   __declspec(selectany)
//   char cFRDummyStorageOwner::storage[1] = {0};
//
// REQUEST then contains a natural 0x1d-byte ordinary BSS contribution holding
// the three named globals and five function literals.  SEARCH emits a separate
// one-byte, four-byte-aligned BSS COMDAT before its eight-byte-aligned 0x8c-byte
// state block.  Untouched VC6 LINK.EXE places those contributions at exactly
// 0x00533d78..0x00533d94, 0x00533d98, and 0x00533da0 respectively.
//
// The complete four-pass historical-PDB link with those two unmodified probe
// objects and the other established inputs produced the retail SHA-256
// bc7e9c9320aa3e5c1ffca6d2bfa530ecedb5a3bca1b91c959501c15ad72c329a:
// 1,208,393/1,208,393 bytes, zero mismatches.
//
// Disposition: REJECTED as reconstructed source.  The PoL/Classic sibling
// independently preserves the developer-shaped declaration `char* cFRDummy =
// "";`, while the cross-TU selectany owner has no semantic evidence and exists
// only to steer COFF placement.  The exact probe proves that ordinary LINK can
// realize the retail topology and narrows the residual to original ownership;
// it does not license a fake class/global in production source.

// Same-TU compiler-generated COMDAT matrix:
//
//   build/link/request-natural-comdat/results.json
//
// Four ordinary C++ ownership forms were measured both before and after the
// named REQUEST BSS definitions: an initialized __declspec(selectany) array, a
// template static-data member, and a class-static selectany definition (the
// uninitialized selectany spelling is rejected by VC6).  All six compiling arms
// produce the same linked SHA-256
// 406939f12fdb055b21cb4c8359195d183f9cbf5b98f9178da10b63d261b686c3.
// VC6 emits their one-byte BSS COMDAT before REQUEST's 0x1d-byte ordinary BSS
// contribution regardless of lexical definition order.  That shifts 41 retail
// BSS references by four bytes and leaves the backing cell at the wrong end of
// the contribution.  A same-object COMDAT is therefore not a transformation-
// free replacement for the rotation; only the already rejected cross-TU form
// reaches the needed post-REQUEST boundary.
