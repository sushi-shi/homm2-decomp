; ?LineLength@font@@QAEHPADH@Z
; unit BASE/FONT  src_hash 4702f87fc49b  score 100.0000  seed 1179602516  trial 4
; size 435  text_sha 00a1a7ac1cfa8996
; relocs ["000000bf:0014:?GetCharacterWidth@font@@QAEHE@Z:00000000", "00000113:0014:?GetCharacterWidth@font@@QAEHE@Z:00000000", "00000177:0014:?GetCharacterWidth@font@@QAEHE@Z:00000000"]
; hex 558bec83ec38535657894dc88b7d08b9ffffffff2bc0f2aef7d18d41ff8945f0c645d020c745d400000000c745ec00000000c745f400000000c745d800000000c745fc00000000c745e800000000c745dc00000000c745cc000000008b45088945e08b45f03945fc0f8d360100008b45fc8b4de00fbe040885c00f84240100008b45fc8b4de00fbe040885c00f843c0000008b45fc8b4de00fbe040883f80a0f84290000008b450c3945dc0f8f1d0000008b45e08b4dfc8a0408508b4dc8e8000000000145dcff45fce9b2ffffff8b450c3945dc0f8ea8000000ff4dfcc745cc000000008b45fc8b4de00fbe040883f8200f844b0000008b45f43945fc0f8c3f00000033db8b45e08b4dfc8a0408508b4dc8e8000000002bd8f7db295ddc837dcc000f85120000008b450c3945dc0f8d060000008b45fc8945ccff4dfce9a2ffffff8b45f43945fc0f8f060000008b45cc8945fc8b45fc8b4de00fbe040883f8200f851b00000033db8b45e08b4dfc8a0408508b4dc8e8000000002bd8f7db295ddc8b45fc8945d8ff45d48b45d8408945f48b45f48945fcc745dc00000000e9befeffff8b45d4e9000000005f5e5bc9c20800

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 38             	sub    esp,0x38
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 4d c8             	mov    DWORD PTR [ebp-0x38],ecx
   c:	8b 7d 08             	mov    edi,DWORD PTR [ebp+0x8]
   f:	b9 ff ff ff ff       	mov    ecx,0xffffffff
  14:	2b c0                	sub    eax,eax
  16:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  18:	f7 d1                	not    ecx
  1a:	8d 41 ff             	lea    eax,[ecx-0x1]
  1d:	89 45 f0             	mov    DWORD PTR [ebp-0x10],eax
  20:	c6 45 d0 20          	mov    BYTE PTR [ebp-0x30],0x20
  24:	c7 45 d4 00 00 00 00 	mov    DWORD PTR [ebp-0x2c],0x0
  2b:	c7 45 ec 00 00 00 00 	mov    DWORD PTR [ebp-0x14],0x0
  32:	c7 45 f4 00 00 00 00 	mov    DWORD PTR [ebp-0xc],0x0
  39:	c7 45 d8 00 00 00 00 	mov    DWORD PTR [ebp-0x28],0x0
  40:	c7 45 fc 00 00 00 00 	mov    DWORD PTR [ebp-0x4],0x0
  47:	c7 45 e8 00 00 00 00 	mov    DWORD PTR [ebp-0x18],0x0
  4e:	c7 45 dc 00 00 00 00 	mov    DWORD PTR [ebp-0x24],0x0
  55:	c7 45 cc 00 00 00 00 	mov    DWORD PTR [ebp-0x34],0x0
  5c:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  5f:	89 45 e0             	mov    DWORD PTR [ebp-0x20],eax
  62:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  65:	39 45 fc             	cmp    DWORD PTR [ebp-0x4],eax
  68:	0f 8d 36 01 00 00    	jge    0x1a4
  6e:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
  71:	8b 4d e0             	mov    ecx,DWORD PTR [ebp-0x20]
  74:	0f be 04 08          	movsx  eax,BYTE PTR [eax+ecx*1]
  78:	85 c0                	test   eax,eax
  7a:	0f 84 24 01 00 00    	je     0x1a4
  80:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
  83:	8b 4d e0             	mov    ecx,DWORD PTR [ebp-0x20]
  86:	0f be 04 08          	movsx  eax,BYTE PTR [eax+ecx*1]
  8a:	85 c0                	test   eax,eax
  8c:	0f 84 3c 00 00 00    	je     0xce
  92:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
  95:	8b 4d e0             	mov    ecx,DWORD PTR [ebp-0x20]
  98:	0f be 04 08          	movsx  eax,BYTE PTR [eax+ecx*1]
  9c:	83 f8 0a             	cmp    eax,0xa
  9f:	0f 84 29 00 00 00    	je     0xce
  a5:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
  a8:	39 45 dc             	cmp    DWORD PTR [ebp-0x24],eax
  ab:	0f 8f 1d 00 00 00    	jg     0xce
  b1:	8b 45 e0             	mov    eax,DWORD PTR [ebp-0x20]
  b4:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
  b7:	8a 04 08             	mov    al,BYTE PTR [eax+ecx*1]
  ba:	50                   	push   eax
  bb:	8b 4d c8             	mov    ecx,DWORD PTR [ebp-0x38]
  be:	e8 00 00 00 00       	call   0xc3
  c3:	01 45 dc             	add    DWORD PTR [ebp-0x24],eax
  c6:	ff 45 fc             	inc    DWORD PTR [ebp-0x4]
  c9:	e9 b2 ff ff ff       	jmp    0x80
  ce:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
  d1:	39 45 dc             	cmp    DWORD PTR [ebp-0x24],eax
  d4:	0f 8e a8 00 00 00    	jle    0x182
  da:	ff 4d fc             	dec    DWORD PTR [ebp-0x4]
  dd:	c7 45 cc 00 00 00 00 	mov    DWORD PTR [ebp-0x34],0x0
  e4:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
  e7:	8b 4d e0             	mov    ecx,DWORD PTR [ebp-0x20]
  ea:	0f be 04 08          	movsx  eax,BYTE PTR [eax+ecx*1]
  ee:	83 f8 20             	cmp    eax,0x20
  f1:	0f 84 4b 00 00 00    	je     0x142
  f7:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
  fa:	39 45 fc             	cmp    DWORD PTR [ebp-0x4],eax
  fd:	0f 8c 3f 00 00 00    	jl     0x142
 103:	33 db                	xor    ebx,ebx
 105:	8b 45 e0             	mov    eax,DWORD PTR [ebp-0x20]
 108:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 10b:	8a 04 08             	mov    al,BYTE PTR [eax+ecx*1]
 10e:	50                   	push   eax
 10f:	8b 4d c8             	mov    ecx,DWORD PTR [ebp-0x38]
 112:	e8 00 00 00 00       	call   0x117
 117:	2b d8                	sub    ebx,eax
 119:	f7 db                	neg    ebx
 11b:	29 5d dc             	sub    DWORD PTR [ebp-0x24],ebx
 11e:	83 7d cc 00          	cmp    DWORD PTR [ebp-0x34],0x0
 122:	0f 85 12 00 00 00    	jne    0x13a
 128:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 12b:	39 45 dc             	cmp    DWORD PTR [ebp-0x24],eax
 12e:	0f 8d 06 00 00 00    	jge    0x13a
 134:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
 137:	89 45 cc             	mov    DWORD PTR [ebp-0x34],eax
 13a:	ff 4d fc             	dec    DWORD PTR [ebp-0x4]
 13d:	e9 a2 ff ff ff       	jmp    0xe4
 142:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
 145:	39 45 fc             	cmp    DWORD PTR [ebp-0x4],eax
 148:	0f 8f 06 00 00 00    	jg     0x154
 14e:	8b 45 cc             	mov    eax,DWORD PTR [ebp-0x34]
 151:	89 45 fc             	mov    DWORD PTR [ebp-0x4],eax
 154:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
 157:	8b 4d e0             	mov    ecx,DWORD PTR [ebp-0x20]
 15a:	0f be 04 08          	movsx  eax,BYTE PTR [eax+ecx*1]
 15e:	83 f8 20             	cmp    eax,0x20
 161:	0f 85 1b 00 00 00    	jne    0x182
 167:	33 db                	xor    ebx,ebx
 169:	8b 45 e0             	mov    eax,DWORD PTR [ebp-0x20]
 16c:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 16f:	8a 04 08             	mov    al,BYTE PTR [eax+ecx*1]
 172:	50                   	push   eax
 173:	8b 4d c8             	mov    ecx,DWORD PTR [ebp-0x38]
 176:	e8 00 00 00 00       	call   0x17b
 17b:	2b d8                	sub    ebx,eax
 17d:	f7 db                	neg    ebx
 17f:	29 5d dc             	sub    DWORD PTR [ebp-0x24],ebx
 182:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
 185:	89 45 d8             	mov    DWORD PTR [ebp-0x28],eax
 188:	ff 45 d4             	inc    DWORD PTR [ebp-0x2c]
 18b:	8b 45 d8             	mov    eax,DWORD PTR [ebp-0x28]
 18e:	40                   	inc    eax
 18f:	89 45 f4             	mov    DWORD PTR [ebp-0xc],eax
 192:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
 195:	89 45 fc             	mov    DWORD PTR [ebp-0x4],eax
 198:	c7 45 dc 00 00 00 00 	mov    DWORD PTR [ebp-0x24],0x0
 19f:	e9 be fe ff ff       	jmp    0x62
 1a4:	8b 45 d4             	mov    eax,DWORD PTR [ebp-0x2c]
 1a7:	e9 00 00 00 00       	jmp    0x1ac
 1ac:	5f                   	pop    edi
 1ad:	5e                   	pop    esi
 1ae:	5b                   	pop    ebx
 1af:	c9                   	leave
 1b0:	c2 08 00             	ret    0x8
