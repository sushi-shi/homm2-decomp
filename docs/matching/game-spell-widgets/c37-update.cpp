// C34/C37/S39; clean parent0bd4eda06.
// Final18-arm product: 19.27148383948952 seconds; all compiled/restored/no truncation.
// Retain trial16: both named commands, initial overwritten message.type store retained.
// Text1167 bytes / e62ffb02dbe4ffd1 unchanged; private-label hashes vary.
// Initial c37-update8, c37-command-structure4 and c37-command-store8 arms all complete.
// Diagnostic one-arm c37-bool-replay retains best.asm: AND ECX,ff adds6 bytes.
// Removing overwritten type assignment loses7 bytes in every command CFG family.
// All those source arms are included in this final exact-span family.
// Artifacts: build/source-variant-batch/c37-<family>/results.json;
// build/c37-<family>-axes.json and -manifest.json; replay input omits -axes suffix.
// Native, retail, source-dialect and text bounds: C34-C37-S39-B59.md.
// No generated AST transformations or TU-state census is claimed.
/* Exact final axes:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x548cf",
  "axes": [
    {
      "name": "hundreds",
      "find": "message.payload.widget.command = static_cast<BaseWidgetCommand>(\n        IDX(WIDGET_COMMAND_SET_FLAGS)\n        + static_cast<i32>(spellPoints0 <= VIEW_SPELL_MANA_HUNDREDS_THRESHOLD)\n    );",
      "options": [
        {
          "name": "arithmetic"
        },
        {
          "name": "implicit_bool",
          "replace": "message.payload.widget.command = static_cast<BaseWidgetCommand>(\n        IDX(WIDGET_COMMAND_SET_FLAGS)\n        + (spellPoints0 <= VIEW_SPELL_MANA_HUNDREDS_THRESHOLD)\n    );"
        },
        {
          "name": "conditional",
          "replace": "message.payload.widget.command = (spellPoints0 <= VIEW_SPELL_MANA_HUNDREDS_THRESHOLD)\n        ? WIDGET_COMMAND_CLEAR_FLAGS : WIDGET_COMMAND_SET_FLAGS;"
        }
      ]
    },
    {
      "name": "tens",
      "find": "message.payload.widget.command = static_cast<BaseWidgetCommand>(\n        IDX(WIDGET_COMMAND_SET_FLAGS)\n        + static_cast<i32>(spellPoints0 <= VIEW_SPELL_MANA_TENS_THRESHOLD)\n    );",
      "options": [
        {
          "name": "arithmetic"
        },
        {
          "name": "implicit_bool",
          "replace": "message.payload.widget.command = static_cast<BaseWidgetCommand>(\n        IDX(WIDGET_COMMAND_SET_FLAGS)\n        + (spellPoints0 <= VIEW_SPELL_MANA_TENS_THRESHOLD)\n    );"
        },
        {
          "name": "conditional",
          "replace": "message.payload.widget.command = (spellPoints0 <= VIEW_SPELL_MANA_TENS_THRESHOLD)\n        ? WIDGET_COMMAND_CLEAR_FLAGS : WIDGET_COMMAND_SET_FLAGS;"
        }
      ]
    },
    {
      "name": "overwritten_store",
      "find": "    message.type = MESSAGE_WIDGET;\n    spellPoints0 = m_viewSpellsHero->m_spellPoints;",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "    spellPoints0 = m_viewSpellsHero->m_spellPoints;"
        }
      ]
    }
  ]
}
*/
