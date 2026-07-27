// SetGameDefaults, RVA 0x004c49a0
//
// Retail loop evidence:
//
//     mov ebx, [giMainVideoModeWidth]
//     mov [eax-20], edi
//     mov [eax-16], ecx
//     cmp ebx, edx
//     mov [eax-12], ecx
//     mov dword ptr [eax+4], 0
//     mov [eax], edi
//
// The back edge returns to the global load, so a function-wide cached width is
// contradictory.  The following per-iteration lifetimes were reviewed:
//
//     do {
//         i32 mainVideoModeWidth = giMainVideoModeWidth;
//         ...
//         if (mainVideoModeWidth <= DEFAULT_WINDOW_WIDTH) {
//             ...
//         }
//     } while (...);
//
//     i32 mainVideoModeWidth;
//     do {
//         mainVideoModeWidth = giMainVideoModeWidth;
//         ...
//     } while (...);
//
//     do {
//         i32 mainVideoModeWidth;
//         <show-menu assignment>
//         mainVideoModeWidth = giMainVideoModeWidth;
//         ...
//     } while (...);
//
// A loop-head initializer introduces the missing load, but clean MSVC assigns
// the width to ECX and hoists zero into EBX: 94.447365% with the retained
// pointer order (438 bytes).  Assigning after show-menu reaches 97.096490%
// (438 bytes), but its register/constants island and instruction order are less
// retail-like than the retained 96.780700% direct-read shape.
//
// The complete surviving matrix was three reviewed shapes crossed with clean
// plus 50 TU states (153/153 builds, not truncated).  No state exceeded the
// clean 97.096490% clue and none closed exactly.
//
// Artifacts:
//   build/set-game-defaults-video-width-clean/
//   build/set-game-defaults-video-width-states/
//   build/set-game-defaults-width-assignment-clean/
//   build/set-game-defaults-survivor-states/
//
// Disposition: do not retain a width local.  Preserve the 97.096490% branch as
// clue evidence; retain the structurally stronger 96.780700% clean source.
