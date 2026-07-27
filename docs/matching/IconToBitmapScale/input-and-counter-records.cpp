/*
 * IconToBitmapScale, RVA 0xd2f90: input/counter record and scope attempts.
 *
 * The retail frame is 0x18 bytes while the reconstructed 18-block family
 * uses 0x10.  These matrices tested whether source-level lifetime ownership
 * accounts for the missing slots before changing the established loop CFG.
 *
 * Input owner forms:
 */

#if 0
struct SIconScaleInputs {
    bitmap* destination;
    i32 scaledSize;
} scaleInputs = {dest, scale};

struct SIconScaleInputsReverse {
    i32 scaledSize;
    bitmap* destination;
} reverseInputs = {scale, dest};
#endif

/*
 * Aggregate initialization in both field orders and separate assignment were
 * swept over clean + 50 forest states:
 *
 *   build/icon2bs-input-owner-record-axes-20260727.json
 *   build/match-variants/icon2bs-input-owner-record-20260727/
 *
 * All three record forms are byte-identical within a state.  Their clean
 * object immediately reaches the retained 85.829790% island at size 365,
 * versus direct parameters at 75.695040%/361.  Destination moves into ESI,
 * but scale remains stack-owned rather than retail EBX and the frame remains
 * 0x10.  This is useful allocation evidence, not the missing source shape.
 *
 * Moving the existing rows/cols declarations to function, slow-path, or
 * copy-loop scope was also swept over 4 * (clean + 50) = 204 cells:
 *
 *   build/icon2bs-counter-scope-axes-20260727.json
 *   build/match-variants/icon2bs-counter-scope-20260727/
 *
 * All four scopes are byte-identical within a state, so declaration scope
 * alone cannot account for the retail frame.
 *
 * Finally, rows and cols were given explicit shared ownership in both field
 * orders and crossed with direct parameters versus the input record:
 */

#if 0
struct SIconScaleCounters {
    i32 rows;
    i32 cols;
} counters;

struct SIconScaleCountersReverse {
    i32 cols;
    i32 rows;
} reverseCounters;
#endif

/*
 * Complete product: 2 input owners * 3 counter owners * (clean + 50 states)
 * = 306 compilations:
 *
 *   build/icon2bs-input-counter-record-cross-axes-20260727.json
 *   build/match-variants/icon2bs-input-counter-record-cross-20260727/
 *
 * Separate counters preserve the existing 13 exact + five size-only blocks
 * and peak at 85.829790%.  Both counter-record orders form coherent but worse
 * islands: 85.496450% for rows/cols and 85.489365% for cols/rows.  They reduce
 * block alignment to five exact + three size-only and still retain the 0x10
 * frame.  Crossing either with the input record does not recover the retail
 * allocation.
 *
 * No reconstructed source was retained and MAX did not change.
 */

void IconToBitmapScale_input_and_counter_record_attempts() {
}
