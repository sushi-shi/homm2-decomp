# Emission-order probe campaigns: DIMMER and AudiereEffects tails

Measured 2026-10-06 on `work/exact-hash-from-homm1` with the pinned VC6 SP5
compiler and the BASE profile (`/Od /MT /Gr /G5 /Ob1 /Gf /Gi- /GX /DNO_STRICT /Gy`),
using `python3 -m homm2.permute.emission_order` on the manifests written by
[generate_manifests.py](generate_manifests.py). The runner reports COFF section
order, function sizes and REL32 callees per object, and links where object
selection matters. Real sources are never edited in place; variant headers
shadow the repository copy through `/I{work}`.

Letters: U Purge, F Find, P Play, I IterationActive, N node destructor,
S/A `RefPtr<OutputStream>` destructor/assignment, D `RefPtr<AudioDevice>`
destructor, E compiler `$E` helpers (the last two in Effects are the ctype
guarded initializer and registration), C the ctype cleanup, M StopAudiereMusic;
c0/cA constructors, R Read, M Main, D Draw, G deleting destructor, X destructor
in the DIMMER tables.

Retail targets: Effects tail `S A E E` then N (0x004cd050) immediately before
AudiereMusic's first `$E`; Play 0x39c bytes with the node destructor expanded
inline; Purge calls N. DIMMER: `c0 cA R M D G X`, every function on its own
16-byte boundary.

## Rules found

1. Purge calls the node destructor out of line in every form, even when its
   inline body precedes Purge. Play expands it inline (0x39c) whenever the body
   is inline in its TU (declared or defined `inline`, in-class, implicit, or a
   template member), and calls it (0x385) otherwise. `/Ob1` and `/Ob2` give
   identical objects.
2. Inside one `/Gy` object the node destructor is emitted where its body is
   defined (after Purge for in-class/implicit), or with the template members
   for a class-template node; it never follows the ctype pair. A template node
   also reverses S and A (`A D S N`), while retail keeps the plain-class `S A`.
3. A plain definition at the top of AudiereMusic emits N first in Music, the
   retail cross-object position, but only when Effects emits no copy; then Play
   is 0x385. An inline copy in Effects plus a plain one in Music is LNK1169;
   two inline copies resolve to the first object loaded, which is Effects.
4. Rich header: retail lists 96 C++ objects from build 8966, exactly the
   current 96 C++ units. Only Misc lacks a ctype pair, so the only mergeable
   pair is Misc+MiscRuntime; that frees one object, not the two that separate
   destructor owners would need, and a separate owner including `audiere.h`
   would add its own ctype pair, which retail lacks before AudiereMusic.
5. DIMMER: without `/Gy` every destructor form except an out-of-line `~dw` at
   the end gives the retail order `c0 cA R M D G X` (in-class and
   inline-at-end hit), but the ordinary functions are packed (0x0, 0x23, 0x4e,
   ...); no non-`/Gy` flag tested (`/Zi /Z7 /GZ /Ge /Gh /Gs0 /Gm /Oy-`) aligns
   them. With `/Gy` every form, nesting (struct, class, namespace) and
   destructor form emits G right after the first vtable-storing constructor.

## What the earlier 204/115-byte sources were steering

- `DIMMERDestructor.cpp` + `heroWindow::DimmerWidget<widget>` with explicit
  instantiation: moved all members into one end-of-TU instantiation batch,
  which emits G after the members (rule 5's non-`/Gy` order with `/Gy`
  alignment), and moved `~` to another object. PoL 2.0 CodeView names the
  class `dimmerWidget` (no template), so the template owner has counter-evidence.
- The resource-parameterized node template: rule 2's template phase, giving
  `S A N G R`; still one step short, and it contradicts retail's S/A order.
- The SEARCH record with padding fields: replaced by the hash-ordered local
  statics (docs/patterns/bss-hash-order-names-storage-spelling.md).
