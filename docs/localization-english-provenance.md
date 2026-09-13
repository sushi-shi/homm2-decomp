# English localization provenance and corrections

Reviewed all 91 IDs added in `a813e8254` against English `source-pol-2.0`
at `aa3fd27c56be0efb7d8e422c6544e7f936f92800`. New catalog IDs are not evidence of
Buka-exclusive messages. The original migration missed this source comparison.

This correction changes 55 English entries and their PO `msgid` snapshots.
All Russian `msgstr` values, IDs, game source and printf argument signatures remain
unchanged. Most replacements are verbatim originals. The few composition/argument
adaptations and entries without an identified original are explicitly noted below.
The reference is the project's 2.0 reconstruction, not a new extraction or proof
against the retail executable.

Prefer original English from the corresponding call site over a fresh translation.
Check argument meanings as well as types: equal `%s` signatures alone do not prove
compatibility. Do not change Russian bytes or Buka call arguments to fit English.
Do not introduce positional printf syntax unsupported by the VC6 CRT.

This catalog correction does not change the classic source generator; its
ID-to-readable-UTF-8 integration remains separate work.

## Verification

- All 2,495 Russian values and the complete generated CP1251 macro header equal
  `a813e8254`; expanded compiler text for all 96 game source files is unchanged.
- `homm2 build` passes. All 98 objects still pass the migration baseline comparison
  (42,433 ordered relocations); the field audit reports 38,307 sites and no items.
- All 28 localization tests pass; the full suite runs 933 tests with six existing
  skips. Tests cover original wording, Buka argument composition and English-only
  edits preserving the Russian compiler view.

## Russian / corrected English / original 2.0

All 91 additions are included. Quoted text preserves spaces and shows newlines as
`\n`. Source locations refer to the pinned 2.0 revision.

### 1. `adventure.search.found_artifact.prefix`

| Version | Text |
|---|---|
| Russian | `"Поздравляем! Проведя многие часы в раскопках, вы разыскали артефакт: "` |
| Corrected English | `"Congratulations! After spending many hours digging here, you have uncovered the "` |
| Original 2.0 | `"Congratulations! After spending many hours digging here, you have uncovered the "` |

Reference: `src/SOURCE/ADVMGR.cpp:2718`. Restored original 2.0 text verbatim.

### 2. `army.hit_points_left.prefix`

| Version | Text |
|---|---|
| Russian | `"Осталось здоровья: "` |
| Corrected English | `"Hit Points Left: "` |
| Original 2.0 | `"Hit Points Left: "` |

Reference: `src/SOURCE/GAME.cpp:3658`. Original 2.0 text, unchanged.

### 3. `calendar.day.label`

| Version | Text |
|---|---|
| Russian | `"День"` |
| Corrected English | `"Day"` |
| Original 2.0 | `"Day"` |

Reference: `src/SOURCE/ADVMGR.cpp:5781`. Original 2.0 text, unchanged.

### 4. `calendar.month.label`

| Version | Text |
|---|---|
| Russian | `"Месяц"` |
| Corrected English | `"Month"` |
| Original 2.0 | `"Month"` |

Reference: `src/SOURCE/ADVMGR.cpp:5760`. Original 2.0 text, unchanged.

### 5. `calendar.week.label`

| Version | Text |
|---|---|
| Russian | `"Неделя"` |
| Corrected English | `"Week"` |
| Original 2.0 | `"Week"` |

Reference: `src/SOURCE/ADVMGR.cpp:5760`. Original 2.0 text, unchanged.

### 6. `campaign.bonus.experience.label`

| Version | Text |
|---|---|
| Russian | `"Опыт"` |
| Corrected English | `"Experience"` |
| Original 2.0 | `"Experience"` |

Reference: `src/SOURCE/Campaign.cpp:665`. Original 2.0 text, unchanged.

### 7. `campaign.bonus.puzzle_pieces.label`

| Version | Text |
|---|---|
| Russian | `"Обрывки карты"` |
| Corrected English | `"Puzzle Pieces"` |
| Original 2.0 | `"Puzzle Pieces"` |

Reference: `src/SOURCE/Campaign.cpp:662`. Original 2.0 text, unchanged.

### 8. `color.abbreviated.blue`

| Version | Text |
|---|---|
| Russian | `"син."` |
| Corrected English | `"blu."` |
| Original 2.0 | No standalone counterpart identified. |

Reference: `No counterpart identified.`. Retained: no corresponding abbreviation table in the inspected 2.0 source. Full color names are not abbreviation evidence.

### 9. `color.abbreviated.green`

| Version | Text |
|---|---|
| Russian | `"зел."` |
| Corrected English | `"grn."` |
| Original 2.0 | No standalone counterpart identified. |

Reference: `No counterpart identified.`. Retained: no corresponding abbreviation table in the inspected 2.0 source. Full color names are not abbreviation evidence.

