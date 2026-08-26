# An implicit polymorphic destructor does not store the class vptr

**Symptom.** A destructor COMDAT destroys a member but never writes the owning
class's vtable. Declaring the destructor explicitly adds that write even when
the out-of-line body is empty.

Measured on `soundManager::~soundManager` (RVA `0x70df0`, VC6 SP5 `/Od /Ob1`).
The incorrect user-declared form opened with the extra store; retail proceeds
directly from the `this` spill to the member at `+0x42`:

```text
user-declared probe                         retail / implicit
mov  eax, [ebp-8]                          mov  eax, [ebp-8]
mov  dword ptr [eax], OFFSET ??_7...        add  eax, 42h
add  eax, 42h                              mov  [ebp-4], eax
mov  [ebp-4], eax                          cmp  dword ptr [ecx], 0
```

The retained retail body is 0x37 bytes and byte-identical, including the
Audiere `RefPtr` release call and complete ordered relocations.

**Close.** Remove the destructor declaration and definition so the compiler
generates it implicitly. This is the inverse of
[user-dtor-out-of-line](user-dtor-out-of-line.md): first inspect the vptr store,
then choose the destructor ownership.

