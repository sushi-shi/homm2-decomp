; ?SortStats@@YIXQAJQAC@Z
; unit SOURCE/TOWNMGR  src_hash b1cdf5f858d2  score 100.0000  seed 1213156658  trial 1
; size 217  text_sha 0955918a2ccc2797
; relocs ["0000001f:0006:?gpGame@@3PAVgame@@A:00000000", "00000044:0006:?gpGame@@3PAVgame@@A:00000000"]
; hex 558bec83ec185356578955e8894decc745f400000000e903000000ff45f4a1000000000fbe808e040000483b45f40f8e9b0000008b45f4408945f8e903000000ff45f8a1000000000fbe808e0400003b45f80f8e720000008b45f88b4dec8b55f48b5dec8b14933914810f8e550000008b45f48b4dec8b04818945f08b45f88b4dec8b04818b4df48b55ec89048a8b45f08b4df88b55ec89048a8b45f48b4de80fbe04088945fc8b45f88b4de88a04088b4df48b55e88804118a45fc8b4df88b55e8880411e976ffffffe94cffffffe9000000005f5e5bc9c3

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 18             	sub    esp,0x18
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 55 e8             	mov    DWORD PTR [ebp-0x18],edx
   c:	89 4d ec             	mov    DWORD PTR [ebp-0x14],ecx
   f:	c7 45 f4 00 00 00 00 	mov    DWORD PTR [ebp-0xc],0x0
  16:	e9 03 00 00 00       	jmp    0x1e
  1b:	ff 45 f4             	inc    DWORD PTR [ebp-0xc]
  1e:	a1 00 00 00 00       	mov    eax,ds:0x0
  23:	0f be 80 8e 04 00 00 	movsx  eax,BYTE PTR [eax+0x48e]
  2a:	48                   	dec    eax
  2b:	3b 45 f4             	cmp    eax,DWORD PTR [ebp-0xc]
  2e:	0f 8e 9b 00 00 00    	jle    0xcf
  34:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
  37:	40                   	inc    eax
  38:	89 45 f8             	mov    DWORD PTR [ebp-0x8],eax
  3b:	e9 03 00 00 00       	jmp    0x43
  40:	ff 45 f8             	inc    DWORD PTR [ebp-0x8]
  43:	a1 00 00 00 00       	mov    eax,ds:0x0
  48:	0f be 80 8e 04 00 00 	movsx  eax,BYTE PTR [eax+0x48e]
  4f:	3b 45 f8             	cmp    eax,DWORD PTR [ebp-0x8]
  52:	0f 8e 72 00 00 00    	jle    0xca
  58:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
  5b:	8b 4d ec             	mov    ecx,DWORD PTR [ebp-0x14]
  5e:	8b 55 f4             	mov    edx,DWORD PTR [ebp-0xc]
  61:	8b 5d ec             	mov    ebx,DWORD PTR [ebp-0x14]
  64:	8b 14 93             	mov    edx,DWORD PTR [ebx+edx*4]
  67:	39 14 81             	cmp    DWORD PTR [ecx+eax*4],edx
  6a:	0f 8e 55 00 00 00    	jle    0xc5
  70:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
  73:	8b 4d ec             	mov    ecx,DWORD PTR [ebp-0x14]
  76:	8b 04 81             	mov    eax,DWORD PTR [ecx+eax*4]
  79:	89 45 f0             	mov    DWORD PTR [ebp-0x10],eax
  7c:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
  7f:	8b 4d ec             	mov    ecx,DWORD PTR [ebp-0x14]
  82:	8b 04 81             	mov    eax,DWORD PTR [ecx+eax*4]
  85:	8b 4d f4             	mov    ecx,DWORD PTR [ebp-0xc]
  88:	8b 55 ec             	mov    edx,DWORD PTR [ebp-0x14]
  8b:	89 04 8a             	mov    DWORD PTR [edx+ecx*4],eax
  8e:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  91:	8b 4d f8             	mov    ecx,DWORD PTR [ebp-0x8]
  94:	8b 55 ec             	mov    edx,DWORD PTR [ebp-0x14]
  97:	89 04 8a             	mov    DWORD PTR [edx+ecx*4],eax
  9a:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
  9d:	8b 4d e8             	mov    ecx,DWORD PTR [ebp-0x18]
  a0:	0f be 04 08          	movsx  eax,BYTE PTR [eax+ecx*1]
  a4:	89 45 fc             	mov    DWORD PTR [ebp-0x4],eax
  a7:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
  aa:	8b 4d e8             	mov    ecx,DWORD PTR [ebp-0x18]
  ad:	8a 04 08             	mov    al,BYTE PTR [eax+ecx*1]
  b0:	8b 4d f4             	mov    ecx,DWORD PTR [ebp-0xc]
  b3:	8b 55 e8             	mov    edx,DWORD PTR [ebp-0x18]
  b6:	88 04 11             	mov    BYTE PTR [ecx+edx*1],al
  b9:	8a 45 fc             	mov    al,BYTE PTR [ebp-0x4]
  bc:	8b 4d f8             	mov    ecx,DWORD PTR [ebp-0x8]
  bf:	8b 55 e8             	mov    edx,DWORD PTR [ebp-0x18]
  c2:	88 04 11             	mov    BYTE PTR [ecx+edx*1],al
  c5:	e9 76 ff ff ff       	jmp    0x40
  ca:	e9 4c ff ff ff       	jmp    0x1b
  cf:	e9 00 00 00 00       	jmp    0xd4
  d4:	5f                   	pop    edi
  d5:	5e                   	pop    esi
  d6:	5b                   	pop    ebx
  d7:	c9                   	leave
  d8:	c3                   	ret
