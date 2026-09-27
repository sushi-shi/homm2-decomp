# In-process VC6 queue and function-body trace

Measured on 2026-09-27 in matcher-2, branch `matcher/bss-audiere`.
This is diagnostic instrumentation, not a production compiler or linker change.
The actual source is the integration-ready 115-byte Audiere parent under
`build/audiere-unsplit-integration/candidate/`; its include search then uses the
root matcher-4 headers, exactly as recorded in the response files. No production
source, original tool binary, or Wine configuration was changed.

## Reproduction and binary control

From the lane's persistent `nix develop .#build` shell, with `HOMM2_DIR`
resolving to matcher-2:

```
python3 build/audiere-inprocess-trace/bootstrap.py
python3 build/audiere-inprocess-trace/actual.py
```

The artifact directory contains the proxy source, commands, response files,
compiler logs, complete traces, and `equality.json`. CL.EXE and its support DLLs
were copied to `tool/`. The real frontend was renamed `C1XX_REAL.DLL`; a VC6-built
proxy exports and forwards `_AbortCompilerPass@4` and `_InvokeCompilerPass@12`.
The proxy registers a vectored exception handler and uses temporary memory-only
INT3/single-step breakpoints. The real DLL on disk remains unchanged. Both the
original and renamed C1XX hashes are:

```
f014b3bee650224adf6cb44e51f0eeac5abbf5da666fa299d5250b2f3d1937c6
```

A tiny semantic template compile succeeded first. The actual Audiere control and
instrumented compiles both returned zero with no timeout. The initial three-hook
run produced byte-identical objects, SHA256
`335c58f7f2e00bca26b75661320546e3728395eea24254cc1b0a35bccdd8f46d`.
The final six-hook run differs at only file offset 4, in the COFF timestamp;
every section's bytes, characteristics, size, symbols, and complete ordered
relocation records are identical. No anonymous-symbol normalization is needed
for that equality. It logged 3,535 selected events and one INIT entry; the
100,000-trap safety cutoff did not fire. Initial and final trace audits are kept
separately (`actual-trace.log.first`, `equality.json.first`, and final files).
This equality validates the diagnostic observation against an ordinary same-source
compile; it does not establish an exact retail executable.

## Hook meanings

| Address | Observation |
| --- | --- |
| 10427303 | Queue insertion entry, ECX queue and stack arguments |
| 1041c5bb | Pending-registration entry, not proof of insertion |
| 1044b373 | Static initializer processing entry; increments trace phase |
| 104161d8 | Function-body setup; first argument is the function symbol |
| 10417f43 | Function-body teardown just before clearing current symbol |
| 10425394 | Synthesized member dispatcher; second argument is member symbol |

The symbol decoder follows symbol+8 to the interned name handle, then handle+4
to its text. The signed owner index at symbol+6 indexes the pointer table rooted
at `DAT_104D75C0`. The current function is `DAT_104E6A88`; setup104161d8 returns
its input symbol (104162d4), and teardown10417f43 clears that global.

The first-run label `SYMBOL current` is deliberately not used as evidence: it
read `DAT_104D7620`, an interned-name global, rather than the current function.
The final trace replaces it with `SYMBOL current_function` using the verified
function global. Opaque queue stream objects are not decoded as symbols.

## Actual Node observations

The concrete class owner is `?$AudiereSampleListNode@Vsample@@`, owner pointer
`78708f8c`. Its user-defined destructor symbol is `7870934c` and has name
`__dtor`, kind byte+0x32 equal to **0**, tag8, and flag byte+0x38 equal to01.
Final event1731 enters1041c5bb with that symbol. Because bit08 is clear, this
entry does **not** append it to the pending table. The initial trace records the
same observation at event833. There is no final dispatcher event identifying
this Node destructor through10425394.

The Node deleting destructor is a distinct symbol `78709454`, name`__delDtor`,
kind6. Its first pending-registration entry has flag09; later entries have01.
The dispatch hook and subsequent function-body setup independently identify
that deleting destructor. This separates it from the real 44-byte Node helper.

Final events3272 and3274 identify the real Node destructor at function setup and
teardown. Both are phase0, before the first initializer entry at event3527.
The setup caller returns at10419607, the ordinary member-body path through
104195fe..1041960d. During that body, event3273 sees the real OutputStream RefPtr
destructor call-registration entry while the current function remains Node.
Thus this is identification by exact member/owner pointer and body context,
not chronological association with an unrelated queued helper.

This demonstrates pre-initializer body processing. It does not identify a
unique A/B/C queue item for the Node, nor equate frontend body processing with
backend COFF emission. The ordinary same-source object separately establishes
its retained output order S A N G R. No queue kind should be assigned solely
from the position of this function in the trace.

## The observed post-initializer template callback

Final event3532 queues opaque item `78aa6108` to queueC (`104da6c0`) from caller
1040af34, the specialization scanner. Event3533 resumes that same item through
`104da710`; its IL cursor is107f. Event3534 then sets up a function with the same
IL cursor: symbol `78aa60b4`, name`__ctor`, owner`?$pair@PBIPBI` (the standard
pair of two const unsigned-int pointers). Teardown3535 preserves that identity.
Consequently the observed post-INIT item is not the Node or a ctype destructor.
The trace confirms a real post-initializer template route, without demonstrating
that the current Node can enter it.

The ctype registration wrapper itself is identified at event3528 as
`?id@?$ctype@G@std@@$E`. Subsequent pending-entry events3530/3531 identify its
guarded initializer and `$E20` while current function is `$E21`.

## Bounded conclusion

The user-defined Node destructor is a kind0 ordinary member, not the compiler's
synthesized kind4 destructor. Its observed call does not register it in the
pending table, and its body is processed before INIT on this exact source
parent. The corrected static kind table therefore cannot by itself provide a
mechanism to delay this member. A future ownership hypothesis needs to explain
how this actual ordinary-member body becomes eligible only after initialization;
changing pending-kind labels is not such an explanation. This is a boundary on
this measured parent, not a universal impossibility claim.

`node-phase-excerpt.log` preserves the selected Node and initializer events.
All proxy code and compiled probe objects remain disposable under build/.
