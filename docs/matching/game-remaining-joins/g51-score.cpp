// Parent e6bc8724; 3 complete arms in 4.586455795913935 seconds.
// Selected trial 0: {'source_shape': 'original', 'candidate': 'baseline'}; size/text/label-sensitive relocation signature (375, '8b90fbc83c273a17', 'aa91a587d7dca816').
// Source restored; all arms compiled; no truncation, AST mutation or TU-state census.
// Selected combined source passes all 98 native objects and full focused retail checks.
// Monster cast signatures contain counter-named local labels: only the selected arm has full resolved-target proof.
// Artifacts: build/g51-score-manifest.json; build/source-variant-batch/g51-score/results.json.
// See docs/reconstruction/C34-C39-G49-G51-G54.md for results and disposition.
/* Baseline:
i32 CalcBaseScore(i32 days) {
    i32 score = SCORE_BASE;

    if (gpGame->m_mapHeader.width == MAP_DIMENSION_XLARGE)
        days = static_cast<i32>(days * GAME_SCORE_EXTRA_LARGE_DAY_SCALE);
    else if (gpGame->m_mapHeader.width == MAP_DIMENSION_LARGE)
        days = static_cast<i32>(days * GAME_SCORE_LARGE_DAY_SCALE);
    else if (gpGame->m_mapHeader.width == MAP_DIMENSION_MEDIUM)
        days = static_cast<i32>(days * GAME_SCORE_MEDIUM_DAY_SCALE);
    else if (gpGame->m_mapHeader.width == MAP_DIMENSION_SMALL)
        days = static_cast<i32>(days * GAME_SCORE_SMALL_DAY_SCALE);

    if (days <= SCORE_FIRST_TIER) {
        score -= days;
        goto clampScore;
    } else
        score -= SCORE_FIRST_TIER;
    if (days <= SCORE_SECOND_TIER) {
        score = static_cast<i32>(
            score - (days - SCORE_FIRST_TIER) * SCORE_SECOND_TIER_FACTOR
        );
        goto clampScore;
    } else
        score = static_cast<i32>(score - SCORE_SECOND_TIER_BASE_DEDUCTION);
    if (days <= SCORE_THIRD_TIER) {
        score = static_cast<i32>(
            score - (days - SCORE_SECOND_TIER) * SCORE_THIRD_TIER_FACTOR
        );
        goto clampScore;
    } else
        score = static_cast<i32>(score - SCORE_THIRD_TIER_BASE_DEDUCTION);
    score = static_cast<i32>(
        score - (days - SCORE_THIRD_TIER) * SCORE_FINAL_TIER_FACTOR
    );

clampScore:
    if (score < SCORE_MINIMUM)
        score = SCORE_MINIMUM;
    return score;
}

Option original:

Option scoped_breaks:
--- baseline
+++ scoped_breaks
@@ -10,30 +10,31 @@
     else if (gpGame->m_mapHeader.width == MAP_DIMENSION_SMALL)
         days = static_cast<i32>(days * GAME_SCORE_SMALL_DAY_SCALE);

-    if (days <= SCORE_FIRST_TIER) {
-        score -= days;
-        goto clampScore;
-    } else
-        score -= SCORE_FIRST_TIER;
-    if (days <= SCORE_SECOND_TIER) {
+    do {
+        if (days <= SCORE_FIRST_TIER) {
+            score -= days;
+            break;
+        } else
+            score -= SCORE_FIRST_TIER;
+        if (days <= SCORE_SECOND_TIER) {
+            score = static_cast<i32>(
+                score - (days - SCORE_FIRST_TIER) * SCORE_SECOND_TIER_FACTOR
+            );
+            break;
+        } else
+            score = static_cast<i32>(score - SCORE_SECOND_TIER_BASE_DEDUCTION);
+        if (days <= SCORE_THIRD_TIER) {
+            score = static_cast<i32>(
+                score - (days - SCORE_SECOND_TIER) * SCORE_THIRD_TIER_FACTOR
+            );
+            break;
+        } else
+            score = static_cast<i32>(score - SCORE_THIRD_TIER_BASE_DEDUCTION);
         score = static_cast<i32>(
-            score - (days - SCORE_FIRST_TIER) * SCORE_SECOND_TIER_FACTOR
+            score - (days - SCORE_THIRD_TIER) * SCORE_FINAL_TIER_FACTOR
         );
-        goto clampScore;
-    } else
-        score = static_cast<i32>(score - SCORE_SECOND_TIER_BASE_DEDUCTION);
-    if (days <= SCORE_THIRD_TIER) {
-        score = static_cast<i32>(
-            score - (days - SCORE_SECOND_TIER) * SCORE_THIRD_TIER_FACTOR
-        );
-        goto clampScore;
-    } else
-        score = static_cast<i32>(score - SCORE_THIRD_TIER_BASE_DEDUCTION);
-    score = static_cast<i32>(
-        score - (days - SCORE_THIRD_TIER) * SCORE_FINAL_TIER_FACTOR
-    );
+    } while (0);

-clampScore:
     if (score < SCORE_MINIMUM)
         score = SCORE_MINIMUM;
     return score;
Option nested_tiers:
--- baseline
+++ nested_tiers
@@ -12,28 +12,27 @@

     if (days <= SCORE_FIRST_TIER) {
         score -= days;
-        goto clampScore;
-    } else
+    } else {
         score -= SCORE_FIRST_TIER;
-    if (days <= SCORE_SECOND_TIER) {
-        score = static_cast<i32>(
-            score - (days - SCORE_FIRST_TIER) * SCORE_SECOND_TIER_FACTOR
-        );
-        goto clampScore;
-    } else
-        score = static_cast<i32>(score - SCORE_SECOND_TIER_BASE_DEDUCTION);
-    if (days <= SCORE_THIRD_TIER) {
-        score = static_cast<i32>(
-            score - (days - SCORE_SECOND_TIER) * SCORE_THIRD_TIER_FACTOR
-        );
-        goto clampScore;
-    } else
-        score = static_cast<i32>(score - SCORE_THIRD_TIER_BASE_DEDUCTION);
-    score = static_cast<i32>(
-        score - (days - SCORE_THIRD_TIER) * SCORE_FINAL_TIER_FACTOR
-    );
+        if (days <= SCORE_SECOND_TIER) {
+            score = static_cast<i32>(
+                score - (days - SCORE_FIRST_TIER) * SCORE_SECOND_TIER_FACTOR
+            );
+        } else {
+            score = static_cast<i32>(score - SCORE_SECOND_TIER_BASE_DEDUCTION);
+            if (days <= SCORE_THIRD_TIER) {
+                score = static_cast<i32>(
+                    score - (days - SCORE_SECOND_TIER) * SCORE_THIRD_TIER_FACTOR
+                );
+            } else {
+                score = static_cast<i32>(score - SCORE_THIRD_TIER_BASE_DEDUCTION);
+                score = static_cast<i32>(
+                    score - (days - SCORE_THIRD_TIER) * SCORE_FINAL_TIER_FACTOR
+                );
+            }
+        }
+    }

-clampScore:
     if (score < SCORE_MINIMUM)
         score = SCORE_MINIMUM;
     return score;
*/
