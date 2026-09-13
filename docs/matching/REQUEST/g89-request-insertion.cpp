// VC6 SP5 REQUEST source family g89-request-insertion; parent13e6af40.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/g89-request-insertion/results.json; RVA 0x8e836
// Complete 2 arms; elapsed 1.658701s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 1827 / 0b83b728b871977f / d778ce17cfcff435 / 82:82 / 82,0,0,0 / True
// 1 / {'source_family': 'sort_break', 'candidate': 'baseline'} / 1819 / 078ce8330f1db46c / ec8f1b4d2db26dbe / 81:82 / 67,0,13,1 / False

// AXIS source_family
// BEFORE (complete exact span):
// |                     }
// |                     goto InsertName;
// |                 }
// |             }
// |         InsertName:
// |             strcpy(m_fileNames[indexData5].text, nameBuffer);
// ALTERNATIVE sort_break (unified source delta from BEFORE):
// | --- before
// | +++ sort_break
// | @@ -1,6 +1,5 @@
// |                      }
// | -                    goto InsertName;
// | +                    break;
// |                  }
// |              }
// | -        InsertName:
// |              strcpy(m_fileNames[indexData5].text, nameBuffer);