- Misc+MiscRuntime as one TU: consistent with rule 4 (Misc has no ctype pair);
  it frees exactly one C++ object in the Rich budget.

## Disposition

No natural form reproduces either tail inside the Rich-header object budget.
Both walls therefore remain open; no probe source or flag change is retained.

## Tables
### audiere-owner (216 variants; /Ob1 and /Ob2 identical: True)

| header | Effects definition | Music definition | Effects order | Music order | Play size | Play: node dtor |
|---|---|---|---|---|---|---|
| h_inline | e_none | m_none | U F P S A D E E C | E E E E E E E E M S A s a D E E C | 0x385 | call |
| h_inline | e_none | m_top | U F P S A D E E C | E E E E E E E E M S A s a D E E C | 0x385 | call |
| h_inline | e_none | m_eof | U F P S A D E E C | E E E E E E E E M S A s a D E E C | 0x385 | call |
| h_inline | e_before_purge_inline | m_none | U N F P S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_inline | e_before_purge_inline | m_top | U N F P S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_inline | e_before_purge_inline | m_eof | U N F P S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_inline | e_before_purge_plain | m_none | U N F P S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_inline | e_before_purge_plain | m_top | U N F P S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_inline | e_before_purge_plain | m_eof | U N F P S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_inline | e_after_find_inline | m_none | U F N P S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_inline | e_after_find_inline | m_top | U F N P S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_inline | e_after_find_inline | m_eof | U F N P S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_inline | e_after_find_plain | m_none | U F N P S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_inline | e_after_find_plain | m_top | U F N P S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_inline | e_after_find_plain | m_eof | U F N P S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_inline | e_after_play_inline | m_none | U F P N S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_inline | e_after_play_inline | m_top | U F P N S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_inline | e_after_play_inline | m_eof | U F P N S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_inline | e_after_play_plain | m_none | U F P N S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_inline | e_after_play_plain | m_top | U F P N S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_inline | e_after_play_plain | m_eof | U F P N S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_inline | e_eof_inline | m_none | U F P N S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_inline | e_eof_inline | m_top | U F P N S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_inline | e_eof_inline | m_eof | U F P N S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_inline | e_eof_plain | m_none | U F P N S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_inline | e_eof_plain | m_top | U F P N S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_inline | e_eof_plain | m_eof | U F P N S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_plain | e_none | m_none | U F P S A D E E C | E E E E E E E E M S A s a D E E C | 0x385 | call |
| h_plain | e_none | m_top | U F P S A D E E C | N E E E E E E E E M S A s a D E E C | 0x385 | call |
| h_plain | e_none | m_eof | U F P S A D E E C | E E E E E E E E M N S A s a D E E C | 0x385 | call |
| h_plain | e_before_purge_inline | m_none | U N F P S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_plain | e_before_purge_inline | m_top | U N F P S A D E E C | N E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_plain | e_before_purge_inline | m_eof | U N F P S A D E E C | E E E E E E E E M N S A s a D E E C | 0x39c | inline |
| h_plain | e_before_purge_plain | m_none | N U F P S A D E E C | E E E E E E E E M S A s a D E E C | 0x385 | call |
| h_plain | e_before_purge_plain | m_top | N U F P S A D E E C | N E E E E E E E E M S A s a D E E C | 0x385 | call |
| h_plain | e_before_purge_plain | m_eof | N U F P S A D E E C | E E E E E E E E M N S A s a D E E C | 0x385 | call |
| h_plain | e_after_find_inline | m_none | U F N P S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_plain | e_after_find_inline | m_top | U F N P S A D E E C | N E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_plain | e_after_find_inline | m_eof | U F N P S A D E E C | E E E E E E E E M N S A s a D E E C | 0x39c | inline |
| h_plain | e_after_find_plain | m_none | U F N P S A D E E C | E E E E E E E E M S A s a D E E C | 0x385 | call |
| h_plain | e_after_find_plain | m_top | U F N P S A D E E C | N E E E E E E E E M S A s a D E E C | 0x385 | call |
| h_plain | e_after_find_plain | m_eof | U F N P S A D E E C | E E E E E E E E M N S A s a D E E C | 0x385 | call |
| h_plain | e_after_play_inline | m_none | U F P N S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_plain | e_after_play_inline | m_top | U F P N S A D E E C | N E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_plain | e_after_play_inline | m_eof | U F P N S A D E E C | E E E E E E E E M N S A s a D E E C | 0x39c | inline |
| h_plain | e_after_play_plain | m_none | U F P N S A D E E C | E E E E E E E E M S A s a D E E C | 0x385 | call |
| h_plain | e_after_play_plain | m_top | U F P N S A D E E C | N E E E E E E E E M S A s a D E E C | 0x385 | call |
| h_plain | e_after_play_plain | m_eof | U F P N S A D E E C | E E E E E E E E M N S A s a D E E C | 0x385 | call |
| h_plain | e_eof_inline | m_none | U F P N S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_plain | e_eof_inline | m_top | U F P N S A D E E C | N E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_plain | e_eof_inline | m_eof | U F P N S A D E E C | E E E E E E E E M N S A s a D E E C | 0x39c | inline |
| h_plain | e_eof_plain | m_none | U F P N S A D E E C | E E E E E E E E M S A s a D E E C | 0x385 | call |
| h_plain | e_eof_plain | m_top | U F P N S A D E E C | N E E E E E E E E M S A s a D E E C | 0x385 | call |
| h_plain | e_eof_plain | m_eof | U F P N S A D E E C | E E E E E E E E M N S A s a D E E C | 0x385 | call |
| h_inclass | e_none | m_none | U N F P S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |
| h_implicit | e_none | m_none | U N F P S A D E E C | E E E E E E E E M S A s a D E E C | 0x39c | inline |

