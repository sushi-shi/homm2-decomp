// soundManager::MIDIIsPlaying (PoL RVA 0xd3f80)
// Buka free-function homolog RVA 0xc5b90.
//
// Reviewed complete guard family:
//   positive conjunction, CurrentMidiFile != MIDI_NO_TRACK
//   negative early return, CurrentMidiFile == MIDI_NO_TRACK
//   positive conjunction, CurrentMidiFile >= 0
//   negative early return, CurrentMidiFile < 0
// Artifact: build/source-variant-batch/midi-is-playing-guard-family/results.json
// Coverage: complete 4/4 clean-source product; source restored.
//
// Both sentinel arms are exact at 70 bytes, 7/7 CFG, and one complete
// relocation stream. Both nonnegative-domain arms are 95.000000%, 69 bytes.
// PoL adopts Buka's negative guard but retains its byte-pinned == -1 test.
// Buka's broader < 0 test is version logic; do not port it to PoL.
