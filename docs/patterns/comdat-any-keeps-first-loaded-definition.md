# LINK keeps the first `Any` COMDAT it loads and appends contributions in arrival order

Measured with the pinned VC6 SP5 `LINK.EXE` (SHA-256 `9672e578…`): Ghidra
reading of the linker plus native two-object links (`/OPT:NOREF`), 2026-10-06.

## Mechanism

- COMDAT selection (`0x004627e0`): when an `Any` definition of a symbol is
  already selected, a later identical `Any` definition is discarded. The
  copy that survives is the one in the object LINK loaded first. Other
  selection modes (associative, same-size, largest) take separate paths and
  were not measured.
- A plain (non-COMDAT) definition plus any other definition of the same
  symbol, COMDAT or not, is `LNK1169`.
- Contributions (`0x0040e5a0`) are appended to the tail of the group keyed by
  their complete input section name; groups are created in lexical order of
  that name and the output section is the name truncated at `$`
  (`0x0040e330`). Within one group nothing is sorted: an ordinary `.text`
  holds functions in object arrival order.

Header-inline `N` in two objects `a`, `b`, linked in both orders:

| `a.cpp` | `b.cpp` | `a b` | `b a` |
| --- | --- | --- | --- |
| inline | inline, used | `N` from a | `N` from b |
| inline | absent | from a | from a |
| plain | absent | from a | from a |
| inline or plain | plain | LNK1169 | LNK1169 |
| plain | inline, used | LNK1169 | LNK1169 |

## Use

A header-inline function or deleting destructor that retail places inside
another unit's contribution was selected from that unit: the copy that
survives belongs to the first object in link order that emitted it, not to
the unit that "owns" the class. Archive members arrive in reference order
([library-pull-order-is-reference-fifo](library-pull-order-is-reference-fifo.md)),
direct objects in command-line order.

Making one copy plain to pin its owner fails as soon as another unit still
expands it inline.
