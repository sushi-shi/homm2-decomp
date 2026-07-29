; ?GetAdjacentCellIndex@army@@QAEHHH@Z
; unit SOURCE/PATH  src_hash 46dd80061b7b  score 100.0000  seed 2718  trial 4
; size 165  text_sha 32c915a81abdc3cf
; relocs ["0000008d:0006:?gpCombatManager@@3PAVcombatManager@@A:00000000"]
; hex 558bec83ec04535657894dfc837d08ff0f850a000000b8ffffffffe97e000000837d0c060f85280000008b45fc83b886000000010f850c000000c7450c05000000e907000000c7450c00000000e92d000000837d0c070f85230000008b45fc83b886000000010f850c000000c7450c03000000e907000000c7450c020000008b450c8b4d088d0c498d04488b0d000000000fbe8408adf50000e9000000005f5e5bc9c20800

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 04             	sub    esp,0x4
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 4d fc             	mov    DWORD PTR [ebp-0x4],ecx
   c:	83 7d 08 ff          	cmp    DWORD PTR [ebp+0x8],0xffffffff
  10:	0f 85 0a 00 00 00    	jne    0x20
  16:	b8 ff ff ff ff       	mov    eax,0xffffffff
  1b:	e9 7e 00 00 00       	jmp    0x9e
  20:	83 7d 0c 06          	cmp    DWORD PTR [ebp+0xc],0x6
  24:	0f 85 28 00 00 00    	jne    0x52
  2a:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
  2d:	83 b8 86 00 00 00 01 	cmp    DWORD PTR [eax+0x86],0x1
  34:	0f 85 0c 00 00 00    	jne    0x46
  3a:	c7 45 0c 05 00 00 00 	mov    DWORD PTR [ebp+0xc],0x5
  41:	e9 07 00 00 00       	jmp    0x4d
  46:	c7 45 0c 00 00 00 00 	mov    DWORD PTR [ebp+0xc],0x0
  4d:	e9 2d 00 00 00       	jmp    0x7f
  52:	83 7d 0c 07          	cmp    DWORD PTR [ebp+0xc],0x7
  56:	0f 85 23 00 00 00    	jne    0x7f
  5c:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
  5f:	83 b8 86 00 00 00 01 	cmp    DWORD PTR [eax+0x86],0x1
  66:	0f 85 0c 00 00 00    	jne    0x78
  6c:	c7 45 0c 03 00 00 00 	mov    DWORD PTR [ebp+0xc],0x3
  73:	e9 07 00 00 00       	jmp    0x7f
  78:	c7 45 0c 02 00 00 00 	mov    DWORD PTR [ebp+0xc],0x2
  7f:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
  82:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  85:	8d 0c 49             	lea    ecx,[ecx+ecx*2]
  88:	8d 04 48             	lea    eax,[eax+ecx*2]
  8b:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
  91:	0f be 84 08 ad f5 00 	movsx  eax,BYTE PTR [eax+ecx*1+0xf5ad]
  98:	00
  99:	e9 00 00 00 00       	jmp    0x9e
  9e:	5f                   	pop    edi
  9f:	5e                   	pop    esi
  a0:	5b                   	pop    ebx
  a1:	c9                   	leave
  a2:	c2 08 00             	ret    0x8
