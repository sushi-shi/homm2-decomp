// VC6 SP5 source family s70-setup-empty-dispatch; parent292b9b73.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state census is claimed.
// build/source-variant-batch/s70-setup-empty-dispatch/results.json; RVA 0x9417c
// Complete 2 arms; elapsed 1.619148s; source restored.
// Disposition: retain original; every measured nonoriginal changes function text.
// Full98 native objects/allocated sections/ordered relocations preserved after restoration.
// Nine private EH/FS site-model gaps remain explicit, not waived by native equality.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow,missing / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 231 / ce9b65960c564c02 / 92a0e2e15fd7f5c3 / 22:22 / 22,0,0,0,0 / True
// 1 / {'family': 'remove_empty_dispatch', 'candidate': 'baseline'} / 192 / d716576c368f4983 / b63c6ff0f749d914 / 17:22 / 15,0,1,1,5 / False

// AXIS family
// BEFORE (complete exact span):
// |     } else if (message.type == MESSAGE_WIDGET) {
// |         switch (message.payload.widget.command) {
// |             case WIDGET_COMMAND_DESELECT:
// |                 switch (message.payload.widget.id) {
// |                     case CHOICE_ONE:
// |                     case CHOICE_TWO:
// |                     case CHOICE_THREE:
// |                         break;
// |                 }
// |         }
// |     }
// ALTERNATIVE remove_empty_dispatch (unified delta from BEFORE):
// | --- before
// | +++ remove_empty_dispatch
// | @@ -1,11 +1 @@
// | -    } else if (message.type == MESSAGE_WIDGET) {
// | -        switch (message.payload.widget.command) {
// | -            case WIDGET_COMMAND_DESELECT:
// | -                switch (message.payload.widget.id) {
// | -                    case CHOICE_ONE:
// | -                    case CHOICE_TWO:
// | -                    case CHOICE_THREE:
// | -                        break;
// | -                }
// | -        }
// |      }
