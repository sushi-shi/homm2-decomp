/*
SmackManagerMain cross-version boundary

PoL target:  RVA 0x126d, 0x118c bytes, 180 blocks, 334 relocations.
Buka target: RVA 0x94769, 0x117d bytes, 207 blocks, 334 relocations.

Do not transfer Buka gameplay/platform policy to PoL:

    // PoL-only low-memory policy
    if (gbLowMemory && bSmackNum == LOW_MEMORY_MOVIE && !gConfig.slowVideo)
        preloadFlags26 = 0;

    // Buka-only backend lifecycle
    gpSoundManager->SaveBackend();
    ... StartupMilesBackend / master-volume override ...
    gpSoundManager->RestoreBackend();

Other Buka-only structure: BUKA_CREDITS sound handling, CD-root movie paths,
and removal of PoL's network movie path and IVYPOL palette extraction.

The portable matching frontier is only the two compare orientations documented
in compare-orientation-forest-matrix.cpp. Two complete 204-cell products tested
all four arms against clean + 50 forest states (default and seed 777); none
closed. Current clean evidence is retail size, exact 180/180 block topology,
ordered 334/334 relocations, and only the two global/local compare-load islands.

DISPOSITION: retain PoL version logic and source. Record the platform/feature
changes as Buka-only; keep the two comparison islands as a measured VC4.2
compiler-state residual.
*/
