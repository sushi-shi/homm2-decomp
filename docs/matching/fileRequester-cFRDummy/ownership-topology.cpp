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
