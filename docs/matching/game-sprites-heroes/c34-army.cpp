// Parent cleanb080908d8; all8 arms in9.4538834374398 seconds. Retain3(three columns, both natural assignments),834 bytes text753cfdffb7b5cd96. All four three-column arms have this text; all four two-column arms emit642. Three selection flags stay because selected[2] is read; the table's third column is not read but remains after native-changing deletion.
// Complete, all compiled/restored; no generator or wall-time truncation.
// No generated AST transformations or TU-state census is claimed.
// Artifacts: build/source-variant-batch/c34-army/results.json;
// build/c34-army-axes.json and -manifest.json.
// Native/retail proof and contracts: C34-C40-S41.md.
/* Exact axes:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x5ad00",
  "axes": [
    {
      "name": "table_extent",
      "find": "    RandomHeroArmyRange armyTable[IDX(FACTION_COUNT)][RANDOM_HERO_ARMY_OPTION_COUNT] = {\n        {{IDX(CREATURE_PEASANT), 30, 50},\n         {IDX(CREATURE_ARCHER), 3, 5},\n         {IDX(CREATURE_PIKEMAN), 2, 4}},\n        {{IDX(CREATURE_GOBLIN), 15, 25}, {IDX(CREATURE_ORC), 3, 5}, {IDX(CREATURE_WOLF), 2, 3}},\n        {{IDX(CREATURE_SPRITE), 10, 20}, {IDX(CREATURE_DWARF), 2, 4}, {IDX(CREATURE_ELF), 1, 2}},\n        {{IDX(CREATURE_CENTAUR), 6, 10},\n         {IDX(CREATURE_GARGOYLE), 2, 4},\n         {IDX(CREATURE_GRIFFIN), 1, 2}},\n        {{IDX(CREATURE_HALFLING), 6, 10},\n         {IDX(CREATURE_BOAR), 2, 4},\n         {IDX(CREATURE_IRON_GOLEM), 1, 2}},\n        {{IDX(CREATURE_SKELETON), 6, 10}, {IDX(CREATURE_ZOMBIE), 2, 4}, {IDX(CREATURE_MUMMY), 1, 2}}\n    };",
      "options": [
        {
          "name": "three_columns"
        },
        {
          "name": "two_columns",
          "replace": "    RandomHeroArmyRange armyTable[IDX(FACTION_COUNT)][RANDOM_HERO_ARMY_SELECTION_COUNT] = {\n        {{IDX(CREATURE_PEASANT), 30, 50},\n         {IDX(CREATURE_ARCHER), 3, 5}},\n        {{IDX(CREATURE_GOBLIN), 15, 25}, {IDX(CREATURE_ORC), 3, 5}},\n        {{IDX(CREATURE_SPRITE), 10, 20}, {IDX(CREATURE_DWARF), 2, 4}},\n        {{IDX(CREATURE_CENTAUR), 6, 10},\n         {IDX(CREATURE_GARGOYLE), 2, 4}},\n        {{IDX(CREATURE_HALFLING), 6, 10},\n         {IDX(CREATURE_BOAR), 2, 4}},\n        {{IDX(CREATURE_SKELETON), 6, 10}, {IDX(CREATURE_ZOMBIE), 2, 4}}\n    };"
        }
      ]
    },
    {
      "name": "creature",
      "find": "static_cast<i8>(armyTable[IDX(m_heroRecs[heroId].m_cursorType)][index].creature)",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "natural",
          "replace": "armyTable[IDX(m_heroRecs[heroId].m_cursorType)][index].creature"
        }
      ]
    },
    {
      "name": "count",
      "find": "static_cast<i16>(Random(minimum3, maximum) / RANDOM_HERO_COUNT_SCALE)",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "natural",
          "replace": "Random(minimum3, maximum) / RANDOM_HERO_COUNT_SCALE"
        }
      ]
    }
  ]
}
*/
