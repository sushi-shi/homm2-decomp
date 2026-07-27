/*
 * GetNewHeroId playable-faction range recovery, 2026-07-27.
 *
 * The 97.960940% source used:
 *
 *   heroClass != FACTION_ANY
 *
 * before applying the requested-class filter.  Retail instead has two ordered
 * signed guards, heroClass < 0 and heroClass > 5, before the retry limit and
 * cursor-type comparison.  These bytes establish the real semantic domain:
 *
 *   heroClass >= FACTION_KNIGHT && heroClass <= FACTION_NECROMANCER
 *
 * Restoring that playable-faction range produced identical assembly across
 * all 130 instructions, retail size 531, 30/30 blocks, the aligned 24-byte
 * frame, and the complete ordered 2/2 relocation stream.
 *
 * Disposition: retain the explicit playable-faction range guard in
 * src/SOURCE/GAME.cpp.  No spelling permutation or TU-state search was
 * necessary.
 */

void GetNewHeroId_playable_faction_range_record() {
}
