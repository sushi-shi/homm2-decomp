# Post-merge comparison refresh

The merge `5831026d6` integrated the published reconstruction checkpoint into
native ownership recovery. The subsequent build reported 1,692/1,727 exact
functions against an older 1,727/1,727 checkpoint. That total was reported
before regenerating the source-owned retail symbol model. This was an
integration verification omission, not evidence that the losses were owned
by the separate native-linker campaign.

## Observed stale identities

At RVA `0xc5df0`, `bitmap::bitmap(u32l)` has identical disassembled
instructions on both sides. Its differing call identity is:

```text
compiled: ?ReadBlock@resourceManager@@QAEXPAXK@Z
retail comparison: ?ReadBlock@resourceManager@@QAEXPACK@Z
```

The merged header changes `ReadBlock(i8*, u32l)` to `ReadBlock(void*, u32l)`
and `Read13(i8*)` to `Read13(char*)`. The pre-refresh report has 33 scored
project functions below 100%, plus two unpaired old definitions of those
methods. The merge also replaces the executive text aggregate with separate
annotated owners, combines Miles sample state, and restores separately
annotated search scratch owners. Such changes require the source-owned
symbol and data manifests to be regenerated before comparison.

The existing retail-only local-name `#define` fixes remain present. This
correction changes no reconstructed function, local, argument, helper,
member, storage layout, macro fix, or retail byte. It runs the existing
regeneration workflow and records its result. The existing drift checker
still expects an obsolete private-symbol ledger and has additional compiler
data classification drift; enabling it on ordinary builds was tested and
withheld because it produced warnings even after a verified full refresh.
Repairing that checker is separate work, not part of this checkpoint.

## Reproduction

Use a separate worktree at the merged commit, enter its own build shell,
and verify `pwd`, branch, and `HOMM2_DIR` before running:

```sh
homm2 redelink
homm2 build
homm2 model-drift
homm2 relocs --resolved
homm2 relocs --addends
```

Regeneration updates disposable comparison inputs. It does not patch the
retail control or the reconstructed executable. The control's SHA-256 is
`bc7e9c9320aa3e5c1ffca6d2bfa530ecedb5a3bca1b91c959501c15ad72c329a`.
The generated retained maxima are produced by the build, not hand-edited.

## Review boundary and earlier direct changes

The correction is proposed on `fix/post-merge-comparison-refresh` and must
remain unmerged for user review. The naming review is separately in PR #65;
none of its 80 proposed renames has been applied.

Earlier direct changes made in this task, before the user clarified the PR
boundary, are recorded here rather than presented as part of this PR diff:

- `5831026d6`: reconstruction reconciliation merge, already pushed. The user
  approved its KB/Misc split, upstream edits, baseline refresh, and obsolete
  script deletion conflict resolutions. Exact comparison regeneration was
  missed after integration.
- `a7c401fb4` and `e0689d3f7`: initial naming documentation and generated
  ledger refresh, local on the reconstruction branch; not pushed by this task.
  The expanded naming review is committed separately in PR #65.
- Generated exports, already pushed: `1eb6bc513` (PoL source), `1c7cd09be`
  (PoL classic), `6ccd8bd62` (Gold source), `2a4ff4a20` (Gold classic).
- The divergent `master`, `ironfist`, and `ironfist-master` branches were
  reconciled and pushed at `c89cf4058`, `2cb93a6a7`, and `c0c754804`.

This PR does not rewrite those published histories. All further corrections
and naming implementation must be proposed for review before integration.

## Verified refresh result

The full isolated build reports **1,727/1,727 exact functions**, 100% matched
code bytes, 100% fuzzy, and **291,987/291,987 data bytes**. All 35 pre-refresh
non-exact claims resolve by the same retail RVA and are exact in the refreshed
report. This is current evidence, not a retained-max substitution.

`homm2 relocs --resolved` passes all 1,727 functions and **38,307 ordered
relocation sites**, with zero structural review items. The focused bitmap
constructor has no remaining instruction/relocation diff after refresh.

The broader `homm2 relocs --addends` raw-name diagnostic does **not** pass:
it reports 9 value-set rows in 8 functions, 7,922 one-sided rows, 152 code-local
rows, and 3 missing objects. It compares raw names, including compiler literals
and changed owners, separately from the canonical exact comparison. These
auxiliary findings are recorded rather than treated as full raw-name closure.
The separate native linked-image campaign also remains outside this result;
exact function comparisons do not imply an exact final executable.

Artifacts in the isolated worktree: `build/post-merge-{redelink,build}.log`,
`build/post-merge-losses-before.json`, `build/post-merge-resolved.log`,
`build/post-merge-addends.log`, and `build/gen/function_reloc_addends.json`.
No generated comparison objects or build caches are committed.

