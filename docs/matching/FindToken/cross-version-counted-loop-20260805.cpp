/*
 * Cross-version reconstruction for FindToken, BASE/Misc RVA 0x000c48c0.
 * Donor: exact Gold/Buka counted for-loop with *(text + i) access.
 *
 * The donor replaces the expanded guarded do/while reconstruction. A clean
 * plus 50 forest/top census found three legitimate VC4.2 states and raised
 * current-hash MAX from 97.2414% to 99.6552% at trial 2:
 *
 *   build/find-token-cross-version-states.json
 *   build/tu-state-noise/find-token-cross-version
 *
 * The best state has retail size 49, exact 5/5 topology, no relocations, and
 * one raw-byte difference: the equivalent SIB encoding (%esi,%eax) versus
 * (%eax,%esi). A complete 2 x 51 matrix proved *(text + i) and *(i + text)
 * byte-identical in every state:
 *
 *   build/find-token-order-manifest.json
 *   build/source-variant-batch/find-token-order/results.json
 *
 * Disposition: retain the exact donor loop and pointer ownership; preserve
 * trial 2 as paired clue evidence. The one-byte compiler encoding residual
 * remains live.
 *
 * A fresh seed-20260805 expansion completed 50 trials and reproduced the same
 * one-byte 99.655174% state at trials 7, 17, 29, and 45. A second 50-state
 * seed-20260806 layer over trial 7 again emitted only the same three known
 * target encodings and no audited exact closure:
 *
 *   build/find-token-seed-20260805-state-summary.json
 *   build/tu-state-noise/find-token-seed-20260805/
 *   build/find-token-layer2-seed-20260806-state-summary.json
 *   build/tu-state-noise/find-token-layer2-seed-20260806/
 */

char* FindToken(char* text, char token) {
    i32 len = strlen(text);
    for (i32 i = 0; i < len; ++i) {
        if (*(text + i) == token)
            return text + i;
    }
    return NULL;
}
