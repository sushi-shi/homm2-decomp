# overwritten-store-vs-discarded-call

**Symptom.** Retail makes a call, stores its return value into a member, and
then immediately overwrites that same member with a constant. Ours makes the
same call and throws the result away, so we are exactly one `mov` short and
every following register allocation rotates.

`textEntryWidget::Read` (RVA 0xd1f60), the last three deserialisation reads.

## The bytes

```
ours: gpResourceManager->ReadWord();          /* result discarded */
      m_kind = WIDGET_KIND_TEXT_ENTRY;
--------------------------------------------------------------------------
50e: 8b 0d 00 00 00 00     movl    <gpResourceManager>, %ecx
514: e8 00 00 00 00        calll   <resourceManager::ReadWord>
519: 8b 45 e8              movl    -0x18(%ebp), %eax
51c: 66 c7 40 14 00 40     movw    $0x4000, 0x14(%eax)     ; m_kind

retail
--------------------------------------------------------------------------
5a4: 8b 0d 00 00 00 00     movl    <gpResourceManager>, %ecx
5aa: e8 00 00 00 00        calll   <resourceManager::ReadWord>
5af: 8b 55 e8              movl    -0x18(%ebp), %edx
5b2: 66 89 42 14           movw    %ax, 0x14(%edx)         ; m_kind = ReadWord()
5b6: 8b 45 e8              movl    -0x18(%ebp), %eax
5b9: 66 c7 40 14 00 40     movw    $0x4000, 0x14(%eax)     ; m_kind = <const>
```

## The rule

`/Od` performs no dead-store elimination whatsoever. A store whose value is
overwritten by the very next statement still costs its `mov`, and the reload of
`this` for the second store is emitted too. So:

- **call, then a store of `eax`/`ax` into a field that the next instruction
  overwrites** = the source really assigns the call result to that field and
  then reassigns it. It is a dead statement in the original, kept verbatim.
- a bare call with **no** store = the source discarded the result
  (`f();` as a statement).

Do not "clean up" the redundant assignment when reconstructing: the second
store's `mov this` reload is the tell that two separate statements wrote the
same field. The same reasoning identifies the dead `m_maxLength = maxLength;`
that `textEntryWidget`'s big constructor performs twice (RVA 0xd1d50, offsets
+0x6c and +0xfc), where the second store is followed by a read of the member,
so only the *first* one is dead.

## What made it match

```cpp
m_iconFrame = gpResourceManager->ReadWord();
m_id = gpResourceManager->ReadWord();
m_kind = static_cast<WidgetKind>(gpResourceManager->ReadWord());
m_kind = WIDGET_KIND_TEXT_ENTRY;
```

The `static_cast` is only for the strict-enum build; it is byte-neutral because
the destination is 16 bits wide, so VC6 keeps the 16-bit store of `%ax` and
never materialises the widened value.

`textEntryWidget::Read` 91.12% -> EXACT (with the `m_alignment` mask and the
`m_preserveTextOnFocus` if/else in the same change).
