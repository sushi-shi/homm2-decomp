# Retail version differences

The authoritative behavioral ledger for Price of Loyalty 2.0, NWC Gold 2.1,
and the Buka Gold 2.1 executable is
[`docs/version-changes.md`](https://github.com/sushi-shi/homm2-decomp/blob/decomp-gold-2.1-buka/docs/version-changes.md)
on `decomp-gold-2.1-buka`.

That reconstruction branch owns the evidence because every entry is reviewed
against retail code while matching. Entries are classified as:

- **[2.1]** — an upstream NWC Gold 2.1 change from PoL 2.0;
- **[Buka]** — a Buka/localization-era change;
- **[unclassified]** — not yet separated by comparison with the GOG Gold 2.1
  executable.

`master` and `ironfist` consume the generated source but do not duplicate the
ledger: a copied snapshot would immediately drift from matcher evidence. Port-
specific differences belong in commit history and `docs/porting.md`; they are
not retail 2.0/2.1 behavior.
