# resource-getter-cache — load-or-create cache accessor

**Tags:** branch-layout, class-hierarchy, /Od
**Seen in:** BASE/RESMGR `resourceManager::GetTileset/GetFont/GetSample/GetMIDIWrap/GetIcon`

## Shape
A typed getter looks the resource up in a cache by hashed id, bumps a refcount on a hit, or
constructs + registers it on a miss:

```cpp
tileset * resourceManager::GetTileset(char *name)
{
    unsigned long id = MakeId(name, 1);
    resource *r = Query(id);
    if (r != 0) {                 // cache hit: bump refcount and return EARLY
        r->field_0x6++;
        return static_cast<tileset *>(r);   // hierarchy downcast, NOT a C-cast
    }
    r = new tileset(id);          // miss: construct + register
    AddResource(r);
    return static_cast<tileset *>(r);
}
```

## What made it match (22% → 96%)
The decompiler renders this as `if (r == 0) { create } else { refcount++ }`, but the retail
branch layout is the **inverse** — the cache-hit (refcount) path is the *fall-through* and the
create path is *jumped to*:

```
  retail:            ours (if r==0 first):
  cmp [r], 0         cmp [r], 0
  je   CREATE        jne  ELSE           <- wrong: hit path jumped, create fell through
  inc word [r+6]     ... create ...
  ... return r ...   jmp  END
  CREATE: ...        ELSE: inc word [r+6]
```

So write it as **`if (r != 0) { hit } else { create }`** (hit first). MSVC /Od then emits
`je CREATE` and the refcount path as the fall-through, matching the retail.

## Notes
- `Query` returns the base `resource*`; the getter returns the derived `X*`. Model the hierarchy
  with `static_cast<X*>(r)` — never a C-cast (the classes single-inherit, offset 0, so it is
  codegen-neutral).
- `MakeId(name, 1)` is computed once and reused for both `Query` and the ctor.
- The `X(unsigned long id)` ctor must exist in the class header. If the recovered header only lists
  a different ctor (e.g. `sample(char*,...)`), that getter is walled until the id-ctor is recovered.
