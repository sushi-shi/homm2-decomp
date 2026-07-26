/*
 * DoBlur structural matrix: input/output pointer construction.
 *
 * Each pointer independently used these reviewed source arms:
 *
 *     u8* input = destination->m_pixels + rowOffset + BORDER_RADIUS;
 *
 *     u8* input = destination->m_pixels;
 *     input += rowOffset + BORDER_RADIUS;
 *
 *     u8* input = destination->m_pixels + BORDER_RADIUS;
 *     input += rowOffset;
 *
 * The same direct/base-then-offset/border-then-row arms were applied to
 * `output`, giving a complete 3 x 3 matrix.
 *
 * Result (2026-07-26):
 *   all nine arms compile to the same 90.489450% function state
 *   size: 1707 bytes
 *   ordered relocations: 43/43
 *
 * Artifacts:
 *   build/doblur-pointer-shapes-axes.json
 *   build/doblur-pointer-shapes-manifest.json
 *   build/match-variants/doblur-pointer-shapes-clean
 *
 * Disposition: rejected as a steering axis.  MSVC 4.2 /O2 canonicalizes all
 * nine spellings before allocation, so the split statements expose no new
 * structural orbit to cross with islands.
 */
