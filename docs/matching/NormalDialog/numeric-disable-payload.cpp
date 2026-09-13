// C33: build/c33-axes.json, c33-index-manifest.json, c33-index-batch/results.json.
// Complete 3/3 source matrix, 5.80 s, restored, no TU-state probes/truncation.
// Prior 2/2 pointer/bare-integer matrix was byte-neutral, but the latter needs
// IDX for modern strict enums; the complete family was rerun including IDX.
// Both 5975 bytes and 252 relocations; same complete text hash, 99.929825 metric.
// Whole KB before/after allocated-section/ordered-relocation comparison passes.
// Independent retail ordinary bytes pass, but EH identities/DIR32 still fail;
// do not claim whole-function retail closure. See reconstruction/C28-C33.md.

// Baseline:
message_b.payload.widget.data.text = reinterpret_cast<char*>(NORMAL_DIALOG_DISABLE_COMMAND);
// Measured but rejected: modern strict enums require explicit numeric extraction.
message_b.payload.widget.data.value = NORMAL_DIALOG_DISABLE_COMMAND;
// Retained: CLEAR_FLAGS consumes the existing numeric union arm.
message_b.payload.widget.data.value = IDX(NORMAL_DIALOG_DISABLE_COMMAND);
