# Automatic precompiled headers move header-inline emission to the object tail

Measured with the pinned VC6 SP5 compiler on BASE/DIMMER, BASE/AudiereEffects
and reduced units (`docs/matching/emission-order/pch/`).

## Signature

A `/Gy` object whose retail order has a header-inline function, or a header
class's scalar deleting destructor, after every ordinary function, although
the ordinary build emits it right after its first user.

## Mechanism

Under `/YX` (or `/Yc`/`/Yu`), header-inline functions and a precompiled class's
deleting destructor are emitted at the end of the object, not after the first
function that references them. They land before the ctype initializer pair
when `<windows.h>` (here via `SOURCE/KB.h`) is parsed after the class's header,
and after the pair when it is parsed first. Bodies and relocations do not
change; only section order does.

## Use

BASE compiles with `/YX` (tier rule in `homm2.manifest.unit_flags`); the
flag is byte-neutral for every other unit. Put the inline body in the header
(in-class), then let the include order select the side of the ctype pair:

- DIMMER: in-class `virtual ~dimmerWidget() {}` gives
  ctor, ctor, Read, Main, Draw, `??_G`, `??1`, ctype pair.
- AudiereEffects: in-class `~AudiereSampleNode() {}` with `SOURCE/KB.h`
  included first gives the node destructor after the ctype pair.

An inline body defined in the `.cpp` is not deferred.