| Unit | Function at the retail RVA | Before refresh | After refresh |
| --- | --- | --- | --- |
| `SOURCE/ARMY` | `public: void __thiscall army::LoadResources(void)` (`0x18b7d`) | 99.98168% | 100% |
| `SOURCE/GAME` | `public: void __thiscall game::ViewArmy(int, int, int, int, class town *, int, int, int, class hero *, class army *, class armyGroup *, int)` (`0x55529`) | 99.99379% | 100% |
| `SOURCE/SEARCH` | `public: void __thiscall searchArray::SeedPosition(int, int, int, int, int, int, int, int, int, int, int, int)` (`0x917d6`) | 98.69830% | 100% |
| `BASE/WINMGR` | `public: void __thiscall heroWindowManager::FizzleForward(int, int, int, int, int, signed char *, signed char *)` (`0xb79b0`) | 99.98576% | 100% |
| `BASE/RESMGR` | `public: void __thiscall resourceManager::GetBackdrop(char *, class bitmap *, int)` (`0xb7fa0`) | 99.90000% | 100% |
| `BASE/RESMGR` | `public: void __thiscall resourceManager::GetBackdropAtLoc(char *, class bitmap *, int, int, int)` (`0xb8040`) | 99.92538% | 100% |
| `BASE/RESMGR` | `public: void __thiscall resourceManager::Read13(char *)` (`0xb8f40`) | unpaired | 100% |
| `BASE/RESMGR` | `public: void __thiscall resourceManager::ReadBlock(void *, unsigned long)` (`0xb8f60`) | unpaired | 100% |
| `BASE/MOUSEMGR` | `public: void __thiscall mouseManager::SetPointer(int)` (`0xb95e0`) | 99.97214% | 100% |
| `BASE/ICONWDGT` | `public: void __thiscall iconWidget::Read(void)` (`0xbb890`) | 99.92754% | 100% |
| `BASE/ICON` | `public: __thiscall icon::icon(unsigned long)` (`0xc26f0`) | 99.90385% | 100% |
| `BASE/TEXTWDGT` | `public: void __thiscall textWidget::Read(void)` (`0xc3090`) | 99.87013% | 100% |
| `BASE/FONT` | `public: __thiscall font::font(unsigned long)` (`0xc3620`) | 99.91525% | 100% |
| `BASE/EXEC` | `public: int __thiscall executive::InitSystem(void)` (`0xc4f60`) | 99.63636% | 100% |
| `BASE/EXEC` | `public: int __thiscall executive::DoDialog(class baseManager *)` (`0xc50d0`) | 99.78947% | 100% |
| `BASE/EXEC` | `public: void __thiscall executive::PrintManagerList(void)` (`0xc5240`) | 99.43396% | 100% |
| `BASE/EXEC` | `public: void __thiscall executive::CallManager(class baseManager *)` (`0xc5500`) | 99.76190% | 100% |
| `BASE/EXEC` | `public: void __thiscall executive::Terminate(void)` (`0xc5700`) | 99.44444% | 100% |
| `BASE/BITMAP` | `public: __thiscall bitmap::bitmap(unsigned long)` (`0xc5df0`) | 99.92308% | 100% |
| `BASE/PALETTE` | `public: __thiscall palette::palette(unsigned long)` (`0xcaf40`) | 99.88372% | 100% |
| `BASE/BORDER` | `public: void __thiscall border::Read(void)` (`0xcb250`) | 99.87342% | 100% |
| `BASE/Blur` | `void __fastcall DoBlur(class bitmap *, class bitmap *, int, int, int, int)` (`0xcba60`) | 99.99118% | 100% |
| `BASE/MilesSound` | `void __fastcall StopMilesSampleHandle(struct _SAMPLE *)` (`0xcdb90`) | 99.84849% | 100% |
| `BASE/MilesSound` | `void __fastcall AllocateMilesSampleHandles(struct _DIG_DRIVER *)` (`0xcdc00`) | 99.48276% | 100% |
| `BASE/MilesSound` | `void __fastcall SetMilesSampleHandleVolume(struct _SAMPLE *, int)` (`0xcdc60`) | 99.76190% | 100% |
| `BASE/MilesSound` | `void __fastcall StopAllMilesSamples(void)` (`0xcdcf0`) | 99.58334% | 100% |
| `BASE/MilesSound` | `void __fastcall PlayMilesSample(class sample *)` (`0xcddf0`) | 99.89655% | 100% |
| `BASE/MilesSound` | `void __fastcall AdjustMilesSampleVolumes(void)` (`0xcdfd0`) | 99.69697% | 100% |
| `BASE/TILESET` | `public: __thiscall tileset::tileset(unsigned long)` (`0xce090`) | 99.92754% | 100% |
| `BASE/SAMPLE` | `public: __thiscall sample::sample(char *)` (`0xce250`) | 99.97340% | 100% |
| `BASE/SAMPLE` | `public: __thiscall MIDIWrap::MIDIWrap(char *)` (`0xce520`) | 99.91071% | 100% |
| `BASE/listbox` | `public: void __thiscall listBoxWidget::Read(void)` (`0xce840`) | 99.96491% | 100% |
| `BASE/droplist` | `public: void __thiscall dropListWidget::Read(void)` (`0xcff40`) | 99.94898% | 100% |
| `BASE/Textntry` | `public: void __thiscall textEntryWidget::Read(int)` (`0xd1f60`) | 99.93056% | 100% |
| `BASE/BUTTON` | `public: void __thiscall button::Read(void)` (`0xd3710`) | 99.92754% | 100% |
