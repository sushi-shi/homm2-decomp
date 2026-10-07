# `va_end` is a store, not a no-op

Measured with the pinned VC6 SP5 compiler and headers, 2026-10-07.

## Mechanism

The pinned `STDARG.H` defines, for `_M_IX86`,

```c
#define va_end(ap)      ( ap = (va_list)0 )
```

(the empty definition at line 68 is the MIPS branch). At `/Od` every
`va_end` is therefore a seven-byte store of zero to the `va_list` slot:

```
int sum_no_end(int n, ...)                   int sum_end(int n, ...)
{ va_list ap; va_start(ap, n);               { ...same...; va_end(ap);
  int s = va_arg(ap, int); return s; }         return s; }

8b 42 fc     mov eax,[edx-4]                 8b 42 fc     mov eax,[edx-4]
89 45 fc     mov [ebp-4],eax                 89 45 fc     mov [ebp-4],eax
                                             c7 45 f8 00 00 00 00
                                                          mov dword [ebp-8],0
8b 45 fc     mov eax,[ebp-4]                 8b 45 fc     mov eax,[ebp-4]
```

## Signature and use

A retail variadic function whose exits carry no zero store to the `va_list`
slot omits `va_end` on those paths. Keep the omission; adding `va_end` is not
byte-neutral.

`SOURCE/netwin` `nb_sess` (RVA 0x74372, 1,223 bytes) omits it on all four
exits. Adding it to both early-zero returns, the default return and the common
result gave 1,254 bytes with the same 31 blocks and five longer blocks.
