# A scalar delete call can represent trivial-array deletion

Measured on Buka VC6 SP5, SOURCE/ADVMGR, the native `/Od` profile. Do not
infer a scalar object merely from a call to `??3@YAXPAX@Z`.

The owner here is `u16* m_visibilityMap` at `advManager+0x9e`; initialization
allocates it with `new u16[MAP_WIDTH * MAP_HEIGHT]`. A complete two-arm matrix
shows `delete m_visibilityMap` and `delete[] m_visibilityMap` emit the same
body, including the deletion temporary. The array form is retained.

| Relative offset | Genuine retail bytes / instruction | Both native arms |
| --- | --- | --- |
| +0x37b | `8b 45 ec` mov eax,[ebp-0x14] | same |
| +0x37e | `8b 88 9e 00 00 00` mov ecx,[eax+0x9e] | same |
| +0x384 | `89 4d f0` mov [ebp-0x10],ecx | same |
| +0x387 | `8b 55 f0` mov edx,[ebp-0x10] | same |
| +0x38a | `52` push edx | same |
| +0x38b | `e8 eb 48 0d 00` call VA 0x004d68c2 | `e8 00 00 00 00`, REL32 `??3@YAXPAX@Z` at+0x38c, addend0 |
| +0x390 | `83 c4 04` add esp,4 | same |

Full native and genuine-retail proof covers all952 function bytes and all46
ordered targets, not only this excerpt. All95 native TU functions and every
allocated section are identical. See [B44](../reconstruction/B44.md) for
artifacts and reproduction. This does not generalize to nontrivial element
destructors, class-specific allocators, or another compiler configuration.