### 10. `color.abbreviated.orange`

| Version | Text |
|---|---|
| Russian | `"ор."` |
| Corrected English | `"ora."` |
| Original 2.0 | No standalone counterpart identified. |

Reference: `No counterpart identified.`. Retained: no corresponding abbreviation table in the inspected 2.0 source. Full color names are not abbreviation evidence.

### 11. `color.abbreviated.purple`

| Version | Text |
|---|---|
| Russian | `"фиол."` |
| Corrected English | `"pur."` |
| Original 2.0 | No standalone counterpart identified. |

Reference: `No counterpart identified.`. Retained: no corresponding abbreviation table in the inspected 2.0 source. Full color names are not abbreviation evidence.

### 12. `color.abbreviated.red`

| Version | Text |
|---|---|
| Russian | `"кр."` |
| Corrected English | `"red"` |
| Original 2.0 | No standalone counterpart identified. |

Reference: `No counterpart identified.`. Retained: no corresponding abbreviation table in the inspected 2.0 source. Full color names are not abbreviation evidence.

### 13. `color.abbreviated.yellow`

| Version | Text |
|---|---|
| Russian | `"жел."` |
| Corrected English | `"yel."` |
| Original 2.0 | No standalone counterpart identified. |

Reference: `No counterpart identified.`. Retained: no corresponding abbreviation table in the inspected 2.0 source. Full color names are not abbreviation evidence.

### 14. `combat.fragment.attack`

| Version | Text |
|---|---|
| Russian | `"Атака"` |
| Corrected English | `"The"` |
| Original 2.0 | No separate prefix; the sentence starts with the creature name. |

Reference: `src/SOURCE/ARMY.cpp:1281 (sentence composition; no standalone prefix in 2.0)`. Buka adds a separate attack prefix and always supplies the plural creature name; use an article, not the unrelated 2.0 UI label "Attack".

### 15. `combat.fragment.damage_points`

| Version | Text |
|---|---|
| Russian | `"ед. урона"` |
| Corrected English | `"damage"` |
| Original 2.0 | `"damage"` |

Reference: `src/SOURCE/ARMY.cpp:1286`. Original 2.0 text, unchanged.

### 16. `combat.fragment.destroy_plural`

| Version | Text |
|---|---|
| Russian | `"уничтожают"` |
| Corrected English | `"destroy"` |
| Original 2.0 | `"destroy"` |

Reference: `src/SOURCE/ARMY.cpp:1574`. Original 2.0 text, unchanged.

### 17. `combat.fragment.destroy_singular`

| Version | Text |
|---|---|
| Russian | `"уничтожает"` |
| Corrected English | `"destroys"` |
| Original 2.0 | `"destroys"` |

Reference: `src/SOURCE/ARMY.cpp:1574`. Original 2.0 text, unchanged.

### 18. `combat.fragment.dies`

| Version | Text |
|---|---|
| Russian | `"умирает"` |
| Corrected English | `"perishes"` |
| Original 2.0 | `"perishes"` |

Reference: `src/SOURCE/ARMY.cpp:1290`. Restored original 2.0 text verbatim.

### 19. `combat.fragment.does_damage`

| Version | Text |
|---|---|
| Russian | `"наносит"` |
| Corrected English | `"do"` |
| Original 2.0 | `"do"` |

Reference: `src/SOURCE/ARMY.cpp:1284`. Use the original plural verb: every Buka caller supplies gArmyNamesPlural.

### 20. `combat.fragment.killed`

| Version | Text |
|---|---|
| Russian | `"убито"` |
| Corrected English | `"perish"` |
| Original 2.0 | `"perish"` |

Reference: `src/SOURCE/ARMY.cpp:1290`. Original plural branch at ARMY.cpp:1290, not the neighboring singular "perishes".

### 21. `combat.fragment.troop`

| Version | Text |
|---|---|
| Russian | `"воин"` |
| Corrected English | `"creature"` |
| Original 2.0 | `"creature"` |

Reference: `src/SOURCE/ARMY.cpp:1424`. Restored original 2.0 text verbatim.

### 22. `combat.fragment.troops`

| Version | Text |
|---|---|
| Russian | `"воинов"` |
| Corrected English | `"creatures"` |
| Original 2.0 | `"creatures"` |

Reference: `src/SOURCE/ARMY.cpp:1424`. Restored original 2.0 text verbatim.

### 23. `combat.genie.half_army.buka`

| Version | Text |
|---|---|
| Russian | `"%s %s половину вражеских войск!"` |
| Corrected English | `"%s %s half the enemy troops!"` |
| Original 2.0 | `"%s %s half the enemy troops!"` |

Reference: `src/SOURCE/ARMY.cpp:1571`. Original 2.0 text, unchanged.

### 24. `combat.luck.bad.buka`

