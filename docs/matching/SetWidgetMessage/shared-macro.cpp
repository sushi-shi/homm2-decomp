// Readability audit H01: measured shared widget-message structures.
// Owner: include/BASE/message.h. No original macro spelling is claimed.
//
// Complete clean-state structural products, four arms per target:
//   button::Main     RVA 0xd3890: 1240-byte retail function, 22 relocations
//   textWidget::Main RVA 0xc3240:  550-byte retail function,  4 relocations
// Commands (repeat for TEXTWDGT.cpp, 0xc3240 and matching filename prefix):
//   homm2 permute src/BASE/BUTTON.cpp 0xd3890 \
//     --axes-from build/readability/h01-BUTTON-axes.json \
//     --min-depth 0 --max-depth 0 --state-trials 0 --limit 4 \
//     -o build/readability/h01-BUTTON-manifest.json --run \
//     --batch-output build/readability/h01-BUTTON-matrix
// Both products completed 4/4; no sampling or truncated state census.
// These are structural comparisons in the clean TU state, not island sweeps.
// results.csv: first three arms exact on both targets, loop arm non-exact.
// The retained comma expression preserves statement safety and store order.
// Full combined build then compared all 98 raw objects to the pre-edit copies:
// identical 1516 non-debug sections and 42433 ordered relocation records, with
// every target identity/section/offset/type retained. No generated probes retained.
//
// Loop arm adds xor/test/jne and rotates later register allocation at /Od:
//   button: 1246 bytes, 84 blocks versus retail 1240 bytes, 83 blocks
//   text:    566 bytes versus retail 550 bytes
// This repeats docs/patterns/do-while-zero-macro-loop-test.md; no new pattern.

// Arm 1: original unparenthesized sequence (exact; less safe as a shared macro).
#define RAW_SEQUENCE(messageValue, commandValue, idValue) \
    messageValue.type = MESSAGE_WIDGET; \
    messageValue.payload.widget.command = commandValue; \
    messageValue.payload.widget.id = idValue

// Arm 2: parenthesized sequence (exact; still multiple statements).
#define PARENTHESIZED_SEQUENCE(messageValue, commandValue, idValue) \
    (messageValue).type = MESSAGE_WIDGET; \
    (messageValue).payload.widget.command = (commandValue); \
    (messageValue).payload.widget.id = (idValue)

// Arm 3: RETAINED. Every supplied expression is stable at the reviewed callers.
#define COMMA_EXPRESSION(messageValue, commandValue, idValue) \
    ((messageValue).type = MESSAGE_WIDGET, \
     (messageValue).payload.widget.command = (commandValue), \
     (messageValue).payload.widget.id = (idValue))

// Arm 4: REJECTED. VC6 emits the otherwise dead loop test at /Od.
#define SCOPED_LOOP(messageValue, commandValue, idValue) \
    do { \
        (messageValue).type = MESSAGE_WIDGET; \
        (messageValue).payload.widget.command = (commandValue); \
        (messageValue).payload.widget.id = (idValue); \
    } while (0)
