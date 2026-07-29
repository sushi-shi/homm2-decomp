; ?GetAdjacentCellIndexNoArmy@@YIHHH@Z
; unit SOURCE/PATH  src_hash dd32291a7b23  score 100.0000  seed 2718  trial 1
; size 110  text_sha 544e61c261d9ec0f
; relocs ["00000058:0006:?gpCombatManager@@3PAVcombatManager@@A:00000000"]
; hex 558bec83ec085356578955f8894dfc837dfcff0f850a000000b8ffffffffe946000000837df8060f850c000000c745f805000000e911000000837df8070f8507000000c745f8030000008b45f88b4dfc8d0c498d04488b0d000000000fbe8408adf50000e9000000005f5e5bc9c3

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 08             	sub    esp,0x8
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 55 f8             	mov    DWORD PTR [ebp-0x8],edx
   c:	89 4d fc             	mov    DWORD PTR [ebp-0x4],ecx
   f:	83 7d fc ff          	cmp    DWORD PTR [ebp-0x4],0xffffffff
  13:	0f 85 0a 00 00 00    	jne    0x23
  19:	b8 ff ff ff ff       	mov    eax,0xffffffff
  1e:	e9 46 00 00 00       	jmp    0x69
  23:	83 7d f8 06          	cmp    DWORD PTR [ebp-0x8],0x6
  27:	0f 85 0c 00 00 00    	jne    0x39
  2d:	c7 45 f8 05 00 00 00 	mov    DWORD PTR [ebp-0x8],0x5
  34:	e9 11 00 00 00       	jmp    0x4a
  39:	83 7d f8 07          	cmp    DWORD PTR [ebp-0x8],0x7
  3d:	0f 85 07 00 00 00    	jne    0x4a
  43:	c7 45 f8 03 00 00 00 	mov    DWORD PTR [ebp-0x8],0x3
  4a:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
  4d:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
  50:	8d 0c 49             	lea    ecx,[ecx+ecx*2]
  53:	8d 04 48             	lea    eax,[eax+ecx*2]
  56:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
  5c:	0f be 84 08 ad f5 00 	movsx  eax,BYTE PTR [eax+ecx*1+0xf5ad]
  63:	00 
  64:	e9 00 00 00 00       	jmp    0x69
  69:	5f                   	pop    edi
  6a:	5e                   	pop    esi
  6b:	5b                   	pop    ebx
  6c:	c9                   	leave
  6d:	c3                   	ret
