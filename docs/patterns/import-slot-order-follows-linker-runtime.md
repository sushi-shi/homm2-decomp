# Import slot order follows LINK's C runtime

Measured with the pinned VC6 SP5 `LINK.EXE` under Wine on the historical
`HMM2PL.exe` link, 2026-10-06.

## Mechanism

`LINK.EXE` imports `qsort` from `MSVCRT.DLL` and sorts each DLL's
`.idata$4`/`.idata$5` contributions with it. The sort key is the DLL, so the
imports of one DLL compare equal and their final ILT/IAT order is whatever
that `qsort` leaves for equal keys. Import thunk placement in `.text` and the
`.idata$6` hint/name blob keep the member pull order; only the slots are
permuted.

Equal-key behavior differs between runtimes. Wine's builtin `msvcrt` uses a
median-of-three `qsort`; the VC6-era native `MSVCRT.DLL` uses a middle pivot.
Under Wine, which one LINK loads therefore decides the slot order.

## Measurement

The same response file (`build/link/historical/HMM2PL.rsp`, objects,
libraries and resources unchanged) was linked twice by a renamed copy of
`LINK.EXE`, once with the prefix's builtin `msvcrt` and once with a native
VC6-era `MSVCRT.DLL` selected for that executable name alone by a Wine
`AppDefaults\<exe>\DllOverrides` entry:

| LINK runtime | DLLs whose IAT order differs from retail | differing `.rdata` bytes | differing `.text` bytes |
| --- | ---: | ---: | ---: |
| Wine builtin `msvcrt` | 0 of 11 | 8 (debug-directory stamp and PDB record of the one-pass probe) | 496 (known residual) |
| native `MSVCRT.DLL` | 10 of 11 | 648 | 1,399 (+903 thunk operands) |

Both images import exactly the same names, ordinals and hints. Retail
`HMM2PL.exe` reproduces the builtin (median-of-three) order in every DLL, so
the ordinary build keeps Wine's builtin runtime; no override is configured.

## Use

When an ILT/IAT is a permutation of retail's while thunks and hint/name
strings already match, test the linker's C runtime before object order,
archive packaging, or forcing roots: those change the pull order and so also
move thunks and hint/name strings, which a pure slot permutation does not.

Earlier differences recorded in
[idata-thunk-order-is-resolution-history](idata-thunk-order-is-resolution-history.md)
also moved thunks and hint/name strings; they were pull-order (archive and
undefined-symbol resolution) differences and closed with the reconstructed
archive order. This note covers only the slot permutation that remains once
pull order is exact.
