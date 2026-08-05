# Inlined callee locals live in the CALLER's frame, unnamed

Under `/Ob1` an `inline` function's own locals are allocated in the frame of every
function that inlines it, once per call site. They get real stack space but no
`S_BPREL32` record, so they are invisible to the debug info and to any tool that
reasons only about declared locals.

Measured on VC6 SP5 (`/Od /MT /Gr /G5 /Ob1 /Z7`), one accessor called twice:

| locals in the inlined accessor | caller frame | `this`/arg spill |
|---|---|---|
| 0 | `0x10` | `-0x10` |
| 2 | `0x20` | `-0x20` |
| 4 | `0x30` | `-0x30` |

The named locals never move - they stay at `-0x4`, `-0x8`, `-0xc`. What grows is
the region BELOW them, and the register-argument spill is pushed down ahead of it.
Identical on MSVC 4.2, so this is not a version difference.

## The frame really has four regions, not three

```
  -0x4 ...    named locals, by (bucket, -declaration index), scope group by group
  ...         COMPILER TEMPORARIES - materialised ternaries, call-result temps,
              and the locals of every /Ob1-inlined callee, once per call site
  ...         __fastcall / this register-argument spills, in argument order
  deepest
```

## This retires the "`this`-slot counter" rule

`docs/campaign-state.md` claimed: *"retail spilling `this` at `[ebp-N]` means retail
has exactly `N/4 - 1` named locals"*. That is only true for a function that inlines
nothing and materialises no temporaries. Any function calling an inlined accessor
that has locals of its own breaks it, and the game's headers are full of such
accessors. Do not use the spill depth as a count of named locals.

What the spill depth IS good for is a difference signal. `retail_spill - our_spill`
is the frame space the two sides disagree about below the named group:

- **negative** (retail shallower): we allocate temporaries retail does not - a
  value we hoisted into a named local that retail materialises inline, or an
  accessor we wrote with locals that retail's has not got.
- **positive** (retail deeper): retail inlines something we call out-of-line, or
  its inlined accessor holds locals ours does not.

Census over non-exact member functions: **177 of 539 (32%) differ**, so this is a
large and previously unattributed class. The differences cluster by TU, which is
what a shared header full of accessors predicts - every function in `BASE/BUTTON`
shows `-4`, pointing at one accessor in the button header rather than at 30
independent function-level defects.

## Why the scope-group census looked arbitrary

`homm2 od-frames --slots` maps OUR named locals onto retail's displacements and
infers the lexical grouping that would produce them. When the real difference is an
inline-temporary region of a different size, no grouping of named locals can
explain the displacements, so the tool reports an implausible number of groups.
That is very likely the "4-6 groups for 5-14 locals" the two-sided census flagged
as an arbitrary reshuffle no brace structure produces - an artifact of attributing
temporary space to named locals, not evidence about the developers' source.

Check the spill delta BEFORE reading a required scope structure. If it is nonzero,
the scope story is unreliable until the temporary region is reconciled.
