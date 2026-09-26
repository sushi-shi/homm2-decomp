// VC6 SP5 source family o07-newgame-setup-owner; parent f84e0b7c plus U18 incoming packet header.
// Except O07 itself, parent includes O07 recovered outgoing300-byte owner.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state search is claimed.
// build/source-variant-batch/o07-newgame-setup-owner/results.json; RVA 0x7734a
// Complete 3 arms; elapsed 3.397133s; source restored.
// Disposition: retain whole_reserved300 owner, not125-byte shrink; two copies60+65/send125 within one owner.
// Private label/SG/EH counters can change relocation hashes without changing destinations.
// Final native objects/retail sites and targets decide retention, not fuzzy scores.
// Existing four EH/FS model gaps remain open; reviewed _stricmp alias is pinned separately.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 3537 / a0a83514955879df / da8573cd3d2ea607 / 161:161 / 160,1,0,0 / True
// 1 / {'source_family': 'whole_reserved', 'candidate': 'baseline'} / 3537 / a0a83514955879df / d917a82cb0289086 / 161:161 / 160,1,0,0 / True
// 2 / {'source_family': 'whole_wire', 'candidate': 'baseline'} / 3537 / 5aae5cb40f67c383 / d917a82cb0289086 / 161:161 / 160,1,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// | char mapNamePacket[MAP_HEADER_NAME_SIZE];
// ALTERNATIVE whole_reserved (unified delta from BEFORE):
// | --- before
// | +++ whole_reserved
// | @@ -1 +1 @@
// | -char mapNamePacket[MAP_HEADER_NAME_SIZE];
// | +char mapNamePacket[MAP_HEADER_NAME_SIZE + GAME_SETUP_BUFFER_SIZE];
// REQUIRED simultaneous companion edit:
// | --- before
// | +++ whole_reserved
// | @@ -1 +0,0 @@
// | -        char setupData[GAME_SETUP_BUFFER_SIZE];
// REQUIRED simultaneous companion edit:
// | --- before
// | +++ whole_reserved
// | @@ -1 +1 @@
// | -memcpy(setupData, gpGame->m_setupPlayerColor, GAME_SETUP_DATA_SIZE);
// | +memcpy(mapNamePacket + MAP_HEADER_NAME_SIZE, gpGame->m_setupPlayerColor, GAME_SETUP_DATA_SIZE);
// ALTERNATIVE whole_wire (unified delta from BEFORE):
// | --- before
// | +++ whole_wire
// | @@ -1 +1 @@
// | -char mapNamePacket[MAP_HEADER_NAME_SIZE];
// | +char mapNamePacket[GAME_SETUP_PACKET_SIZE];
// REQUIRED simultaneous companion edit:
// | --- before
// | +++ whole_wire
// | @@ -1 +0,0 @@
// | -        char setupData[GAME_SETUP_BUFFER_SIZE];
// REQUIRED simultaneous companion edit:
// | --- before
// | +++ whole_wire
// | @@ -1 +1 @@
// | -memcpy(setupData, gpGame->m_setupPlayerColor, GAME_SETUP_DATA_SIZE);
// | +memcpy(mapNamePacket + MAP_HEADER_NAME_SIZE, gpGame->m_setupPlayerColor, GAME_SETUP_DATA_SIZE);
