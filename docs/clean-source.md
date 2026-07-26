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
python3 scripts/clean_source.py --out build/clean --publish  # ...and commit to `clean`
```

## Why it is safe

Every rule reproduces the *production* expansion of its macro — the expansion
MSVC 4.2 already performs when building the matching objects. The transform is
therefore semantics-preserving by construction rather than by inspection. Keep
the rule table in step with `include/va.h` and `include/Ints.h`; that agreement
is the entire correctness argument.

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

## What it produces

| Construct | Sites | Becomes |
|---|---:|---|
| `DATA_COMPGEN(addr, name, value)` | 5142 | `value` |
| `IDX(x)`, `HAS(f,b)`, `BIT(x)` | 5810 | the expression itself |
| `VA(...)`, `DATA(...)`, `VTBL(...)` | 2868 | deleted |
| `H2_ENUM_*` declarations | 1443 | real `enum class` declarations |
| `H2_ALLOC_AT` / `H2_FREE_AT` / `H2_ASSERT` | 282 | `H2_ALLOC(n)` / `H2_FREE(p)` / `H2_ASSERT(c)` |
| `RETAIL_FILE`, `#line N` | 366 | deleted; `__FILE__` where a file operand remains |
| `OD_STEER(x)`, `OR_STEER(x)` | 96 | `x` |
| `__fastcall`, `__cdecl`, `__declspec(dllexport)`, `OVERRIDE`, `register` | 121 | deleted, or `override` |

Retail threaded a frozen source path and line number through every allocation so
its leak tracker could name the site. The clean tree keeps the tracking and lets
the compiler supply `__FILE__`/`__LINE__`, which is both accurate and free.

Only `include/` and `src/` are carried. The matching toolchain — `configure.py`,
`scripts/homm2/`, `config/units.toml`, the delinker and objdiff plumbing — is
deliberately absent, because none of it means anything to a consumer of the
source. Anything a downstream branch needs to build with, it adds itself.

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
