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
2. **Queue:** `nix develop .#build --command python3 -m homm2.match.gen_queue`, then read
   `config/match-queue.md`. Filter out already-reconstructed RVAs
   (`grep -rhoE 'VA\(0x[0-9a-f]{8}' src --include=*.cpp | grep -oE '0x[0-9a-f]{8}' | sort -u`)
   and skip `@early-stop`. Middle-small (64–512 B) first, then by size.
3. **Fan out:** N background matchers (`subagent_type="matcher"`, `run_in_background: true`,
   **NOT** `isolation: worktree`). Each prompt: absolute worktree path + `cd` there first,
   absolute paths, the target RVA/name/size/TU, the 8-digit convention, the
   **`tools/od_slots.py` stack-naming workflow**, push-to-100% + byte-proven `@early-stop`, report % +
   one-line summary + full `git diff`. **Lane discipline:** all targets of one TU through
   ONE slot.
4. **Integrate SERIALLY:** one at a time — guard master clean → apply only that matcher's
   file(s) → `homm2 build` → confirm % → `homm2 status update` (`--accept-regressions` only
   for trivial cross-fn fuzzy drift) → commit ONLY those files + `config/match_baseline.tsv`
   as `match: <fn> -> <result>` with the Co-Authored-By trailer. One matcher = one commit.
   **Refill immediately:** reset the slot to master, pick next, dispatch.
5. **Stop** when the queue is dry/parked or the user winds down: let in-flight matchers
   finish, integrate, print the ledger (`fn -> result -> commit`) + a regressions summary.
   **Leave the `matcher-N` worktrees in place**.

Keep your context SMALL — hold only the ledger; never pull a matcher's disassembly or
diffs into this session beyond the file(s) you integrate.