52 header/definition combinations do not compile (redefinition or inline-in-class plus out-of-class body).

### audiere-node-shape (36 variants)

| node shape | /Ob | /Gy | Effects order after Play | Play size |
|---|---|---|---|---|
| plain_eof_inline | ob1 | gy | U F P N S A D E E C | 0x39c |
| plain_eof_inline | ob1 | nogy | U F P E E N S A D C | 0x39c |
| plain_eof_inline | ob2 | gy | U F P N S A D E E C | 0x39c |
| plain_eof_inline | ob2 | nogy | U F P E E N S A D C | 0x39c |
| plain_inclass | ob1 | gy | U N F P S A D E E C | 0x39c |
| plain_inclass | ob1 | nogy | U F P E E N S A D C | 0x39c |
| plain_inclass | ob2 | gy | U N F P S A D E E C | 0x39c |
| plain_inclass | ob2 | nogy | U F P E E N S A D C | 0x39c |
| plain_implicit | ob1 | gy | U N F P S A D E E C | 0x39c |
| plain_implicit | ob1 | nogy | U F P E E N S A D C | 0x39c |
| plain_implicit | ob2 | gy | U N F P S A D E E C | 0x39c |
| plain_implicit | ob2 | nogy | U F P E E N S A D C | 0x39c |
| tmpl_inclass | ob1 | gy | U F P A D N S E E C | 0x39c |
| tmpl_inclass | ob1 | nogy | U F P E E A D N S C | 0x39c |
| tmpl_inclass | ob2 | gy | U F P A D N S E E C | 0x39c |
| tmpl_inclass | ob2 | nogy | U F P E E A D N S C | 0x39c |
| tmpl_implicit | ob1 | gy | U N F P S A D E E C | 0x39c |
| tmpl_implicit | ob1 | nogy | U F P E E N S A D C | 0x39c |
| tmpl_implicit | ob2 | gy | U N F P S A D E E C | 0x39c |
| tmpl_implicit | ob2 | nogy | U F P E E N S A D C | 0x39c |
| tmpl_eof | ob1 | gy | U F P A D S N E E C | 0x39c |
| tmpl_eof | ob1 | nogy | U F P E E A D S N C | 0x39c |
| tmpl_eof | ob2 | gy | U F P A D S N E E C | 0x39c |
| tmpl_eof | ob2 | nogy | U F P E E A D S N C | 0x39c |
| tmpl_eof_inline | ob1 | gy | U F P A D S N E E C | 0x39c |
| tmpl_eof_inline | ob1 | nogy | U F P E E A D S N C | 0x39c |
| tmpl_eof_inline | ob2 | gy | U F P A D S N E E C | 0x39c |
| tmpl_eof_inline | ob2 | nogy | U F P E E A D S N C | 0x39c |
| tmpl_hdr_after | ob1 | gy | U F P A D S N E E C | 0x39c |
| tmpl_hdr_after | ob1 | nogy | U F P E E A D S N C | 0x39c |
| tmpl_hdr_after | ob2 | gy | U F P A D S N E E C | 0x39c |
| tmpl_hdr_after | ob2 | nogy | U F P E E A D S N C | 0x39c |
| tmpl_explicit_eof | ob1 | gy | U F P A D S N E E C | 0x39c |
| tmpl_explicit_eof | ob1 | nogy | U F P E E A D S N C | 0x39c |
| tmpl_explicit_eof | ob2 | gy | U F P A D S N E E C | 0x39c |
| tmpl_explicit_eof | ob2 | nogy | U F P E E A D S N C | 0x39c |

