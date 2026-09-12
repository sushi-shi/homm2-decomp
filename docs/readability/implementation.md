# Applying the common-helper audit

Work in progress on `refactor/buka-common-helpers`, based on the completed audit
at `77b87cd47` and canonical `decomp-gold-2.1-buka` at `ca2904a91`.
The requested deliverable is the applied readability changes and a pull request
against the canonical branch. Generated/portable-source propagation remains a
separate step.

## Scope and acceptance

Apply the [ranked findings](findings.md) according to their precise contracts in
[candidates.md](candidates.md). Work through the A and B families in small measured
groups; give every H family an explicit final disposition. Rank C observations
are not mandatory abstractions, and R entries remain exclusions. Do not make a
generic helper out of variants with different evaluation, narrowing, mutation,
ownership or event order. Preserve meaningful call policies and existing APIs.

Retained changes require the canonical build and byte/ordered-relocation review.
Before/after compiler-object comparisons supplement the retail comparison; they
are not substitutes for it. Test the strict modern declarations too. Update the
reading inventory for reviewed changes, then publish the branch and open the PR.

Planned groups:

1. Existing widget-message macro and seven optional-argument API families.
2. Existing accessors, affected-army indexing and CP1251 vocabulary.
3. Narrow widget, graphics and resource operations.
4. Army, creature, spell, combat-grid and action contracts.
5. Map, town, player-resource, event and calendar contracts.
6. Small transport storage operations and fixed UI/campaign protocols.
7. Whole-tree caller/contract review, explicit disposition of every finding,
   combined verification, refreshed inventory and PR handoff.

No audit family is marked implemented by this initial progress record.

## Reproducible verification inputs

- Retail image extracted without running the installer from the existing local
  Buka disc archive: `homm2g-buka.iso`,
  `Autorun/Launch/Setup2/data1.cab` / `data1.hdr` / `data2.cab`,
  `Program Executable Files\\HMM2PL.exe`.
  SHA-256: `bc7e9c9320aa3e5c1ffca6d2bfa530ecedb5a3bca1b91c959501c15ad72c329a`;
  this is the checked-in retail-image pin. All extracted files stay gitignored.
- The published `toolchain-vc6-sp5` archive currently hashes to
  `b243b68a1df8c2c54f7830a14221fa0a73fd3bbca78343fd1dd81a815a32ecec`, not the
  repository's pin. The installer correctly refused it; no checksum was changed.
- Rebuilt with `nix-shell scripts/toolchain/create-toolchain-release.nix`.
  All 12 pinned compiler/linker/assembler/runtime artifacts verified, and the
  produced archive matches the repository pin exactly:
  `accfbb0ab1a63d4b698d0d205950611386c39b81b1868b336dbfdab413bb84c4`.
  `homm2.init.toolchain --check` passes after extraction into `build/toolchain`.

## Setup repairs and controls

Fresh `homm2 build` failed before compiling: the link-graph generator split its
address-sorted objects at data-only X_GLOBAL. Without generated symbols this
placed BASE/Midi in the direct-link group and left no Midi archive member.
Archive membership now follows the BASE unit tier, preserving address order
within each group. Four regressions cover fresh inventories, unanchored X_GLOBAL,
known ordering and fixed OMF assembly members; all four pass.

The first parallel annotation pass raced while initializing lowercase SDK-header
symlinks. Running `python3 -m homm2.init.clangd` serially initialized the mirror;
no SDK file, compiler flag or game declaration was changed for that setup race.

All 98 original TUs compiled. Annotation parsing then rejected OldMain's ternary
of two string literals passed to the legacy mutable-char score API. An explicit
`const_cast<char*>` preserves the existing interface and does not authorize a
write to the literal. Recompiled KB before/after: all 11 non-debug sections have
identical bytes, sizes and flags; all 5,085 ordered relocations retain their
types, sites and targets. Compiler-generated `$SG`/`$T`/`$L` numbers may differ,
but resolve to identical sections/offsets; named symbol identities are unchanged.
The KB annotation pass now succeeds (508 symbols). This is a compatibility
prerequisite, not a newly implemented audit helper or a retail-exactness claim.

The initialized baseline passes `homm2 build`; its current report is 1,727/1,727
exact compared functions and 291,995/291,995 data bytes. Those report totals do
not replace the raw-byte/ordered-relocation controls above. `homm2 selftest`
passes all 888 tests, including the four new link-graph regressions.

Current implementation stage: the shared widget-message macro and seven
declaration-default families.