| Version | Text |
|---|---|
| Russian | `"Плохая удача была ниспослана на %s!"` |
| Corrected English | `"Bad luck descends on the %s"` |
| Original 2.0 | `"Bad luck descends on the %s"` |

Reference: `src/SOURCE/ARMY.cpp:1969`. Restored original 2.0 text verbatim.

### 25. `combat.luck.good.buka`

| Version | Text |
|---|---|
| Russian | `"Удача на стороне отряда %s!"` |
| Corrected English | `"Good luck shines on the %s"` |
| Original 2.0 | `"Good luck shines on the %s"` |

Reference: `src/SOURCE/ARMY.cpp:1978`. Restored original 2.0 text verbatim.

### 26. `combat.necromancy.multiple.buka`

| Version | Text |
|---|---|
| Russian | `"Использование черной магии некромантии позволило вам вернуть на службу %d поверженных врагов в виде скелетов."` |
| Corrected English | `"Practicing the dark arts of necromancy, you are able to raise %d of the enemy's dead to return under your service as Skeletons."` |
| Original 2.0 | `"Practicing the dark arts of necromancy, you are able to raise %d of the enemy's dead to return under your service as Skeletons."` |

Reference: `src/SOURCE/COMMAND.cpp:1745`. Restored original 2.0 text verbatim.

### 27. `combat.necromancy.one.buka`

| Version | Text |
|---|---|
| Russian | `"Использование черной магии некромантии позволило вам вернуть на службу поверженного врага в виде скелета."` |
| Corrected English | `"Practicing the dark arts of necromancy, you are able to raise one of the enemy's dead to return under your service as a Skeleton."` |
| Original 2.0 | `"Practicing the dark arts of necromancy, you are able to raise one of the enemy's dead to return under your service as a Skeleton."` |

Reference: `src/SOURCE/COMMAND.cpp:1753`. Restored original 2.0 text verbatim.

### 28. `combat.siege.destroyed.buka`

| Version | Text |
|---|---|
| Russian | `"\n\n%s уничтожена."` |
| Corrected English | `"\n\nThe %s is destroyed."` |
| Original 2.0 | `"\n\nThe %s is destroyed."` |

Reference: `src/SOURCE/COMMAND.cpp:3078`. Restored original 2.0 text verbatim.

### 29. `combat.siege.strength.buka`

| Version | Text |
|---|---|
| Russian | `"\n\n%s стреляет с силой %d стрелков."` |
| Corrected English | `"\n\nThe %s fires with the strength of %d Archers."` |
| Original 2.0 | `"\n\nThe %s fires with the strength of %d Archers."` |

Reference: `src/SOURCE/COMMAND.cpp:3091`. Restored original 2.0 text verbatim.

### 30. `combat.siege.strength_bonus.buka`

| Version | Text |
|---|---|
| Russian | `"\n\n%s стреляет с силой %d стрелков, каждая +%d бонусом к их уровню атаки."` |
| Corrected English | `"\n\nThe %s fires with the strength of %d Archers, each with a +%d bonus to their attack skill."` |
| Original 2.0 | `"\n\nThe %s fires with the strength of %d Archers, each with a +%d bonus to their attack skill."` |

Reference: `src/SOURCE/COMMAND.cpp:3082`. Restored original 2.0 text verbatim.

### 31. `combat.spell.resist.buka`

| Version | Text |
|---|---|
| Russian | `"%s%s сопротивляется этому заклинанию!"` |
| Corrected English | `"The %s%s resists the spell!"` |
| Original 2.0 | `"The %s %s the spell!"` |

Reference: `src/SOURCE/SPELLS.cpp:3579`. Original "The %s %s the spell!" takes creature + verb; Buka takes optional troop prefix + creature. Retain the original wording with Buka composition.

### 32. `combat.spell.resist.troop_prefix`

| Version | Text |
|---|---|
| Russian | `"Отряд "` |
| Corrected English | `"troop of "` |
| Original 2.0 | No standalone counterpart identified. |

Reference: `src/SOURCE/SPELLS.cpp:3582`. Buka-only prefix: pairs with the singular "resists" for a troop of plural-named creatures.

### 33. `combat.spell.resurrect.multiple.buka`

| Version | Text |
|---|---|
| Russian | `"%d %s воскресают из мертвых!"` |
| Corrected English | `"%d %s rise from the dead!"` |
| Original 2.0 | `"%d %s rise from the dead!"` |

Reference: `src/SOURCE/SPELLS.cpp:3503`. Original 2.0 text, unchanged.

### 34. `combat.spell.resurrect.one.buka`

| Version | Text |
|---|---|
| Russian | `"%d %s воскресает из мертвых!"` |
| Corrected English | `"%d %s rises from the dead!"` |
| Original 2.0 | `"%d %s rises from the dead!"` |

Reference: `src/SOURCE/SPELLS.cpp:3510`. Original 2.0 text, unchanged.

### 35. `combat.tower.garrison.damage.prefix`

