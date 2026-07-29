; ?GetBestArmy@combatManager@@QAEHHH@Z
; unit SOURCE/AI  src_hash 59fc1a2bd34f  score 100.0000  seed 798154  trial 3
; size 497  text_sha 267c53f86da0e1c2
; relocs ["0000008c:0014:?Strength@army@@QAEKXZ:00000000"]
; hex 558bec83ec18535657894de8c745f000000000c745f401000000c745fc00000000c745ecffffffffc745f000000000e903000000ff45f08b45088b4de88b55f0399481473500000f8e950100008b45f485450c0f84810100008b45088bc8c1e0062bc18d04c1c1e0032bc18d04408b4df08bd1c1e1038d0cc98d0cca03c98d04418b4de88d8c084f350000e8000000008945f88b45088bc8c1e0062bc18d04c1c1e0032bc18d04408b4df08bd1c1e1038d0cc98d0cca03c98d04418b4de833d28a94085b36000085d20f85f00000008b45088bc8c1e0062bc18d04c1c1e0032bc18d04408b4df08bd1c1e1038d0cc98d0cca03c98d04418b4de833d28a94085f36000085d20f85b40000008b45088bc8c1e0062bc18d04c1c1e0032bc18d04408b4df08bd1c1e1038d0cc98d0cca03c98d04418b4de833d28a94086436000085d20f85780000008b45088bc8c1e0062bc18d04c1c1e0032bc18d04408b4df08bd1c1e1038d0cc98d0cca03c98d04418b4de833d28a94085e36000085d20f853c0000008b45088bc8c1e0062bc18d04c1c1e0032bc18d04408b4df08bd1c1e1038d0cc98d0cca03c98d04418b4de833d28a94086036000085d20f8403000000d16df88b45f83945fc0f830c0000008b45f08945ec8b45f88945fcd165f4e952feffff8b45ece9000000005f5e5bc9c20800

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 18             	sub    esp,0x18
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 4d e8             	mov    DWORD PTR [ebp-0x18],ecx
   c:	c7 45 f0 00 00 00 00 	mov    DWORD PTR [ebp-0x10],0x0
  13:	c7 45 f4 01 00 00 00 	mov    DWORD PTR [ebp-0xc],0x1
  1a:	c7 45 fc 00 00 00 00 	mov    DWORD PTR [ebp-0x4],0x0
  21:	c7 45 ec ff ff ff ff 	mov    DWORD PTR [ebp-0x14],0xffffffff
  28:	c7 45 f0 00 00 00 00 	mov    DWORD PTR [ebp-0x10],0x0
  2f:	e9 03 00 00 00       	jmp    0x37
  34:	ff 45 f0             	inc    DWORD PTR [ebp-0x10]
  37:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  3a:	8b 4d e8             	mov    ecx,DWORD PTR [ebp-0x18]
  3d:	8b 55 f0             	mov    edx,DWORD PTR [ebp-0x10]
  40:	39 94 81 47 35 00 00 	cmp    DWORD PTR [ecx+eax*4+0x3547],edx
  47:	0f 8e 95 01 00 00    	jle    0x1e2
  4d:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
  50:	85 45 0c             	test   DWORD PTR [ebp+0xc],eax
  53:	0f 84 81 01 00 00    	je     0x1da
  59:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  5c:	8b c8                	mov    ecx,eax
  5e:	c1 e0 06             	shl    eax,0x6
  61:	2b c1                	sub    eax,ecx
  63:	8d 04 c1             	lea    eax,[ecx+eax*8]
  66:	c1 e0 03             	shl    eax,0x3
  69:	2b c1                	sub    eax,ecx
  6b:	8d 04 40             	lea    eax,[eax+eax*2]
  6e:	8b 4d f0             	mov    ecx,DWORD PTR [ebp-0x10]
  71:	8b d1                	mov    edx,ecx
  73:	c1 e1 03             	shl    ecx,0x3
  76:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  79:	8d 0c ca             	lea    ecx,[edx+ecx*8]
  7c:	03 c9                	add    ecx,ecx
  7e:	8d 04 41             	lea    eax,[ecx+eax*2]
  81:	8b 4d e8             	mov    ecx,DWORD PTR [ebp-0x18]
  84:	8d 8c 08 4f 35 00 00 	lea    ecx,[eax+ecx*1+0x354f]
  8b:	e8 00 00 00 00       	call   0x90
  90:	89 45 f8             	mov    DWORD PTR [ebp-0x8],eax
  93:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  96:	8b c8                	mov    ecx,eax
  98:	c1 e0 06             	shl    eax,0x6
  9b:	2b c1                	sub    eax,ecx
  9d:	8d 04 c1             	lea    eax,[ecx+eax*8]
  a0:	c1 e0 03             	shl    eax,0x3
  a3:	2b c1                	sub    eax,ecx
  a5:	8d 04 40             	lea    eax,[eax+eax*2]
  a8:	8b 4d f0             	mov    ecx,DWORD PTR [ebp-0x10]
  ab:	8b d1                	mov    edx,ecx
  ad:	c1 e1 03             	shl    ecx,0x3
  b0:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  b3:	8d 0c ca             	lea    ecx,[edx+ecx*8]
  b6:	03 c9                	add    ecx,ecx
  b8:	8d 04 41             	lea    eax,[ecx+eax*2]
  bb:	8b 4d e8             	mov    ecx,DWORD PTR [ebp-0x18]
  be:	33 d2                	xor    edx,edx
  c0:	8a 94 08 5b 36 00 00 	mov    dl,BYTE PTR [eax+ecx*1+0x365b]
  c7:	85 d2                	test   edx,edx
  c9:	0f 85 f0 00 00 00    	jne    0x1bf
  cf:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  d2:	8b c8                	mov    ecx,eax
  d4:	c1 e0 06             	shl    eax,0x6
  d7:	2b c1                	sub    eax,ecx
  d9:	8d 04 c1             	lea    eax,[ecx+eax*8]
  dc:	c1 e0 03             	shl    eax,0x3
  df:	2b c1                	sub    eax,ecx
  e1:	8d 04 40             	lea    eax,[eax+eax*2]
  e4:	8b 4d f0             	mov    ecx,DWORD PTR [ebp-0x10]
  e7:	8b d1                	mov    edx,ecx
  e9:	c1 e1 03             	shl    ecx,0x3
  ec:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  ef:	8d 0c ca             	lea    ecx,[edx+ecx*8]
  f2:	03 c9                	add    ecx,ecx
  f4:	8d 04 41             	lea    eax,[ecx+eax*2]
  f7:	8b 4d e8             	mov    ecx,DWORD PTR [ebp-0x18]
  fa:	33 d2                	xor    edx,edx
  fc:	8a 94 08 5f 36 00 00 	mov    dl,BYTE PTR [eax+ecx*1+0x365f]
 103:	85 d2                	test   edx,edx
 105:	0f 85 b4 00 00 00    	jne    0x1bf
 10b:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 10e:	8b c8                	mov    ecx,eax
 110:	c1 e0 06             	shl    eax,0x6
 113:	2b c1                	sub    eax,ecx
 115:	8d 04 c1             	lea    eax,[ecx+eax*8]
 118:	c1 e0 03             	shl    eax,0x3
 11b:	2b c1                	sub    eax,ecx
 11d:	8d 04 40             	lea    eax,[eax+eax*2]
 120:	8b 4d f0             	mov    ecx,DWORD PTR [ebp-0x10]
 123:	8b d1                	mov    edx,ecx
 125:	c1 e1 03             	shl    ecx,0x3
 128:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
 12b:	8d 0c ca             	lea    ecx,[edx+ecx*8]
 12e:	03 c9                	add    ecx,ecx
 130:	8d 04 41             	lea    eax,[ecx+eax*2]
 133:	8b 4d e8             	mov    ecx,DWORD PTR [ebp-0x18]
 136:	33 d2                	xor    edx,edx
 138:	8a 94 08 64 36 00 00 	mov    dl,BYTE PTR [eax+ecx*1+0x3664]
 13f:	85 d2                	test   edx,edx
 141:	0f 85 78 00 00 00    	jne    0x1bf
 147:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 14a:	8b c8                	mov    ecx,eax
 14c:	c1 e0 06             	shl    eax,0x6
 14f:	2b c1                	sub    eax,ecx
 151:	8d 04 c1             	lea    eax,[ecx+eax*8]
 154:	c1 e0 03             	shl    eax,0x3
 157:	2b c1                	sub    eax,ecx
 159:	8d 04 40             	lea    eax,[eax+eax*2]
 15c:	8b 4d f0             	mov    ecx,DWORD PTR [ebp-0x10]
 15f:	8b d1                	mov    edx,ecx
 161:	c1 e1 03             	shl    ecx,0x3
 164:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
 167:	8d 0c ca             	lea    ecx,[edx+ecx*8]
 16a:	03 c9                	add    ecx,ecx
 16c:	8d 04 41             	lea    eax,[ecx+eax*2]
 16f:	8b 4d e8             	mov    ecx,DWORD PTR [ebp-0x18]
 172:	33 d2                	xor    edx,edx
 174:	8a 94 08 5e 36 00 00 	mov    dl,BYTE PTR [eax+ecx*1+0x365e]
 17b:	85 d2                	test   edx,edx
 17d:	0f 85 3c 00 00 00    	jne    0x1bf
 183:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 186:	8b c8                	mov    ecx,eax
 188:	c1 e0 06             	shl    eax,0x6
 18b:	2b c1                	sub    eax,ecx
 18d:	8d 04 c1             	lea    eax,[ecx+eax*8]
 190:	c1 e0 03             	shl    eax,0x3
 193:	2b c1                	sub    eax,ecx
 195:	8d 04 40             	lea    eax,[eax+eax*2]
 198:	8b 4d f0             	mov    ecx,DWORD PTR [ebp-0x10]
 19b:	8b d1                	mov    edx,ecx
 19d:	c1 e1 03             	shl    ecx,0x3
 1a0:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
 1a3:	8d 0c ca             	lea    ecx,[edx+ecx*8]
 1a6:	03 c9                	add    ecx,ecx
 1a8:	8d 04 41             	lea    eax,[ecx+eax*2]
 1ab:	8b 4d e8             	mov    ecx,DWORD PTR [ebp-0x18]
 1ae:	33 d2                	xor    edx,edx
 1b0:	8a 94 08 60 36 00 00 	mov    dl,BYTE PTR [eax+ecx*1+0x3660]
 1b7:	85 d2                	test   edx,edx
 1b9:	0f 84 03 00 00 00    	je     0x1c2
 1bf:	d1 6d f8             	shr    DWORD PTR [ebp-0x8],1
 1c2:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
 1c5:	39 45 fc             	cmp    DWORD PTR [ebp-0x4],eax
 1c8:	0f 83 0c 00 00 00    	jae    0x1da
 1ce:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 1d1:	89 45 ec             	mov    DWORD PTR [ebp-0x14],eax
 1d4:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
 1d7:	89 45 fc             	mov    DWORD PTR [ebp-0x4],eax
 1da:	d1 65 f4             	shl    DWORD PTR [ebp-0xc],1
 1dd:	e9 52 fe ff ff       	jmp    0x34
 1e2:	8b 45 ec             	mov    eax,DWORD PTR [ebp-0x14]
 1e5:	e9 00 00 00 00       	jmp    0x1ea
 1ea:	5f                   	pop    edi
 1eb:	5e                   	pop    esi
 1ec:	5b                   	pop    ebx
 1ed:	c9                   	leave
 1ee:	c2 08 00             	ret    0x8
