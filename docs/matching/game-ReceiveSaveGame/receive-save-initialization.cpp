// Parent 68ed2e25a; complete reviewed source product, no AST mutation or TU-state census.
// All 32 arms compiled in 35.03313057217747 seconds; restored, no truncation.
// Retain trial 0: {'initializations': 'retain', 'candidate': 'baseline'}.
// Selected size 1736, text fcae3c3c634cd051, normalized relocs 19a3f19dffe7fd75.
// Baseline-identical trials: [0].
// Artifacts: build/source-variant-batch/receive-save-initialization/results.json; build/receive-save-initialization-manifest.json.
// Scope, semantic review and full native/retail proof: docs/reconstruction/C44-G50-S43.md.
/* Exact source product:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x5e236",
  "axes": [
    {
      "name": "initializations",
      "find": "    ackBuffer = NULL;\n    incomingData = NULL;\n    decodedData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    received = NULL;\n    success = false;\n    oldTrack = -1;\n",
      "options": [
        {
          "name": "retain",
          "replace": "    ackBuffer = NULL;\n    incomingData = NULL;\n    decodedData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    received = NULL;\n    success = false;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_ackBuffer",
          "replace": "    incomingData = NULL;\n    decodedData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    received = NULL;\n    success = false;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_incomingData",
          "replace": "    ackBuffer = NULL;\n    decodedData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    received = NULL;\n    success = false;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_ackBuffer_incomingData",
          "replace": "    decodedData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    received = NULL;\n    success = false;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_decodedData",
          "replace": "    ackBuffer = NULL;\n    incomingData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    received = NULL;\n    success = false;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_ackBuffer_decodedData",
          "replace": "    incomingData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    received = NULL;\n    success = false;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_incomingData_decodedData",
          "replace": "    ackBuffer = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    received = NULL;\n    success = false;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_ackBuffer_incomingData_decodedData",
          "replace": "    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    received = NULL;\n    success = false;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_received",
          "replace": "    ackBuffer = NULL;\n    incomingData = NULL;\n    decodedData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    success = false;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_ackBuffer_received",
          "replace": "    incomingData = NULL;\n    decodedData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    success = false;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_incomingData_received",
          "replace": "    ackBuffer = NULL;\n    decodedData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    success = false;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_ackBuffer_incomingData_received",
          "replace": "    decodedData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    success = false;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_decodedData_received",
          "replace": "    ackBuffer = NULL;\n    incomingData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    success = false;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_ackBuffer_decodedData_received",
          "replace": "    incomingData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    success = false;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_incomingData_decodedData_received",
          "replace": "    ackBuffer = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    success = false;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_ackBuffer_incomingData_decodedData_received",
          "replace": "    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    success = false;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_success",
          "replace": "    ackBuffer = NULL;\n    incomingData = NULL;\n    decodedData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    received = NULL;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_ackBuffer_success",
          "replace": "    incomingData = NULL;\n    decodedData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    received = NULL;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_incomingData_success",
          "replace": "    ackBuffer = NULL;\n    decodedData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    received = NULL;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_ackBuffer_incomingData_success",
          "replace": "    decodedData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    received = NULL;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_decodedData_success",
          "replace": "    ackBuffer = NULL;\n    incomingData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    received = NULL;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_ackBuffer_decodedData_success",
          "replace": "    incomingData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    received = NULL;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_incomingData_decodedData_success",
          "replace": "    ackBuffer = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    received = NULL;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_ackBuffer_incomingData_decodedData_success",
          "replace": "    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    received = NULL;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_received_success",
          "replace": "    ackBuffer = NULL;\n    incomingData = NULL;\n    decodedData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_ackBuffer_received_success",
          "replace": "    incomingData = NULL;\n    decodedData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_incomingData_received_success",
          "replace": "    ackBuffer = NULL;\n    decodedData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_ackBuffer_incomingData_received_success",
          "replace": "    decodedData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_decodedData_received_success",
          "replace": "    ackBuffer = NULL;\n    incomingData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_ackBuffer_decodedData_received_success",
          "replace": "    incomingData = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_incomingData_decodedData_received_success",
          "replace": "    ackBuffer = NULL;\n    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_ackBuffer_incomingData_decodedData_received_success",
          "replace": "    packet = NULL;\n    file = 0;\n    finished = false;\n    unused2080 = 0;\n    oldTrack = -1;\n"
        }
      ]
    }
  ]
}
*/