| Version | Text |
|---|---|
| Russian | `"Гарнизон наносит"` |
| Corrected English | `"Garrison does"` |
| Original 2.0 | `"Garrison does"` |

Reference: `src/SOURCE/CMBTMGR.cpp:1723`. Restored original 2.0 text verbatim.

### 36. `combat.tower.keep.damage.prefix`

| Version | Text |
|---|---|
| Russian | `"Башня наносит"` |
| Corrected English | `"Tower does"` |
| Original 2.0 | `"Tower does"` |

Reference: `src/SOURCE/CMBTMGR.cpp:1723`. Restored original 2.0 text verbatim.

### 37. `hero.summary.buka`

| Version | Text |
|---|---|
| Russian | `"%s %d уровня %s. Артефактов: %d."` |
| Corrected English | `"%s is a level %d %s with %d artifacts."` |
| Original 2.0 | `"%s is a level %d %s with %d artifacts."` |

Reference: `src/SOURCE/TOWNMGR.cpp:2763`. Restored original 2.0 text verbatim.

### 38. `network.attack.owner.buka`

| Version | Text |
|---|---|
| Russian | `"%s, ваш %s атакован!"` |
| Corrected English | `"%s's %s is under attack!"` |
| Original 2.0 | `"%s's %s is under attack!"` |

Reference: `src/SOURCE/EVENTS.cpp:8043`. Restored original 2.0 text verbatim.

### 39. `network.directplay.guests_ready.buka`

| Version | Text |
|---|---|
| Russian | `"К вам присоединились %d гостей. Нажмите 'ОК' чтобы продолжить или подождите других игроков."` |
| Corrected English | `"You have %d guest(s) now logged in.  Click 'OK' to move on, or wait for additional guests."` |
| Original 2.0 | `"You have %d guest(s) now logged in.  Click 'OK' to move on, or wait for additional guests."` |

Reference: `src/SOURCE/dpnetwin.cpp:132`. Restored original 2.0 text verbatim.

### 40. `network.directplay.load_failed`

| Version | Text |
|---|---|
| Russian | `"Невозможно загрузить 'DPLAYX.DLL'"` |
| Corrected English | `"Can't load 'DPLAYX.DLL'"` |
| Original 2.0 | `"Can't load 'DPLAYX.DLL'"` |

Reference: `src/SOURCE/dpnetwin.cpp:95`. Restored original 2.0 text verbatim.

### 41. `network.initialization_failed`

| Version | Text |
|---|---|
| Russian | `"Ошибка инициализации сети."` |
| Corrected English | `"Network initialization failed"` |
| Original 2.0 | `"Network initialization failed"` |

Reference: `src/SOURCE/Netbios.cpp:56`. Restored original 2.0 text verbatim.

### 42. `network.modem.dialing.prefix`

| Version | Text |
|---|---|
| Russian | `"Звоню..."` |
| Corrected English | `"Dialing..."` |
| Original 2.0 | `"Dialing..."` |

Reference: `src/SOURCE/Modem.cpp:112`. Original 2.0 text, unchanged.

### 43. `network.netbios.initializing`

| Version | Text |
|---|---|
| Russian | `"Инициализация сети.\n\n  Нажмите 'ОТМЕНА', чтобы прервать соединение."` |
| Corrected English | `"Initializing network.\n\n  Press 'CANCEL' to abort."` |
| Original 2.0 | `"Initializing network.\n\n  Press 'CANCEL' to abort."` |

Reference: `src/SOURCE/Netbios.cpp:182`. Restored original 2.0 text verbatim.

### 44. `network.netbios.not_loaded`

| Version | Text |
|---|---|
| Russian | `"NETBIOS не загружена."` |
| Corrected English | `"NETBIOS is not loaded."` |
| Original 2.0 | `"NETBIOS is not loaded."` |

Reference: `src/SOURCE/Netbios.cpp:34`. Original 2.0 text, unchanged.

### 45. `network.netbios.waiting_host`

| Version | Text |
|---|---|
| Russian | `"Ожидание хоста.\n\n  Нажмите 'ОТМЕНА', чтобы прервать соединение."` |
| Corrected English | `"Waiting On Host.\n\n  Press 'CANCEL' to abort."` |
| Original 2.0 | `"Waiting On Host.\n\n  Press 'CANCEL' to abort."` |

Reference: `src/SOURCE/Netbios.cpp:202`. Restored original 2.0 text verbatim.

### 46. `network.player_exit.continue_alone.buka`

| Version | Text |
|---|---|
| Russian | `"Данная игра сохранена под названием 'Игрок вышел'. Желаете продолжить игру, где компьютер займет место выбывших игроков?"` |
| Corrected English | `"The current game has been saved as 'PLYREXIT'. Do you wish to keep playing with the computer filling in for the other humans?"` |
| Original 2.0 | `"The current game has been saved as 'PLYREXIT'. Do you wish to keep playing with the computer filling in for the other humans?"` |

