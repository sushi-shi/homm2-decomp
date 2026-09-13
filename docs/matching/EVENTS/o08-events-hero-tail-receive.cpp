// VC6 SP5 source product o08-events-hero-tail-receive; RVA 0x49582.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 3.029005s; no compile failure or timeout; source restored.
// build/source-variant-batch/o08-events-hero-tail-receive/results.json and build/o08-events-hero-tail-receive-axes.json
// Disposition: apply trial1. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'whole-owner': 'keep', 'candidate': 'baseline'} / 990 / 44bed41da6c7feb6 / 5fdc30127eca6140 / 33:33 / 33,0,0,0,0 / True
// 1 / {'whole-owner': 'recover', 'candidate': 'baseline'} / 990 / 44bed41da6c7feb6 / 5fdc30127eca6140 / 33:33 / 33,0,0,0,0 / True

// AXIS whole-owner
// BEFORE (complete exact span):
// | &(*firstHero)->m_spells[COMBAT_REMOTE_HERO_SECOND_SPELL_INDEX]
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +1 @@
// | -&(*firstHero)->m_spells[COMBAT_REMOTE_HERO_SECOND_SPELL_INDEX]
// | +reinterpret_cast<char*>((*firstHero)) + COMBAT_REMOTE_HERO_FIRST_SIZE
// EXTRA BEFORE:
// | &(*secondHero)->m_spells[COMBAT_REMOTE_HERO_SECOND_SPELL_INDEX]
// EXTRA REPLACEMENT:
// | reinterpret_cast<char*>((*secondHero)) + COMBAT_REMOTE_HERO_FIRST_SIZE
