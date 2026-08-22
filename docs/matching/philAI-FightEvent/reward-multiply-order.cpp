/*
Function: philAI::FightEvent
RVA: 0x000884f1
Source: src/SOURCE/PHILAI.cpp

Measured matrix
---------------
Artifact manifest: build/fight-event-operand-order-manifest.json
Result artifacts: build/source-variant-batch/fight-event-operand-order/results.json

The complete matrix contained one reviewed exact-span axis and no AST or TU-state
dimensions (2/2 variants compiled in 2.50 seconds):

    resource_value_first:
        gafAITurnCostResource[IDX(RES_GOLD)] * IDX(FIGHT_EVENT_REWARD_500)

    reward_amount_first:
        IDX(FIGHT_EVENT_REWARD_500) * gafAITurnCostResource[IDX(RES_GOLD)]

The axis changed all twelve instances together because the retail disassembly
showed the same first divergence at every reward case: retail loaded the reward
float before multiplying by gafAITurnCostResource. The amount-first arm improved
the disposable raw-object score from 99.460785% to 99.607840% and restored the
68/68-block retail topology. The remaining raw-object delta was anonymous float
pool identity and resolved switch-table entries, not function-body code.

Retained disposition
--------------------
Retain the amount-first source shape. The five emitted float values were bound
to the verified retail .rdata cells with DATA_COMPGEN:

    500.0f  -> VA 0x004ea6a0
    1000.0f -> VA 0x004ea7a4
    2000.0f -> VA 0x004ea79c
    3000.0f -> VA 0x004ea7a0
    5000.0f -> VA 0x004ea798

After `homm2 redelink` and `homm2 build`, the canonical object is exact at the
retail size (0x623 bytes) with all 102 relocation owners present. The compact
instruction diff has no executable-body difference; its post-return listing is
the three relocated switch tables being decoded as instructions.
*/
