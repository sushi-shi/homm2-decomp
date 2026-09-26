// Parent 9d1aeeeaf; 2 complete arms in 3.147170682437718 seconds.
// Retain trial 0: {'source_shape': 'original', 'candidate': 'baseline'}; size/text/normalized relocs (185, 'b23465c4688bc3de', '720288b567e23459').
// All arms compiled; restored; no generator/wall-time truncation, AST mutation or TU-state census.
// Full artifacts: build/g52-map-size-manifest.json; build/source-variant-batch/g52-map-size/results.json.
// Each option below is an exact unified diff from the single baseline, including call-preserving omissions.
// See docs/reconstruction/C44-G52-G53-S43-B63.md for semantics, bounds and full verification.
/* Baseline:
void game::SetMapSize(i32 w, i32 h) {
    if (MAP_HEIGHT == h && MAP_WIDTH == w && bMapInitialized)
        goto mapSized;
    {
        bMapInitialized = true;
        MAP_WIDTH = w;
        MAP_HEIGHT = h;
        gpSearchArray->Init();
    }
mapSized:
    if (mapExtra)
        H2_FREE(mapExtra);
    mapExtra = static_cast<u8*>(H2_ALLOC(MAP_HEIGHT * MAP_WIDTH));
    memset(mapExtra, 0, MAP_HEIGHT * MAP_WIDTH);
}

Option original:

Option guard:
--- baseline
+++ guard
@@ -1,13 +1,10 @@
 void game::SetMapSize(i32 w, i32 h) {
-    if (MAP_HEIGHT == h && MAP_WIDTH == w && bMapInitialized)
-        goto mapSized;
-    {
+    if (!(MAP_HEIGHT == h && MAP_WIDTH == w && bMapInitialized)) {
         bMapInitialized = true;
         MAP_WIDTH = w;
         MAP_HEIGHT = h;
         gpSearchArray->Init();
     }
-mapSized:
     if (mapExtra)
         H2_FREE(mapExtra);
     mapExtra = static_cast<u8*>(H2_ALLOC(MAP_HEIGHT * MAP_WIDTH));
*/
