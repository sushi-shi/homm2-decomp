# Bounded dynamic-trace setup result

Tested in matcher-2 (`matcher/bss-audiere`) on 2026-09-27 using the existing
verified Nix environment. The unchanged integration-ready 115-byte Audiere
source was selected, with its ordinary include-directory overlay and current
matcher-4 dependency headers. No source mutation, compiler patch, or binary
layout correction was made.

The installed tools are Wine WoW64 staging11.8, both PE32 and PE32+ WineDbg,
and GDB17.1 (available in the local Nix store, although absent from the shell's
PATH). Exact commands, compiler response file, paths, selected environment and
hashes are retained under `build/audiere-dynamic-queue/`.

## WineDbg launch failure

Both the normal WineDbg executable and the explicit PE32
`windows/syswow64/winedbg.exe` were tested as GDB proxies with `--gdb --no-start`.
Each debuggee failed during Wine process initialization, before loading C1XX:

```
dlls/ntdll/unix/server.c:1812: server_init_process_done: Assertion `!status' failed.
```

The proxy records process creation, two Wine runtime DLL loads, then process
exit with status1. The first GDB connection also reported conflicting enabled
responses for `qXfer:features:read`. Disabling both target-features and qSupported
negotiation removed that protocol error but could not recover the already-dead
debuggee: GDB then reported "The target is not running".

A separate direct WineDbg run, without GDB/proxy networking, reproduced the
same initialization assertion. Its process-information commands confirmed no
compiler process remained to inspect; `info share` and `cont` failed because no
process was loaded. This localizes the blocker beyond GDB packet negotiation.
It does not establish the cause of the Wine assertion.

A final bounded control used the already-installed Wine11.0 runtime in a new,
separate disposable prefix. Initialization succeeded, but direct PE32 WineDbg
again failed before C1XX with the same assertion (`server.c:1767` in11.0).
This does not isolate the underlying cause, but the failure is not specific to
the existing11.8 prefix or the GDB proxy. Production Wine configuration was not
changed; the disposable11.0 server was stopped after the check.

## Native-host GDB fallback

A bounded native GDB launch of Wine was prepared to avoid the Windows debugger
startup path. The script would stop at the Linux loader, install a hardware
breakpoint at pinned C1XX address1041c5bb, and then inspect registers, stack and
instructions. Startup failed before any breakpoint could be installed:

```
warning: Could not trace the inferior process.
warning: ptrace: Operation not permitted
During startup program exited with code127.
```

The identical prepared action was retried with sandbox escalation. Automatic
review permitted the invocation, but the runtime still denied ptrace. This is a
runtime capability limitation, not an automatic-approval rejection. No software
or hardware breakpoint was reached and no compiler queue record was observed.

## Control and disposition

An ordinary Wine compilation using the same compiler response file completed
successfully, producing the candidate object. C1XX SHA256 remained unchanged:
`f014b3bee650224adf6cb44e51f0eeac5abbf5da666fa299d5250b2f3d1937c6`.
`control-result.json` records command, success, output existence and binary hash.
The build itself remains usable; only the attempted debugger entry paths failed.

No dynamic mapping of Node to pending-kind/queue can be claimed from this
attempt. The preceding static audit remains valid within its stated bounds.
Those debugger routes stopped within the assigned 20-minute limit. A later
[in-process proxy trace](inprocess-proxy-trace.md) succeeded without ptrace or
Windows debug-process flags. It uses disposable memory instrumentation, leaves
the original compiler on disk unchanged, and verifies its output against an
ordinary same-source compilation. That later result supersedes the debugger
availability limitation, while the failures recorded here remain reproducible.

Artifacts include `launch.py`, `launch32.py`, `native.py`, `native.gdb`,
`direct.py`, their command/provenance JSON files, WineDbg/GDB logs, and the
ordinary control, plus `older-runtime.py`, `wine11-command.json` and its logs. The selected build-environment JSON contains only named
compiler/runtime configuration variables, not a full environment dump.
