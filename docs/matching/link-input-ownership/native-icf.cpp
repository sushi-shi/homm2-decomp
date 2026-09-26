// Native identical-COMDAT-folding audit, 2026-09-26.
// Raw project/compiler/library inputs are the current correction-free link
// inputs after REQUEST BSS recovery. No object or output modification occurs.
//
// Reproducer and artifacts:
//   build/native-icf-audit/probe.py
//   build/native-icf-audit/results.json
//   build/native-icf-audit/{explicit_noicf,explicit_icf}/
// Each arm uses final_inputs(ninja_link_args(), include_resources=True),
// link_prefix(...), and the same four native historical LINK/PDB invocations.
// The only independent change is an added /OPT:NOICF or /OPT:ICF option.
// Both arms completed all four links and retain response files, logs, EXE/MAP.
//
// Explicit NOICF reproduces the production executable exactly:
//   size 1208393; 500 differing bytes from retail;
//   SHA256 c0eacb837c0d8bdec918962df5bce11270d05247908f4be34badd4015255a1b8.
// Explicit ICF produces:
//   size 1208393; 143115 differing bytes from retail;
//   SHA256 d0d21a60c27e20598502c0887a2ddb46384fe79e23084e85aaa8b9a0014c1f1b.
// Equal file size does not establish equal placement: folding changes code
// addresses while section/file alignment retains the same overall size.
//
// Untouched native MAP destructor destinations:
//                                 NOICF         ICF
// RefPtr<AudioDevice>             0x004b6b00    0x004b6b00
// AudiereSampleNode               0x004ccf70    0x004cce60
// RefPtr<OutputStream>            0x004ccfa0    0x004cce60
// RefPtr<SampleSource>            0x004cda50    0x004cce60
//
// Retail independently distinguishes these semantic destinations:
// RefPtr<AudioDevice> 0x004b6b00; RefPtr<OutputStream> 0x004ccf70;
// AudiereSampleNode 0x004cd050; RefPtr<SampleSource> 0x004cda50.
// See ../Audiere-helper-identities/typed-unwind-ownership.cpp for the complete
// typed call/cleanup proof, which corrects former body-only identity claims.
//
// Disposition: enabling native global ICF on these raw source objects is not
// a solution. It coalesces three independently required retail destinations.
// This rejects the measured global-link flag, not every possible original
// source model with different COMDAT eligibility. No flag is retained.
