/*
 * DoBlur exact-frame structural matrix: materialize pointers or references to
 * the final three red component-table entries.
 *
 * Representative pointer and reference arms:
 *
 *     i32* redWestFour = &BLUR_COMPONENT(redComponents, sample13);
 *     i32& redNorthOne = BLUR_COMPONENT(redComponents, sample14);
 *     redSum += *redWestFour;
 *     redSum += redNorthOne;
 *
 * Direct access, all six pointer acquisition orders, and all six reference
 * acquisition orders were crossed with the clean state plus 50 TU states.
 *
 * Result (2026-07-27):
 *   complete matrix: 13 x 51 = 663/663
 *   register-perfect trial 40: all 13 arms are the identical 93.708860%,
 *     1705-byte target state with 43/43 relocations
 *   every pointer/reference order at trial 9 is the same 92.607600%,
 *     1702-byte state observed for copied values
 *
 * MSVC eliminates the pointer/reference ownership distinction completely on
 * the useful island.  The only distinct shorter island has input=ESI and
 * north1/south4=ECX/EDX, so it fails the retail sample-prefix discriminator.
 *
 * Artifacts:
 *   build/doblur-exact-frame-red-final-entry-owners-axes.json
 *   build/doblur-exact-frame-red-final-entry-owners-manifest.json
 *   build/match-variants/doblur-exact-frame-red-final-entry-owners
 *
 * Disposition: rejected; no new register-perfect red-tail schedule.
 */
