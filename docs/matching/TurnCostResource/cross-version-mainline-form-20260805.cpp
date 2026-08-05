/*
 * Cross-version source matrix for philAI::TurnCostResource.
 *
 * Target: SOURCE/PHILAI, PoL RVA 0x0003fb8c.
 * Source donor: the exact decomp-gold-2.1-buka body at VA 0x00485740.
 * Matrix: build/turn-cost-cross-version-manifest.json
 * Results: build/source-variant-batch/turn-cost-cross-version/results.json
 *
 * Complete product: two resource-value product orders x two final-ratio
 * orders x the clean state plus 50 forest/top states = 204/204 variants.
 * No truncation occurred.
 *
 * The prior source computed
 *     (fraction / 2 + 0.5) / gResourceBaseValue
 * while both the Buka exact source and PoL retail's fdivrp compute
 *     gResourceBaseValue / (fraction / 2 + 0.5).
 * This is a semantic reconstruction bug, not compiler steering.
 *
 * Correcting the ratio raised the full matrix's best state to 99.863640%,
 * retail size 384 and exact 19/19 topology. That mixed arm did not have an
 * exact ordered relocation stream.
 * The base-first and sum-first multiplication arms were byte-identical in
 * every retained best state under VC4.2, so the Buka/mainline base-first arm
 * is retained as the stronger source witness.
 *
 * Focused retained-source replay:
 *   build/tu-state-noise/turn-cost-cross-version-best
 *   seed 1213156658, forest/top trial 1, tag 484f4d32-0001-76e1c088
 *   source hash 7ff4172edb65, score 100.0000%, size 384/384,
 *   byte delta 0, exact 19/19 CFG, complete ordered 16/16 relocations.
 *
 * Disposition: semantic correction retained; audited exact MAX banked.
 */

void philAI::TurnCostResource(i32 player) {
    i32 nb;
    playerAIData* kn;
    float jb[AI_PURCHASE_RESOURCE_COUNT];
    float idx;
    i32 total;
    i32 cost[AI_PURCHASE_RESOURCE_COUNT];
    kn = &gpGame->m_players[player].m_aiData;
    total = 0;
    for (nb = 0; nb < AI_PURCHASE_RESOURCE_COUNT; nb++) {
        cost[nb] = (i32)(
            (double)gResourceBaseValue[nb]
            * ((double)(kn->m_income[nb] * 5) * 0.7
               + (double)gpGame->m_players[player].m_resources[nb])
        );
        total += cost[nb];
    }
    idx = (float)(total / AI_PURCHASE_RESOURCE_COUNT);
    for (nb = 0; nb < AI_PURCHASE_RESOURCE_COUNT; nb++) {
        jb[nb] = (float)cost[nb] / idx;
        gafAITurnCostResource[nb] =
            (float)(gResourceBaseValue[nb] / (jb[nb] / 2.0f + 0.5));
    }
}