Reference: `src/SOURCE/REMOTE.cpp:714`. Restored original 2.0 text verbatim.

### 47. `network.player_exit.exiting_confirm.buka`

| Version | Text |
|---|---|
| Russian | `"%s покидает игру. Данная игра сохранена под названием 'ИГРОК ВЫШЕЛ'. Желаете продолжить игру, с компьютером, занявшим место %s?"` |
| Corrected English | `"%s is exiting the game.  The current game has been saved as 'PLYREXIT'.  Do you wish to continue playing with a computer player filling in for %s?"` |
| Original 2.0 | `"%s is exiting the game.  The current game has been saved as 'PLYREXIT'.  Do you wish to continue playing with a computer player filling in for %s?"` |

Reference: `src/SOURCE/KB.cpp:4554`. Restored original 2.0 text verbatim.

### 48. `network.player_exit.host_terminated.buka`

| Version | Text |
|---|---|
| Russian | `"%s покидает игру, а %s решает прекратить всю игру. Игра была сохранена под названием 'ИГРОК ВЫШЕЛ'. Желаете продолжить игру самостоятельно, где компьютер займет места выбывших игроков?"` |
| Corrected English | `"%s left the game, and the %s decided to terminate the entire game.  The game has been saved as 'PLYREXIT'.  Do you wish to play on alone with the computer taking over for all human players?"` |
| Original 2.0 | `"%s left the game, and the %s decided to terminate the entire game.  The game has been saved as 'PLYREXIT'.  Do you wish to play on alone with the computer taking over for all human players?"` |

Reference: `src/SOURCE/KB.cpp:4437`. Restored original 2.0 text verbatim.

### 49. `network.player_exit.timed_out_confirm.buka`

| Version | Text |
|---|---|
| Russian | `"%s вышел из игры. Данная игра сохранена под названием 'ИГРОК ВЫШЕЛ'. Желаете продолжить игру, с компьютером, занявшим место %s?"` |
| Corrected English | `"%s has been timed out of the game.  The current game has been saved as 'PLYREXIT'.  Do you wish to continue playing with a computer player filling in for %s?"` |
| Original 2.0 | `"%s has been timed out of the game.  The current game has been saved as 'PLYREXIT'.  Do you wish to continue playing with a computer player filling in for %s?"` |

Reference: `src/SOURCE/KB.cpp:4545`. Restored original 2.0 text verbatim.

### 50. `network.tcp.host.guest_progress.buka`

| Version | Text |
|---|---|
| Russian | `"Создание игры по адресу %s.\n\nУ вас %d гостей из ожидавшихся %d гостей. Нажмите 'ОТМЕНА', чтобы продолжить игру, не дожидаясь  остальных гостей."` |
| Corrected English | `"Hosting game at %s.\n\nYou have %d guest(s) out of an expected total of %d guest(s) now logged in.  Click 'CANCEL' to move on without waiting for additional guests."` |
| Original 2.0 | `"Hosting game at %s.\n\nYou have %d guest(s) out of an expected total of %d guest(s) now logged in.  Click 'CANCEL' to move on without waiting for additional guests."` |

Reference: `src/SOURCE/Wsnetwin.cpp:99`. Restored original 2.0 text verbatim.

### 51. `network.tcp.host.guests_ready.buka`

| Version | Text |
|---|---|
| Russian | `"Создание игры на %s.\n\nУ вас %d гостей. Нажмите 'ОК', чтобы продолжить или подождите других игроков."` |
| Corrected English | `"Hosting game at %s.\n\nYou have %d guest(s) now logged in.  Click 'OK' to move on, or wait for additional guests."` |
| Original 2.0 | `"Hosting game at %s.\n\nYou have %d guest(s) now logged in.  Click 'OK' to move on, or wait for additional guests."` |

Reference: `src/SOURCE/Wsnetwin.cpp:135`. Restored original 2.0 text verbatim.

### 52. `network.waiting_guest.buka`

| Version | Text |
|---|---|
| Russian | `"Ожидание гостя.\n\n  Нажмите 'ОТМЕНА', чтобы прервать соединение."` |
| Corrected English | `"Waiting On Guest.\n\n  Press 'CANCEL' to abort."` |
| Original 2.0 | `"Waiting On Guest.\n\n  Press 'CANCEL' to abort."` |

Reference: `src/SOURCE/Netbios.cpp:187`. Restored original 2.0 text verbatim.

### 53. `network.waiting_player_start`

| Version | Text |
|---|---|
| Russian | `"Ожидаю игрока для начала игры."` |
| Corrected English | `"Waiting for other remote player to set up game."` |
| Original 2.0 | `"Waiting for other remote player to set up game."` |

Reference: `src/SOURCE/Wsnetwin.cpp:411`. Restored original 2.0 text verbatim.

### 54. `new_game.rating.label`

