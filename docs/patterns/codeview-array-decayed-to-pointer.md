# CodeView array decayed to pointer (global `T*` that is really `T[]`)

**Tags:** o2, od, globals, codeview, array
**Symptom:** a global that the code indexes (`g[i]`, `&g + off`) is declared `T *g` in its owner
header, and functions that index it plateau — every `g[i]` emits an extra pointer load
(`mov eax,[g]` then `[eax+i]`) that retail (a direct array `[g+i]`) doesn't have.

## The tell

Old MSVC (2.x/4.x) CodeView mangles a global **array** `T g[N]` with a **pointer-decayed type**
(`PAE...`/`PAH...`/`PAJ...`), but the **symbol-table SIZE field holds the true byte count**:

```
?bSaveMusicPosition@@3PAEA  size 0x3c   ->  unsigned char [60], not unsigned char*
?lastAggZ@@3PAHA            size 0x28   ->  int [10],           not int*
?lastPositionZ@@3PAJA       size 0x30   ->  long [12],          not long*
```

A pointer is 4 bytes; any `PA..` symbol with **size > 4** whose storage is indexed as an array
is a decayed array.

## Fix

Change the owner-header decl AND the owner-TU definition from `T *g;` to `T g[SIZE];`
(get SIZE from `grep <name> build/gen/symbol_names.csv`). Nothing else changes — every `g[i]`
now emits the direct `[g + i]` addressing.

### Real asm (resourceManager::SavePosition, before/after)

```
; before  (int *lastAggZ) — 0% region
mov  ecx, dword ptr [<addr>]        ; load the lastAggZ POINTER   reloc ?lastAggZ
mov  dword ptr [ecx + eax*4], edx   ; *(ptr + idx) = agg
; after   (int lastAggZ[10]) — byte-exact
mov  dword ptr [<addr> + eax*4], edx ; lastAggZ[idx] = agg        reloc ?lastAggZ  (direct)
```

## Result
`bSaveMusicPosition`/`bMusicIsLooping` → lifted MIDIStop/MIDIPlay + unlocked soundManager::Open.
`lastAggZ`/`lastPositionZ` → made resourceManager::AddResource/SavePosition/RestorePosition
**byte-exact (100%)**. Same shape as the array-as-pointer accesses that instead keep the pointer
type and use `reinterpret_cast<T*>(&g)[i]` (e.g. `iLastVolume`, `SCS`).
