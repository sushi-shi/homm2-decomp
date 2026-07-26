/*
 * DoBlur structural experiment: one contiguous sixteen-element sample array.
 *
 * Reviewed source shape:
 *
 *     i32 samples[KERNEL_SAMPLE_COUNT];
 *     u8* input = destination->m_pixels + rowOffset + BORDER_RADIUS;
 *     i32 remaining = INTERIOR_COLUMN_COUNT;
 *     u8* output = source->m_pixels + rowOffset + BORDER_RADIUS;
 *
 *     samples[0]  = ...;
 *     ...
 *     samples[13] = static_cast<u32>(input[-BORDER_RADIUS])
 *                   << COMPONENT_INDEX_SHIFT;
 *     samples[14] = static_cast<u32>(input[-SCREEN_WIDTH])
 *                   << COMPONENT_INDEX_SHIFT;
 *     samples[15] = static_cast<u32>(input[SCREEN_WIDTH * BORDER_RADIUS])
 *                   << COMPONENT_INDEX_SHIFT;
 *
 * All RGB accumulation sites use samples[0] through samples[15].
 *
 * Result (2026-07-26):
 *   clean fuzzy: 91.497890%
 *   best of 50 target-adjacent forest states: 92.141350% (trial 27)
 *   retail/candidate size: 1700/1703 bytes
 *   ordered relocations: 43/43, identical identities
 *   CFG: the same 29-block family
 *   frame: retail 0xc5c, candidate 0xc68
 *
 * Artifacts:
 *   build/tu-state-noise/doblur-all16-target-50
 *   build/doblur-all16-target-50-states.json
 *
 * Disposition: rejected.  The all-array form creates three extra spilled
 * dwords, whereas retail keeps samples 13, 14, and 15 in registers.  Its
 * bounded island maximum also remains below the unchanged-source historical
 * MAX of 92.4346%.
 */
