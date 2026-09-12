# API output types and aligned scalar reservations

Measured with this branch's VC6 SP5 profile. A compiler's four-byte stack
reservation is not evidence for a four-element byte array, and a two-word
array is not the right owner for one `DWORD` API output.

S22 retained `u8 value; com_rcv(0, 1, &value); return value;`. Retail and the
candidate agree, including the wide load followed by an eight-bit mask:

```
retail address / bytes                 candidate function offset / bytes
00473220 83 ec 08  sub esp,8            +003 83 ec 08
00473223 8d 45 fc  lea eax,[ebp-4]      +006 8d 45 fc
00473227 ba 01 00 00 00 mov edx,1       +00a ba 01 00 00 00
0047322e e8 54 fd fb ff call com_rcv    +011 e8 00 00 00 00 ; REL32 com_rcv
0047323f 8b 45 fc  mov eax,[ebp-4]      +022 8b 45 fc
00473242 25 ff 00 00 00 and eax,0ffh    +025 25 ff 00 00 00
```

The API writes one byte. VC6's masked wide load does not require inventing
three extra source elements; its native scalar arm emits the identical load.

C15 retained `DWORD bytesRead; ReadFile(..., &bytesRead, NULL); return
static_cast<i16>(bytesRead);`. This preserves the previous low-word ABI:

```
retail address / bytes                 candidate function offset / bytes
00433006 8d 45 f8 lea eax,[ebp-8]       +07f 8d 45 f8
00433020 ff 15 3c a1 4e 00             +099 ff 15 00 00 00 00
         call [ReadFile]                    ; DIR32 __imp__ReadFile@20
00433039 66 8b 45 f8 mov ax,[ebp-8]     +0b2 66 8b 45 f8
```

Separate complete two-arm matrices produce identical 51/193-byte targets
and 4/12 blocks. Both final objects pass full native whole-section comparison;
both functions pass scoped genuine-retail bytes, DIR32 sites and ordered
target checks. See [serial-scalar evidence](../reconstruction/serial-scalars.md).
These are sufficient reconstructions, not claims about the lost original
variable names or declarations.
