// Experiment: shared byte-reader only at the first five command/control reads.
// Result: the same gross 77/76 block topology regression as the all-site form.
// Disposition: rejected.

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

// Pixel-loop reads remained direct in this arm.
i32 px = *src++;
#endif
