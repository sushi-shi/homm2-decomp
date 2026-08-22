/*
Function: InitAIMapVars
RVA: 0x0008aee1
Source: src/SOURCE/PHILAI.cpp

Measured matrix
---------------
Manifest: build/init-ai-map-vars-operand-order-manifest.json
Results: build/source-variant-batch/init-ai-map-vars-operand-order/results.json

The complete five-axis product independently tested `MAP_WIDTH * MAP_HEIGHT`
against `MAP_HEIGHT * MAP_WIDTH` for the three i16 allocations and the two i8
allocations. All 32 variants compiled in 30.00 seconds with no AST or TU-state
dimension. Each height-first site removed exactly one fifth of the residual:
the all-width-first baseline was 99.038460%, every four-of-five height-first arm
was 99.807690%, and only the all-height-first arm reached 100.000000%.

Retain `MAP_HEIGHT * MAP_WIDTH` at all five allocation sites. The selected arm
is exact at the retail size (0xc5 bytes), with its single block exact and all
23 relocation owners present.
*/
