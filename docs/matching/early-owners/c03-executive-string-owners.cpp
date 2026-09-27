// C03: parent9c81a378 plus the tested direct text record.
// Complete product; no TU-state probes; source restored; no truncation.
// 3 arms in 1.8772461414337158 seconds.
// Results: build/source-variant-batch/c03-executive-string-owners-proof/results.json
// Whole object evidence: build/c03-string-owner-matrix.log
// OWNER baseline.obj split False native-exact False function-exact True data [(3, '0xc0400040', 604)]
// OWNER trial-0000.obj split False native-exact False function-exact True data [(3, '0xc0400040', 604)]
// OWNER trial-0001.obj split True native-exact False function-exact True data [(3, '0xc0300040', 603)]
// OWNER trial-0002.obj split True native-exact True function-exact True data [(3, '0xc0300040', 604)]
// trial 0 size 176 text 7d9f459eb4131398 relocs 76a250938837e21c blocks 12/12 graph True
// trial 1 size 176 text 497068befb3e978d relocs b53e577dde825e54 blocks 12/12 graph True
// trial 2 size 176 text 497068befb3e978d relocs b53e577dde825e54 blocks 12/12 graph True
// Final disposition: retain slot-arrays: all13 emitted functions and every allocated byte/attribute/ordered target match after exactly18 owner-relative rewrites. Inferred literal arrays leave final .data one byte shorter; direct aggregate changes alignment.

