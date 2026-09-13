# Reconstruction checkpoint: PRs #59 and #63

The Gold reconstruction checkpoint was squash-merged in order:

- [#59](https://github.com/sushi-shi/homm2-decomp/pull/59): `8a44b8247`.
- [#63](https://github.com/sushi-shi/homm2-decomp/pull/63): `cb73b3b84`.
- Generated-build compatibility follow-up: `2037def88` on
  `decomp-gold-2.1-buka`.

The portable integration applies the first-parent delta from generated source
`830658dec` to `a2c70f14c`. Its downstream route is
`master` → `ironfist` → `ironfist-master`, using cherry-picks. The corresponding
Russian UTF-8 classic view is `fdd70881c`; classic is not a build input.

## Portable adaptation

The recovered names, shared constants, message roles, and owned data layouts
are propagated while retaining the port's SDL backends, runtime localization,
bounded UTF-8 text handling, exact file transfers, binary packet-index access,
compression replacement, and documented gameplay/safety fixes. Obsolete retail
Win32 backends and generated Ninja build graphs are not reintroduced into the
portable CMake project. Message codes remain open wrapper types so the port's
text-input event remains supported.

The integration passes a native Debug build and all 23 CTest checks. The added
`checkpoint_layout` check pins packet sizes/offsets, configuration walk-speed
ownership, zero-based town spell-count storage, and notification values.
English and Russian startup/exit replays are also checked against Buka assets.
These checks do not establish multiplayer interoperability or exhaustive
gameplay equivalence. Generated Windows source was separately built in both
RU and EN configurations before publication.

## Where to continue

The Ironfist adaptation retains custom campaigns, XML saves, mod combat hooks,
and expanded faction/creature/artifact/spell tables. Its XML town spell-count
reader and writer now use the recovered zero-based array together; existing
XML tag names remain unchanged for save compatibility. Mod-only path consumers
also use the recovered zero-based direction storage. The packet-index binary
helper and its regression test accompany the updated network layouts.

The mod-consumer follow-up also migrates Cyborg spell-count initialization in
both town setup and mage-guild construction. `checkpoint_mod_consumers` pins
these indices and the XML consumers; Ironfist passes all 24 CTest checks with
this follow-up.

All approximately 100,000 lines of reconstruction audit material remain on the
evidence-owning `decomp-gold-2.1-buka` branch. Generated and portable branches
do not duplicate that archive. Continue from that branch's
`docs/reconstruction/continuation.md`, not from an archived inventory's counts.

This is a checkpoint, not semantic-review closure. The continuation records
the six known naming follow-ups, stale/missing function review hashes, and
inventory-key migration work. Further reconstruction fixes should originate
there and follow the same downstream chain.
