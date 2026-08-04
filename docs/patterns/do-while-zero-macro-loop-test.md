# A `do { ... } while (0)` macro emits a real loop test at /Od

**Trigger.** Three ours-only instructions — `xor <reg>,<reg>` / `test <reg>,<reg>` /
`jne <back to the start of the macro body>` — sitting immediately after a statement
group that came from a helper macro. VC6 at `/Od` does not fold the constant `while (0)`
away: it emits the zero, tests it and emits a backward `jne` that is never taken. The
`xor` also consumes the next register in the per-function round-robin, so **every
statement after the macro is register-rotated by one** and the whole tail of the function
mis-compares even where the source is already right.

Measured on `advManager::RecruitSiteEvent` (RVA 0x42952) and `advManager::GenericSiteEvent`
(RVA 0x42256); the same three instructions are still visible in `philAI::EvaluateGenericSite`
(RVA 0x8aacf), which uses the same macro.

The macro is `H2_ENUM_DECODE_MASKED` from `include/Ints.h`; its production (non-strict)
expansion is

```c
#define H2_ENUM_DECODE_MASKED(type, target, raw, mask)                                             \
    do {                                                                                           \
        (target) = (raw);                                                                          \
        (target) &= (mask);                                                                        \
    } while (0)
```

## The divergence

```
ours                                          retail
--------------------------------------------- ---------------------------------------------
81 e1 ff ff 00 00  andl  $0xffff, %ecx        81 e1 ff ff 00 00  andl  $0xffff, %ecx
89 4d f4           movl  %ecx, -0xc(%ebp)     89 4d f0           movl  %ecx, -0x10(%ebp)
8b 55 f4           movl  -0xc(%ebp), %edx     8b 55 f0           movl  -0x10(%ebp), %edx
83 e2 07           andl  $0x7, %edx           83 e2 07           andl  $0x7, %edx
89 55 f4           movl  %edx, -0xc(%ebp)     89 55 f0           movl  %edx, -0x10(%ebp)
33 c0              xorl  %eax, %eax           8b 45 08           movl  0x8(%ebp), %eax
85 c0              testl %eax, %eax           66 8b 48 04        movw  0x4(%eax), %cx
75 d8              jne   <macro body start>   66 c1 e9 03        shrw  $0x3, %cx
8b 4d 08           movl  0x8(%ebp), %ecx      ...
66 8b 51 04        movw  0x4(%ecx), %dx
66 c1 ea 03        shrw  $0x3, %dx
...
```

Note the register rotation that follows: retail resumes at `%eax`, ours at `%ecx`,
because the dead `xorl %eax, %eax` took a turn. That single stolen register kept
`RecruitSiteEvent` at 94.38% even though every later statement was already correct.

## What made it match

Spell the two statements out at the call site instead of using the macro; keep the
strict-enum build compiling by casting the mask into the domain (production expands
`static_cast<Enum>(x)` to `(x)`, so the bytes cannot change):

```cpp
    H2_ENUM_DECODE_MASKED(                                       // ours
        RecruitSiteType, siteType2, cell->m_objectMetadata, EVENT_RECRUIT_TYPE_MASK
    );
```
```cpp
    recruitSiteType = cell->m_objectMetadata;                    // retail
    recruitSiteType &= static_cast<RecruitSiteType>(EVENT_RECRUIT_TYPE_MASK);
```

For a plain `enum class` local (no `H2_ENUM_STORAGE` wrapper) the second statement is

```cpp
    siteType4 = static_cast<GenericSiteType>(IDX(siteType4) & GENERIC_SITE_TYPE_MASK);
```

`RecruitSiteEvent` 94.38% -> EXACT, `RecruitSiteAIEvent` 92.22% -> EXACT,
`GenericSiteEvent` 93.71% -> EXACT, `GenericSiteAIEvent` 91.80% -> EXACT.
