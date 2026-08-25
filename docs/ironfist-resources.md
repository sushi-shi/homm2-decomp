# Ironfist resource pack

Ironfist's runtime content is distributed as a GitHub Release asset instead of
being committed to this branch. It does not replace the retail game data:
Heroes II: The Price of Loyalty is still required.

Current immutable package:

- Release/tag: `ironfist-pol-2.0-resources-v1`
- Asset: `ironfist-pol-2.0-resources-v1.zip`
- SHA-256: `37194d329c43a32615e2f1926a4ab33d7fdeda4be3add99acb6b59c21175876a`
- Ironfist source reference: `jkoppel/project-ironfist@31493201`

The archive has the game-directory layout at its root. It contains the
Ironfist aggregate, XML databases, campaign metadata and maps, music, and Lua
scripts. Extract it over a writable retail PoL installation, or use:

```sh
nix run .#ironfist-resources -- /path/to/heroes2
```

The installer requires both retail aggregates, downloads the pinned asset,
checks its SHA-256 digest, extracts it, and validates representative required
files. It uses an authenticated `gh` session when anonymous downloads are not
available, as is the case while the repository is access-restricted. For a
mirror, set `HOMM2_IRONFIST_RESOURCES_URL` to the exact same archive; the
checksum remains mandatory.

## Publishing an update

Resource releases are immutable. Build a new archive with `DATA/`, `MAPS/`,
`CAMPAIGNS/`, `MUSIC/`, and `SCRIPTS/` at its root, choose a new versioned tag,
then update the version, URL, and SHA-256 in
`scripts/install-ironfist-resources.sh`. Never replace an existing release
asset in place because installed source revisions pin its digest.