| Version | Text |
|---|---|
| Russian | `"Рейтинг"` |
| Corrected English | `"Rating"` |
| Original 2.0 | `"Rating"` |

Reference: `src/SOURCE/Newgame.cpp:998`. Original 2.0 text, unchanged.

### 55. `recruit.available.prefix`

| Version | Text |
|---|---|
| Russian | `"Доступно: "` |
| Corrected English | `"Available: "` |
| Original 2.0 | `"Available: "` |

Reference: `src/SOURCE/RECRUIT.cpp:87`. Original 2.0 text, unchanged.

### 56. `requester.map.size_mismatch.multiple.buka`

| Version | Text |
|---|---|
| Russian | `"Нет карт такого размера для %d игроков-людей."` |
| Corrected English | `"No maps exist for %d human player at that size."` |
| Original 2.0 | `"No maps exist for %d human player at that size."` |

Reference: `src/SOURCE/REQUEST.cpp:665`. Legacy ID says multiple, but this is the giNumHumanPlayers == 1 branch; restore the original singular message without changing the ID or Russian bytes.

### 57. `requester.map.size_mismatch.one.buka`

| Version | Text |
|---|---|
| Russian | `"Нет карты этого размера для %d игроков-людей."` |
| Corrected English | `"No maps exist for %d human players at that size."` |
| Original 2.0 | `"No maps exist for %d human players at that size."` |

Reference: `src/SOURCE/REQUEST.cpp:671`. Legacy ID says one, but this is the plural branch; restore the original plural message without changing the ID or Russian bytes.

### 58. `save.filename.autosave`

| Version | Text |
|---|---|
| Russian | `"Автосохранение"` |
| Corrected English | `"AUTOSAVE"` |
| Original 2.0 | `"AUTOSAVE"` |

Reference: `src/SOURCE/GAME.cpp:1148`. Original 2.0 text, unchanged.

### 59. `save.filename.new_game`

| Version | Text |
|---|---|
| Russian | `"НОВАЯ ИГРА"` |
| Corrected English | `"NEWGAME"` |
| Original 2.0 | `"NEWGAME"` |

Reference: `src/SOURCE/GAME.cpp:1279`. Restored original 2.0 text verbatim.

### 60. `save.filename.player_exit`

| Version | Text |
|---|---|
| Russian | `"Игрок Вышел"` |
| Corrected English | `"PLYREXIT"` |
| Original 2.0 | `"PLYREXIT"` |

Reference: `src/SOURCE/GAME.cpp:1149`. Restored original 2.0 text verbatim.

### 61. `save.filename.victory_prefix`

| Version | Text |
|---|---|
| Russian | `"ПОБЕДА_"` |
| Corrected English | `"WIN_"` |
| Original 2.0 | `"WIN_"` |

Reference: `src/SOURCE/KB.cpp:2590`. Original 2.0 text, unchanged.

### 62. `scenario.fragment.castle`

| Version | Text |
|---|---|
| Russian | `"замок"` |
| Corrected English | `"castle"` |
| Original 2.0 | `"castle"` |

Reference: `src/SOURCE/Newgame.cpp:2031`. Original 2.0 text, unchanged.

### 63. `scenario.fragment.hero`

| Version | Text |
|---|---|
| Russian | `"герой"` |
| Corrected English | `"Hero"` |
| Original 2.0 | `"Hero"` |

Reference: `src/SOURCE/EVENTS.cpp:8045`. Original capitalized Hero at the network attack call site; shared town fragment remains lowercase because it also appears inside scenario sentences.

### 64. `scenario.fragment.town`

| Version | Text |
|---|---|
| Russian | `"город"` |
| Corrected English | `"town"` |
| Original 2.0 | `"town"` |

Reference: `src/SOURCE/Newgame.cpp:2031`. Original 2.0 text, unchanged.

### 65. `scenario.loss.settlement.buka`

| Version | Text |
|---|---|
| Russian | `"Потерять %s '%s'."` |
| Corrected English | `"Lose the %s '%s'."` |
| Original 2.0 | `"Lose the %s '%s'."` |

Reference: `src/SOURCE/Newgame.cpp:2030`. Restored original 2.0 text verbatim.

### 66. `scenario.side.and`

| Version | Text |
|---|---|
| Russian | `" и "` |
| Corrected English | `" and "` |
| Original 2.0 | `" and "` |

Reference: `src/SOURCE/Newgame.cpp:2178`. Original 2.0 text, unchanged.

### 67. `scenario.side.enemy.prefix`

| Version | Text |
|---|---|
| Russian | `"враг - "` |
| Corrected English | `"the enemy - "` |
| Original 2.0 | `"the enemy - "` |

Reference: `src/SOURCE/Newgame.cpp:2188`. Original 2.0 text, unchanged.

### 68. `scenario.side.enemy_alliance.prefix`

