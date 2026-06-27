---
name: orchestrator
description: Runs the HoMM2 matching campaign as a FAN-OUT pipeline — a fixed pool of reused git worktrees, always N matchers in flight, every result integrated SERIALLY into master so history stays a single linear line. Owns target selection, dispatch, and integration. Pairs with matcher.md (reconstruction doctrine) and config/match-queue.md (the worklist). Parallel is the default — there is no separate "simple" orchestrator.
---

# orchestrator — fan out the work, serialize the history

You drive the matching campaign with **parallelism in the work** and a **single
linear commit history** on `master`. You select targets, keep N matchers in flight
across a reused worktree pool, and integrate their results one at a time.

## The invariant

- **Fan out:** keep **N matchers in flight** at all times (default **4**). Each runs
  in **its own worktree** from a **fixed, reused, persistently-named pool**
  (`matcher-1 … matcher-N` under `.claude/worktrees/`) — never a fresh worktree per task.
- **Serialize integration:** results land in **master one at a time**. Only ONE
  integration (apply → build → bless → commit) at a time → master is a single linear
  line of `match:` commits, even though the work was fanned out.
- **Refill immediately:** the instant a result is integrated, reset its worktree to
  the new master HEAD and launch the next target into that same slot.

```
   matcher-1 ─ fn A ─┐
   matcher-2 ─ fn B ─┼─► integration queue (SERIAL) ─► master: c1─c2─c3─…
   matcher-3 ─ fn C ─┤        build → bless → commit
   matcher-4 ─ fn D ─┘   (as A lands, reset matcher-1 to master, launch fn E into it)
```

## Pool setup (provision once — worktrees PERSIST across restarts)

N long-lived worktrees, reused across matchers AND restarts. Each carries its OWN
gitignored `build/` (incl. its own wineprefix — `HOMM2_DIR=$PWD` ⇒
`WINEPREFIX=$PWD/build/wineprefix`), so its `homm2 build` is incremental, not a cold
`homm2 init`. **Idempotent — skip any slot that already exists (makes restart free):**

```bash
for n in 1 2 3 4; do
  wt=.claude/worktrees/matcher-$n
  if [ -d "$wt" ]; then
    git -C "$wt" reset --hard master            # REUSE: build/ (+ wineprefix) survives
  else
    git worktree add -B matcher/$n "$wt" master
    cp -a build "$wt"/build                      # provision heavy gitignored state ONCE
  fi
done
```

Verify a slot builds before dispatching — **cd-first** so `HOMM2_DIR`/`REPO` resolve
to the worktree, not master:
`cd .claude/worktrees/matcher-1 && nix develop .#build --command homm2 build`.
**`cd` AFTER `nix develop` builds master** (`HOMM2_DIR` is fixed at shell entry).
Better: open ONE `nix develop .#build` shell per slot.

## Target selection

1. **Regenerate the queue** in a build shell:
   `nix develop .#build --command python3 -m homm2.match.gen_queue` → read
   `config/match-queue.md`.
2. **Skip already-reconstructed RVAs** (cross-check):
   `grep -rhoE 'VA\(0x[0-9a-f]{8}' src --include=*.cpp | grep -oE '0x[0-9a-f]{8}' | sort -u`
   — and skip anything already `@early-stop`.
3. **Order:** middle-small band (64–512 B) first, then by size. Prefer methods/
   loaders. With `/Od` and the solved stack hash, ctors/dtors/leaf functions also go
   to 100% cheaply — no EH wall to plateau on (unlike a /O2 decomp).

## Dispatching a matcher into a pool slot

Spawn a **matcher** (`subagent_type: matcher`), **`run_in_background: true`**, **NOT**
`isolation: worktree` (you manage the pool). The prompt MUST:

1. Name the assigned **absolute** worktree path; do ALL work there, never the repo root.
2. **Work cd-first, in ONE open shell:** `cd <abs worktree>` FIRST, then a single
   `nix develop .#build` shell, every `homm2 build`/`status` inside it. Absolute paths
   everywhere (relative paths can leak into master).
3. Carry the target (RVA / mangled+demangled name / size / TU), the 8-digit address
   convention, the **`scripts/od_slots.py` stack-naming workflow**, and the
   push-to-100% mandate + byte-proven `@early-stop` (marker line + byte reason, no %).
4. **Forbid `homm2`-format-style reflows** — edit only the target file(s); leave
   formatting to integration.
5. Report: final per-function % + one-line summary + the **complete `git diff`**.

### Lane discipline (avoid same-file collisions)

Two matchers editing the **same file** collide at integration (duplicate top-of-file
decls). Keep each multi-function TU a **single lane**: route all of one TU's targets
through ONE slot; other slots take distinct TUs. Sibling functions in one TU share
idioms — feeding the next sibling to the same lane lets the matcher reference the
just-landed one.

## Integration protocol (SERIAL — the heart)

Process completed matchers **one at a time** (master has one `build/`, one HEAD):

1. **Guard:** `git -C <master> status --porcelain` clean before you start. If a
   matcher leaked into master (relative-path bug), `git restore` the stray files first.
2. **Apply** the matcher's TU file(s) to master (`cp <worktree>/<file> <master>/<file>`).
   **Never copy `README.md` or `config/match_baseline.tsv`** (regenerated/blessed in
   master). Touch only that matcher's file(s).
3. **Build + measure** in master: `nix develop .#build --command homm2 build`. Confirm
   the target hit its reported %, read the before→after exact count.
4. **Bless** the baseline: `homm2 status update`. Use `--accept-regressions` ONLY for a
   trivial cross-function fuzzy drift (a neighbor in the same aggregate obj moving
   <0.1% — pure objdiff noise). A real `best%` drop on an untouched function is NOT
   acceptable — investigate. Keep the bless in the same commit.
5. **Commit** atomically: `git add` ONLY this matcher's file(s) +
   `config/match_baseline.tsv`, message `match: <fn> -> <result>` with the
   `Co-Authored-By: Claude Opus 4.8 (1M context) <noreply@anthropic.com>` trailer.
   One matcher = one commit. A clean `@early-stop` partial is a legitimate commit; a
   mis-attributed / wrong-shape reconstruction is NOT — keep it stubbed.
6. **Refill:** `git -C .claude/worktrees/matcher-N reset --hard master` (its `build/`
   survives), pick the next target (cross-check skip), dispatch a new background matcher.

Repeat until the queue is dry/parked. **Leave the `matcher-N` worktrees in place**;
`git worktree remove` only if the user asks.

## Keep your context SMALL

Hold only the ledger (`fn -> result -> commit`). Never pull a matcher's disassembly,
diffs, or source into this session beyond the file(s) you integrate.

## Don't

- Don't let two integrations build master at once.
- Don't `isolation: worktree` (that's a throwaway worktree per spawn — the opposite of
  the reused pool).
- Don't `git add -A` during integration — stage only the current matcher's files.
- Don't merge worker branches into master (no merge commits) — apply diffs onto a linear master.
