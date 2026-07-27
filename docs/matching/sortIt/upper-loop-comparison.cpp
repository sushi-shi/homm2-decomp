/*
 * sortIt residual audit and upper-bound loop structure matrices.
 *
 * An unchanged-source clean-plus-50 declaration-forest census reproduced the
 * durable 99.993630% MAX at trials 10, 24, and 26.  The paired trial-10 object
 * has exact retail size (1076 bytes), 68/68 relocations, and exactly three raw
 * byte differences.  All three are one comparison at the inner update loop:
 *
 *   retail:
 *     mov eax, [upper5]     // [ebp-0x34]
 *     cmp [j], eax          // [ebp-0x2c]
 *     jg  done
 *
 *   candidate:
 *     mov eax, [j]
 *     cmp [upper5], eax
 *     jl  done
 *
 * The conditions are semantically identical.  The residual is evaluation
 * ownership, not a frame-slot, size, CFG, or relocation mismatch.
 *
 * Five equivalent counted-loop conditions were crossed with clean plus 50
 * states: `j <= upper5`, `upper5 >= j`, `!(j > upper5)`,
 * `!(upper5 < j)`, and a boolean ternary.  All five were byte-identical
 * within each state.  VC4.2 canonicalizes those expression spellings before
 * choosing the memory operand.
 *
 * Three materially different loop structures were also crossed with the same
 * states:
 *   - `for (;;)` plus an early `j > upper5` break: best 98.380040%, size 1081
 *   - top-tested while: best 98.350320%, size 1071
 *   - mirrored top-tested while: identical to the while arm
 *
 * Those structures leave the correct counted-loop CFG/size island.  A bounded
 * identifier matrix on the earlier bound `j` declaration was byte-neutral and
 * did not affect this inner comparison.
 *
 * Artifacts:
 *   build/tu-state-noise/sortit-clean-20260727
 *   build/sortit-clean-states-20260727.json
 *   build/sortit-upper-loop-condition-manifest.json
 *   build/match-variants/sortit-upper-loop-condition
 *   build/sortit-upper-loop-guard-manifest.json
 *   build/match-variants/sortit-upper-loop-guard
 *   build/sortit-upper-loop-structure-manifest.json
 *   build/match-variants/sortit-upper-loop-structure
 *   build/sortit-j-rename-manifest.json
 *   build/match-variants/sortit-j-rename
 *
 * Disposition: preserve the three-byte paired object as exact-local clue
 * evidence.  The source is restored, generated states are absent, and the
 * 99.993630% MAX remains unchanged.
 */