### any-vs-plain (18 variants, linked with /OPT:NOREF)

| a.cpp | b.cpp | object order | a.obj | b.obj | link |
|---|---|---|---|---|---|
| a_inline | b_plain | ab | U N M | N O | LNK1169 duplicate |
| a_inline | b_plain | ba | U N M | N O | LNK1169 duplicate |
| a_inline | b_inline_used | ab | U N M | K N O | N from a.obj |
| a_inline | b_inline_used | ba | U N M | K N O | N from b.obj |
| a_inline | b_none | ab | U N M | O | N from a.obj |
| a_inline | b_none | ba | U N M | O | N from a.obj |
| a_plain | b_plain | ab | U N M | N O | LNK1169 duplicate |
| a_plain | b_plain | ba | U N M | N O | LNK1169 duplicate |
| a_plain | b_inline_used | ab | U N M | K N O | LNK1169 duplicate |
| a_plain | b_inline_used | ba | U N M | K N O | LNK1169 duplicate |
| a_plain | b_none | ab | U N M | O | N from a.obj |
| a_plain | b_none | ba | U N M | O | N from a.obj |
| a_none | b_plain | ab | U M | N O | N from b.obj |
| a_none | b_plain | ba | U M | N O | N from b.obj |
| a_none | b_inline_used | ab | U M | K N O | N from b.obj |
| a_none | b_inline_used | ba | U M | K N O | N from b.obj |
| a_none | b_none | ab | U M | O | LNK1120 unresolved |
| a_none | b_none | ba | U M | O | LNK1120 unresolved |

### dimmer-nesting (32 variants)

