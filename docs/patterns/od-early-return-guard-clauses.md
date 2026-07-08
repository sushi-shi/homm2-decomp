# Early return / guard clauses — the NWC devs' default control-flow shape

**Tags:** topic:od topic:control-flow toolchain:vc42
**Symptom:** a `/Od` function whose whole body is wrapped in a guard `if` — or whose error
paths flow through a result local — plateaus in the 60–90s. The retail emits **guard clauses
(early returns)**, and every `&&` in the guard is its own conditional exit.

## The tell

The original developers wrote **early returns everywhere** — it is by far the most common
`/Od` shape in this binary, so it is the FIRST rewrite to try. A guarded body compiles to a
`je <continue>; jmp <epilogue>` pair (jump-to-body / jump-to-return), NOT the single `jne`
that a nested `if(cond){ body }` produces:

```
; retail (guard clause: `if (gbNoSound != 0) return;`)
cmp   dword ptr [gbNoSound], 0
je    continue          ; ==0 -> run the body
jmp   epilogue          ; !=0 -> return
continue:
...

; ours BEFORE (nested `if (gbNoSound == 0) { body }`)
cmp   dword ptr [gbNoSound], 0
jne   epilogue          ; single jump over the body  <-- one instruction, wrong shape
...
```

Multi-condition guards fan out — `if (A && B && C) { body }` in retail is three separate
`cmp; je continue; jmp epilogue` triples, one per term, not a short-circuit chain into one block.

## Fix

Invert the guard `if` into early returns; make error paths return **directly** (`return 3;`,
which is `mov eax,3; jmp epilogue`, or `xor eax,eax` for `return 0`) rather than assigning a
result local and falling through to a single `return iVar;`.

```cpp
// BEFORE                                  // AFTER (matches retail)
if (gbNoSound == 0) {                      if (gbNoSound != 0) return;
    ... big body ...            ==>        ... big body ...          // brace dropped
}

if (gbNoSound == 0 && ready != 0) {        if (gbNoSound != 0) return;
    ...                         ==>        if (ready == 0) return;
}                                          ...
```

**Slot-safe:** dropping the outer brace only removes a scope; as long as the function's
locals are declared at the top (before the guard) — which they are at `/Od` — the frame is
unchanged. Confirm with `--base`/`--lite` that `(%ebp)` displacements didn't move.

## Result
This single rewrite drove essentially the whole soundmgr pass (79→85%) and much of RESMGR
(Open, LoadAggregateHeader, Dispose) and the resource getters. Functions taken to **100%
EXACT** by it: CDStop, CDShutdown, CDIsPlaying, ForcePollSound, SetReady2Poll, ConvertVolume,
resourceManager::Open, and more. See also [[od-cell-access-and-block-jmps]] and the sibling
shapes (if/else-both-return two-jmp, `for`→`while`, branch inversion) in the memory
catalog `od-control-flow-shapes`.
