/*
Unit: BASE/AudiereEffects
Retail storage: 0x005395c0..0x005395db

The seven reconstructed scalar names described one contiguous 0x1c-byte state
record. Compiling them separately gives VC6 a four-byte-aligned .bss section,
so LINK places the contribution at 0x005395bc. Retail begins it at 0x005395c0.

Modeling the same fields as AudiereEffectsState preserves every field address
and every function byte while making the ordinary VC6 section eight-byte
aligned, as required for a record larger than eight bytes. Substituting that
unmodified object into the raw LINK inputs keeps .text and .data byte-exact.
No padding member, alignment attribute, or post-link adjustment is involved.

The separate AudiereSampleNode destructor COMDAT-order residual is unchanged
by this recovery and remains an independent source-shape question.
*/
