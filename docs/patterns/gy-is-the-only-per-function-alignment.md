# Only `/Gy` gives every function its own 16-byte-aligned section

Measured with the pinned VC6 SP5 compiler (BASE profile) on BASE/DIMMER and
reduced units, 2026-10-06.

## Mechanism

Without `/Gy`, VC6 packs ordinary functions back to back into one `.text`
section at unaligned offsets. Functions that must be COMDATs anyway (inline
bodies, `??_G`, template members) still get their own sections, after the
packed `.text`; the `$E` initializer helpers come before them. With `/Gy`
every function is its own 16-byte-aligned COMDAT, in emission order.

DIMMER (constructors `c0`/`cA`, `Read`, `Main`, `Draw`, deleting destructor
`G`, destructor `X`), section:offset of each function start:

| Flags | Starts |
| --- | --- |
| `/Gy` | `c0:2+0 G:4+0 cA:5+0 R:6+0 M:7+0 D:8+0 X:9+0` |
| no `/Gy` | `c0:2+0 cA:2+0x23 R:2+0x4e M:2+0x62 D:2+0x7b G:4+0 X:5+0` |

None of `/Zi`, `/Z7`, `/GZ`, `/Ge`, `/Gh`, `/Gs0`, `/Gm`, `/Oy-` without `/Gy`
moves a function off the packed `.text`; they change only function sizes or
section numbers.

## Use

Retail functions of one unit that each start on a 16-byte boundary, with
padding between them, were compiled with `/Gy`. The order of functions
inside such a unit is then the compiler's emission order, so moving a
function in the image means moving its emission (definition order, inline
deferral), not changing flags; see
[yx-defers-header-inline-emission](yx-defers-header-inline-emission.md).
