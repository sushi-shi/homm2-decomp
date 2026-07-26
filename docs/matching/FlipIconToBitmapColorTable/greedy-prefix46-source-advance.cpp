// Descendants of the 46-byte state-39 branch.
//
// The first divergence moved to offset 0x2e.  Retail emits the direct memory
// operation:
//     add esi, dword ptr [edi + 9]
// The retained clue emits:
//     mov eax, dword ptr [edi + 9]
//     inc ebp
//     add esi, eax
//
// Source-advance spellings (8 arms) all retained the same 46-byte state-39
// prefix.  Moving x0++ before the source advance improved that clue's fuzzy
// score to 86.657430% without extending the prefix.  Its complete 50-state
// census confirmed trial 39 as the only 46-byte island in the bounded sweep.
// Artifacts:
//   build/match-variants/iconf2bc-prefix46-src-advance-20260726/
//   build/match-variants/iconf2bc-prefix46-increment-first-states-20260726/
//
// The following descendants were also crossed with clean and retained state 39:
//   - seven signed/unsigned member and packed-offset lvalue forms;
//   - six comma/dependency sequencing forms;
//   - six width copy/reference lifetime forms;
//   - four inline source-advance helper bodies;
//   - five pointer-as-integer addition forms;
//   - seven endpoint construction/publication forms.
// None exceeded 46 bytes.  Width references and pointer-as-integer forms moved
// to contradictory early-prefix orbits; ordinary member/lvalue spellings
// collapsed to the retained code shape.
// Artifacts:
//   build/match-variants/iconf2bc-prefix46-offset-lvalue-20260726/
//   build/match-variants/iconf2bc-prefix46-sequenced-advance-20260726/
//   build/match-variants/iconf2bc-prefix46-width-alias-20260726/
//   build/match-variants/iconf2bc-prefix46-inline-advance-20260726/
//   build/match-variants/iconf2bc-prefix46-integer-pointer-20260726/
//   build/match-variants/iconf2bc-prefix46-endpoint-20260726/
//
// Split endpoint publication retained a 46-byte trial-39 prefix at 86.254410%,
// size 1335.  Its independent 50-state census again found no longer island.
// Artifact:
//   build/match-variants/iconf2bc-prefix46-split-endpoint-states-20260726/
//
// Disposition: retain the 46-byte assignment-expression/state-39 object as the
// active clue.  Do not replace clean source or lower MAX.  The next structural
// search should target why packed srcOffset becomes a temporary rather than a
// direct memory add, and should resweep each genuinely new lowering orbit.

#if 0
// Source-advance arms.
src += entry->srcOffset;
src = src + entry->srcOffset;
src = &src[entry->srcOffset];
src = entry->srcOffset + src;

// Reordered descendant: better fuzzy score, same 46-byte prefix at state 39.
x0++;
src += entry->srcOffset;

// Offset-lvalue descendants.
src += *reinterpret_cast<i32l*>(&entry->srcOffset);
src += *reinterpret_cast<i32l*>(reinterpret_cast<u8*>(entry) + 9);

// Endpoint descendants.
i32 X = w + x0 - 1;
gFCXEnd = X;

X = w + x0;
X--;
gFCXEnd = X;
#endif
