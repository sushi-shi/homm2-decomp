// VC6 SP5 REQUEST source family c73-request-update-numeric; parent13e6af40.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/c73-request-update-numeric/results.json; RVA 0x90ab6
// Complete 4 arms; elapsed 2.725721s; input restored.
// Disposition: retain all scalar removals in this family (nine-character scratch for S66); combined all98 native proof.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 2718 / 7ee6c6bc25f6fda0 / e89cb2a46eab0aa7 / 60:60 / 60,0,0,0 / True
// 1 / {'source_family': 'number1', 'candidate': 'baseline'} / 2718 / 7ee6c6bc25f6fda0 / e89cb2a46eab0aa7 / 60:60 / 60,0,0,0 / True
// 2 / {'source_family': 'number0', 'candidate': 'baseline'} / 2718 / 7ee6c6bc25f6fda0 / e89cb2a46eab0aa7 / 60:60 / 60,0,0,0 / True
// 3 / {'source_family': 'number0_number1', 'candidate': 'baseline'} / 2718 / 7ee6c6bc25f6fda0 / e89cb2a46eab0aa7 / 60:60 / 60,0,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// |         m_scrollKnob->m_y =
// |             static_cast<i16>(
// |                 fGutterTravelLength
// |                     / IDX(SCROLL_CENTER_DIVISOR)
// |                 + fGutterMinY
// |             );
// |     } else {
// |         gutterStepCount1 = fGutterTravelLength / (m_fileCount - iMaxListSize);
// |         m_scrollKnob->m_y = static_cast<i16>(fGutterMinY + m_topIndex * gutterStepCount1);
// |     }
// ALTERNATIVE number1 (unified source delta from BEFORE):
// | --- before
// | +++ number1
// | @@ -6,5 +6,5 @@
// |              );
// |      } else {
// |          gutterStepCount1 = fGutterTravelLength / (m_fileCount - iMaxListSize);
// | -        m_scrollKnob->m_y = static_cast<i16>(fGutterMinY + m_topIndex * gutterStepCount1);
// | +        m_scrollKnob->m_y = (fGutterMinY + m_topIndex * gutterStepCount1);
// |      }
// ALTERNATIVE number0 (unified source delta from BEFORE):
// | --- before
// | +++ number0
// | @@ -1,9 +1,5 @@
// |          m_scrollKnob->m_y =
// | -            static_cast<i16>(
// | -                fGutterTravelLength
// | -                    / IDX(SCROLL_CENTER_DIVISOR)
// | -                + fGutterMinY
// | -            );
// | +            (fGutterTravelLength / IDX(SCROLL_CENTER_DIVISOR) + fGutterMinY);
// |      } else {
// |          gutterStepCount1 = fGutterTravelLength / (m_fileCount - iMaxListSize);
// |          m_scrollKnob->m_y = static_cast<i16>(fGutterMinY + m_topIndex * gutterStepCount1);
// ALTERNATIVE number0_number1 (unified source delta from BEFORE):
// | --- before
// | +++ number0_number1
// | @@ -1,10 +1,6 @@
// |          m_scrollKnob->m_y =
// | -            static_cast<i16>(
// | -                fGutterTravelLength
// | -                    / IDX(SCROLL_CENTER_DIVISOR)
// | -                + fGutterMinY
// | -            );
// | +            (fGutterTravelLength / IDX(SCROLL_CENTER_DIVISOR) + fGutterMinY);
// |      } else {
// |          gutterStepCount1 = fGutterTravelLength / (m_fileCount - iMaxListSize);
// | -        m_scrollKnob->m_y = static_cast<i16>(fGutterMinY + m_topIndex * gutterStepCount1);
// | +        m_scrollKnob->m_y = (fGutterMinY + m_topIndex * gutterStepCount1);
// |      }
