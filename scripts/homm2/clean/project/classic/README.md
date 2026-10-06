# Heroes of Might and Magic II — Gold 2.1 (Buka) source, reading view

The generated C++ source of the Buka release of Heroes of Might and Magic II
Gold 2.1 (`HMM2PL.exe`) with the original integer-enum and name-mangling model
and its text written out in Russian. Every text reference of the source tree is
replaced by the Russian string the retail program shows, as readable UTF-8.
Code is that of `source-gold-2.1-buka`.

## Branches

```text
decomp-pol-2.0 -------------------> decomp-gold-2.1-buka
    |                                   |
    +------------------+                +------------------------+
    |                  |                |                        |
    v                  v                v                        v
source-pol-2.0     classic-pol-2.0   source-gold-2.1-buka    classic-gold-2.1-buka
    |                                   |
    +-----------------+-----------------+
                      |
                      v
                    port --------> source-ironfist --------> port-ironfist
```

- [`decomp-pol-2.0`](https://github.com/sushi-shi/homm2-decomp/tree/decomp-pol-2.0#branches) — Price of Loyalty 2.0 `HEROES2W.EXE` (1997), VC4.2
- [`source-pol-2.0`](https://github.com/sushi-shi/homm2-decomp/tree/source-pol-2.0#branches) — Clean source, PoL 2.0
- [`classic-pol-2.0`](https://github.com/sushi-shi/homm2-decomp/tree/classic-pol-2.0#branches) — Reading view, PoL 2.0
- [`decomp-gold-2.1-buka`](https://github.com/sushi-shi/homm2-decomp/tree/decomp-gold-2.1-buka#branches) — Gold 2.1 (Buka) game, byte-identical; editor in progress
- [`source-gold-2.1-buka`](https://github.com/sushi-shi/homm2-decomp/tree/source-gold-2.1-buka#branches) — Clean source, Gold 2.1 (ru/en)
- [`classic-gold-2.1-buka`](https://github.com/sushi-shi/homm2-decomp/tree/classic-gold-2.1-buka#branches) — Reading view, UTF-8 Russian
- [`port`](https://github.com/sushi-shi/homm2-decomp/tree/port#branches) — Native port: Linux, Windows, browser
- [`source-ironfist`](https://github.com/sushi-shi/homm2-decomp/tree/source-ironfist#branches) — Project Ironfist on the source
- [`port-ironfist`](https://github.com/sushi-shi/homm2-decomp/tree/port-ironfist#branches) — Project Ironfist on the port

## Reading, not building

This view is for reading. Its strings are UTF-8, while the retail program
stores them as Windows-1251, and it carries no locale build. To build, use
`source-gold-2.1-buka`, which keeps the text in an English and Russian catalog
and compiles either language.

## Regeneration

`decomp-gold-2.1-buka` generates this branch with `homm2 clean --classic-from`
and `--classic-russian` from `source-gold-2.1-buka`. Make source changes there
and regenerate; do not edit this branch by hand.

## License

Project-authored reconstruction source and tooling are dedicated to the public
domain under [CC0 1.0](LICENSE), to the extent the contributors can do so.
Files carrying separate copyright or license notices retain those terms. No
binary game assets are stored in this repository; retail inputs and build
outputs incorporating them are not covered by this dedication.
