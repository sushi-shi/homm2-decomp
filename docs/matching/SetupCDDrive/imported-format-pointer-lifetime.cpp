/*
SetupCDDrive, retail RVA 0x000c5a60, size 0x3ed.
Imported formatter pointer lifetime, 2026-07-27.

The retained retry-loop scope already reproduces the retail 0x2f0 frame and
all 27 basic blocks. Current source differed in only two adjacent imported
pointer loads: it loaded mciSendStringA into ESI before wsprintfA into EBP,
whereas retail loads wsprintfA first. The clean object was 99.935486%, size
1005, with exact block flow and the same relocation count.

An unchanged-source top-of-TU census completed 50/50 forest states and found
one normalized state only:

  build/setup-cd-drive-top-states-20260727.json
  build/tu-state-noise/setup-cd-drive-top-20260727

Five direct/single/combined inline call owners were then crossed with the same
clean plus 50 states. All 255/255 variants remained byte-identical to the
99.935486% object:

  build/setup-cd-drive-iat-inline-axes-20260727.json
  build/setup-cd-drive-iat-inline-manifest-20260727.json
  build/match-variants/setup-cd-drive-iat-inline-20260727/results.json

The distinct imported-function-pointer matrix tested direct calls, formatter
then sender, sender then formatter, formatter only, and sender only. It
completed 255/255 variants:

  build/setup-cd-drive-iat-pointer-axes-20260727.json
  build/setup-cd-drive-iat-pointer-manifest-20260727.json
  build/match-variants/setup-cd-drive-iat-pointer-20260727/results.json

Both clean formatter-first arms are audited exact: 100.000000%, retail size
1005, exact 27/27 blocks, and complete ordered relocation identity/addends.
Sender-first and sender-only reproduce the old object. Several generated
forest states retain equal instruction bytes but renumber private relocation
identities, so they correctly fail exact closure; the retained clean source
does not depend on a compiler-state probe.

Disposition: retain the minimal formatter-only pointer. Its lexical lifetime
expresses the real loop-invariant imported callable, forces retail load order,
and makes SetupCDDrive live exact without generated declarations or steering.
*/

#if 0
typedef i32 (__cdecl *CDFormatCommand)(char*, const char*, ...);
CDFormatCommand formatCommand = wsprintfA;

formatCommand(command, gMiscText.cd.openAudioCommand.text, driveLetter);
#endif
