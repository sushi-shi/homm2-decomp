# Repository Git hooks

Enable the hooks for one worktree:

```sh
git config extensions.worktreeConfig true
git config --worktree core.hooksPath .githooks
```

The pre-commit hook regenerates the Clang compilation database and runs the
whole-tree strict-enum syntax check. Ordinary warnings are summarized but do not
block a commit; any hard Clang error does.

Use Git's standard `--no-verify` escape hatch only when intentionally repairing a
broken strict-enum baseline.
