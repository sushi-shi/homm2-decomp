; ?GetTraitorMask@combatManager@@QAEHH@Z
; unit SOURCE/AI  src_hash f4bc452ff45c  score 100.0000  seed 1213156658  trial 3
; size 214  text_sha 1d1240d87e7167e9
; relocs []
; hex 558bec83ec14535657894decc745f400000000c745f801000000c745fc00000000c745f400000000e903000000ff45f48b45088b4dec8b55f4399481473500000f8e810000008b45088bc8c1e0062bc18d04c1c1e0032bc18d04408b4df48bd1c1e1038d0cc98d0cca03c98d04410345ec054f3500008945f0837df0000f843c0000008b45f0f680d0000000100f852c0000008b45f033c98a880f01000085c90f85130000008b45f033c98a881101000085c90f84060000008b45f80945fcd165f8e966ffffff8b45fce9000000005f5e5bc9c20400

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 14             	sub    esp,0x14
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 4d ec             	mov    DWORD PTR [ebp-0x14],ecx
   c:	c7 45 f4 00 00 00 00 	mov    DWORD PTR [ebp-0xc],0x0
  13:	c7 45 f8 01 00 00 00 	mov    DWORD PTR [ebp-0x8],0x1
  1a:	c7 45 fc 00 00 00 00 	mov    DWORD PTR [ebp-0x4],0x0
  21:	c7 45 f4 00 00 00 00 	mov    DWORD PTR [ebp-0xc],0x0
  28:	e9 03 00 00 00       	jmp    0x30
  2d:	ff 45 f4             	inc    DWORD PTR [ebp-0xc]
  30:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  33:	8b 4d ec             	mov    ecx,DWORD PTR [ebp-0x14]
  36:	8b 55 f4             	mov    edx,DWORD PTR [ebp-0xc]
  39:	39 94 81 47 35 00 00 	cmp    DWORD PTR [ecx+eax*4+0x3547],edx
  40:	0f 8e 81 00 00 00    	jle    0xc7
  46:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  49:	8b c8                	mov    ecx,eax
  4b:	c1 e0 06             	shl    eax,0x6
  4e:	2b c1                	sub    eax,ecx
  50:	8d 04 c1             	lea    eax,[ecx+eax*8]
  53:	c1 e0 03             	shl    eax,0x3
  56:	2b c1                	sub    eax,ecx
  58:	8d 04 40             	lea    eax,[eax+eax*2]
  5b:	8b 4d f4             	mov    ecx,DWORD PTR [ebp-0xc]
  5e:	8b d1                	mov    edx,ecx
  60:	c1 e1 03             	shl    ecx,0x3
  63:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  66:	8d 0c ca             	lea    ecx,[edx+ecx*8]
  69:	03 c9                	add    ecx,ecx
  6b:	8d 04 41             	lea    eax,[ecx+eax*2]
  6e:	03 45 ec             	add    eax,DWORD PTR [ebp-0x14]
  71:	05 4f 35 00 00       	add    eax,0x354f
  76:	89 45 f0             	mov    DWORD PTR [ebp-0x10],eax
  79:	83 7d f0 00          	cmp    DWORD PTR [ebp-0x10],0x0
  7d:	0f 84 3c 00 00 00    	je     0xbf
  83:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  86:	f6 80 d0 00 00 00 10 	test   BYTE PTR [eax+0xd0],0x10
  8d:	0f 85 2c 00 00 00    	jne    0xbf
  93:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  96:	33 c9                	xor    ecx,ecx
  98:	8a 88 0f 01 00 00    	mov    cl,BYTE PTR [eax+0x10f]
  9e:	85 c9                	test   ecx,ecx
  a0:	0f 85 13 00 00 00    	jne    0xb9
  a6:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  a9:	33 c9                	xor    ecx,ecx
  ab:	8a 88 11 01 00 00    	mov    cl,BYTE PTR [eax+0x111]
  b1:	85 c9                	test   ecx,ecx
  b3:	0f 84 06 00 00 00    	je     0xbf
  b9:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
  bc:	09 45 fc             	or     DWORD PTR [ebp-0x4],eax
  bf:	d1 65 f8             	shl    DWORD PTR [ebp-0x8],1
  c2:	e9 66 ff ff ff       	jmp    0x2d
  c7:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
  ca:	e9 00 00 00 00       	jmp    0xcf
  cf:	5f                   	pop    edi
  d0:	5e                   	pop    esi
  d1:	5b                   	pop    ebx
  d2:	c9                   	leave
  d3:	c2 04 00             	ret    0x4
