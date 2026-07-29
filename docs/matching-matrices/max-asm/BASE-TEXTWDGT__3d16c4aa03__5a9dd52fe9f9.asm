; ?Main@textWidget@@UAEHAAUtag_message@@@Z
; unit BASE/TEXTWDGT  src_hash 5a9dd52fe9f9  score 100.0000  seed 1213156658  trial 2
; size 528  text_sha aef4cac623df6470
; relocs ["00000020:0014:?Main@widget@@UAEHAAUtag_message@@@Z:00000000", "00000048:0014:?Main@widget@@UAEHAAUtag_message@@@Z:00000000", "000001a1:0006:_gTextWidgetSourceFiles$Sdata_data_800eaaeddcd525bb02f9c2dc8765d2ebd13dd1a8fd2ba2e8f4ae538f8ec9cd15_0:58000000", "000001a9:0014:?BaseFree@@YIXPAXPADH@Z:00000000", "000001b3:0006:_gTextWidgetSourceFiles$Sdata_data_800eaaeddcd525bb02f9c2dc8765d2ebd13dd1a8fd2ba2e8f4ae538f8ec9cd15_0:84000000", "000001bb:0014:?BaseAlloc@@YIPAXIPADH@Z:00000000"]
; hex 5356668b511657558bd98b4c2414f6c2028b01751f3d00020000750f518bcbe8000000005d5f5e5bc2040033c05d5f5e5bc2040083f8107f1a0f84ac00000083f808742a518bcbe8000000005d5f5e5bc2040083f820741683f8400f848a0000003d000200000f84c0000000ebd68b43048b7904662b78288b7108662b702c668b4318663bf87c5a663b731a7c540fbf6b1c0fbfc00fbfff03e83bef7e440fbf7b1e0fbf431a0fbfee03f83bfd7e3380ca01668953168339207507c7410c000200005dc70100020000c741040c0000000fbf43105f894108b8020000005e5bc2040033c05d5f5e5bc20400f6c201743380e2fe668953168339407507c7410c000200005dc70100020000c741040d0000000fbf43105f894108b8020000005e5bc2040033c05d5f5e5bc204008b410483f803740e83f8080f84b3000000e902ffffff0fbf43103b41080f85f5feffff668b43148b7118663d00027415663d0040740fb8010000005d5f8973205e5bc204008bfeb9ffffffff2bc0f2aef7d18b7b20908d69ffb9ffffffff81e5ffff00002bc0f2aef7d1493bcd732768d3000000ba580000008b4b20e80000000068d4000000ba840000008d4d05e8000000008943208bfeb9ffffffff2bc0f2aef7d12bf98bc1c1e9028bf78b7b20f3a58bc85d83e103f3a4b8010000005f5e5bc204000fbf43103b41080f8547feffff8b41185d668943285fb8010000005e5bc20400

   0:	53                   	push   ebx
   1:	56                   	push   esi
   2:	66 8b 51 16          	mov    dx,WORD PTR [ecx+0x16]
   6:	57                   	push   edi
   7:	55                   	push   ebp
   8:	8b d9                	mov    ebx,ecx
   a:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
   e:	f6 c2 02             	test   dl,0x2
  11:	8b 01                	mov    eax,DWORD PTR [ecx]
  13:	75 1f                	jne    0x34
  15:	3d 00 02 00 00       	cmp    eax,0x200
  1a:	75 0f                	jne    0x2b
  1c:	51                   	push   ecx
  1d:	8b cb                	mov    ecx,ebx
  1f:	e8 00 00 00 00       	call   0x24
  24:	5d                   	pop    ebp
  25:	5f                   	pop    edi
  26:	5e                   	pop    esi
  27:	5b                   	pop    ebx
  28:	c2 04 00             	ret    0x4
  2b:	33 c0                	xor    eax,eax
  2d:	5d                   	pop    ebp
  2e:	5f                   	pop    edi
  2f:	5e                   	pop    esi
  30:	5b                   	pop    ebx
  31:	c2 04 00             	ret    0x4
  34:	83 f8 10             	cmp    eax,0x10
  37:	7f 1a                	jg     0x53
  39:	0f 84 ac 00 00 00    	je     0xeb
  3f:	83 f8 08             	cmp    eax,0x8
  42:	74 2a                	je     0x6e
  44:	51                   	push   ecx
  45:	8b cb                	mov    ecx,ebx
  47:	e8 00 00 00 00       	call   0x4c
  4c:	5d                   	pop    ebp
  4d:	5f                   	pop    edi
  4e:	5e                   	pop    esi
  4f:	5b                   	pop    ebx
  50:	c2 04 00             	ret    0x4
  53:	83 f8 20             	cmp    eax,0x20
  56:	74 16                	je     0x6e
  58:	83 f8 40             	cmp    eax,0x40
  5b:	0f 84 8a 00 00 00    	je     0xeb
  61:	3d 00 02 00 00       	cmp    eax,0x200
  66:	0f 84 c0 00 00 00    	je     0x12c
  6c:	eb d6                	jmp    0x44
  6e:	8b 43 04             	mov    eax,DWORD PTR [ebx+0x4]
  71:	8b 79 04             	mov    edi,DWORD PTR [ecx+0x4]
  74:	66 2b 78 28          	sub    di,WORD PTR [eax+0x28]
  78:	8b 71 08             	mov    esi,DWORD PTR [ecx+0x8]
  7b:	66 2b 70 2c          	sub    si,WORD PTR [eax+0x2c]
  7f:	66 8b 43 18          	mov    ax,WORD PTR [ebx+0x18]
  83:	66 3b f8             	cmp    di,ax
  86:	7c 5a                	jl     0xe2
  88:	66 3b 73 1a          	cmp    si,WORD PTR [ebx+0x1a]
  8c:	7c 54                	jl     0xe2
  8e:	0f bf 6b 1c          	movsx  ebp,WORD PTR [ebx+0x1c]
  92:	0f bf c0             	movsx  eax,ax
  95:	0f bf ff             	movsx  edi,di
  98:	03 e8                	add    ebp,eax
  9a:	3b ef                	cmp    ebp,edi
  9c:	7e 44                	jle    0xe2
  9e:	0f bf 7b 1e          	movsx  edi,WORD PTR [ebx+0x1e]
  a2:	0f bf 43 1a          	movsx  eax,WORD PTR [ebx+0x1a]
  a6:	0f bf ee             	movsx  ebp,si
  a9:	03 f8                	add    edi,eax
  ab:	3b fd                	cmp    edi,ebp
  ad:	7e 33                	jle    0xe2
  af:	80 ca 01             	or     dl,0x1
  b2:	66 89 53 16          	mov    WORD PTR [ebx+0x16],dx
  b6:	83 39 20             	cmp    DWORD PTR [ecx],0x20
  b9:	75 07                	jne    0xc2
  bb:	c7 41 0c 00 02 00 00 	mov    DWORD PTR [ecx+0xc],0x200
  c2:	5d                   	pop    ebp
  c3:	c7 01 00 02 00 00    	mov    DWORD PTR [ecx],0x200
  c9:	c7 41 04 0c 00 00 00 	mov    DWORD PTR [ecx+0x4],0xc
  d0:	0f bf 43 10          	movsx  eax,WORD PTR [ebx+0x10]
  d4:	5f                   	pop    edi
  d5:	89 41 08             	mov    DWORD PTR [ecx+0x8],eax
  d8:	b8 02 00 00 00       	mov    eax,0x2
  dd:	5e                   	pop    esi
  de:	5b                   	pop    ebx
  df:	c2 04 00             	ret    0x4
  e2:	33 c0                	xor    eax,eax
  e4:	5d                   	pop    ebp
  e5:	5f                   	pop    edi
  e6:	5e                   	pop    esi
  e7:	5b                   	pop    ebx
  e8:	c2 04 00             	ret    0x4
  eb:	f6 c2 01             	test   dl,0x1
  ee:	74 33                	je     0x123
  f0:	80 e2 fe             	and    dl,0xfe
  f3:	66 89 53 16          	mov    WORD PTR [ebx+0x16],dx
  f7:	83 39 40             	cmp    DWORD PTR [ecx],0x40
  fa:	75 07                	jne    0x103
  fc:	c7 41 0c 00 02 00 00 	mov    DWORD PTR [ecx+0xc],0x200
 103:	5d                   	pop    ebp
 104:	c7 01 00 02 00 00    	mov    DWORD PTR [ecx],0x200
 10a:	c7 41 04 0d 00 00 00 	mov    DWORD PTR [ecx+0x4],0xd
 111:	0f bf 43 10          	movsx  eax,WORD PTR [ebx+0x10]
 115:	5f                   	pop    edi
 116:	89 41 08             	mov    DWORD PTR [ecx+0x8],eax
 119:	b8 02 00 00 00       	mov    eax,0x2
 11e:	5e                   	pop    esi
 11f:	5b                   	pop    ebx
 120:	c2 04 00             	ret    0x4
 123:	33 c0                	xor    eax,eax
 125:	5d                   	pop    ebp
 126:	5f                   	pop    edi
 127:	5e                   	pop    esi
 128:	5b                   	pop    ebx
 129:	c2 04 00             	ret    0x4
 12c:	8b 41 04             	mov    eax,DWORD PTR [ecx+0x4]
 12f:	83 f8 03             	cmp    eax,0x3
 132:	74 0e                	je     0x142
 134:	83 f8 08             	cmp    eax,0x8
 137:	0f 84 b3 00 00 00    	je     0x1f0
 13d:	e9 02 ff ff ff       	jmp    0x44
 142:	0f bf 43 10          	movsx  eax,WORD PTR [ebx+0x10]
 146:	3b 41 08             	cmp    eax,DWORD PTR [ecx+0x8]
 149:	0f 85 f5 fe ff ff    	jne    0x44
 14f:	66 8b 43 14          	mov    ax,WORD PTR [ebx+0x14]
 153:	8b 71 18             	mov    esi,DWORD PTR [ecx+0x18]
 156:	66 3d 00 02          	cmp    ax,0x200
 15a:	74 15                	je     0x171
 15c:	66 3d 00 40          	cmp    ax,0x4000
 160:	74 0f                	je     0x171
 162:	b8 01 00 00 00       	mov    eax,0x1
 167:	5d                   	pop    ebp
 168:	5f                   	pop    edi
 169:	89 73 20             	mov    DWORD PTR [ebx+0x20],esi
 16c:	5e                   	pop    esi
 16d:	5b                   	pop    ebx
 16e:	c2 04 00             	ret    0x4
 171:	8b fe                	mov    edi,esi
 173:	b9 ff ff ff ff       	mov    ecx,0xffffffff
 178:	2b c0                	sub    eax,eax
 17a:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
 17c:	f7 d1                	not    ecx
 17e:	8b 7b 20             	mov    edi,DWORD PTR [ebx+0x20]
 181:	90                   	nop
 182:	8d 69 ff             	lea    ebp,[ecx-0x1]
 185:	b9 ff ff ff ff       	mov    ecx,0xffffffff
 18a:	81 e5 ff ff 00 00    	and    ebp,0xffff
 190:	2b c0                	sub    eax,eax
 192:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
 194:	f7 d1                	not    ecx
 196:	49                   	dec    ecx
 197:	3b cd                	cmp    ecx,ebp
 199:	73 27                	jae    0x1c2
 19b:	68 d3 00 00 00       	push   0xd3
 1a0:	ba 58 00 00 00       	mov    edx,0x58
 1a5:	8b 4b 20             	mov    ecx,DWORD PTR [ebx+0x20]
 1a8:	e8 00 00 00 00       	call   0x1ad
 1ad:	68 d4 00 00 00       	push   0xd4
 1b2:	ba 84 00 00 00       	mov    edx,0x84
 1b7:	8d 4d 05             	lea    ecx,[ebp+0x5]
 1ba:	e8 00 00 00 00       	call   0x1bf
 1bf:	89 43 20             	mov    DWORD PTR [ebx+0x20],eax
 1c2:	8b fe                	mov    edi,esi
 1c4:	b9 ff ff ff ff       	mov    ecx,0xffffffff
 1c9:	2b c0                	sub    eax,eax
 1cb:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
 1cd:	f7 d1                	not    ecx
 1cf:	2b f9                	sub    edi,ecx
 1d1:	8b c1                	mov    eax,ecx
 1d3:	c1 e9 02             	shr    ecx,0x2
 1d6:	8b f7                	mov    esi,edi
 1d8:	8b 7b 20             	mov    edi,DWORD PTR [ebx+0x20]
 1db:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
 1dd:	8b c8                	mov    ecx,eax
 1df:	5d                   	pop    ebp
 1e0:	83 e1 03             	and    ecx,0x3
 1e3:	f3 a4                	rep movs BYTE PTR es:[edi],BYTE PTR ds:[esi]
 1e5:	b8 01 00 00 00       	mov    eax,0x1
 1ea:	5f                   	pop    edi
 1eb:	5e                   	pop    esi
 1ec:	5b                   	pop    ebx
 1ed:	c2 04 00             	ret    0x4
 1f0:	0f bf 43 10          	movsx  eax,WORD PTR [ebx+0x10]
 1f4:	3b 41 08             	cmp    eax,DWORD PTR [ecx+0x8]
 1f7:	0f 85 47 fe ff ff    	jne    0x44
 1fd:	8b 41 18             	mov    eax,DWORD PTR [ecx+0x18]
 200:	5d                   	pop    ebp
 201:	66 89 43 28          	mov    WORD PTR [ebx+0x28],ax
 205:	5f                   	pop    edi
 206:	b8 01 00 00 00       	mov    eax,0x1
 20b:	5e                   	pop    esi
 20c:	5b                   	pop    ebx
 20d:	c2 04 00             	ret    0x4
