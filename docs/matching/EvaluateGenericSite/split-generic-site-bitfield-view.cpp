// Reviewed structural experiment for philAI::EvaluateGenericSite.
//
// Attempted mapCell view:
//
// struct {
//     u16 : 3;
//     u16 m_genericSiteTypeBits : 6;
//     u16 m_genericSiteLevel : 7;
// };
//
// Attempted use:
//
// siteType0 = static_cast<GenericSiteType>(cell->m_genericSiteTypeBits);
// siteLevel5 = cell->m_genericSiteLevel;
//
// MSVC 4.2 moved both masks into the field loads. That removed the retail
// post-store type mask and level shift, shortened the function, and shifted
// its internal jump-table relocation addends. Disposition: rejected. Retain
// the neutral 13-bit object metadata view and decode type and level explicitly.
