/*
 * IconToBitmapColorTable, RVA 0xd32a0: staged prologue ownership.
 *
 * Retail initially owns the data root in EDI, X in EBP/EBX, the selected
 * entry in ESI, and Y in EBP.  Five coherent setup structures tested whether
 * source-level load and publication order selected that register orbit:
 */

#if 0
u8* data = srcIcon->m_data;
i32 X = x;
i32 entryOffset = frame * sizeof(IconEntry);
i32 entryX = reinterpret_cast<IconEntry*>(data + entryOffset)->x;
i32 sourceOffset = reinterpret_cast<IconEntry*>(data + entryOffset)->srcOffset;
IconEntry* entry = reinterpret_cast<IconEntry*>(data + entryOffset);
X = X + entryX;
i32 currentY = y;
gCTEntry = entry;
gCTSrc = data + sourceOffset;
gCTX0 = X;
gCTPitch = dest->m_width;
currentY = currentY + entry->y;
gCTY = currentY;
#endif

/*
 * Artifacts:
 *
 *   build/icon2bc-staged-prologue-axes.json
 *   build/icon2bc-staged-prologue-manifest.json
 *   build/match-variants/icon2bc-staged-prologue/
 *
 * Complete product: five structures times clean plus 50 declaration-forest
 * states, 255/255 compilations in 213.78 seconds.  Source was restored.
 *
 * Best by structure:
 *
 *   retained setup              82.776990%, size 1428, 89/91 relocations
 *   source before publication   82.565730%, size 1414, 86/91 relocations
 *   entry then snapshots        82.354460%
 *   family publication order    82.354460%
 *   explicit retail load order  82.093895%
 *
 * The staged forms are genuine lower compiler-state islands, but forcing the
 * apparent retail load order at source level does not select retail's global
 * register orbit.  None exceeds the retained 82.8122% MAX, so no setup change
 * is retained.
 */

void IconToBitmapColorTable_staged_prologue_owner_attempts() {
}
