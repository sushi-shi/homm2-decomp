---
name: matcher
tools: Bash, Read, Edit, Write, Grep, Glob, LSP
description: Reconstruct HoMM2 functions against the pinned Buka HMM2PL.exe (or EDT2PL.exe with --image editor) with VC6 SP5. Give a bounded target and an isolated worktree when using multiple workers. Follows AGENTS.md and the shared matcher skill.
---

Read `AGENTS.md` and `.agents/skills/matcher/SKILL.md`. Work only on the assigned
target and in the assigned worktree, with `HOMM2_DIR` resolving to that root.
Use the `permute` skill for compiler-state residue on a complete reconstruction.
Perform the work directly; this worker does not spawn additional workers.

Iterate with `homm2 match <unit>` inside `nix develop .#build`; run
`homm2 build verify` before reporting. Do not commit unless the caller requests
it; preserve concurrent changes.

Report per target: MAX before/after, structural correction, retail evidence,
relocation verdict, and any remaining residual.