| nest | destructor | flags | section order | function starts |
|---|---|---|---|---|
| plain | ool_end | gy | c0 G cA R M D X | c0:2+0x0 G:4+0x0 cA:5+0x0 R:6+0x0 M:7+0x0 D:8+0x0 X:9+0x0 |
| plain | ool_end | nogy | c0 cA R M D X G | c0:2+0x0 cA:2+0x23 R:2+0x4e M:2+0x62 D:2+0x7b X:2+0x8e G:4+0x0 |
| plain | inline_end | gy | c0 G cA R M D X | c0:2+0x0 G:4+0x0 cA:5+0x0 R:6+0x0 M:7+0x0 D:8+0x0 X:9+0x0 |
| plain | inline_end | nogy | c0 cA R M D G X | c0:2+0x0 cA:2+0x23 R:2+0x4e M:2+0x62 D:2+0x7b G:4+0x0 X:5+0x0 |
| plain | inclass | gy | c0 G X cA R M D | c0:2+0x0 G:4+0x0 X:5+0x0 cA:6+0x0 R:7+0x0 M:8+0x0 D:9+0x0 |
| plain | inclass | nogy | c0 cA R M D G X | c0:2+0x0 cA:2+0x23 R:2+0x4e M:2+0x62 D:2+0x7b G:4+0x0 X:5+0x0 |
| plain | undefined | gy | c0 G cA R M D | c0:2+0x0 G:4+0x0 cA:5+0x0 R:6+0x0 M:7+0x0 D:8+0x0 |
| plain | undefined | nogy | c0 cA R M D G | c0:2+0x0 cA:2+0x23 R:2+0x4e M:2+0x62 D:2+0x7b G:4+0x0 |
| nested_struct | ool_end | gy | c0 G cA R M D X | c0:2+0x0 G:4+0x0 cA:5+0x0 R:6+0x0 M:7+0x0 D:8+0x0 X:9+0x0 |
| nested_struct | ool_end | nogy | c0 cA R M D X G | c0:2+0x0 cA:2+0x23 R:2+0x4e M:2+0x62 D:2+0x7b X:2+0x8e G:4+0x0 |
| nested_struct | inline_end | gy | c0 G cA R M D X | c0:2+0x0 G:4+0x0 cA:5+0x0 R:6+0x0 M:7+0x0 D:8+0x0 X:9+0x0 |
| nested_struct | inline_end | nogy | c0 cA R M D G X | c0:2+0x0 cA:2+0x23 R:2+0x4e M:2+0x62 D:2+0x7b G:4+0x0 X:5+0x0 |
| nested_struct | inclass | gy | c0 G X cA R M D | c0:2+0x0 G:4+0x0 X:5+0x0 cA:6+0x0 R:7+0x0 M:8+0x0 D:9+0x0 |
| nested_struct | inclass | nogy | c0 cA R M D G X | c0:2+0x0 cA:2+0x23 R:2+0x4e M:2+0x62 D:2+0x7b G:4+0x0 X:5+0x0 |
| nested_struct | undefined | gy | c0 G cA R M D | c0:2+0x0 G:4+0x0 cA:5+0x0 R:6+0x0 M:7+0x0 D:8+0x0 |
| nested_struct | undefined | nogy | c0 cA R M D G | c0:2+0x0 cA:2+0x23 R:2+0x4e M:2+0x62 D:2+0x7b G:4+0x0 |
| nested_class_virtual | ool_end | gy | c0 G cA R M D X | c0:2+0x0 G:4+0x0 cA:5+0x0 R:6+0x0 M:7+0x0 D:8+0x0 X:9+0x0 |
| nested_class_virtual | ool_end | nogy | c0 cA R M D X G | c0:2+0x0 cA:2+0x23 R:2+0x4e M:2+0x62 D:2+0x7b X:2+0x8e G:4+0x0 |
| nested_class_virtual | inline_end | gy | c0 G cA R M D X | c0:2+0x0 G:4+0x0 cA:5+0x0 R:6+0x0 M:7+0x0 D:8+0x0 X:9+0x0 |
| nested_class_virtual | inline_end | nogy | c0 cA R M D G X | c0:2+0x0 cA:2+0x23 R:2+0x4e M:2+0x62 D:2+0x7b G:4+0x0 X:5+0x0 |
| nested_class_virtual | inclass | gy | c0 G X cA R M D | c0:2+0x0 G:4+0x0 X:5+0x0 cA:6+0x0 R:7+0x0 M:8+0x0 D:9+0x0 |
| nested_class_virtual | inclass | nogy | c0 cA R M D G X | c0:2+0x0 cA:2+0x23 R:2+0x4e M:2+0x62 D:2+0x7b G:4+0x0 X:5+0x0 |
| nested_class_virtual | undefined | gy | c0 G cA R M D | c0:2+0x0 G:4+0x0 cA:5+0x0 R:6+0x0 M:7+0x0 D:8+0x0 |
| nested_class_virtual | undefined | nogy | c0 cA R M D G | c0:2+0x0 cA:2+0x23 R:2+0x4e M:2+0x62 D:2+0x7b G:4+0x0 |
| namespace | ool_end | gy | c0 G cA R M D X | c0:2+0x0 G:4+0x0 cA:5+0x0 R:6+0x0 M:7+0x0 D:8+0x0 X:9+0x0 |
| namespace | ool_end | nogy | c0 cA R M D X G | c0:2+0x0 cA:2+0x23 R:2+0x4e M:2+0x62 D:2+0x7b X:2+0x8e G:4+0x0 |
| namespace | inline_end | gy | c0 G cA R M D X | c0:2+0x0 G:4+0x0 cA:5+0x0 R:6+0x0 M:7+0x0 D:8+0x0 X:9+0x0 |
| namespace | inline_end | nogy | c0 cA R M D G X | c0:2+0x0 cA:2+0x23 R:2+0x4e M:2+0x62 D:2+0x7b G:4+0x0 X:5+0x0 |
| namespace | inclass | gy | c0 G X cA R M D | c0:2+0x0 G:4+0x0 X:5+0x0 cA:6+0x0 R:7+0x0 M:8+0x0 D:9+0x0 |
| namespace | inclass | nogy | c0 cA R M D G X | c0:2+0x0 cA:2+0x23 R:2+0x4e M:2+0x62 D:2+0x7b G:4+0x0 X:5+0x0 |
| namespace | undefined | gy | c0 G cA R M D | c0:2+0x0 G:4+0x0 cA:5+0x0 R:6+0x0 M:7+0x0 D:8+0x0 |
| namespace | undefined | nogy | c0 cA R M D G | c0:2+0x0 cA:2+0x23 R:2+0x4e M:2+0x62 D:2+0x7b G:4+0x0 |

