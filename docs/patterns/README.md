# patterns - VC4.2 /O2 codegen idioms for matching

When a function's objdiff residual is a *steerable* codegen idiom (a source spelling that makes
MSVC 4.2 emit the retail instructions), document it here so it isn't re-derived. One file per
idiom: `docs/patterns/<short-name>.md` with **symptom** / **cause** / **fix** (tiny before/after)
/ **tags** (e.g. `cpp:switch`, `asm:neg`, `topic:regalloc`, `topic:wall`). Add a one-line entry
to `INDEX.md` in the SAME change. `topic:wall` = correct code at a non-steerable plateau (stop).

Fresh catalog: VC4.2 (cl 10.20) idioms differ from later MSVC; nothing ports from other decomps.
