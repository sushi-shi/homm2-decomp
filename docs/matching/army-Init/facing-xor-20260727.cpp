// army::Init, RVA 0x0044aa85
//
// The previous conditional expression emitted two assignment arms and three
// extra basic blocks:
//
//   m_facing =
//       side == COMBAT_ATTACKER_SIDE ? ARMY_FACING_RIGHT : ARMY_FACING_LEFT;
//
// Retail instead loads the side, XORs it with 1, and stores the result. Combat
// sides and army facings are both binary domains, with attacker/left equal to
// zero and defender/right equal to one, so the following is the corresponding
// high-level expression:
//
//   m_facing =
//       static_cast<ArmyFacing>(static_cast<i32>(side) ^ ARMY_FACING_RIGHT);
//
// Result: exact clean-source closure. The compiled function has the retail
// size (0x443), identical 288-instruction assembly, identical 17/17 block
// skeleton, and 22/22 ordered relocations. `homm2 status update` advanced the
// generated MAX entry for source hash 7099f208133e from 97.8007% to 100%.
