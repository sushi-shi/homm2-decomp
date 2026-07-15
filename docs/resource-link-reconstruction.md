# Resource link reconstruction

The final-link target reconstructs the shipping executable's Win32 resources as
structured linker input. `ninja link-resources` reads the user's
`build/orig/HEROES2W.EXE` and writes:

- `build/link/HEROES2W.res`, a standard Win32 resource stream consumed by the pinned LINK 3.00.
- `build/link/HEROES2W.resources.json`, a resource identity and size inventory.

No retail resource payload is committed to the repository. The extractor walks the
PE resource directory and emits one typed `(type, name, language, payload)` `.res`
record per leaf in original payload order. It does not copy the raw `.rsrc` section,
its RVA-bearing directory entries, linker timestamp, alignment bytes, or padding.
LINK rebuilds those structures for the candidate image.

The retail resource inventory is 12 leaves:

- Two `RT_ICON` payloads and two `RT_GROUP_ICON` records.
- Six named `RT_MENU` resources.
- One named `RT_DIALOG` resource.
- One `RT_VERSION` resource.

The strict link audit requires exact identities, languages, payload bytes, payload
order, `.rsrc` raw and virtual sizes, section characteristics, and PE section order.
Linker-generated resource-directory timestamps are recorded but excluded from the
semantic comparison.

Candidate and retail `.rsrc` are both `0x257c` virtual bytes and `0x2600` raw bytes
with characteristics `0x40000040`. Adding `.rsrc` moves candidate `.reloc` forward by
the expected aligned `0x3000` RVA span. The live candidate/retail RVA delta is recorded
in `HEROES2W.link.json`; it is cumulative from earlier sections and will change as
those sections converge. Do not compensate for it with resource padding or forced
section addresses.

Source-level reconstruction of these resources is intentionally deferred. See
[`resource-source-reconstruction-plan.md`](resource-source-reconstruction-plan.md)
for the staged plan; the extracted `.res` remains the active linker input and oracle.
