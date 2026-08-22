# `dpnetwin` `"TCP"` relocation closure

The four bytes of the compiler-generated string `"TCP\0"` at retail RVA
`0xf1bbc` are `54 43 50 00`.  Read as a little-endian dword, they form the
in-image address `0x00504354`, so the generic image-only relocation sweep
misclassified the string payload as a `DIR32` field and the delinker replaced
it with an unrelated symbol-relative addend.

The candidate COFF emits the four-byte string with no relocation, and automatic
compiler-string attribution independently places that exact payload at
`.data+0x1c`.  Excluding the false relocation restores the retail bytes without
adding a source `DATA_COMPGEN` wrapper.
