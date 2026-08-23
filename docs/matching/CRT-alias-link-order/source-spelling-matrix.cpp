// Functions:
//   SetupCDDrive       retail RVA 0x000bf370, size 0x035f
//   game::GetMap       retail RVA 0x000754b0, size 0x01d2
//   game::NewGame      retail RVA 0x00075b4b, size 0x0a71
//
// Reviewed final-link spelling matrix:
//   /tmp/homm2_crt_alias_matrix.py
//   build/link/native-crt-alias-matrix/{00..0f}/
//
// COMPLETE: 16/16 combinations of these independent COFF identities:
//   RESMGR lseek, Misc lseek, Misc chdir, and dpnetwin strupr.
// The matrix used the untouched VC6 SP5 monolithic LIBCMT.LIB and
// OLDNAMES.LIB. Selected CRT members were recovered from each LINK map and
// compared with config/retail_crt_order.txt.
//
// Relevant arms:
//
//   mask 00: legacy Misc lseek/chdir
//     first CRT divergence 43: fputs.obj vs retail lseek.obj
//     Rich OLDNAMES count 11; whole file 93.548462%
//
//   mask 02: direct Misc lseek, legacy Misc chdir
//     first CRT divergence 44: fputs.obj vs retail chdir.obj
//     Rich OLDNAMES count 11; whole file 94.098930%
//
//   mask 04: legacy Misc lseek, direct Misc chdir
//     first CRT divergence 43: chdir.obj vs retail lseek.obj
//     Rich OLDNAMES count 10; whole file 96.688086%
//
//   mask 06: direct Misc lseek and direct Misc chdir
//     first CRT divergence 119: initcoll.obj vs retail strcspn.obj
//     Rich OLDNAMES count 10; whole file 99.097503%
//
// Direct `_lseek` in Misc does not remove the lseek alias producer because
// RESMGR still calls `lseek`. Direct `_chdir` removes the only chdir alias and
// is the source of the remaining 10-vs-11 producer-count gap.
//
// Balance probe:
//   /tmp/homm2_stricmp_balance_probe.py
//   build/link/native-stricmp-balance/
//
// Replacing Newgame.obj's `_strcmpi` undefined identity with `_stricmp`
// models `stricmp` throughout that TU. VC6 OLDNAMES has distinct strcmpi.obi
// and stricmp.obi members which weak-alias the same entry in stricmp.obj.
// Other TUs retain `strcmpi`, so both alias members are selected. Against mask
// 06 this changed no .text or .data byte, retained the 119-member exact CRT
// prefix, raised the OLDNAMES count from 10 to retail 11, and reproduced the
// complete retail Rich producer order and counts without rewriting the PE.
//
// Disposition: ACCEPT the ordinary VC6 spellings below. The stripped image
// cannot prove them from call bytes alone; the evidence is the monolithic
// archive extraction order plus the untouched LINK Rich census. Generated
// object-identity probes are not retained.
//
//     _chdir(gcRegAppPath)
//     _lseek(handle, 0, SEEK_END)
//     _lseek(handle, -CD_PROBE_TRAILER_SIZE, SEEK_CUR)
//     stricmp(...)  // Newgame.cpp only
//
// Integrated-source verification:
//   build/link/native-current-source/
//
// A fresh untouched VC6 LINK using the rebuilt Misc.obj and Newgame.obj
// reproduced the complete retail Rich producer sequence and counts, retained
// the exact first 119/177 retail CRT members (first residual remains
// initcoll.obj vs retail strcspn.obj), and reached 99.119429% whole-file
// identity. The ordinary reviewed comparison remains 1727/1727 because its
// paired same-RVA alias pass measures linked-target semantics. A direct raw
// delinked-object comparison instead exposes the spelling-only residuals:
// SetupCDDrive 99.870690%, GetMap 99.903700%, and NewGame 99.969124%.
