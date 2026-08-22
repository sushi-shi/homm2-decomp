# `gcThisNetName` data closure

`SOURCE/REMOTE` emitted the correct `0xb6d`-byte BSS section, but
`gcThisNetName` lacked a source `DATA` identity.  The candidate normalizer
therefore treated its 32 bytes as the tail of the preceding `iLastIds` object.

The retail object places `iLastIds[30]` at `0x533ba0` for 120 bytes,
`gcThisNetName[32]` at `0x533c18`, and `rcvBufOut` at `0x533c38`.  These
contiguous boundaries prove the missing address without relying on a symbol
name alone.  Adding `DATA(0x00533c18)` is byte-neutral and restores the
candidate object's semantic identity and exact BSS allocation boundary.
