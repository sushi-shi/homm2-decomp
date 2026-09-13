// Parent 68ed2e25a; complete reviewed source product, no AST mutation or TU-state census.
// All 32 arms compiled in 34.99541841354221 seconds; restored, no truncation.
// Retain trial 0: {'initializations': 'retain', 'candidate': 'baseline'}.
// Selected size 1971, text 8001d38acae64c9a, normalized relocs e44b6f46977541d4.
// Baseline-identical trials: [0].
// Artifacts: build/source-variant-batch/transmit-save-initialization/results.json; build/transmit-save-initialization-manifest.json.
// Scope, semantic review and full native/retail proof: docs/reconstruction/C44-G50-S43.md.
/* Exact source product:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x5da83",
  "axes": [
    {
      "name": "initializations",
      "find": "    header = NULL;\n    reply = NULL;\n    transmitData = NULL;\n    fileData = NULL;\n    success = false;\n    result = 0;\n    acknowledged = NULL;\n    oldTrack = -1;\n",
      "options": [
        {
          "name": "retain",
          "replace": "    header = NULL;\n    reply = NULL;\n    transmitData = NULL;\n    fileData = NULL;\n    success = false;\n    result = 0;\n    acknowledged = NULL;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_header",
          "replace": "    reply = NULL;\n    transmitData = NULL;\n    fileData = NULL;\n    success = false;\n    result = 0;\n    acknowledged = NULL;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_fileData",
          "replace": "    header = NULL;\n    reply = NULL;\n    transmitData = NULL;\n    success = false;\n    result = 0;\n    acknowledged = NULL;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_header_fileData",
          "replace": "    reply = NULL;\n    transmitData = NULL;\n    success = false;\n    result = 0;\n    acknowledged = NULL;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_result",
          "replace": "    header = NULL;\n    reply = NULL;\n    transmitData = NULL;\n    fileData = NULL;\n    success = false;\n    acknowledged = NULL;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_header_result",
          "replace": "    reply = NULL;\n    transmitData = NULL;\n    fileData = NULL;\n    success = false;\n    acknowledged = NULL;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_fileData_result",
          "replace": "    header = NULL;\n    reply = NULL;\n    transmitData = NULL;\n    success = false;\n    acknowledged = NULL;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_header_fileData_result",
          "replace": "    reply = NULL;\n    transmitData = NULL;\n    success = false;\n    acknowledged = NULL;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_acknowledged",
          "replace": "    header = NULL;\n    reply = NULL;\n    transmitData = NULL;\n    fileData = NULL;\n    success = false;\n    result = 0;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_header_acknowledged",
          "replace": "    reply = NULL;\n    transmitData = NULL;\n    fileData = NULL;\n    success = false;\n    result = 0;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_fileData_acknowledged",
          "replace": "    header = NULL;\n    reply = NULL;\n    transmitData = NULL;\n    success = false;\n    result = 0;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_header_fileData_acknowledged",
          "replace": "    reply = NULL;\n    transmitData = NULL;\n    success = false;\n    result = 0;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_result_acknowledged",
          "replace": "    header = NULL;\n    reply = NULL;\n    transmitData = NULL;\n    fileData = NULL;\n    success = false;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_header_result_acknowledged",
          "replace": "    reply = NULL;\n    transmitData = NULL;\n    fileData = NULL;\n    success = false;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_fileData_result_acknowledged",
          "replace": "    header = NULL;\n    reply = NULL;\n    transmitData = NULL;\n    success = false;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_header_fileData_result_acknowledged",
          "replace": "    reply = NULL;\n    transmitData = NULL;\n    success = false;\n    oldTrack = -1;\n"
        },
        {
          "name": "omit_oldTrack",
          "replace": "    header = NULL;\n    reply = NULL;\n    transmitData = NULL;\n    fileData = NULL;\n    success = false;\n    result = 0;\n    acknowledged = NULL;\n"
        },
        {
          "name": "omit_header_oldTrack",
          "replace": "    reply = NULL;\n    transmitData = NULL;\n    fileData = NULL;\n    success = false;\n    result = 0;\n    acknowledged = NULL;\n"
        },
        {
          "name": "omit_fileData_oldTrack",
          "replace": "    header = NULL;\n    reply = NULL;\n    transmitData = NULL;\n    success = false;\n    result = 0;\n    acknowledged = NULL;\n"
        },
        {
          "name": "omit_header_fileData_oldTrack",
          "replace": "    reply = NULL;\n    transmitData = NULL;\n    success = false;\n    result = 0;\n    acknowledged = NULL;\n"
        },
        {
          "name": "omit_result_oldTrack",
          "replace": "    header = NULL;\n    reply = NULL;\n    transmitData = NULL;\n    fileData = NULL;\n    success = false;\n    acknowledged = NULL;\n"
        },
        {
          "name": "omit_header_result_oldTrack",
          "replace": "    reply = NULL;\n    transmitData = NULL;\n    fileData = NULL;\n    success = false;\n    acknowledged = NULL;\n"
        },
        {
          "name": "omit_fileData_result_oldTrack",
          "replace": "    header = NULL;\n    reply = NULL;\n    transmitData = NULL;\n    success = false;\n    acknowledged = NULL;\n"
        },
        {
          "name": "omit_header_fileData_result_oldTrack",
          "replace": "    reply = NULL;\n    transmitData = NULL;\n    success = false;\n    acknowledged = NULL;\n"
        },
        {
          "name": "omit_acknowledged_oldTrack",
          "replace": "    header = NULL;\n    reply = NULL;\n    transmitData = NULL;\n    fileData = NULL;\n    success = false;\n    result = 0;\n"
        },
        {
          "name": "omit_header_acknowledged_oldTrack",
          "replace": "    reply = NULL;\n    transmitData = NULL;\n    fileData = NULL;\n    success = false;\n    result = 0;\n"
        },
        {
          "name": "omit_fileData_acknowledged_oldTrack",
          "replace": "    header = NULL;\n    reply = NULL;\n    transmitData = NULL;\n    success = false;\n    result = 0;\n"
        },
        {
          "name": "omit_header_fileData_acknowledged_oldTrack",
          "replace": "    reply = NULL;\n    transmitData = NULL;\n    success = false;\n    result = 0;\n"
        },
        {
          "name": "omit_result_acknowledged_oldTrack",
          "replace": "    header = NULL;\n    reply = NULL;\n    transmitData = NULL;\n    fileData = NULL;\n    success = false;\n"
        },
        {
          "name": "omit_header_result_acknowledged_oldTrack",
          "replace": "    reply = NULL;\n    transmitData = NULL;\n    fileData = NULL;\n    success = false;\n"
        },
        {
          "name": "omit_fileData_result_acknowledged_oldTrack",
          "replace": "    header = NULL;\n    reply = NULL;\n    transmitData = NULL;\n    success = false;\n"
        },
        {
          "name": "omit_header_fileData_result_acknowledged_oldTrack",
          "replace": "    reply = NULL;\n    transmitData = NULL;\n    success = false;\n"
        }
      ]
    }
  ]
}
*/
