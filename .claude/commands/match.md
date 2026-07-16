---
description: Run the HoMM2 matching campaign as a fan-out pipeline — N matchers always in flight across a reused worktree pool, every result integrated SERIALLY into a single linear master history.
argument-hint: [n-matchers]
allowed-tools: Agent, Bash, Read, Write, Edit, Grep, Glob
---

You are the **parallel match orchestrator** in the top-level session (so you CAN
dispatch subagents). Drive the campaign per **`.claude/agents/orchestrator.md`**
(pool + serial integration + target selection) and **`.claude/agents/matcher.md`**
(the /Od reconstruction doctrine). HEROES2W.EXE is one binary — go straight to the queue.

Pool size / concurrency: **N = $1** (default **4**). Always keep N matchers in flight
until the queue is dry or the user winds down.

**Builds run cd-first, ideally inside one open shell** (each worktree is self-contained:
`HOMM2_DIR=$PWD` → its own `build/` + wineprefix). `cd <worktree>` BEFORE entering the
shell — never `nix develop … --command 'cd <dir> && …'`.

In short (full rules in the two agent docs):

1. **Pool (persistent, reused):** slots `matcher-1 … matcher-N` under `.claude/worktrees/`.
   On startup, reuse any that exist (`git -C … reset --hard master`; their `build/`
   survives — no cold re-provision); only create + provision missing slots
   (`orchestrator.md` § Pool setup). A restart does NOT regenerate the pool.
2. **Queue:** regenerate objdiff, then select **every live non-100% function** in descending
   fuzzy-percentage order (closest to exact first). `@early-stop` and `@semantic` never remove a
   function from this residual-audit queue; reproduce or reject their claims. Note src `VA()` carries
   **absolute VAs (RVA + 0x400000)** while the queue lists RVAs — normalise before any
   hand cross-check.
3. **Fan out:** N background matchers (`subagent_type="matcher"`, `run_in_background: true`,
   **NOT** `isolation: worktree`). Each prompt: absolute worktree path + `cd` there first,
   absolute paths, a **whole-TU 20+ function batch** (RVA/name/size each, in retail-RVA
   order) for **one TU**, the 8-digit **absolute-VA** convention, the
   **`scripts/od_slots.py` stack-naming workflow**, account for every residual byte and external
   relocation, and push to 100% or a newly reproduced byte-proven `@early-stop`,
   report per-fn % + one-line summary + full `git diff`. **Lane discipline:** each lane
   owns one TU and works it in 20+ batches until done, then takes the highest remaining TU-safe batch.
   **Any new `docs/patterns/*.md` MUST carry real asm (retail vs ours, side by side) +
   what made it match** — never prose alone (`docs/patterns/INDEX.md` header). Two
   `@early-stop` flavors are legit: permanent reloc/delinker artifacts, and the soft
   `tu-cumulative-eval-order` class (resolves as sibling TU funcs land). `@semantic` means
   behavior/structure/types/frame/CFG/relocations are complete and only binary code shape remains;
   it is not a byte-wall claim.
4. **Integrate SERIALLY:** one at a time — guard master clean → apply only that matcher's
   file(s) → `homm2 build` (recompiles + regenerates README's match block) → confirm % →
   `homm2 status update` (`--accept-regressions` only for trivial cross-fn fuzzy drift) →
   commit those files + `config/match_baseline.tsv` + **`README.md`** (ALWAYS stage the
   regenerated README so the scoreboard never drifts; do NOT stage `config/match-queue.md`)
   as `match: <fn> -> <result>` with the Co-Authored-By trailer. One matcher = one commit.
   **Refill immediately:** reset the slot to master, pick next, dispatch.
5. **Stop** when no live non-100% function remains or the user winds down: let in-flight matchers
   finish, integrate, print the ledger (`fn -> result -> commit`) + a regressions summary.
   **Leave the `matcher-N` worktrees in place**.

Keep your context SMALL — hold only the ledger; never pull a matcher's disassembly or
diffs into this session beyond the file(s) you integrate.
