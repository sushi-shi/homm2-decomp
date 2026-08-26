# Building the Ironfist resources

This branch does not store or publish an Ironfist resource archive. Its Nix
flake fetches the original Project Ironfist source and builds an immutable
resource payload in the user's local Nix store. The installer then merges that
payload into a user-supplied complete Heroes II installation: either Heroes II
Gold, or the base game upgraded with The Price of Loyalty.

The source input is pinned to:

- Repository: `https://github.com/jkoppel/project-ironfist.git`
- Commit: `314932011ed5308efb9f35cecc62e8ca638a7375`
- Sparse-source Nix hash:
  `sha256-j6ANYrR2XoC9ZCADOE406dKj+1QtcQzP2ktZCTGbnFw=`

## Nix workflow

Run:

```sh
nix run .#ironfist-resources -- /path/to/heroes2
```

The target must be writable and contain both `DATA/HEROES2.AGG` and
`DATA/HEROES2X.AGG`. Nix then:

1. fetches only upstream `assets/agg`, `assets/music`, `cmp`, `data`, and `maps`;
2. verifies the complete commit and fixed Nix content hash;
3. uses the standard-library Python builder to convert the indexed BMP/INI
   sources into ICN and FRM data and package `DATA/ironfist.agg`;
4. assembles XML, maps, campaign metadata, Lua scripts, and music as the
   `ironfist-resource-payload` derivation; and
5. runs the small installer that validates the retail directory and copies the
   payload into it.

The fetch and build are ordinary Nix dependencies, rather than network work
performed by the installer. Repeated installations reuse the payload from the
Nix store. `nix build .#ironfist-resource-payload` exposes the immutable payload
directly when needed.

For the pinned source, the builder reproduces the reviewed upstream packer
output byte for byte. Both the payload builder and installer enforce its
SHA-256 digest,
`2952e91a5d6c38216d8c805346f4941e7527ff31ba05c0d7a1161e90f56a5599`.

The Cyborg theme is upstream `assets/music/homm2_43.ogg`, installed under the
portable engine's expected name `MUSIC/Track44.ogg`. The portable audio path
plays that Ogg directly, so the generated aggregate does not need a copied
retail `MIDI0044.XMI` placeholder.

No resource data is committed to this repository. The fetched source and built
payload do exist in the user's Nix store. They are not included in the game
package or uploaded anywhere by this repository. The upstream resources retain
Project Ironfist's terms. The Heroes II retail aggregates are checked for
presence but are never copied into or used to build `ironfist.agg`.

Neither workflow uses `pack.bat`, the GrayFace Windows packers, or Wine.

## Without Nix

Check out the pinned upstream commit yourself, then build and install the same
payload with Git, Python, and ordinary Unix tools:

```sh
git clone https://github.com/jkoppel/project-ironfist.git project-ironfist
git -C project-ironfist checkout 314932011ed5308efb9f35cecc62e8ca638a7375

./scripts/build-ironfist-resource-payload.sh \
  ./project-ironfist ./build/ironfist-resource-payload

HOMM2_IRONFIST_RESOURCE_PAYLOAD="$PWD/build/ironfist-resource-payload" \
  ./scripts/install-ironfist-resources.sh /path/to/heroes2
```

The payload output directory must be absent or empty. The builder validates the
aggregate digest before it reports success.

## Updating the source pin

Review a new upstream commit and its resource layout first. Update the revision
and sparse-source hash in `flake.nix`, then update the aggregate digest in both
resource shell scripts. Rebuild into an empty test installation, compare the
native aggregate against the reviewed upstream packer output when the source
format changes, and verify the maps, campaign, scripts, music, and an actual
game startup. Do not introduce a prebuilt resource release as an alternate
input path.
