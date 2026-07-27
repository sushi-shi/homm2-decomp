/*
 * mouseManager::Close, RVA 0xc93f0: imported API pointer lifetimes,
 * 2026-07-27.
 *
 * Fresh evidence gives retail size 237, exact 17/17 block flow, and 18/18
 * relocation counts.  The sole live disassembly divergence is the ownership
 * of two imported calls across the cursor loop:
 *
 *   candidate: edi = DeleteObject; ebx = DestroyIcon
 *   retail:    edi = DestroyIcon;   ebx = DeleteObject
 *
 * An unchanged-source clean plus 50 top-of-TU declaration-forest census
 * emitted one text island at 99.692310%, size 237, with 18/18 relocations.
 *
 * Because the same kind of explicit imported-function lifetime closed
 * SetupCDDrive, a complete five-arm source matrix tested direct calls,
 * DestroyIcon then DeleteObject pointer declarations, the reverse declaration
 * order, and each pointer independently:
 */

#if 0
typedef BOOL (WINAPI* DestroyIconCommand)(HICON);
typedef BOOL (WINAPI* DeleteObjectCommand)(HGDIOBJ);

DestroyIconCommand destroyCursorIcon = DestroyIcon;
DeleteObjectCommand deleteBitmapObject = DeleteObject;

destroyCursorIcon(hMouseCursor[cursorIndex]);
deleteBitmapObject(hbmpAndMask[cursorIndex]);
#endif

/*
 * Every source arm was compiled against the clean state and all 50 forest
 * states: 5 x 51 = 255/255 cells in 239.46 seconds, without truncation.
 * The source was restored.
 *
 *   structure                    best        size
 *   direct calls                 99.692310   237
 *   Destroy then Delete          93.830770   237
 *   Delete then Destroy          93.523080   237
 *   Destroy pointer only         96.923080   237
 *   Delete pointer only          96.615390   237
 *
 * Thus explicit callable ownership is a real lower family here, unlike the
 * exact SetupCDDrive result.  It cannot produce retail's register assignment,
 * and no arm has exact ordered relocation identity.  Retain the direct source
 * and the historical 99.692310% MAX.
 *
 * Artifacts:
 *   build/mouse-close-top-states-20260727.json
 *   build/tu-state-noise/mouse-close-top-20260727
 *   build/mouse-close-import-pointer-axes-20260727.json
 *   build/mouse-close-import-pointer-manifest-20260727.json
 *   build/match-variants/mouse-close-import-pointer-20260727
 */

void mouseManager_Close_imported_api_pointer_lifetime_attempts() {
}
