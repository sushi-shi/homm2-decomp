// C28: three independent source matrices, run against the unchanged KB parent.
// build/c28-{shutdown,wait,icons}-{axes,manifest}.json and -batch/results.json.
// Each site: C-style / static_cast / implicit public-derived-to-base conversion.
// Products 27/27, 3/3, 9/9; no truncation; all text bytes identical per target.
// Retained implicit arms: complete native KB comparison passes; all three
// targets additionally pass independent genuine-retail bytes/sites/targets.
// Full proof and private-relocation caveats: docs/reconstruction/C28-C33.md.

// For each E in ShutDown {gPalette, bigFont, smallFont}, WaitEndSample {*s},
// and UnloadSystemwideIcons {gBuyBuildIcons, gSystemIcons}:
gpResourceManager->Dispose((resource*)E);                 // measured baseline
gpResourceManager->Dispose(static_cast<resource*>(E));    // measured alternative
gpResourceManager->Dispose(E);                           // retained
