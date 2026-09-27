// Pinned VC6 noinline-attribute acceptance test, measured2026-09-27.
// Worktree matcher-2, branch matcher/bss-audiere; cwd and HOMM2_DIR verified
// in the existing persistent Nix shell. No production/root changes.
//
// Parent: the real typed static-member cleanup owner in template-purge-owner.cpp,
// primary-implicit arm. The only effective source change is the function
// declaration below. Actual seven-field state, primary Purge body, all callers,
// constructor specialization and Node destructor are unchanged byte-for-byte.
//
// Prior local source/header/dossier/probe search found no noinline test in this
// family. Binary spelling search found no 'noinline' text in pinned CL.EXE,
// C1XX.DLL or C2.DLL; that observation alone is not a compiler acceptance test.
// One actual-parent compilation then establishes the dialect result:
//
//   static __declspec(noinline) void PurgeFinishedSamples(void);
//
// VC6 returns2, no timeout, with:
//   error C2485: 'noinline' : unrecognized extended attribute
// The same declaration is diagnosed during primary class-template parsing and
// concrete sample instantiation. There is no successful object to audit.
//
// Disposition: stop this descendant at the requested acceptance gate. The
// planned {primary body original/EOF} x {implicit/explicit class instantiation}
// product is NOT RUN because the required declaration attribute is rejected.
// No alternative-keyword sweep, output correction, pragma or retained flag.
// This excludes this attribute under the pinned dialect, not all possible
// ordinary ownership mechanisms or ways to preserve a call boundary.
//
// Artifacts: build/audiere-purge-noinline/
// probe.py, result.json, compile.log, exact source/header overlay.
// result.json records flags, parent path, source SHA256 and unchanged-body check.
// Flags are production unit_flags(BASE/AudiereEffects), /Od /Ob1 /Gy /GX /MT.
// Reproduce with python3 build/audiere-purge-noinline/probe.py in the lane shell.

// Sole tested declaration change in AudiereEffectsState<Resource>:
// static __declspec(noinline) void PurgeFinishedSamples(void);
