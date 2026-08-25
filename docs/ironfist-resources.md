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
3. runs upstream `assets/pack.bat` and its checked-in GrayFace packers under an
   isolated Wine prefix to produce `DATA/ironfist.agg`;
4. installs upstream XML, maps, campaign metadata, Lua scripts, and music into
   the corresponding game directories; and
5. validates representative required outputs before reporting success.

The Cyborg theme is upstream `assets/music/homm2_43.ogg`, installed under the
portable engine's expected name `MUSIC/Track44.ogg`. The portable audio path
plays that Ogg directly, so the generated aggregate does not need a copied
retail `MIDI0044.XMI` placeholder.

Set `HOMM2_IRONFIST_SOURCE_URL` to an alternate Git remote or local mirror when
needed. The commit ID remains fixed and is still verified. The temporary clone,
build tree, and Wine prefix are removed after installation.

These upstream resources retain Project Ironfist's terms; building them does
not place them under this repository's source license. The Heroes II retail
aggregates are checked for presence but are never copied into or used to build
`ironfist.agg`.

## Updating the source pin

Review a new upstream commit and its resource layout first. Then update the
commit in `scripts/install-ironfist-resources.sh`, rebuild into an empty test
installation, and verify the aggregate, maps, campaign, scripts, music, and an
actual game startup. Do not introduce a prebuilt resource release as an
alternate input path.
