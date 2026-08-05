# A by-value class argument's position in the parameter list is byte-visible at the call site

**Trigger.** A `__fastcall` call whose argument list mixes register-eligible
arguments (`int`, `int&`, pointers) with one **by-value class** argument that needs a
constructor. Retail computes the register arguments FIRST, parks them in `esi`/`edi`
(dragging `push esi`/`push edi` into the prologue), and constructs the class
temporary LAST. Ours constructs the temporary immediately after the trailing stack
push and loads `ecx`/`edx` last, with no `esi`/`edi` at all. CFG, block count,
relocations and every immediate agree; only three block sizes differ.

**The rule.** VC6 `/Od` evaluates arguments strictly **right to left**, and a
register-passed argument that is evaluated *before* a call must survive it, so it is
copied into a callee-saved register. Therefore the class argument's **source
position** decides the whole shape:

- class argument LAST-but-one (`f(a, b, C, d)`): order `d`, `C`, `b`, `a` — the
  register args come after the ctor call, so nothing is preserved.
- class argument FIRST (`f(C, a, b, d)`): order `d`, `b`, `a`, `C` — `b` and `a` are
  evaluated before the ctor call and land in `esi`/`edi`.

**The callee cannot tell you which.** `__fastcall` assigns `ecx`/`edx` to the first
two *register-eligible* parameters and skips the class one, and the stack args are
pushed right to left, so `f(a, b, C, d)` and `f(C, a, b, d)` produce an
**identical callee frame**: `ecx = a`, `edx = b`, `C` at `0x8(%ebp)`, `d` at
`0xc(%ebp)`. `BASE/AudiereMusic`'s `PlayAudiereMusic` stayed byte-for-byte EXACT
across the reorder. The delinked call target's mangled name is a *source claim*, not
evidence — the parameter order is recoverable only from the caller.

## The divergence

`soundManager::PlayAmbientMusic` (RVA 0xb64d0), the audiere arm.

```
retail (exact)                                    ours (82.46%)
------------------------------------------------- -------------------------------------------------
55                 pushl  %ebp                     55                 pushl  %ebp
8b ec              movl   %esp, %ebp               8b ec              movl   %esp, %ebp
83 ec 14           subl   $0x14, %esp              83 ec 14           subl   $0x14, %esp
56                 pushl  %esi                     (none)
57                 pushl  %edi                     (none)
...
8b 45 08           movl   0x8(%ebp), %eax          8b 45 08           movl   0x8(%ebp), %eax
50                 pushl  %eax          ; track    50                 pushl  %eax          ; track
8b 4d f8           movl   -0x8(%ebp), %ecx         8b 4d f8           movl   -0x8(%ebp), %ecx
83 c1 4a           addl   $0x4a, %ecx   ; &steps   83 c1 42           addl   $0x42, %ecx   ; &device
8b f1              movl   %ecx, %esi               51                 pushl  %ecx          ; alloc temp
8b 55 f8           movl   -0x8(%ebp), %edx         8b d4              movl   %esp, %edx
83 c2 4e           addl   $0x4e, %edx   ; &track   89 65 fc           movl   %esp, -0x4(%ebp)
8b fa              movl   %edx, %edi               51                 pushl  %ecx
8b 45 f8           movl   -0x8(%ebp), %eax         8b ca              movl   %edx, %ecx
83 c0 42           addl   $0x42, %eax   ; &device  e8 ..              calll  RefPtr::RefPtr
51                 pushl  %ecx          ; alloc    89 45 f0           movl   %eax, -0x10(%ebp)
8b cc              movl   %esp, %ecx               8b 55 f8           movl   -0x8(%ebp), %edx
89 65 fc           movl   %esp, -0x4(%ebp)         83 c2 4a           addl   $0x4a, %edx
50                 pushl  %eax                     8b 4d f8           movl   -0x8(%ebp), %ecx
e8 ..              calll  RefPtr::RefPtr           83 c1 4e           addl   $0x4e, %ecx
89 45 f0           movl   %eax, -0x10(%ebp)        e8 ..              calll  PlayAudiereMusic
8b cf              movl   %edi, %ecx
8b d6              movl   %esi, %edx
e8 ..              calll  PlayAudiereMusic
```

Retail is 9 bytes longer: 4 for `push`/`pop esi,edi`, 6 for the two `mov esi,ecx` /
`mov edi,edx` parks plus the reload pair, less 1 because the shifted register
rotation later in the body reaches the short `25` form of `and $0xff, %eax`.

The residual is inert to source respelling and to compiler state: eight call-site
spellings (one line, `this->`-qualified, an explicit `audiere::AudioDevicePtr(...)`
temporary, `static_cast`, parenthesised operands, `*&`) and a 50-trial `forest`
TU-state census all produced exactly one byte state, 82.459770%, 20/20 blocks,
`edges=0 pred=0 flow=0 shift=0`. An inert body search is a declaration defect.

## The probe that decided it

One compile with the unit's own flags (`/Od /MT /Gr /G5 /Ob1 /Gf /Gi- /GX`):

```cpp
class RefLike {
public:
    RefLike(const RefLike& other) { m_ptr = other.m_ptr; }
    ~RefLike() { m_ptr = 0; }          // the dtor is what adds `mov [ebp-4],esp`
    void* m_ptr;                       // and the ctor-result temp slot
};
void Callee (int& a, int& b, RefLike c, int d);
void Callee5(RefLike c, int& a, int& b, int d);

void Holder::A(int t) { Callee (m_track, m_steps, m_dev, t); }   // -> our shape
void HolderD(Holder* h, int t) {
    Callee5(h->m_dev, h->m_track, h->m_steps, t);                // -> retail's shape
}
```

`HolderD` reproduces retail instruction for instruction, including `push esi`/`push
edi`, `mov esi,ecx` / `mov edi,edx`, the `mov ecx,esp` that only fits because
`&m_dev` sits in `eax`, and the closing `mov ecx,edi` / `mov edx,esi`.

## What made it match

```cpp
// include/BASE/soundBackends.h + src/BASE/AudiereMusic.cpp
void PlayAudiereMusic(i32& currentTrack, i32& fadeSteps, audiere::AudioDevicePtr device, i32 track);  // ours
void PlayAudiereMusic(audiere::AudioDevicePtr device, i32& currentTrack, i32& fadeSteps, i32 track);  // retail

// src/BASE/soundmgr.cpp
PlayAudiereMusic(m_musicTrack, m_musicFadeSteps, m_audiereDevice, track);   // 82.46%
PlayAudiereMusic(m_audiereDevice, m_musicTrack, m_musicFadeSteps, track);   // EXACT
```

`PlayAmbientMusic` 82.46% -> EXACT (BASE/soundmgr 26/26), and `PlayAudiereMusic`
itself stayed EXACT: the reorder is byte-neutral in the callee.