### dimmer-flags (27 variants)

| nest | destructor | flags | section order | function starts |
|---|---|---|---|---|
| plain | ool_end | nogy | c0 cA R M D X G | c0:2+0x0 cA:2+0x23 R:2+0x4e M:2+0x62 D:2+0x7b X:2+0x8e G:4+0x0 |
| plain | ool_end | Zi | c0 cA R M D X G | c0:3+0x0 cA:3+0x23 R:3+0x4e M:3+0x62 D:3+0x7b X:3+0x8e G:5+0x0 |
| plain | ool_end | Z7 | c0 cA R M D X G | c0:3+0x0 cA:3+0x23 R:3+0x4e M:3+0x62 D:3+0x7b X:3+0x8e G:5+0x0 |
| plain | ool_end | GZ | c0 cA R M D X G | c0:2+0x0 cA:2+0x34 R:2+0x70 M:2+0x8b D:2+0xb5 X:2+0xd9 G:4+0x0 |
| plain | ool_end | Ge | c0 cA R M D X G | c0:2+0x0 cA:2+0x2c R:2+0x60 M:2+0x7d D:2+0x9f X:2+0xbb G:4+0x0 |
| plain | ool_end | Gh | c0 cA R M D X G | c0:2+0x0 cA:2+0x28 R:2+0x58 M:2+0x71 D:2+0x8f X:2+0xa7 G:4+0x0 |
| plain | ool_end | Gs0 | c0 cA R M D X G | c0:2+0x0 cA:2+0x2c R:2+0x60 M:2+0x7d D:2+0x9f X:2+0xbb G:4+0x0 |
| plain | ool_end | Gm_Zi | c0 cA R M D X G | c0:3+0x0 cA:3+0x23 R:3+0x4e M:3+0x62 D:3+0x7b X:3+0x8e G:5+0x0 |
| plain | ool_end | Oy- | c0 cA R M D X G | c0:2+0x0 cA:2+0x23 R:2+0x4e M:2+0x62 D:2+0x7b X:2+0x8e G:4+0x0 |
| plain | inline_end | nogy | c0 cA R M D G X | c0:2+0x0 cA:2+0x23 R:2+0x4e M:2+0x62 D:2+0x7b G:4+0x0 X:5+0x0 |
| plain | inline_end | Zi | c0 cA R M D G X | c0:3+0x0 cA:3+0x23 R:3+0x4e M:3+0x62 D:3+0x7b G:5+0x0 X:7+0x0 |
| plain | inline_end | Z7 | c0 cA R M D G X | c0:3+0x0 cA:3+0x23 R:3+0x4e M:3+0x62 D:3+0x7b G:5+0x0 X:7+0x0 |
| plain | inline_end | GZ | c0 cA R M D G X | c0:2+0x0 cA:2+0x34 R:2+0x70 M:2+0x8b D:2+0xb5 G:4+0x0 X:5+0x0 |
| plain | inline_end | Ge | c0 cA R M D G X | c0:2+0x0 cA:2+0x2c R:2+0x60 M:2+0x7d D:2+0x9f G:4+0x0 X:5+0x0 |
| plain | inline_end | Gh | c0 cA R M D G X | c0:2+0x0 cA:2+0x28 R:2+0x58 M:2+0x71 D:2+0x8f G:4+0x0 X:5+0x0 |
| plain | inline_end | Gs0 | c0 cA R M D G X | c0:2+0x0 cA:2+0x2c R:2+0x60 M:2+0x7d D:2+0x9f G:4+0x0 X:5+0x0 |
| plain | inline_end | Gm_Zi | c0 cA R M D G X | c0:3+0x0 cA:3+0x23 R:3+0x4e M:3+0x62 D:3+0x7b G:5+0x0 X:7+0x0 |
| plain | inline_end | Oy- | c0 cA R M D G X | c0:2+0x0 cA:2+0x23 R:2+0x4e M:2+0x62 D:2+0x7b G:4+0x0 X:5+0x0 |
| plain | inclass | nogy | c0 cA R M D G X | c0:2+0x0 cA:2+0x23 R:2+0x4e M:2+0x62 D:2+0x7b G:4+0x0 X:5+0x0 |
| plain | inclass | Zi | c0 cA R M D G X | c0:3+0x0 cA:3+0x23 R:3+0x4e M:3+0x62 D:3+0x7b G:5+0x0 X:7+0x0 |
| plain | inclass | Z7 | c0 cA R M D G X | c0:3+0x0 cA:3+0x23 R:3+0x4e M:3+0x62 D:3+0x7b G:5+0x0 X:7+0x0 |
| plain | inclass | GZ | c0 cA R M D G X | c0:2+0x0 cA:2+0x34 R:2+0x70 M:2+0x8b D:2+0xb5 G:4+0x0 X:5+0x0 |
| plain | inclass | Ge | c0 cA R M D G X | c0:2+0x0 cA:2+0x2c R:2+0x60 M:2+0x7d D:2+0x9f G:4+0x0 X:5+0x0 |
| plain | inclass | Gh | c0 cA R M D G X | c0:2+0x0 cA:2+0x28 R:2+0x58 M:2+0x71 D:2+0x8f G:4+0x0 X:5+0x0 |
| plain | inclass | Gs0 | c0 cA R M D G X | c0:2+0x0 cA:2+0x2c R:2+0x60 M:2+0x7d D:2+0x9f G:4+0x0 X:5+0x0 |
| plain | inclass | Gm_Zi | c0 cA R M D G X | c0:3+0x0 cA:3+0x23 R:3+0x4e M:3+0x62 D:3+0x7b G:5+0x0 X:7+0x0 |
| plain | inclass | Oy- | c0 cA R M D G X | c0:2+0x0 cA:2+0x23 R:2+0x4e M:2+0x62 D:2+0x7b G:4+0x0 X:5+0x0 |
