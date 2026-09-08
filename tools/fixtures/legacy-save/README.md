These are synthetic, asset-free save files produced by `game::SaveGame` from
the unmodified `16df440a` game objects. They cover the base and expansion record
layouts, the original campaign, and the expansion campaign.

Each fixture contains a 36×36 map, two human players, one owned hero and town,
an overlay, fog, and an opaque extra record. The hero has 1,234 peasants and
123 spell points; the first player has 23,456 gold. Campaign fixtures include
scenario or award state. The save writer used Windows-1251 for text.

The producer initially used `OLD_HERO` and `OLD_TOWN`. After writing, those
13-byte fields were replaced with the valid Windows-1251 names `Александр`
and `Белокаменный`, including their terminators. This reproduces a retail save
whose full name the old UTF-8 runtime could not hold. Other bytes were retained
from the original writer. Bzip2 compression keeps the fixtures small; it is
not part of the save format.

SHA-256 of the decompressed fixtures:

| File | SHA-256 |
| --- | --- |
| legacy-base.GM2 | c6e48a1e585761e9f2430fe7747d5303568f93a33203c1dffae352b7ad90a6ae |
| legacy-gold.GX2 | 5b409526929f20d0f97344072cbe55cc4702698b5523187edd95cf9b4eec746a |
| legacy-campaign.GMC | 5462a1a0d47cfe86610f64d4c07521564929a1b0a3476543f49921a97c9c7453 |
| legacy-expansion.GXC | 4cc54bed810d01025d292e2388890e8a4179d2abe4f619f472d0929c00d23b19 |
