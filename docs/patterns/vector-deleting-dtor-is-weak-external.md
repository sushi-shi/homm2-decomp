# The vector deleting destructor in a vtable is a weak external to `??_G`

Measured on the pinned VC6 SP5 objects of this reconstruction (199 candidate
and delinked comparison inputs), 2026-09.

## Mechanism

For a class with a virtual destructor, VC6 fills the vtable's destructor slot
with `??_E<Class>@@UAEPAXI@Z` (vector deleting destructor) and emits that
symbol as a COFF weak external (`IMAGE_SYM_CLASS_WEAK_EXTERNAL`). Its
`AuxWeakExternal` record names `??_G<Class>@@UAEPAXI@Z` (scalar deleting
destructor) as the default, and the same object defines `??_G`. Unless some
object supplies a strong `??_E`, LINK resolves the slot to `??_G`.

## Signature

A vtable that differs only in the destructor slot's relocation symbol:
`??_E` in the compiled object, `??_G` in the delinked retail target. The
bytes and every other slot agree.

## Use

Compare the slot by following the weak external to its default (and then to
a same-name definition) before reporting a data difference; it is not a
source difference. In this corpus there are 15 distinct weak `??_E` names and
none has a strong definition anywhere, so the resolution is always `??_G`.

Abstract slots are a separate case: they name `__purecall`, the 9-byte VC6
`purevirt.obj` body `push 0x19; call __amsg_exit; pop ecx; ret` (retail RVA
0xd9061).
