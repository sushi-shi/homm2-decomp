// C34/C37/S39; clean parent0bd4eda06.
// Final48-arm product: 50.87317479867488 seconds; all compiled/restored/no truncation.
// Retain trial47: direct literals, natural readOnly assignment, named tab with i16 cast.
// Text481 bytes / 5e6c3011bb15e6ff unchanged; private-label hashes vary.
// Initial c37-view32, c37-tab-structure2 and c37-tab-word3 arms all complete.
// Diagnostic one-arm tab-inner-replay and tab-outer-replay retain best.asm.
// Bare inner conversion adds AND EAX,ff (5 bytes); dropping word cast loses MOVSX (3).
// All those source arms are included in this final exact-span family.
// Artifacts: build/source-variant-batch/c37-<family>/results.json;
// build/c37-<family>-axes.json and -manifest.json; replay input omits -axes suffix.
// Native, retail, source-dialect and text bounds: C34-C37-S39-B59.md.
// No generated AST transformations or TU-state census is claimed.
/* Exact final axes:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x546ee",
  "axes": [
    {
      "name": "dialog_literal",
      "find": "NormalDialog(const_cast<char*>(\"\\xcd\\xe5\\xf2 \\xe7\\xe0\\xea\\xeb\\xe8\\xed\\xe0\\xed\\xe8\\xe9.\"),",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "direct",
          "replace": "NormalDialog(\"\\xcd\\xe5\\xf2 \\xe7\\xe0\\xea\\xeb\\xe8\\xed\\xe0\\xed\\xe8\\xe9.\","
        }
      ]
    },
    {
      "name": "window_literal",
      "find": "VIEW_SPELLS_WINDOW_X, VIEW_SPELLS_WINDOW_Y, const_cast<char*>(\"spellwin.bin\")",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "direct",
          "replace": "VIEW_SPELLS_WINDOW_X, VIEW_SPELLS_WINDOW_Y, \"spellwin.bin\""
        }
      ]
    },
    {
      "name": "read_only",
      "find": "m_viewSpellsReadOnly = static_cast<i8>(readOnly);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "m_viewSpellsReadOnly = readOnly;"
        }
      ]
    },
    {
      "name": "tab_id",
      "find": "message.payload.widget.id = static_cast<i16>(\n                VIEW_SPELL_COMBAT_TAB_ID\n                + static_cast<i32>(spellType != SPELL_TYPE_COMBAT)\n            );",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "inner_natural",
          "replace": "message.payload.widget.id = static_cast<i16>(\n                VIEW_SPELL_COMBAT_TAB_ID\n                + (spellType != SPELL_TYPE_COMBAT)\n            );"
        },
        {
          "name": "outer_natural",
          "replace": "message.payload.widget.id =\n                VIEW_SPELL_COMBAT_TAB_ID\n                + static_cast<i32>(spellType != SPELL_TYPE_COMBAT);"
        },
        {
          "name": "both_natural",
          "replace": "message.payload.widget.id =\n                VIEW_SPELL_COMBAT_TAB_ID\n                + (spellType != SPELL_TYPE_COMBAT);"
        },
        {
          "name": "conditional",
          "replace": "message.payload.widget.id = (spellType == SPELL_TYPE_COMBAT)\n                ? VIEW_SPELL_COMBAT_TAB_ID : VIEW_SPELL_ADVENTURE_TAB_ID;"
        },
        {
          "name": "conditional_word",
          "replace": "message.payload.widget.id = static_cast<i16>(\n                spellType == SPELL_TYPE_COMBAT\n                ? VIEW_SPELL_COMBAT_TAB_ID : VIEW_SPELL_ADVENTURE_TAB_ID\n            );"
        }
      ]
    }
  ]
}
*/
