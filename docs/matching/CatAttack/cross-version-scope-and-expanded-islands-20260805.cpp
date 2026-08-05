/*
CatAttack, retail RVA 0x0009311f, size 0x100e.

This file records rejected cross-version source experiments. It is evidence,
not compiled source. The effective source in src/SOURCE/CMBTMGR.cpp was restored
unchanged after every generated candidate.

Later-source adjacent-hex scope
-------------------------------

The exact Heroes II 2.1 descendant declares the catapult miss-loop temporary
before the loop and assigns it in the loop body:

    i32 adjacentHex;
    for (frame18 = 0; frame18 < COMBAT_CATAPULT_DIRECTION_COUNT; frame18++) {
        adjacentHex = GetAdjacentCellIndexNoArmy(...);
        // ...
    }

The PoL reconstruction instead declares and initializes the temporary in the
loop body:

    for (frame18 = 0; frame18 < COMBAT_CATAPULT_DIRECTION_COUNT; frame18++) {
        i32 adjacentHex = GetAdjacentCellIndexNoArmy(...);
        // ...
    }

Both credible structural versions were crossed with the clean state and 50
forest/top states (102/102 candidates compiled):

  build/catattack-cross-version-adjacent-scope-axes.json
  build/catattack-cross-version-adjacent-scope-manifest.json
  build/source-variant-batch/catattack-cross-version-adjacent-scope/results.json

The PoL loop-local arm won at 99.978330% in the standalone matrix scorer. It
retained the retail size, 150/150 relocations, and the exact 173-block graph.
The later predeclared arm was lower in the same state census. Therefore the
later declaration scope is not a useful parent for this version and was not
retained.

Expanded unchanged-source island census
---------------------------------------

Because the later source otherwise confirms the present high-level structure,
the unchanged PoL source received a fresh 200-trial forest/top census with seed
1128350770:

  build/catattack-cross-version-aligned-expanded-state-summary.json
  build/tu-state-noise/catattack-cross-version-aligned-expanded/manifest.json

The 200 generated trials collapsed to seven distinct target states. The best
state scored 99.995440%, preserved the exact 173-block graph, and did not beat
the durable 99.9977% MAX. No exact byte/relocation island appeared.

Disposition
-----------

No source or MAX change was retained. The later scope difference and a larger
unchanged-source island search are exhausted for the current structural parent.
Further CatAttack work needs a new retail-evidenced parent capable of selecting
the missing compiler-state bits; repeating broad state grinding is low value.
*/
