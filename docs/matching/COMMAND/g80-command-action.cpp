// VC6 SP5 COMMAND source family g80-command-action; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/g80-command-action/results.json; RVA 0x30cbe
// Complete 2 arms; elapsed 2.012355s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'structured_control': 'keep', 'candidate': 'baseline'} / 1547 / 1d527828a3dfdd54 / 14ad3cff8e857c99 / 50:50 / 49,1,0,0 / True
// 1 / {'structured_control': 'single_action_pass_with_cleanup', 'candidate': 'baseline'} / 1547 / f75c30abaf16b840 / 025a9fa84b527deb / 47:50 / 17,1,7,22 / False

// AXIS structured_control
// BEFORE (complete exact span):
// | MessageDispatchResult combatManager::ProcessNextAction(struct tag_message& message) {
// |     i32 actionData[IDX(ACTION_DATA_COUNT)];
// |     i32 transmitResult;
// |     army* actingArmy_29;
// |     b32 shouldAdvance;
// |     b32 redraw_10;
// |     MessageDispatchResult dispatchResult_1;
// |
// |     ClearCombatMessages(0);
// |     dispatchResult_1 = MESSAGE_DISPATCH_CONSUME;
// |     redraw_10 = false;
// |     gbProcessingCombatAction = true;
// |     if (m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] != COMBAT_SIDE_NONE
// |         || m_smallViewSide[IDX(COMBAT_DEFENDER_SIDE)] != COMBAT_SIDE_NONE) {
// |         m_smallViewSide[IDX(COMBAT_DEFENDER_SIDE)] = COMBAT_SIDE_NONE;
// |         m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] = COMBAT_SIDE_NONE;
// |         redraw_10 = true;
// |     }
// |     if (giNextAction != ACTION_NONE) {
// |         LogInt(
// |             "Process Act",
// |             IDX(giNextAction),
// |             giNextActionGridIndex,
// |             giNextActionGridIndex2,
// |             giNextActionExtra,
// |             IDX(m_currentArmySide),
// |             m_currentArmyIndex,
// |             m_armies[IDX(m_currentArmySide)][m_currentArmyIndex].m_hex
// |         );
// |     }
// |     gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
// |     UpdateMouseGrid(-1, 1);
// |     memset(m_gridState, IDX(GRID_SHADE_NONE), sizeof(m_gridState));
// |     if (UpdateGrid(0, 0) != 0)
// |         redraw_10 = true;
// |     if (redraw_10 != 0)
// |         DrawFrame(1, 0, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
// |
// |     if (gbThisNetHasControl != 0 && gbRemoteOn != 0 && m_playerId[IDX(COMBAT_ATTACKER_SIDE)] >= 0
// |         && m_playerId[IDX(COMBAT_DEFENDER_SIDE)] >= 0
// |         && gbHumanPlayer[m_playerId[IDX(COMBAT_DEFENDER_SIDE)]] != 0
// |         && gbHumanPlayer[m_playerId[IDX(COMBAT_ATTACKER_SIDE)]] != 0) {
// |         actionData[IDX(ACTION_DATA_ACTION)] = IDX(giNextAction);
// |         actionData[IDX(ACTION_DATA_EXTRA)] = giNextActionExtra;
// |         actionData[IDX(ACTION_DATA_GRID)] = giNextActionGridIndex;
// |         actionData[IDX(ACTION_DATA_SECOND_GRID)] = giNextActionGridIndex2;
// |         LogInt(
// |             "About to T",
// |             iCombatControlNetPos[IDX(COMBAT_DEFENDER_SIDE) - IDX(m_currentSide)],
// |             LOG_UNUSED_VALUE,
// |             LOG_UNUSED_VALUE,
// |             LOG_UNUSED_VALUE,
// |             LOG_UNUSED_VALUE,
// |             LOG_UNUSED_VALUE,
// |             LOG_UNUSED_VALUE
// |         );
// |         transmitResult = TransmitRemoteData(
// |             reinterpret_cast<char*>(actionData),
// |             iCombatControlNetPos[IDX(COMBAT_DEFENDER_SIDE) - IDX(m_currentSide)],
// |             sizeof(actionData),
// |             REMOTE_COMMAND_ACTION,
// |             1,
// |             1,
// |             REMOTE_MESSAGE_DEFAULT
// |         );
// |         LogStr("Post T");
// |         if (transmitResult == 0)
// |             ShutDown(NULL);
// |     }
// |
// |     actingArmy_29 = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
// |     shouldAdvance = false;
// |     if (CheckWin(&message) != 0)
// |         goto Finished;
// |     switch (giNextAction) {
// |         case ACTION_NONE:
// |             break;
// |         case ACTION_CAST_SPELL:
// |             ResetCyclingCreatures();
// |             CastSpell(
// |                 SpellType(giNextActionExtra),
// |                 giNextActionGridIndex,
// |                 0,
// |                 giNextActionGridIndex2
// |             );
// |             if (m_armies[IDX(m_currentArmySide)][m_currentArmyIndex].m_quantity <= 0)
// |                 shouldAdvance = true;
// |             ResetCycleTimers();
// |             break;
// |         case ACTION_MOVE:
// |             ResetCyclingCreatures();
// |             actingArmy_29->MoveAttack(giNextActionGridIndex, 0);
// |             actingArmy_29->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_BAD_MORALE;
// |             if (CheckWin(&message) != 0) {
// |                 dispatchResult_1 = MESSAGE_DISPATCH_FORWARD;
// |                 goto Finished;
// |             }
// |             CheckApplyGoodMorale(m_currentArmySide, m_currentArmyIndex);
// |             shouldAdvance = true;
// |             ResetCycleTimers();
// |             break;
// |         case ACTION_ATTACK:
// |             ResetCyclingCreatures();
// |             if (giNextActionExtra != -1 && actingArmy_29->m_hex != giNextActionExtra) {
// |                 actingArmy_29->MoveAttack(giNextActionExtra, 1);
// |             }
// |             actingArmy_29->MoveAttack(giNextActionGridIndex, 0);
// |             actingArmy_29->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_BAD_MORALE;
// |             if (CheckWin(&message) != 0) {
// |                 dispatchResult_1 = MESSAGE_DISPATCH_FORWARD;
// |                 goto Finished;
// |             }
// |             CheckApplyGoodMorale(m_currentArmySide, m_currentArmyIndex);
// |             shouldAdvance = true;
// |             ResetCycleTimers();
// |             break;
// |         case ACTION_RETREAT:
// |             m_sideRetreated[IDX(m_currentSide)] = true;
// |             gbRetreatWin = true;
// |             ResetCycleTimers();
// |             break;
// |         case ACTION_SURRENDER:
// |             gbCombatSurrender = true;
// |             gbRetreatWin = true;
// |             m_sideDefeated[IDX(m_currentSide)] = true;
// |             gpGame->m_players[m_playerId[IDX(m_currentSide)]].m_resources[IDX(RES_GOLD)] -=
// |                 giNextActionExtra;
// |             gpGame->m_players[m_playerId[IDX(COMBAT_DEFENDER_SIDE) - IDX(m_currentSide)]]
// |                 .m_resources[IDX(RES_GOLD)] += giNextActionExtra;
// |             ResetCycleTimers();
// |             break;
// |         case ACTION_WAIT:
// |             actingArmy_29->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_BAD_MORALE;
// |             shouldAdvance = true;
// |             break;
// |         case ACTION_DEFEND:
// |             actingArmy_29->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_DEFERRED_TURN;
// |             shouldAdvance = true;
// |             break;
// |     }
// |     giNextAction = ACTION_NONE;
// |     if (CheckWin(&message) != 0) {
// |         dispatchResult_1 = MESSAGE_DISPATCH_FORWARD;
// |         goto Finished;
// |     }
// |     TestRaiseDoor();
// |     if (shouldAdvance != 0 && GetNextArmy(1) == 0) {
// |         ResetRound();
// |         GetNextArmy(1);
// |     }
// |     CheckChangeSelector();
// |
// | Finished:
// |     gbProcessingCombatAction = false;
// |     ResetMouse();
// |     return dispatchResult_1;
// | }
// ALTERNATIVE single_action_pass_with_cleanup (unified source delta from BEFORE):
// | --- before
// | +++ single_action_pass_with_cleanup
// | @@ -70,87 +70,89 @@
// |
// |      actingArmy_29 = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
// |      shouldAdvance = false;
// | -    if (CheckWin(&message) != 0)
// | -        goto Finished;
// | -    switch (giNextAction) {
// | -        case ACTION_NONE:
// | -            break;
// | -        case ACTION_CAST_SPELL:
// | -            ResetCyclingCreatures();
// | -            CastSpell(
// | -                SpellType(giNextActionExtra),
// | -                giNextActionGridIndex,
// | -                0,
// | -                giNextActionGridIndex2
// | -            );
// | -            if (m_armies[IDX(m_currentArmySide)][m_currentArmyIndex].m_quantity <= 0)
// | +    do {
// | +        if (CheckWin(&message) != 0)
// | +            continue;
// | +        switch (giNextAction) {
// | +            case ACTION_NONE:
// | +                break;
// | +            case ACTION_CAST_SPELL:
// | +                ResetCyclingCreatures();
// | +                CastSpell(
// | +                    SpellType(giNextActionExtra),
// | +                    giNextActionGridIndex,
// | +                    0,
// | +                    giNextActionGridIndex2
// | +                );
// | +                if (m_armies[IDX(m_currentArmySide)][m_currentArmyIndex].m_quantity <= 0)
// | +                    shouldAdvance = true;
// | +                ResetCycleTimers();
// | +                break;
// | +            case ACTION_MOVE:
// | +                ResetCyclingCreatures();
// | +                actingArmy_29->MoveAttack(giNextActionGridIndex, 0);
// | +                actingArmy_29->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_BAD_MORALE;
// | +                if (CheckWin(&message) != 0) {
// | +                    dispatchResult_1 = MESSAGE_DISPATCH_FORWARD;
// | +                    continue;
// | +                }
// | +                CheckApplyGoodMorale(m_currentArmySide, m_currentArmyIndex);
// |                  shouldAdvance = true;
// | -            ResetCycleTimers();
// | -            break;
// | -        case ACTION_MOVE:
// | -            ResetCyclingCreatures();
// | -            actingArmy_29->MoveAttack(giNextActionGridIndex, 0);
// | -            actingArmy_29->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_BAD_MORALE;
// | -            if (CheckWin(&message) != 0) {
// | -                dispatchResult_1 = MESSAGE_DISPATCH_FORWARD;
// | -                goto Finished;
// | -            }
// | -            CheckApplyGoodMorale(m_currentArmySide, m_currentArmyIndex);
// | -            shouldAdvance = true;
// | -            ResetCycleTimers();
// | -            break;
// | -        case ACTION_ATTACK:
// | -            ResetCyclingCreatures();
// | -            if (giNextActionExtra != -1 && actingArmy_29->m_hex != giNextActionExtra) {
// | -                actingArmy_29->MoveAttack(giNextActionExtra, 1);
// | -            }
// | -            actingArmy_29->MoveAttack(giNextActionGridIndex, 0);
// | -            actingArmy_29->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_BAD_MORALE;
// | -            if (CheckWin(&message) != 0) {
// | -                dispatchResult_1 = MESSAGE_DISPATCH_FORWARD;
// | -                goto Finished;
// | -            }
// | -            CheckApplyGoodMorale(m_currentArmySide, m_currentArmyIndex);
// | -            shouldAdvance = true;
// | -            ResetCycleTimers();
// | -            break;
// | -        case ACTION_RETREAT:
// | -            m_sideRetreated[IDX(m_currentSide)] = true;
// | -            gbRetreatWin = true;
// | -            ResetCycleTimers();
// | -            break;
// | -        case ACTION_SURRENDER:
// | -            gbCombatSurrender = true;
// | -            gbRetreatWin = true;
// | -            m_sideDefeated[IDX(m_currentSide)] = true;
// | -            gpGame->m_players[m_playerId[IDX(m_currentSide)]].m_resources[IDX(RES_GOLD)] -=
// | -                giNextActionExtra;
// | -            gpGame->m_players[m_playerId[IDX(COMBAT_DEFENDER_SIDE) - IDX(m_currentSide)]]
// | -                .m_resources[IDX(RES_GOLD)] += giNextActionExtra;
// | -            ResetCycleTimers();
// | -            break;
// | -        case ACTION_WAIT:
// | -            actingArmy_29->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_BAD_MORALE;
// | -            shouldAdvance = true;
// | -            break;
// | -        case ACTION_DEFEND:
// | -            actingArmy_29->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_DEFERRED_TURN;
// | -            shouldAdvance = true;
// | -            break;
// | -    }
// | -    giNextAction = ACTION_NONE;
// | -    if (CheckWin(&message) != 0) {
// | -        dispatchResult_1 = MESSAGE_DISPATCH_FORWARD;
// | -        goto Finished;
// | -    }
// | -    TestRaiseDoor();
// | -    if (shouldAdvance != 0 && GetNextArmy(1) == 0) {
// | -        ResetRound();
// | -        GetNextArmy(1);
// | -    }
// | -    CheckChangeSelector();
// | +                ResetCycleTimers();
// | +                break;
// | +            case ACTION_ATTACK:
// | +                ResetCyclingCreatures();
// | +                if (giNextActionExtra != -1 && actingArmy_29->m_hex != giNextActionExtra) {
// | +                    actingArmy_29->MoveAttack(giNextActionExtra, 1);
// | +                }
// | +                actingArmy_29->MoveAttack(giNextActionGridIndex, 0);
// | +                actingArmy_29->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_BAD_MORALE;
// | +                if (CheckWin(&message) != 0) {
// | +                    dispatchResult_1 = MESSAGE_DISPATCH_FORWARD;
// | +                    continue;
// | +                }
// | +                CheckApplyGoodMorale(m_currentArmySide, m_currentArmyIndex);
// | +                shouldAdvance = true;
// | +                ResetCycleTimers();
// | +                break;
// | +            case ACTION_RETREAT:
// | +                m_sideRetreated[IDX(m_currentSide)] = true;
// | +                gbRetreatWin = true;
// | +                ResetCycleTimers();
// | +                break;
// | +            case ACTION_SURRENDER:
// | +                gbCombatSurrender = true;
// | +                gbRetreatWin = true;
// | +                m_sideDefeated[IDX(m_currentSide)] = true;
// | +                gpGame->m_players[m_playerId[IDX(m_currentSide)]].m_resources[IDX(RES_GOLD)] -=
// | +                    giNextActionExtra;
// | +                gpGame->m_players[m_playerId[IDX(COMBAT_DEFENDER_SIDE) - IDX(m_currentSide)]]
// | +                    .m_resources[IDX(RES_GOLD)] += giNextActionExtra;
// | +                ResetCycleTimers();
// | +                break;
// | +            case ACTION_WAIT:
// | +                actingArmy_29->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_BAD_MORALE;
// | +                shouldAdvance = true;
// | +                break;
// | +            case ACTION_DEFEND:
// | +                actingArmy_29->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_DEFERRED_TURN;
// | +                shouldAdvance = true;
// | +                break;
// | +        }
// | +        giNextAction = ACTION_NONE;
// | +        if (CheckWin(&message) != 0) {
// | +            dispatchResult_1 = MESSAGE_DISPATCH_FORWARD;
// | +            continue;
// | +        }
// | +        TestRaiseDoor();
// | +        if (shouldAdvance != 0 && GetNextArmy(1) == 0) {
// | +            ResetRound();
// | +            GetNextArmy(1);
// | +        }
// | +        CheckChangeSelector();
// |
// | -Finished:
// | +    } while (false);
// | +
// |      gbProcessingCombatAction = false;
// |      ResetMouse();
// |      return dispatchResult_1;