| Version | Text |
|---|---|
| Russian | `"вражеский союз "` |
| Corrected English | `"the enemy alliance of "` |
| Original 2.0 | `"the enemy alliance of "` |

Reference: `src/SOURCE/Newgame.cpp:2186`. Restored original 2.0 text verbatim.

### 69. `scenario.side.you_and_allies.prefix`

| Version | Text |
|---|---|
| Russian | `"Вы и ваши союзники "` |
| Corrected English | `"You and your allies "` |
| Original 2.0 | `"You and your allies "` |

Reference: `src/SOURCE/Newgame.cpp:2164`. Original 2.0 text, unchanged.

### 70. `scenario.side.you_and_ally.prefix`

| Version | Text |
|---|---|
| Russian | `"Вы и ваш союзник "` |
| Corrected English | `"You and your ally "` |
| Original 2.0 | `"You and your ally "` |

Reference: `src/SOURCE/Newgame.cpp:2166`. Original 2.0 text, unchanged.

### 71. `scenario.victory.capture_settlement.buka`

| Version | Text |
|---|---|
| Russian | `"Захватить %s '%s'"` |
| Corrected English | `"Capture the %s '%s'"` |
| Original 2.0 | `"Capture the %s '%s'"` |

Reference: `src/SOURCE/Newgame.cpp:2079`. Restored original 2.0 text verbatim.

### 72. `scenario.victory.standard_alternative.suffix`

| Version | Text |
|---|---|
| Russian | `", или вы можете выиграть, уничтожив всех вражеских героев и захватив все вражеские города и замки."` |
| Corrected English | `", or you may win by defeating all enemy heroes and capturing all enemy towns and castles."` |
| Original 2.0 | `", or you may win by defeating all enemy heroes and capturing all enemy towns and castles."` |

Reference: `src/SOURCE/Newgame.cpp:2127`. Restored original 2.0 text verbatim.

### 73. `system.audio.initialization_no_device`

| Version | Text |
|---|---|
| Russian | `"Ошибка инициализации звука!  Не найдено устройство."` |
| Corrected English | `"Sound initialization error!  No wave devices found."` |
| Original 2.0 | `"Sound initialization error!  No wave devices found."` |

Reference: `src/BASE/soundmgr.cpp:389`. Restored original 2.0 text verbatim.

### 74. `system.cdrom.read_error.retry`

| Version | Text |
|---|---|
| Russian | `"Ошибка чтения диска Героев 2. Повторить?"` |
| Corrected English | `"Error reading the Heroes 2 Expansion CD.  Retry?"` |
| Original 2.0 | `"Error reading the Heroes 2 Expansion CD.  Retry?"` |

Reference: `src/SOURCE/SMACKMGR.cpp:214`. Restored original 2.0 text verbatim.

### 75. `system.command_line.disable_digital_sound`

| Version | Text |
|---|---|
| Russian | `"/D0 - отключить цифровой звук\n"` |
| Corrected English | `"/D0 - disable digital sound\n"` |
| Original 2.0 | `"/D0 - disable digital sound\n"` |

Reference: `src/SOURCE/KB.cpp:9616`. Original 2.0 text, unchanged.

### 76. `system.command_line.disable_midi`

| Version | Text |
|---|---|
| Russian | `"/M0 - отключить MIDI музыку\n"` |
| Corrected English | `"/M0 - disable MIDI music\n"` |
| Original 2.0 | `"/M0 - disable MIDI music\n"` |

Reference: `src/SOURCE/KB.cpp:9617`. Original 2.0 text, unchanged.

### 77. `system.command_line.disable_music`

| Version | Text |
|---|---|
| Russian | `"/R0 - отключить музыку\n"` |
| Corrected English | `"/R0 - disable Redbook music\n"` |
| Original 2.0 | `"/R0 - disable Redbook music\n"` |

Reference: `src/SOURCE/KB.cpp:9618`. Restored original 2.0 text verbatim.

### 78. `system.command_line.disabled_example`

| Version | Text |
|---|---|
| Russian | `"Звук отключен и интро пропущено.\n"` |
| Corrected English | `"sound disabled and the intro skipped.\n"` |
| Original 2.0 | `"sound disabled and the intro skipped.\n"` |

Reference: `src/SOURCE/KB.cpp:9627`. Restored original 2.0 text verbatim.

### 79. `system.command_line.dos_example`

| Version | Text |
|---|---|
| Russian | `"Загрузить DOS версию Героев 2.\n"` |
| Corrected English | `"Starts the DOS version of Heroes2 with redbook\n"` |
| Original 2.0 | `"Starts the DOS version of Heroes2 with redbook\n"` |

Reference: `src/SOURCE/KB.cpp:9626`. Restored original 2.0 text verbatim.

### 80. `system.command_line.example`

| Version | Text |
|---|---|
| Russian | `"Пример:\n"` |
| Corrected English | `"Example:\n"` |
| Original 2.0 | `"Example:\n"` |

