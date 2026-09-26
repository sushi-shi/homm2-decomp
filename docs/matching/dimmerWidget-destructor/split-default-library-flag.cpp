// Question: does standard VC6 /Zl on either real DIMMER split contribution
// omit producer metadata, while preserving its code/data/relocations?
//
// Measured 2026-09-27 in matcher-1, branch matcher/bss-misc, in its verified
// persistent nix develop .#build shell. Root source and artifacts were read-only.
// Artifact directory: build/link/dimmer-split-zl/
//   run.py, run.log, results.json, audit.json
//   {prefix,suffix}-{control,Zl}/{DIMMER.cpp,heroWindow.h,DIMMER.obj,compile.log}
//   The prefix directories additionally contain the copied resourceGlobals.h.
// Inputs copied verbatim from root build/link/dimmer-nested-owner/
//   split-narrow-prefix and split-narrow-suffix.
//
// Complete four-arm matrix:
//   each of the two source owners x unit_flags(BASE/DIMMER) with/without /Zl.
//   All four compiled successfully using pinned VC6 SP5.
//
// Result:
// - All four raw COFF objects contain absolute @comp.id = 0x000b2306.
// - Every non-.drectve section has identical raw bytes, characteristics,
//   semantic symbols and offsets, and complete ordered relocation sites,
//   types, symbolic targets, and raw addends between control and /Zl.
// - Prefix control .drectve is
//     -defaultlib:LIBCMT -defaultlib:OLDNAMES
//   Prefix /Zl omits this now-empty section entirely. Later section indices
//   decrease by one; the audit ignores only this ordinal renumbering.
// - Suffix control .drectve is
//     -defaultlib:uuid.lib -defaultlib:uuid.lib -defaultlib:libcpmt
//     -defaultlib:LIBCMT -defaultlib:OLDNAMES
//   Suffix /Zl retains both uuid.lib directives and libcpmt, omitting only
//   the compiler defaults LIBCMT and OLDNAMES.
//
// Disposition: negative. /Zl does not remove compiler producer identity and
// therefore does not supply the needed input-count explanation. A full native
// link was not repeated because the requested metadata premise failed. No
// source, build flag, compiler, or COFF change is retained.
