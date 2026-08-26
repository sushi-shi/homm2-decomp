# Building the Ironfist resources

This branch does not store or publish an Ironfist resource archive. Instead,
the installer builds the runtime content from the original Project Ironfist
repository and merges it into a user-supplied Heroes II: The Price of Loyalty
installation.

The source input is pinned to:

- Repository: `https://github.com/jkoppel/project-ironfist.git`
- Commit: `314932011ed5308efb9f35cecc62e8ca638a7375`

Run the source builder with:

```sh
nix run .#ironfist-resources -- /path/to/heroes2
```

The target must be writable and contain both `DATA/HEROES2.AGG` and
`DATA/HEROES2X.AGG`. The command then:

1. creates a temporary sparse clone and fetches only the pinned commit;
2. verifies the complete commit ID before consuming any files;
3. uses this branch's standard-library Python builder to convert the upstream
   indexed BMP/INI sources into ICN and FRM data and package `DATA/ironfist.agg`;
4. installs upstream XML, maps, campaign metadata, Lua scripts, and music into
   the corresponding game directories; and
5. validates representative required outputs before reporting success.

For the pinned source, the builder reproduces the reviewed upstream packer
output byte for byte. The installer enforces its SHA-256 digest,
`2952e91a5d6c38216d8c805346f4941e7527ff31ba05c0d7a1161e90f56a5599`.

The Cyborg theme is upstream `assets/music/homm2_43.ogg`, installed under the
portable engine's expected name `MUSIC/Track44.ogg`. The portable audio path
plays that Ogg directly, so the generated aggregate does not need a copied
retail `MIDI0044.XMI` placeholder.

Set `HOMM2_IRONFIST_SOURCE_URL` to an alternate Git remote or local mirror when
needed. The commit ID remains fixed and is still verified. The temporary clone,
and build tree are removed after installation. The resource build requires Git
and Python; it does not download or run `pack.bat`, the GrayFace Windows
packers, or Wine.

These upstream resources retain Project Ironfist's terms; building them does
not place them under this repository's source license. The Heroes II retail
aggregates are checked for presence but are never copied into or used to build
`ironfist.agg`.

## Updating the source pin

Review a new upstream commit and its resource layout first. Then update the
commit and aggregate digest in `scripts/install-ironfist-resources.sh`, rebuild
into an empty test installation, compare the native aggregate against the
reviewed upstream packer output when the source format changes, and verify the
maps, campaign, scripts, music, and an actual game startup. Do not introduce a
prebuilt resource release as an alternate input path.