/* Exact source axes:
{
  "schema": 1,
  "source": "src/BASE/EXEC.cpp",
  "rva": "0xc4f60",
  "axes": [
    {
      "name": "text-owner-family",
      "find": "DATA(0x0051f2f4) static SExecutiveText gExecutiveTextStorage = {\n    \"\\xed\\xe5 \\xec\\xee\\xe3\\xf3 \\xe8\\xed\\xe8\\xf6\\xe8\\xe0\\xeb\\xe8\\xe7\\xe8\\xf0\\xee\\xe2\\xe0\\xf2\\xfc \\xf0\\xe5\\xf1\\xf3\\xf0\\xf1\\xfb. \\xc2\\xe5\\xf0\\xee\\xff\\xf2\\xed\\xee, \\xee\\xf8\\xe8\\xe1\\xea\\xe0 \\xe4\\xe8\\xf1\\xea\\xe0.\",\n    \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe8\\xed\\xe8\\xf6\\xe8\\xe0\\xeb\\xe8\\xe7\\xe8\\xf0\\xee\\xe2\\xe0\\xf2\\xfc \\xf3\\xf1\\xf2\\xf0\\xee\\xe9\\xf1\\xf2\\xe2\\xe0 \\xe2\\xe2\\xee\\xe4\\xe0. \\xc2\\xe5\\xf0\\xee\\xff\\xf2\\xed\\xee, \\xef\\xf0\\xee\\xe1\\xeb\\xe5\\xec\\xe0 \\xf1 \\xea\\xeb\\xe0\\xe2\\xe8\\xe0\\xf2\\xf3\\xf0\\xee\\xe9 \\xe8\\xeb\\xe8 \\xec\\xfb\\xf8\\xfc\\xfe.\",\n    \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe8\\xed\\xe8\\xf6\\xe8\\xe0\\xeb\\xe8\\xe7\\xe8\\xf0\\xee\\xe2\\xe0\\xf2\\xfc \\xe7\\xe2\\xf3\\xea.\",\n    \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe8\\xed\\xe8\\xf6\\xe8\\xe0\\xeb\\xe8\\xe7\\xe8\\xf0\\xee\\xe2\\xe0\\xf2\\xfc \\xec\\xfb\\xf8\\xfc.\",\n    \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe8\\xed\\xe8\\xf6\\xe8\\xe0\\xeb\\xe8\\xe7\\xe8\\xf0\\xee\\xe2\\xe0\\xf2\\xfc \\xee\\xea\\xed\\xe0. \\xc2\\xe5\\xf0\\xee\\xff\\xf2\\xed\\xee, \\xee\\xf8\\xe8\\xe1\\xea\\xe0 \\xe4\\xe8\\xf1\\xea\\xe0 \\xe8\\xeb\\xe8 \\xef\\xe0\\xec\\xff\\xf2\\xe8.\",\n    \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe4\\xee\\xe1\\xe0\\xe2\\xe8\\xf2\\xfc \\xec\\xe5\\xed\\xe5\\xe4\\xe6\\xe5\\xf0\\xe0!\",\n    \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe4\\xee\\xe1\\xe0\\xe2\\xe8\\xf2\\xfc \\xec\\xe5\\xed\\xe5\\xe4\\xe6\\xe5\\xf0\\xe0!\",\n    \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe4\\xee\\xe1\\xe0\\xe2\\xe8\\xf2\\xfc \\xec\\xe5\\xed\\xe5\\xe4\\xe6\\xe5\\xf0\\xe0!\",\n    \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe4\\xee\\xe1\\xe0\\xe2\\xe8\\xf2\\xfc \\xec\\xe5\\xed\\xe5\\xe4\\xe6\\xe5\\xf0\\xe0!\",\n    \"-----Manager List Start-----\",\n    \"-----\",\n    \"Head %d   Tail %d\",\n    \"-----\",\n    \"Manager %20s  this %d   prev %d  next %d\",\n    \"--*--Manager List Stop --*--\\n\\n\",\n    \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe4\\xee\\xe1\\xe0\\xe2\\xe8\\xf2\\xfc \\xec\\xe5\\xed\\xe5\\xe4\\xe6\\xe5\\xf0\\xe0!\",\n    \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe4\\xee\\xe1\\xe0\\xe2\\xe8\\xf2\\xfc \\xec\\xe5\\xed\\xe5\\xe4\\xe6\\xe5\\xf0\\xe0!\",\n    \"Terminated\"\n};\n\n#define gExecutiveText gExecutiveTextStorage\n",
      "options": [
        {
          "name": "record"
        },
        {
          "name": "literal-arrays",
          "replace": "DATA(0x51f2f4) static char gExecutiveResourceInitError[] = \"\\xed\\xe5 \\xec\\xee\\xe3\\xf3 \\xe8\\xed\\xe8\\xf6\\xe8\\xe0\\xeb\\xe8\\xe7\\xe8\\xf0\\xee\\xe2\\xe0\\xf2\\xfc \\xf0\\xe5\\xf1\\xf3\\xf0\\xf1\\xfb. \\xc2\\xe5\\xf0\\xee\\xff\\xf2\\xed\\xee, \\xee\\xf8\\xe8\\xe1\\xea\\xe0 \\xe4\\xe8\\xf1\\xea\\xe0.\";\nDATA(0x51f330) static char gExecutiveInputInitError[] = \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe8\\xed\\xe8\\xf6\\xe8\\xe0\\xeb\\xe8\\xe7\\xe8\\xf0\\xee\\xe2\\xe0\\xf2\\xfc \\xf3\\xf1\\xf2\\xf0\\xee\\xe9\\xf1\\xf2\\xe2\\xe0 \\xe2\\xe2\\xee\\xe4\\xe0. \\xc2\\xe5\\xf0\\xee\\xff\\xf2\\xed\\xee, \\xef\\xf0\\xee\\xe1\\xeb\\xe5\\xec\\xe0 \\xf1 \\xea\\xeb\\xe0\\xe2\\xe8\\xe0\\xf2\\xf3\\xf0\\xee\\xe9 \\xe8\\xeb\\xe8 \\xec\\xfb\\xf8\\xfc\\xfe.\";\nDATA(0x51f388) static char gExecutiveSoundInitError[] = \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe8\\xed\\xe8\\xf6\\xe8\\xe0\\xeb\\xe8\\xe7\\xe8\\xf0\\xee\\xe2\\xe0\\xf2\\xfc \\xe7\\xe2\\xf3\\xea.\";\nDATA(0x51f3a8) static char gExecutiveMouseInitError[] = \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe8\\xed\\xe8\\xf6\\xe8\\xe0\\xeb\\xe8\\xe7\\xe8\\xf0\\xee\\xe2\\xe0\\xf2\\xfc \\xec\\xfb\\xf8\\xfc.\";\nDATA(0x51f3c8) static char gExecutiveWindowInitError[] = \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe8\\xed\\xe8\\xf6\\xe8\\xe0\\xeb\\xe8\\xe7\\xe8\\xf0\\xee\\xe2\\xe0\\xf2\\xfc \\xee\\xea\\xed\\xe0. \\xc2\\xe5\\xf0\\xee\\xff\\xf2\\xed\\xee, \\xee\\xf8\\xe8\\xe1\\xea\\xe0 \\xe4\\xe8\\xf1\\xea\\xe0 \\xe8\\xeb\\xe8 \\xef\\xe0\\xec\\xff\\xf2\\xe8.\";\nDATA(0x51f40c) static char gExecutiveDialogManagerError1[] = \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe4\\xee\\xe1\\xe0\\xe2\\xe8\\xf2\\xfc \\xec\\xe5\\xed\\xe5\\xe4\\xe6\\xe5\\xf0\\xe0!\";\nDATA(0x51f428) static char gExecutiveDialogManagerError2[] = \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe4\\xee\\xe1\\xe0\\xe2\\xe8\\xf2\\xfc \\xec\\xe5\\xed\\xe5\\xe4\\xe6\\xe5\\xf0\\xe0!\";\nDATA(0x51f444) static char gExecutiveDialogManagerError3[] = \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe4\\xee\\xe1\\xe0\\xe2\\xe8\\xf2\\xfc \\xec\\xe5\\xed\\xe5\\xe4\\xe6\\xe5\\xf0\\xe0!\";\nDATA(0x51f460) static char gExecutiveDialogManagerError4[] = \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe4\\xee\\xe1\\xe0\\xe2\\xe8\\xf2\\xfc \\xec\\xe5\\xed\\xe5\\xe4\\xe6\\xe5\\xf0\\xe0!\";\nDATA(0x51f47c) static char gExecutiveManagerListStart[] = \"-----Manager List Start-----\";\nDATA(0x51f49c) static char gExecutiveManagerListDivider1[] = \"-----\";\nDATA(0x51f4a4) static char gExecutiveManagerListHeaderFormat[] = \"Head %d   Tail %d\";\nDATA(0x51f4b8) static char gExecutiveManagerListDivider2[] = \"-----\";\nDATA(0x51f4c0) static char gExecutiveManagerListEntryFormat[] = \"Manager %20s  this %d   prev %d  next %d\";\nDATA(0x51f4ec) static char gExecutiveManagerListStop[] = \"--*--Manager List Stop --*--\\n\\n\";\nDATA(0x51f50c) static char gExecutiveCallManagerError1[] = \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe4\\xee\\xe1\\xe0\\xe2\\xe8\\xf2\\xfc \\xec\\xe5\\xed\\xe5\\xe4\\xe6\\xe5\\xf0\\xe0!\";\nDATA(0x51f528) static char gExecutiveCallManagerError2[] = \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe4\\xee\\xe1\\xe0\\xe2\\xe8\\xf2\\xfc \\xec\\xe5\\xed\\xe5\\xe4\\xe6\\xe5\\xf0\\xe0!\";\nDATA(0x51f544) static char gExecutiveTerminationMessage[] = \"Terminated\";\n",
          "extra_edits": [
            {
              "find": "gExecutiveText.resourceInitError",
              "replace": "gExecutiveResourceInitError"
            },
            {
              "find": "gExecutiveText.inputInitError",
              "replace": "gExecutiveInputInitError"
            },
            {
              "find": "gExecutiveText.soundInitError",
              "replace": "gExecutiveSoundInitError"
            },
            {
              "find": "gExecutiveText.mouseInitError",
              "replace": "gExecutiveMouseInitError"
            },
            {
              "find": "gExecutiveText.windowInitError",
              "replace": "gExecutiveWindowInitError"
            },
            {
              "find": "gExecutiveText.dialogManagerError1",
              "replace": "gExecutiveDialogManagerError1"
            },
            {
              "find": "gExecutiveText.dialogManagerError2",
              "replace": "gExecutiveDialogManagerError2"
            },
            {
              "find": "gExecutiveText.dialogManagerError3",
              "replace": "gExecutiveDialogManagerError3"
            },
            {
              "find": "gExecutiveText.dialogManagerError4",
              "replace": "gExecutiveDialogManagerError4"
            },
            {
              "find": "gExecutiveText.managerListStart",
              "replace": "gExecutiveManagerListStart"
            },
            {
              "find": "gExecutiveText.managerListDivider1",
              "replace": "gExecutiveManagerListDivider1"
            },
            {
              "find": "gExecutiveText.managerListHeaderFormat",
              "replace": "gExecutiveManagerListHeaderFormat"
            },
            {
              "find": "gExecutiveText.managerListDivider2",
              "replace": "gExecutiveManagerListDivider2"
            },
            {
              "find": "gExecutiveText.managerListEntryFormat",
              "replace": "gExecutiveManagerListEntryFormat"
            },
            {
              "find": "gExecutiveText.managerListStop",
              "replace": "gExecutiveManagerListStop"
            },
            {
              "find": "gExecutiveText.callManagerError1",
              "replace": "gExecutiveCallManagerError1"
            },
            {
              "find": "gExecutiveText.callManagerError2",
              "replace": "gExecutiveCallManagerError2"
            },
            {
              "find": "gExecutiveText.terminationMessage",
              "replace": "gExecutiveTerminationMessage"
            }
          ]
        },
        {
          "name": "slot-arrays",
          "replace": "DATA(0x51f2f4) static char gExecutiveResourceInitError[EXEC_TEXT_RESOURCE_INIT_SLOT_SIZE] = \"\\xed\\xe5 \\xec\\xee\\xe3\\xf3 \\xe8\\xed\\xe8\\xf6\\xe8\\xe0\\xeb\\xe8\\xe7\\xe8\\xf0\\xee\\xe2\\xe0\\xf2\\xfc \\xf0\\xe5\\xf1\\xf3\\xf0\\xf1\\xfb. \\xc2\\xe5\\xf0\\xee\\xff\\xf2\\xed\\xee, \\xee\\xf8\\xe8\\xe1\\xea\\xe0 \\xe4\\xe8\\xf1\\xea\\xe0.\";\nDATA(0x51f330) static char gExecutiveInputInitError[EXEC_TEXT_INPUT_INIT_SLOT_SIZE] = \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe8\\xed\\xe8\\xf6\\xe8\\xe0\\xeb\\xe8\\xe7\\xe8\\xf0\\xee\\xe2\\xe0\\xf2\\xfc \\xf3\\xf1\\xf2\\xf0\\xee\\xe9\\xf1\\xf2\\xe2\\xe0 \\xe2\\xe2\\xee\\xe4\\xe0. \\xc2\\xe5\\xf0\\xee\\xff\\xf2\\xed\\xee, \\xef\\xf0\\xee\\xe1\\xeb\\xe5\\xec\\xe0 \\xf1 \\xea\\xeb\\xe0\\xe2\\xe8\\xe0\\xf2\\xf3\\xf0\\xee\\xe9 \\xe8\\xeb\\xe8 \\xec\\xfb\\xf8\\xfc\\xfe.\";\nDATA(0x51f388) static char gExecutiveSoundInitError[EXEC_TEXT_DEVICE_INIT_SLOT_SIZE] = \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe8\\xed\\xe8\\xf6\\xe8\\xe0\\xeb\\xe8\\xe7\\xe8\\xf0\\xee\\xe2\\xe0\\xf2\\xfc \\xe7\\xe2\\xf3\\xea.\";\nDATA(0x51f3a8) static char gExecutiveMouseInitError[EXEC_TEXT_DEVICE_INIT_SLOT_SIZE] = \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe8\\xed\\xe8\\xf6\\xe8\\xe0\\xeb\\xe8\\xe7\\xe8\\xf0\\xee\\xe2\\xe0\\xf2\\xfc \\xec\\xfb\\xf8\\xfc.\";\nDATA(0x51f3c8) static char gExecutiveWindowInitError[EXEC_TEXT_WINDOW_INIT_SLOT_SIZE] = \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe8\\xed\\xe8\\xf6\\xe8\\xe0\\xeb\\xe8\\xe7\\xe8\\xf0\\xee\\xe2\\xe0\\xf2\\xfc \\xee\\xea\\xed\\xe0. \\xc2\\xe5\\xf0\\xee\\xff\\xf2\\xed\\xee, \\xee\\xf8\\xe8\\xe1\\xea\\xe0 \\xe4\\xe8\\xf1\\xea\\xe0 \\xe8\\xeb\\xe8 \\xef\\xe0\\xec\\xff\\xf2\\xe8.\";\nDATA(0x51f40c) static char gExecutiveDialogManagerError1[EXEC_TEXT_MANAGER_ERROR_SLOT_SIZE] = \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe4\\xee\\xe1\\xe0\\xe2\\xe8\\xf2\\xfc \\xec\\xe5\\xed\\xe5\\xe4\\xe6\\xe5\\xf0\\xe0!\";\nDATA(0x51f428) static char gExecutiveDialogManagerError2[EXEC_TEXT_MANAGER_ERROR_SLOT_SIZE] = \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe4\\xee\\xe1\\xe0\\xe2\\xe8\\xf2\\xfc \\xec\\xe5\\xed\\xe5\\xe4\\xe6\\xe5\\xf0\\xe0!\";\nDATA(0x51f444) static char gExecutiveDialogManagerError3[EXEC_TEXT_MANAGER_ERROR_SLOT_SIZE] = \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe4\\xee\\xe1\\xe0\\xe2\\xe8\\xf2\\xfc \\xec\\xe5\\xed\\xe5\\xe4\\xe6\\xe5\\xf0\\xe0!\";\nDATA(0x51f460) static char gExecutiveDialogManagerError4[EXEC_TEXT_MANAGER_ERROR_SLOT_SIZE] = \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe4\\xee\\xe1\\xe0\\xe2\\xe8\\xf2\\xfc \\xec\\xe5\\xed\\xe5\\xe4\\xe6\\xe5\\xf0\\xe0!\";\nDATA(0x51f47c) static char gExecutiveManagerListStart[EXEC_TEXT_LIST_START_SLOT_SIZE] = \"-----Manager List Start-----\";\nDATA(0x51f49c) static char gExecutiveManagerListDivider1[EXEC_TEXT_LIST_DIVIDER_SLOT_SIZE] = \"-----\";\nDATA(0x51f4a4) static char gExecutiveManagerListHeaderFormat[EXEC_TEXT_LIST_HEADER_SLOT_SIZE] = \"Head %d   Tail %d\";\nDATA(0x51f4b8) static char gExecutiveManagerListDivider2[EXEC_TEXT_LIST_DIVIDER_SLOT_SIZE] = \"-----\";\nDATA(0x51f4c0) static char gExecutiveManagerListEntryFormat[EXEC_TEXT_LIST_ENTRY_SLOT_SIZE] = \"Manager %20s  this %d   prev %d  next %d\";\nDATA(0x51f4ec) static char gExecutiveManagerListStop[EXEC_TEXT_LIST_STOP_SLOT_SIZE] = \"--*--Manager List Stop --*--\\n\\n\";\nDATA(0x51f50c) static char gExecutiveCallManagerError1[EXEC_TEXT_MANAGER_ERROR_SLOT_SIZE] = \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe4\\xee\\xe1\\xe0\\xe2\\xe8\\xf2\\xfc \\xec\\xe5\\xed\\xe5\\xe4\\xe6\\xe5\\xf0\\xe0!\";\nDATA(0x51f528) static char gExecutiveCallManagerError2[EXEC_TEXT_MANAGER_ERROR_SLOT_SIZE] = \"\\xcd\\xe5 \\xec\\xee\\xe3\\xf3 \\xe4\\xee\\xe1\\xe0\\xe2\\xe8\\xf2\\xfc \\xec\\xe5\\xed\\xe5\\xe4\\xe6\\xe5\\xf0\\xe0!\";\nDATA(0x51f544) static char gExecutiveTerminationMessage[EXEC_TEXT_TERMINATION_SLOT_SIZE] = \"Terminated\";\n",
          "extra_edits": [
            {
              "find": "gExecutiveText.resourceInitError",
              "replace": "gExecutiveResourceInitError"
            },
            {
              "find": "gExecutiveText.inputInitError",
              "replace": "gExecutiveInputInitError"
            },
            {
              "find": "gExecutiveText.soundInitError",
              "replace": "gExecutiveSoundInitError"
            },
            {
              "find": "gExecutiveText.mouseInitError",
              "replace": "gExecutiveMouseInitError"
            },
            {
              "find": "gExecutiveText.windowInitError",
              "replace": "gExecutiveWindowInitError"
            },
            {
              "find": "gExecutiveText.dialogManagerError1",
              "replace": "gExecutiveDialogManagerError1"
            },
            {
              "find": "gExecutiveText.dialogManagerError2",
              "replace": "gExecutiveDialogManagerError2"
            },
            {
              "find": "gExecutiveText.dialogManagerError3",
              "replace": "gExecutiveDialogManagerError3"
            },
            {
              "find": "gExecutiveText.dialogManagerError4",
              "replace": "gExecutiveDialogManagerError4"
            },
            {
              "find": "gExecutiveText.managerListStart",
              "replace": "gExecutiveManagerListStart"
            },
            {
              "find": "gExecutiveText.managerListDivider1",
              "replace": "gExecutiveManagerListDivider1"
            },
            {
              "find": "gExecutiveText.managerListHeaderFormat",
              "replace": "gExecutiveManagerListHeaderFormat"
            },
            {
              "find": "gExecutiveText.managerListDivider2",
              "replace": "gExecutiveManagerListDivider2"
            },
            {
              "find": "gExecutiveText.managerListEntryFormat",
              "replace": "gExecutiveManagerListEntryFormat"
            },
            {
              "find": "gExecutiveText.managerListStop",
              "replace": "gExecutiveManagerListStop"
            },
            {
              "find": "gExecutiveText.callManagerError1",
              "replace": "gExecutiveCallManagerError1"
            },
            {
              "find": "gExecutiveText.callManagerError2",
              "replace": "gExecutiveCallManagerError2"
            },
            {
              "find": "gExecutiveText.terminationMessage",
              "replace": "gExecutiveTerminationMessage"
            }
          ]
        }
      ]
    }
  ]
}
*/
