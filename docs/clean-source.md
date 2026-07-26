# Deriving a clean source tree

The matching tree carries scaffolding that exists only to prove the source
reproduces retail object code: RVA annotations (`VA`, `DATA`, `VTBL`), delinker
metadata (`DATA_COMPGEN`), codegen steering hacks (`OD_STEER`, `OR_STEER`), the
frozen retail file/line operands threaded through every allocation, and the
dual-build enum machinery (`H2_ENUM_*`).

All of it is load-bearing for matching and none of it belongs in anything else
built from this source. `scripts/clean_source.py` derives a tree without it.

```sh
python3 scripts/clean_source.py --out build/clean            # generate
python3 scripts/clean_source.py --out build/clean --verify \
  --compat-include ../homm2-decomp-port/include/COMPAT       # native syntax check
python3 scripts/clean_source.py --out build/clean --publish  # ...and commit to `clean`
```

## Why it is safe

Metadata and codegen-steering rules reproduce the *production* expansion of
their macros — the expansion MSVC 4.2 already performs when building matching
objects. The enum rules deliberately select the strict typed branch, `SIZE`
becomes a native `static_assert`, and the two documented native behaviour
patches are explicit exceptions. Keep the rule table and its regression tests in
step with `include/va.h` and `include/Ints.h`.

The implementation is a lexer, not a set of regexes. It tracks comments and
string and character literals, and matches balanced parentheses, so it never
rewrites inside `".\\DATA\\HEROES2.AGG"` or a format string full of commas.
Macro arguments are transformed recursively, innermost first, so nested
invocations resolve in one pass.

Two self-checks run after generation and fail the run:

- no scaffolding macro survives anywhere in the output;
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
| `H2_ENUM_*` declarations | real `enum class` declarations |
| `H2_ALLOC_AT` / `H2_FREE_AT` / `H2_ASSERT` | `H2_ALLOC(n)` / `H2_FREE(p)` / `H2_ASSERT(c)` |
| `RETAIL_FILE`, `#line N` | deleted; `__FILE__` where a file operand remains |
| `OD_STEER(x)`, `OR_STEER(x)` | `x` |
| `__fastcall`, `__cdecl`, `__stdcall`, `__pascal` | portable `H2_*CALL` ABI macros |
| `__declspec(dllexport)`, `OVERRIDE`, `register` | deleted, or `override` |

Retail threaded a frozen source path and line number through every allocation so
its leak tracker could name the site. The clean tree keeps the tracking and lets
the compiler supply `__FILE__`/`__LINE__`, which is both accurate and free.

Only `include/` and `src/` are carried. The matching toolchain — `configure.py`,
`scripts/homm2/`, `config/units.toml`, the delinker and objdiff plumbing — is
deliberately absent, because none of it means anything to a consumer of the
source. Anything a downstream branch needs to build with, it adds itself.

Explicit calling conventions are not matching scaffolding. On 32-bit Win32
they determine stack cleanup, argument passing, and function-pointer
compatibility. The generated `Ints.h` maps them to MSVC keywords or GNU/Clang
i386 attributes, preserving contracts such as `DirectDrawCreateProc`, NetBIOS
post routines, `WinMain`, and the cdecl naked-assembly blitters.

`--publish` accepts only a clean source checkout and either creates a new
generated branch or advances a branch whose tip already carries the generator
provenance marker. It refuses dirty generated worktrees, stages only `include/`
and `src/`, and records unchanged output with an empty descendant commit instead
of rewriting published history.

With the port compatibility headers, the verifier compiles 93 of the 95
translation units for i686 Linux. The only accepted failures are `BITS.cpp` and
`TILE.cpp`, which contain hand-written MSVC assembly and are replaced by portable
implementations in the native port. Any other failure makes verification fail.

## Type information is preserved, not discarded

The matching tree must spell each enum domain as a plain integer, because that
is what MSVC 4.2 lowers identically to retail. But it already carries the
properly typed variant behind `#ifdef HOMM2_STRICT_ENUM_TYPES` — for example
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
which presents the domain type to expressions while storing one byte where
retail stored one byte.

## Open: the field-width audit

`H2EnumStorage` exists because the same domain is stored at different widths in
different structures. Of 67 domains with wrapped fields, 43 are stored at a
single width and could be plain `enum class X : <width>` fields with no wrapper
at all. The other 24 are not:

| Domain | Widths in use |
|---|---|
| `CreatureType` | `i8`, `u8`, `i16`, `i32` |
| `FactionType` | `char`, `i8`, `i16`, `i32` |
| `ArtifactType` | `i8`, `i16`, `i32` |
| `MapDirection` | `i8`, `u8`, `i32` |

It is tempting to collapse each domain to one narrow type and delete the
wrapper. Two things have to be checked first, and neither has been:

**Signedness is meaningful.** `CreatureType` defines `CREATURE_NONE = -1`, used
178 times, so it cannot become `u8`. Retail proves the point itself: the one
field that *is* unsigned, `combatManager::m_summonedCreatureType`, uses a
different sentinel, `CREATURE_SUMMONED_NONE = CREATURE_PEASANT`, precisely
because it cannot represent -1. The narrowest safe common type here is `i8`,
which is already the domain's declared underlying type.

**Layout may be load-bearing.** Narrowing `army::m_monsterType` from `i32` to
`i8` changes `sizeof(army)` and every offset after it. The game reads through
`ReadBlock(destination, size)` rather than field by field, and the tree carries
142 `SIZE(type, bytes)` assertions pinning struct sizes to retail values. Before
any domain is collapsed, each affected structure needs checking: is it ever read
or written as raw bytes — from `.AGG`, from a `.MP2` map, from a save, or over
the network?

That audit is worth doing on its own merits: it is the same prerequisite for a
64-bit build, which changes every pointer-bearing structure. Until it is done,
the wrapper stays. It is generated, so carrying it costs nothing.
