// Structural attempt: express viewed-hero selection as a conditional expression.
//
// The if/else placed an unconditional jump before the true arm; retail places
// the equivalent jump after it. This conditional expression closes both jump
// differences and raises clean state to 99.426865%.
// Matrix: build/match-variants/townmain-viewed-hero-ternary-20260727

viewedHero = m_selectedStrip == m_heroStrip
    ? gpGame->GetHero(m_town->m_occupyingHeroId)
    : NULL;
