; ?MapExtraPosAndAdjacentsSet@@YIHHHE@Z
; unit SOURCE/ADVMGR  src_hash 715ac9bad32c  score 100.0000  seed 1213156658  trial 14
; size 246  text_sha 647f50363843782f
; relocs ["00000010:0006:?MAP_WIDTH@@3HA:00000000", "0000001d:0006:?mapExtra@@3PAEA:00000000", "00000064:0006:?MAP_WIDTH@@3HA:00000000", "0000009d:0006:?MAP_HEIGHT@@3HA:00000000", "000000b5:0006:?MAP_WIDTH@@3HA:00000000", "000000be:0006:?mapExtra@@3PAEA:00000000"]
; hex 558bec83ec105356578955f0894df4a1000000000faf45f00345f48b0d0000000033d28a140833c08a450885d00f840a000000b801000000e9b20000008b45f4488945fce903000000ff45fc8b45f4403b45fc0f8c8f000000837dfc000f8c0e000000a1000000003945fc0f8c05000000e9d3ffffff8b45f0488945f8e903000000ff45f88b45f0403b45f80f8c51000000837df8000f8c0e000000a1000000003945f80f8c05000000e9d3ffffff8b45f80faf05000000000345fc8b0d0000000033d28a140833c08a450885d00f840a000000b801000000e911000000e99fffffffe961ffffff33c0e9000000005f5e5bc9c20400

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 10             	sub    esp,0x10
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 55 f0             	mov    DWORD PTR [ebp-0x10],edx
   c:	89 4d f4             	mov    DWORD PTR [ebp-0xc],ecx
   f:	a1 00 00 00 00       	mov    eax,ds:0x0
  14:	0f af 45 f0          	imul   eax,DWORD PTR [ebp-0x10]
  18:	03 45 f4             	add    eax,DWORD PTR [ebp-0xc]
  1b:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
  21:	33 d2                	xor    edx,edx
  23:	8a 14 08             	mov    dl,BYTE PTR [eax+ecx*1]
  26:	33 c0                	xor    eax,eax
  28:	8a 45 08             	mov    al,BYTE PTR [ebp+0x8]
  2b:	85 d0                	test   eax,edx
  2d:	0f 84 0a 00 00 00    	je     0x3d
  33:	b8 01 00 00 00       	mov    eax,0x1
  38:	e9 b2 00 00 00       	jmp    0xef
  3d:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
  40:	48                   	dec    eax
  41:	89 45 fc             	mov    DWORD PTR [ebp-0x4],eax
  44:	e9 03 00 00 00       	jmp    0x4c
  49:	ff 45 fc             	inc    DWORD PTR [ebp-0x4]
  4c:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
  4f:	40                   	inc    eax
  50:	3b 45 fc             	cmp    eax,DWORD PTR [ebp-0x4]
  53:	0f 8c 8f 00 00 00    	jl     0xe8
  59:	83 7d fc 00          	cmp    DWORD PTR [ebp-0x4],0x0
  5d:	0f 8c 0e 00 00 00    	jl     0x71
  63:	a1 00 00 00 00       	mov    eax,ds:0x0
  68:	39 45 fc             	cmp    DWORD PTR [ebp-0x4],eax
  6b:	0f 8c 05 00 00 00    	jl     0x76
  71:	e9 d3 ff ff ff       	jmp    0x49
  76:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  79:	48                   	dec    eax
  7a:	89 45 f8             	mov    DWORD PTR [ebp-0x8],eax
  7d:	e9 03 00 00 00       	jmp    0x85
  82:	ff 45 f8             	inc    DWORD PTR [ebp-0x8]
  85:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  88:	40                   	inc    eax
  89:	3b 45 f8             	cmp    eax,DWORD PTR [ebp-0x8]
  8c:	0f 8c 51 00 00 00    	jl     0xe3
  92:	83 7d f8 00          	cmp    DWORD PTR [ebp-0x8],0x0
  96:	0f 8c 0e 00 00 00    	jl     0xaa
  9c:	a1 00 00 00 00       	mov    eax,ds:0x0
  a1:	39 45 f8             	cmp    DWORD PTR [ebp-0x8],eax
  a4:	0f 8c 05 00 00 00    	jl     0xaf
  aa:	e9 d3 ff ff ff       	jmp    0x82
  af:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
  b2:	0f af 05 00 00 00 00 	imul   eax,DWORD PTR ds:0x0
  b9:	03 45 fc             	add    eax,DWORD PTR [ebp-0x4]
  bc:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
  c2:	33 d2                	xor    edx,edx
  c4:	8a 14 08             	mov    dl,BYTE PTR [eax+ecx*1]
  c7:	33 c0                	xor    eax,eax
  c9:	8a 45 08             	mov    al,BYTE PTR [ebp+0x8]
  cc:	85 d0                	test   eax,edx
  ce:	0f 84 0a 00 00 00    	je     0xde
  d4:	b8 01 00 00 00       	mov    eax,0x1
  d9:	e9 11 00 00 00       	jmp    0xef
  de:	e9 9f ff ff ff       	jmp    0x82
  e3:	e9 61 ff ff ff       	jmp    0x49
  e8:	33 c0                	xor    eax,eax
  ea:	e9 00 00 00 00       	jmp    0xef
  ef:	5f                   	pop    edi
  f0:	5e                   	pop    esi
  f1:	5b                   	pop    ebx
  f2:	c9                   	leave
  f3:	c2 04 00             	ret    0x4
