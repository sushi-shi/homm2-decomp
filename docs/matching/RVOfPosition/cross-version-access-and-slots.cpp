/*
philAI::RVOfPosition cross-version audit, 2026-08-24.

PoL:  RVA 0x3e918, CodeView size 1581.
Buka: RVA 0x84642, size 0x536, exact.

REJECTED BUKA ACCESSOR

    gpSearchArray->GetRow(x, 1)[MAP_WIDTH * y].distance

PoL retains:

    gpSearchArray->GetRow(y, MAP_WIDTH)[x].distance

Both address the same search node.  With seed 1213156658, 50 complete forest/top
states give:

    PoL accessor: clean 99.611760%; best trial 8 99.929410%, 1581 bytes,
                  exact 77-block CFG.
    Buka accessor: clean 98.152940%; best trial 8 98.291760%, 1582 bytes,
                   one size-only block.

RETAINED PoL SLOT NAMES

    targetLiveChance2   bucket 10  target -0x1c
    currentLiveChance26 bucket 11  target -0x20
    objectType13        bucket 12  target -0x24
    strategicDelta6     bucket 13  target -0x28

The previous four names formed a -0x1c/-0x20/-0x24/-0x28 cycle.  A complete
102-cell current/solved-name x clean/50-state matrix selects the solved layout.
An unchanged-source replay of forest/top trial 27 reaches 99.990585%, retail
size, exact 77/77 CFG, seven raw byte differences, and a complete ordered 56/56
relocation stream.

BOUNDED RESIDUAL

The remaining four non-relocation bytes are two commutative evaluation-order
pairs.  A complete 306-cell matrix tested:

    strategicLiveChance * adjacentMonsterEventChance3
    adjacentMonsterEventChance3 * strategicLiveChance

and these positive-total orders:

    strategicDelta6 + totalValue8 + eventValue9
    totalValue8 + eventValue9 + strategicDelta6       // Buka order
    strategicDelta6 + eventValue9 + totalValue8

All six arms are byte/relocation-identical within every one of 51 states.  Three
other raw differences are relocation-masked fields; ordered relocation identity
already passes.  No exact closure.

ARTIFACTS

    build/rv-of-position-state-summary.json
    build/tu-state-noise/rv-of-position-cross-version/
    build/rv-of-position-buka-access-state-summary.json
    build/tu-state-noise/rv-of-position-buka-access/
    build/rv-of-position-slot-manifest.json
    build/source-variant-batch/rv-of-position-slot/results.json
    build/rv-of-position-retained-slot-replay-summary.json
    build/tu-state-noise/rv-of-position-retained-slot-replay/
    build/rv-of-position-residual-manifest.json
    build/source-variant-batch/rv-of-position-residual/results.json

DISPOSITION

Do not port Buka's search-array spelling or VC6 local names to PoL.  Retain the
four retail-slot PoL names above.  Keep the current arithmetic spelling; Buka's
sum order is proven neutral in the surviving VC4.2 orbit.
*/
