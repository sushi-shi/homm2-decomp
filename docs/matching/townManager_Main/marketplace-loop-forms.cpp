// Rejected structural matrix for the first remaining marketplace-loop jump.
//
// Removing either the loop-body or case-body braces was byte-identical.
// Inverting the for bound was also byte-identical. Rewriting the for as a while
// removed five bytes but changed the loop-head structure and regressed clean
// state from 99.327360% to 99.154724%; its first divergence moved earlier, so
// it was rejected rather than retained as the lower structural branch.
// Matrices:
//   build/match-variants/townmain-marketplace-loop-scope-compile-20260727
//   build/match-variants/townmain-marketplace-case-scope-compile-20260727
//   build/match-variants/townmain-marketplace-loop-form-compile-20260727

index_i = 0;
while (index_i < gpCurPlayer->m_townCount) {
    if (gpGame->GetTown(gpCurPlayer->TownId(index_i))->m_buildings
        & IDX(TOWN_BUILDING_MARKETPLACE))
        ++marketplaceCount_m;
    ++index_i;
}
