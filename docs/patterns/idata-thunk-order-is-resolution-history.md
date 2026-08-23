# Intra-DLL import order is resolution-history evidence

When the DLL descriptor order and each DLL's complete import ABI agree, a different
order inside the ILT/IAT is not by itself a missing import or a source defect. Microsoft
LINK assigns those slots while resolving the global undefined-symbol worklist. The order
therefore depends on whole-link symbol-table and archive-member history, not just the
order of a `.def` file or import library.

The final-link audit consequently keeps two independent results:

- `complete_abi_matches_retail` requires the exact DLL set and, per DLL, the exact
  names or ordinals and hint values.
- `complete_iat_order_matches_retail` reports the raw linker order. When the ABI and
  DLL descriptor order are exact, a remaining order difference is classified as a
  `resolution-history-wall`; it is not normalized out of the raw section-byte report.

This follows the locally measured Gruntz policy, adapted to Buka's VC6 linker. Gruntz's
VC5 probes showed that import-member pull order follows undefined-symbol resolution,
while final slot order has another global contribution-order layer. Object-list order,
archive packaging, and retail-ordered import archives did not move slots toward retail;
one extra early import reference scrambled only that DLL.

## Buka VC6 probe

The inherited homm2 forcing list came from the older LINK 3.00 reconstruction. It is not
a Buka input: the full list names Miles and Smacker imports absent from Buka and fails the
VC6 link with an unresolved `_SmackSoundUseDirectSound@4`.

A bounded WinG-only A/B used the six still-valid `/INCLUDE` roots. The unforced candidate
order was:

```
WinGStretchBlt, WinGSetDIBColorTable, WinGRecommendDIBFormat,
WinGCreateDC, WinGCreateBitmap, WinGBitBlt
```

The forced order became:

```
WinGCreateBitmap, WinGRecommendDIBFormat, WinGSetDIBColorTable,
WinGBitBlt, WinGStretchBlt, WinGCreateDC
```

Neither equals retail:

```
WinGRecommendDIBFormat, WinGSetDIBColorTable, WinGStretchBlt,
WinGBitBlt, WinGCreateDC, WinGCreateBitmap
```

The probe preserved the exact ABI and all source-function RVAs but increased raw `.rdata`
differences from 3,489 to 3,589 bytes. The forcing was reverted. Synthetic `/INCLUDE`
roots are not retained as a byte-steering mechanism.

The actionable inputs remain DLL descriptor order, exact import identities, ordinals, and
hints. Raw IAT order remains useful evidence if later authentic source, object-symbol, CRT,
or archive recovery naturally changes LINK's resolution history.

## Raw Buka link: Gruntz-style semantic closure

The raw VC6 LINK output now carries the same explicit logical byte audit as Gruntz. It pairs
the descriptor pointer fields and both slot dwords by `(DLL, name-or-ordinal)`, while comparing
descriptor timestamps/forwarders, padded DLL strings, padded hint/name records, array
terminators, and the null descriptor literally. Because VC6 merges imports into `.rdata`, the
surrounding padding and unrelated `.rdata` contributions are not attributed to imports.

Measured on `build/link/HMM2PL.raw.exe`: 240/240 imports paired, 6,374/6,374 attributable
logical bytes exact, exact DLL descriptor order, no missing/extra identities, and nine DLLs
with a different raw intra-DLL order. No executable byte is rewritten for this result.