Reference: `src/SOURCE/KB.cpp:9622`. Original 2.0 text, unchanged.

### 81. `system.command_line.skip_intro`

| Version | Text |
|---|---|
| Russian | `"/I0 - пропустить интро\n"` |
| Corrected English | `"/I0 - skip the intro\n"` |
| Original 2.0 | `"/I0 - skip the intro\n"` |

Reference: `src/SOURCE/KB.cpp:9619`. Restored original 2.0 text verbatim.

### 82. `system.display.palette_required`

| Version | Text |
|---|---|
| Russian | `"Для Героев II требуется режим в 256 цветов или больше.\n\nЧтобы изменить режим цветности, щелкните правой кнопкой по рабочему столу Windows и выберите 'Properties'/Свойства'. Затем в установках выберите глубину цвета."` |
| Corrected English | `"Heroes II requires 256 color mode or higher.\n\nTo change color mode, right click in an open area on the Windows 95 background, choose 'Properties', then the 'Settings' tab, then change the entry in the 'Color Palette Box'."` |
| Original 2.0 | `"Heroes II requires 256 color mode or higher.\n\nTo change color mode, right click in an open area on the Windows 95 background, choose 'Properties', then the 'Settings' tab, then change the entry in the 'Color Palette Box'."` |

Reference: `src/SOURCE/wingraph.cpp:893`. Restored original 2.0 text verbatim.

### 83. `system.single_instance`

| Version | Text |
|---|---|
| Russian | `"Только одна копия %s может быть запущена одновременно"` |
| Corrected English | `"Only one copy of %s may run at a time"` |
| Original 2.0 | `"Only one copy of %s may run at a time"` |

Reference: `src/SOURCE/kbwin.cpp:28`. Restored original 2.0 text verbatim.

### 84. `system.title.full`

| Version | Text |
|---|---|
| Russian | `"Герои Меча и Магии II"` |
| Corrected English | `"Heroes of Might and Magic II"` |
| Original 2.0 | `"Heroes of Might and Magic II"` |

Reference: `src/SOURCE/kbwin.cpp:28`. Original 2.0 text, unchanged.

### 85. `system.title.short`

| Version | Text |
|---|---|
| Russian | `"Герои II"` |
| Corrected English | `"Heroes II"` |
| Original 2.0 | `"Heroes II"` |

Reference: `src/SOURCE/kbwin.cpp:25`. Original 2.0 text, unchanged.

### 86. `town.army.owner.garrison`

| Version | Text |
|---|---|
| Russian | `"гарнизона"` |
| Corrected English | `"Garrison"` |
| Original 2.0 | `"Garrison"` |

Reference: `src/SOURCE/TOWNMGR.cpp:1989`. Restored original 2.0 text verbatim.

### 87. `town.army.owner.hero`

| Version | Text |
|---|---|
| Russian | `"героя"` |
| Corrected English | `"Hero's Army"` |
| Original 2.0 | `"Hero's Army"` |

Reference: `src/SOURCE/TOWNMGR.cpp:1989`. Restored original 2.0 text verbatim.

### 88. `town.army.split.buka`

| Version | Text |
|---|---|
| Russian | `"Как много %s перенести из армии %s в армию %s?"` |
| Corrected English | `"Move how many %s troops from %s to %s?"` |
| Original 2.0 | `"Move how many %s troops from %s to %s?"` |

Reference: `src/SOURCE/TOWNMGR.cpp:1987`. Restored original 2.0 text verbatim.

### 89. `town.recruit.new_hero`

| Version | Text |
|---|---|
| Russian | `"Нанять нового героя"` |
| Corrected English | `"Recruit Hero"` |
| Original 2.0 | `"Recruit Hero"` |

Reference: `src/SOURCE/KB.cpp:9923`. Reuse the original 2.0 recruitment UI label (KB.cpp:9923); the corresponding cCastleInfo entry "Recruit %s the %s" requires two arguments absent in Buka.

### 90. `trading.bargain.buka`

| Version | Text |
|---|---|
| Russian | `"{%s}\n\nНа моем рынке %s и %s меняются из соотношения %d %s к %d %s"` |
| Corrected English | `"{%s}\n\n%s for %s: I can offer you %d %s for %d %s."` |
| Original 2.0 | `"{%s}\n\nI can offer you %d %s of %s for %d %s of %s."` |

Reference: `src/SOURCE/tradpost.cpp:101`. Original offer wording, with resource names moved before quantities to preserve Buka argument order: title, received resource, paid resource, received amount/unit, paid amount/unit. No positional printf extension or argument reordering.

### 91. `trading.unit`

| Version | Text |
|---|---|
| Russian | `"ед."` |
| Corrected English | `"units"` |
| Original 2.0 | `"units"` |

Reference: `src/SOURCE/tradpost.cpp:104`. Original 2.0 text, unchanged.
