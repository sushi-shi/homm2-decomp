/*
Function: game::SetMapSize
RVA: 0x0005f1e3
Source: src/SOURCE/GAME.cpp

Measured matrix
---------------
Manifest: build/set-map-size-operand-order-manifest.json
Results: build/source-variant-batch/set-map-size-operand-order/results.json

The complete 2 x 2 matrix independently tested the allocation and memset area
expressions with no AST or TU-state dimensions. All four variants compiled in
5.20 seconds:

    allocation width-first, clear width-first:   99.600000%
    allocation width-first, clear height-first:  99.800000%
    allocation height-first, clear width-first:  99.800000%
    allocation height-first, clear height-first: 100.000000%

Retain `MAP_HEIGHT * MAP_WIDTH` at both sites. The selected arm is exact at the
retail size (0xb9 bytes), with 10/10 exact blocks and 19/19 relocation owners.
*/
