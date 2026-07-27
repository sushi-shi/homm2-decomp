/*
 * FlipDimIconToBitmap persistent decoder-cursor matrix, 2026-07-27.
 *
 * Earlier decoder axes kept gFDSrc as the cursor owner.  This experiment
 * instead kept a local `u8* src` alive across the decoder loop and tested:
 *
 *     i32 cmd = ReadIconRleByte(src); gFDSrc = src;
 *     i32 cmd = *src++;               gFDSrc = src;
 *     src++; gFDSrc = src; i32 cmd = src[-1];
 *
 * The complete product of those three structural forms and the clean plus 50
 * forest states compiled 153/153.  The run was not wall-time truncated and
 * restored the source.
 *
 * The strongest island used the accessor form at forest trial 34:
 * 80.753010%, exact retail size 571, 31/31 blocks with 25 exact and six
 * size-only bodies.  It had only 36/37 relocations, however.  All three local
 * cursor forms moved cursor publication away from a required retail
 * relocation site, so exact size is not sufficient evidence for retention.
 *
 * Artifacts:
 *   build/icondf2b-local-cursor-publication-axes-20260727.json
 *   build/icondf2b-local-cursor-publication-manifest-20260727.json
 *   build/match-variants/icondf2b-local-cursor-publication-20260727
 *
 * Disposition: preserve the exact-size island as clue evidence, but retain no
 * live-source or MAX change.
 */

#if 0
u8* src = reinterpret_cast<u8*>(entries) + entry->srcOffset;
gFDSrc = src;

for (;;) {
    i32 cmd = ReadIconRleByte(src);
    gFDSrc = src;
    // Existing decoder body.
}
#endif
