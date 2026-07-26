// Experiment: inline a shared byte-reader at all seven decoder read sites.
// Result: candidate/retail blocks 77/76: 11 exact, 14 size-only,
// 23 target-shift, 28 flow-kind, and 1 missing.
// Disposition: rejected as a major topology regression.

#if 0
static inline u8 ReadIconRleByte(u8*& src)
{
    return *src++;
}

u8 cmd = ReadIconRleByte(src);
u8 color = ReadIconRleByte(src);
u8 flags = ReadIconRleByte(src);
u8 count = ReadIconRleByte(src);
u8 literal = ReadIconRleByte(src);
u8 dim = ReadIconRleByte(src);
u8 mapped = ReadIconRleByte(src);
#endif
