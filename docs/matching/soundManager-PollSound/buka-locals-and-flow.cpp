// soundManager::PollSound (PoL RVA 0xcd320; Buka RVA 0xb65e0)
//
// Reviewed local-family arms:
//   PoL:  volume, smp, delta, snap, now
//   Buka: musicFadeStep, volume, delta, switchDelta; smp kept last only for
//         PoL's version-specific sample-handle read
// Artifact:
//   build/source-variant-batch/poll-sound-local-family/results.json
// Coverage: complete 2/2 clean-source product; source restored.
// Result: PoL exact, 911 bytes, 46/46 CFG, ordered 37/37 relocations;
// Buka-derived shared locals 99.920560%, same size/CFG/relocation count.
// Disposition: keep PoL's compiler-specific local family.
//
// Outer-flow split (not a source-shape experiment):
//   PoL: gbNoSound/CDPoll, m_pollRequested-or-fade guard, fade-only body,
//        then clear m_pollRequested.
//   Buka: fade-zero guard, volume guard, fade-positive wrapper; no request,
//         CD-poll, debug-log, MCI-position, or sample-handle paths.
// Disposition: Buka backend rewrite; do not port to PoL.
