// C03: parent9c81a378 plus the tested direct text record.
// Complete product; no TU-state probes; source restored; no truncation.
// 4 arms in 2.2657305588945746 seconds.
// Results: build/source-variant-batch/c03-executive-record-packing-owner/results.json
// Whole object evidence: build/c03-record-alignment-matrix.log
// OWNER baseline.obj native-exact False function-exact True data [(3, '0xc0400040', 604)]
// OWNER trial-0000.obj native-exact False function-exact True data [(3, '0xc0400040', 604)]
// OWNER trial-0001.obj native-exact False function-exact True data [(3, '0xc0400040', 604)]
// OWNER trial-0002.obj native-exact False function-exact True data [(3, '0xc0400040', 604)]
// OWNER trial-0003.obj native-exact False function-exact True data [(3, '0xc0400040', 604)]
// trial 0 size 176 text 7d9f459eb4131398 relocs 76a250938837e21c blocks 12/12 graph True
// trial 1 size 176 text 7d9f459eb4131398 relocs 76a250938837e21c blocks 12/12 graph True
// trial 2 size 176 text 7d9f459eb4131398 relocs 76a250938837e21c blocks 12/12 graph True
// trial 3 size 176 text 7d9f459eb4131398 relocs 76a250938837e21c blocks 12/12 graph True
// Final disposition: reject every aggregate packing arm; data alignment remains8 rather than4.

/* Exact source axes:
{
  "schema": 1,
  "source": "src/BASE/EXEC.cpp",
  "rva": "0xc4f60",
  "axes": [
    {
      "name": "record-pack",
      "find": "#include <BASE/EXEC_TYPES.h>",
      "options": [
        {
          "name": "default"
        },
        {
          "name": "pack1",
          "replace": "#pragma pack(push, 1)\n#include <BASE/EXEC_TYPES.h>\n#pragma pack(pop)"
        },
        {
          "name": "pack2",
          "replace": "#pragma pack(push, 2)\n#include <BASE/EXEC_TYPES.h>\n#pragma pack(pop)"
        },
        {
          "name": "pack4",
          "replace": "#pragma pack(push, 4)\n#include <BASE/EXEC_TYPES.h>\n#pragma pack(pop)"
        }
      ]
    }
  ]
}
*/
