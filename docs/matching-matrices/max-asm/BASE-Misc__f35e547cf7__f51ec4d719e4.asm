; ?FadeTo@@YIXPAE0H@Z
; unit BASE/Misc  src_hash f51ec4d719e4  score 100.0000  seed 1178682437  trial 2
; size 248  text_sha 25d7c9e3080c0ce9
; relocs ["00000045:0014:?KBTickCount@@YIJXZ:00000000", "00000051:0014:@PollSound@0:00000000", "00000075:0006:?giChangeThreshold@@3PAEA:00000000", "000000bb:0014:?UpdatePalette@@YIXPAC@Z:00000000", "000000c4:0014:?DelayTil@@YIXPAH@Z:00000000", "000000e7:0014:?UpdatePalette@@YIXPAC@Z:00000000"]
; hex 81ec10030000895424085356578d7c241c558bf1b9c0000000f3a58b842424030000c1f8028984242403000083f8017d0bc784242403000001000000c744241030000000e80000000083c0328944241ce800000000b8400000008b4c24102bc18b9424240300002bc2790233c033c98d7424208a88000000008b7c2418bd00030000894c24148a1e33c98a0f33c08ac32bc88bc19933c22bc28b5424143bd07d102bc285c97e0602c38806eb042ad8881e46474d75d08d4c2420e8000000008d4c241ce8000000008b8424240300008b4c241003c8894c241083f9400f8c62ffffff8b4c2418e8000000005d5f5e5b81c410030000c20400

   0:	81 ec 10 03 00 00    	sub    esp,0x310
   6:	89 54 24 08          	mov    DWORD PTR [esp+0x8],edx
   a:	53                   	push   ebx
   b:	56                   	push   esi
   c:	57                   	push   edi
   d:	8d 7c 24 1c          	lea    edi,[esp+0x1c]
  11:	55                   	push   ebp
  12:	8b f1                	mov    esi,ecx
  14:	b9 c0 00 00 00       	mov    ecx,0xc0
  19:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  1b:	8b 84 24 24 03 00 00 	mov    eax,DWORD PTR [esp+0x324]
  22:	c1 f8 02             	sar    eax,0x2
  25:	89 84 24 24 03 00 00 	mov    DWORD PTR [esp+0x324],eax
  2c:	83 f8 01             	cmp    eax,0x1
  2f:	7d 0b                	jge    0x3c
  31:	c7 84 24 24 03 00 00 	mov    DWORD PTR [esp+0x324],0x1
  38:	01 00 00 00 
  3c:	c7 44 24 10 30 00 00 	mov    DWORD PTR [esp+0x10],0x30
  43:	00 
  44:	e8 00 00 00 00       	call   0x49
  49:	83 c0 32             	add    eax,0x32
  4c:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  50:	e8 00 00 00 00       	call   0x55
  55:	b8 40 00 00 00       	mov    eax,0x40
  5a:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  5e:	2b c1                	sub    eax,ecx
  60:	8b 94 24 24 03 00 00 	mov    edx,DWORD PTR [esp+0x324]
  67:	2b c2                	sub    eax,edx
  69:	79 02                	jns    0x6d
  6b:	33 c0                	xor    eax,eax
  6d:	33 c9                	xor    ecx,ecx
  6f:	8d 74 24 20          	lea    esi,[esp+0x20]
  73:	8a 88 00 00 00 00    	mov    cl,BYTE PTR [eax+0x0]
  79:	8b 7c 24 18          	mov    edi,DWORD PTR [esp+0x18]
  7d:	bd 00 03 00 00       	mov    ebp,0x300
  82:	89 4c 24 14          	mov    DWORD PTR [esp+0x14],ecx
  86:	8a 1e                	mov    bl,BYTE PTR [esi]
  88:	33 c9                	xor    ecx,ecx
  8a:	8a 0f                	mov    cl,BYTE PTR [edi]
  8c:	33 c0                	xor    eax,eax
  8e:	8a c3                	mov    al,bl
  90:	2b c8                	sub    ecx,eax
  92:	8b c1                	mov    eax,ecx
  94:	99                   	cdq
  95:	33 c2                	xor    eax,edx
  97:	2b c2                	sub    eax,edx
  99:	8b 54 24 14          	mov    edx,DWORD PTR [esp+0x14]
  9d:	3b d0                	cmp    edx,eax
  9f:	7d 10                	jge    0xb1
  a1:	2b c2                	sub    eax,edx
  a3:	85 c9                	test   ecx,ecx
  a5:	7e 06                	jle    0xad
  a7:	02 c3                	add    al,bl
  a9:	88 06                	mov    BYTE PTR [esi],al
  ab:	eb 04                	jmp    0xb1
  ad:	2a d8                	sub    bl,al
  af:	88 1e                	mov    BYTE PTR [esi],bl
  b1:	46                   	inc    esi
  b2:	47                   	inc    edi
  b3:	4d                   	dec    ebp
  b4:	75 d0                	jne    0x86
  b6:	8d 4c 24 20          	lea    ecx,[esp+0x20]
  ba:	e8 00 00 00 00       	call   0xbf
  bf:	8d 4c 24 1c          	lea    ecx,[esp+0x1c]
  c3:	e8 00 00 00 00       	call   0xc8
  c8:	8b 84 24 24 03 00 00 	mov    eax,DWORD PTR [esp+0x324]
  cf:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  d3:	03 c8                	add    ecx,eax
  d5:	89 4c 24 10          	mov    DWORD PTR [esp+0x10],ecx
  d9:	83 f9 40             	cmp    ecx,0x40
  dc:	0f 8c 62 ff ff ff    	jl     0x44
  e2:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  e6:	e8 00 00 00 00       	call   0xeb
  eb:	5d                   	pop    ebp
  ec:	5f                   	pop    edi
  ed:	5e                   	pop    esi
  ee:	5b                   	pop    ebx
  ef:	81 c4 10 03 00 00    	add    esp,0x310
  f5:	c2 04 00             	ret    0x4
