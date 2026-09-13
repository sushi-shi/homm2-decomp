// C55/C56/S54, VC6 SP5, SOURCE/ARMY configured profile.
// Matrices: build/c55-<special|attack|walkto|attackto>-{axes,manifest}.json,
//           build/c56-pow-{axes,manifest}.json,
//           build/s54-berserk-{axes,manifest}.json;
// results: corresponding -batch/results.json. All complete/restored/untruncated.
// Commands: homm2 permute src/SOURCE/ARMY.cpp <rva> --axes-from <axes>
//   --min-depth 0 --max-depth 0 --limit <full-product> -o <manifest>
//   --run --batch-output <batch>

// C55 SpecialAttack0x1a1d9: five independent keep/remove axes =>32 arms.
// Full source stays unchanged except each exact span below. All5727-byte text
// hashes equal; private/EH normalized relocation hashes vary, so final complete
// native48-function/allocated-section comparison is the preservation proof.
// Original -> retained:
// static_cast<float>(fullYLen > 0 ? -VERTICAL_ANGLE : VERTICAL_ANGLE)
//   -> fullYLen > 0 ? -VERTICAL_ANGLE : VERTICAL_ANGLE
// static_cast<float>(-fullYLen) -> -fullYLen
// atan(static_cast<double>(incline)) -> atan(incline)
// sqrt(static_cast<double>(xStretch * xStretch + yStretch * yStretch))
//   -> sqrt(xStretch * xStretch + yStretch * yStretch)
// ArmySpellCancelType(1) -> ARMY_CANCEL_SPELLS_AFTER_ATTACK
// Keep the divisor parentheses and explicit float conversion, actual result
// truncations and coordinate narrowings; do not reassociate arithmetic.

// C55 DoAttack0x1bddb, WalkTo(i32)0x1d245, AttackTo(i32)0x1d509:
// one keep/remove axis each =>2 arms. Exact full function spans distinguish
// the otherwise repeated call strings; no other statement is changed.
// DoAttack: ArmySpellCancelType(1) -> ARMY_CANCEL_SPELLS_AFTER_ATTACK
// WalkTo/AttackTo: ArmySpellCancelType(0) -> ARMY_CANCEL_SPELLS_AFTER_MOVE
// Both arms in each family retain identical text/native-relocation hashes.

// C56 PowEffect0x1df4e: seven independent byte-truth casts + integer identity
// =>256 arms under the ORIGINAL char-field owner. The seven exact full-line
// spans occur, in order, at these semantic phases:
// 0 frame maxima: static_cast<u8>(current->m_animationState)
// 1 limit setup: static_cast<u8>(armies[sideNum][armyIndex].m_animationState)
// 2 limit setup: static_cast<u8>(armies[sideNum][armyIndex].m_animationCycle)
// 3 effect setup outer predicate: static_cast<u8>(current->m_animationState)
// 4 effect setup inner predicate: static_cast<u8>(current->m_animationState)
// 5 animation-cycle predicate: static_cast<u8>(current->m_animationCycle)
// 6 start predicate: static_cast<u8>(current->m_animationState)
// identity: static_cast<i32>(spellFrames - animFrame - 1)
// Each removal retains the operand expression; identity retains parentheses.
// Only original and identity-only removal are exact4567 bytes/128 relocs.
// Every byte-cast removal produces4560..4566 bytes, still265 graph blocks.

// STRUCTURAL FOLLOW-UP, retained after all field consumers were read:
// include/SOURCE/army.h:
//   bchar m_animationState; -> u8 m_animationState;
//   bchar m_animationCycle; -> u8 m_animationCycle;
// Remove all seven truth casts in PowEffect; retain identity removal too.
// All reads are unsigned truth tests; all writes true/false, including the
// sole external false store in combatManager::CastSpell. No other reader or
// writer exists in the full source/header scan. No global alias is changed.
// Whole98-object/1826-function native comparison passes, including allocated
// sections and ordered relocs; PowEffect independent retail proof passes.
// This structural parent was not part of the original256-arm char-field
// family: its success demonstrates why that negative result is parent-local.

// S54 GoBerserk0x1fd25: complete keep/remove pair for BOTH lines together:
//     i32 H2_UNUSED(unusedMask_16);
//     unusedMask_16 = 0;
// Removal:1292 bytes versus1299, score98.91892, 55 relocations and60 graph
// blocks both arms. REJECTED for this parent; no padding or fake owner added.
// G63/G64/G65 structured joins remain UNTRIED, not measured rejections.
