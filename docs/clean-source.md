# Deriving a clean source tree

The matching tree carries scaffolding that exists only to prove the source
reproduces retail object code: RVA annotations (`VA`, `DATA`, `VTBL`), delinker
metadata (`DATA_COMPGEN`), codegen steering hacks (`OD_STEER`, `OR_STEER`), the
frozen retail file/line operands threaded through every allocation, and the
dual-build enum machinery (`H2_ENUM_*`).

All of it is load-bearing for matching and none of it belongs in anything else
built from this source. Most comments also describe reconstruction evidence, so
the generated tree omits comments entirely.

```sh
python3 scripts/clean_source.py --out build/clean            # generate
python3 scripts/clean_source.py --out build/clean --verify   # build
python3 scripts/clean_source.py --out build/clean \
  --publish source-pol-2.0                                  # publish source branch
```

## Why it is safe

Metadata and codegen-steering rules reproduce the *production* expansion of
their macros — the expansion MSVC 4.2 already performs when building matching
objects. The enum rules deliberately select the strict typed branch and `SIZE`
becomes an ordinary `static_assert`. Keep the rule table and its regression tests
in step with `include/va.h` and `include/Ints.h`.

Selecting the typed branch is the one choice the generator makes. Generated
conversions in `GENERATED_PATCHES` bridge the few integer-only expressions
that remain in the matching source.

Every patch declares how many sites it expects to hit, and any other number ends
the run naming the file. Nine audited groups cover 67 sites.

Nothing the generator does changes what the game *does*. The clean tree targets
Win32 exactly as retail did, so there is nothing to adapt — only scaffolding to
resolve away.

The lexer distinguishes comments from string and character literals, matches
balanced parentheses, and removes comments without joining adjacent tokens.
Macro arguments are transformed recursively, innermost first.

Two self-checks run after generation and fail the run:

- no comment or scaffolding macro survives anywhere in the output;
- no construct expanded to nothing while stranding its punctuation. A `VTBL(...)`
  used to leave a bare `;` behind. This is measured as a delta against the input,
  because a lone `;` is also legitimate — the game writes `if (easy) ;` to leave
  a branch of an if/else chain empty.

Generation also fails closed before recursive deletion. Output inside the
repository must be a child of `build/`; every existing output directory must
carry the generator marker from a previous run; and a Git worktree is never a
valid output target.

## What it produces

| Construct | Becomes |
|---|---|
| `DATA_COMPGEN(addr, name, value)` | `value` |
| `IDX(x)`, `HAS(f,b)`, `BIT(x)` | the typed expression |
| `VA(...)`, `DATA(...)`, `VTBL(...)` | deleted |
| `H2_ENUM_*` declarations and helpers | typed declarations and expressions |
| `H2_ALLOC_AT` / `H2_FREE_AT` / `H2_ASSERT` | `H2_ALLOC(n)` / `H2_FREE(p)` / `H2_ASSERT(c)` |
| `RETAIL_FILE`, `#line N` | deleted; `__FILE__` where a file operand remains |
| `OD_STEER(x)`, `OR_STEER(x)` | `x` |
| `__declspec(dllexport)`, `register` | deleted |
| `OVERRIDE` | `override` |
| `__cdecl`, `__stdcall`, `__fastcall` | preserved |
| `// ...`, `/* ... */` | deleted |

Retail threaded a frozen source path and line number through every allocation so
its leak tracker could name the site. The clean tree keeps the tracking and lets
the compiler supply `__FILE__`/`__LINE__`, which is both accurate and free.

The branch carries `include/`, `src/`, vendor SDK headers, import definitions,
and a generated `build.ninja`. The Ninja graph builds `build/HEROES2W.EXE` for
32-bit Windows with Clang and LLD. The compiler runtime is linked statically.
The matching toolchain, delinker, objdiff plumbing, MSVC, and Wine are absent.
It requires Ninja, Clang, LLD, LLVM dlltool, and a 32-bit MinGW toolchain.

`--publish` accepts only a generated source checkout and either creates a new
generated branch or advances a branch whose tip already carries the generator
provenance marker. It refuses dirty generated worktrees, stages only generated
project files, and records unchanged output with an empty descendant commit
instead of rewriting published history.

The verifier invokes the generated Ninja graph. All 95 translation units,
including portable replacements for the two old inline-assembly units, must
compile and link into the Windows executable.

## Type information is preserved, not discarded

The matching tree must spell each enum domain as a plain integer, because that
is what MSVC 4.2 lowers identically to retail. But it already carries the
properly typed variant behind `#if H2_STRICT_ENUMS` — for example
`OppositeCampaignSide` is `1 - side` in production and a typed ternary in the
strict build.

The generator resolves those conditionals to the **typed** branch, so the clean
tree gets real `enum class` domains and the game's own typed logic rather than
integer arithmetic that happens to work:

```cpp
enum class CampaignSide : i32 { CAMPAIGN_ROLAND, CAMPAIGN_ARCHIBALD };
using enum CampaignSide;

inline CampaignSide OppositeCampaignSide(CampaignSide side) {
    return side == CAMPAIGN_ROLAND ? CAMPAIGN_ARCHIBALD : CAMPAIGN_ROLAND;
}
```

Struct fields keep their retail width through `H2EnumStorage<Enum, Storage>`,
which presents the enum type while retaining each audited field width.
