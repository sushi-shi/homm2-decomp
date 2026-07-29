; ?SummonElemental@combatManager@@QAEXHH@Z
; unit SOURCE/SPELLS  src_hash 57cd18f1e020  score 100.0000  seed 31337  trial 1
; size 536  text_sha ed1e485c11e8f290
; relocs ["0000002c:0014:?SRandom@@YIHHH@Z:00000000", "0000006a:0014:?HasArtifact@hero@@QAEHH@Z:00000000", "00000136:0014:?AddArmy@combatManager@@QAEXHHHHHH@Z:00000000", "000001d6:0014:?HasArtifact@hero@@QAEHH@Z:00000000", "000001fc:0014:?HasArtifact@hero@@QAEHH@Z:00000000"]
; hex 558bec83ec1c535657894de4c645f40ec645f51bc645f628c645f70bc645f818c645f925ba0200000033c9e8000000008945e8c745fcffffffff8b45e48b80aff200008b4de483bc8187320000000f84250000006a358b45e48b80aff200008b4de48b8c8187320000e80000000085c00f8403000000d1650cc745ec00000000e903000000ff45ec837dec030f8d6d0000008b45e80345ecb90300000099f7f98b45e48b80aff200008d044003c233c98a4c05f48bc18d0c898d0c488d0cc92bc88b45e40fbe84012405000083f8ff0f85250000008b45e80345ecb90300000099f7f98b45e48b80aff200008d044003c233c98a4c05f4894dfce986ffffff8a45088b4de48b89aff200008b55e48884111d3500006a016a008b45fc508b450c8d0440508b4508508b45e48b80aff20000508b4de4e8000000008b45fc8bc88d04808d04418d04c02bc18b4de40fbe8408250500008bc8c1e0038d04c08d04c18b4dfc8bd18d0c898d0c4a8d0cc92bca8b55e40fbe8c11240500008bd1c1e1062bca8d0ccac1e1032bca8d0c4903c98d04410345e4054f3500008945f08b45f08188d0000000000800008b45e48b80aff200008b4de48b84818933000089450c6a2a8b45e48b80aff200008b4de48b8c8187320000e80000000085c00f840400000083450c026a428b45e48b80aff200008b4de48b8c8187320000e80000000085c00f840400000083450c0ae9000000005f5e5bc9c20800

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 1c             	sub    esp,0x1c
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 4d e4             	mov    DWORD PTR [ebp-0x1c],ecx
   c:	c6 45 f4 0e          	mov    BYTE PTR [ebp-0xc],0xe
  10:	c6 45 f5 1b          	mov    BYTE PTR [ebp-0xb],0x1b
  14:	c6 45 f6 28          	mov    BYTE PTR [ebp-0xa],0x28
  18:	c6 45 f7 0b          	mov    BYTE PTR [ebp-0x9],0xb
  1c:	c6 45 f8 18          	mov    BYTE PTR [ebp-0x8],0x18
  20:	c6 45 f9 25          	mov    BYTE PTR [ebp-0x7],0x25
  24:	ba 02 00 00 00       	mov    edx,0x2
  29:	33 c9                	xor    ecx,ecx
  2b:	e8 00 00 00 00       	call   0x30
  30:	89 45 e8             	mov    DWORD PTR [ebp-0x18],eax
  33:	c7 45 fc ff ff ff ff 	mov    DWORD PTR [ebp-0x4],0xffffffff
  3a:	8b 45 e4             	mov    eax,DWORD PTR [ebp-0x1c]
  3d:	8b 80 af f2 00 00    	mov    eax,DWORD PTR [eax+0xf2af]
  43:	8b 4d e4             	mov    ecx,DWORD PTR [ebp-0x1c]
  46:	83 bc 81 87 32 00 00 	cmp    DWORD PTR [ecx+eax*4+0x3287],0x0
  4d:	00
  4e:	0f 84 25 00 00 00    	je     0x79
  54:	6a 35                	push   0x35
  56:	8b 45 e4             	mov    eax,DWORD PTR [ebp-0x1c]
  59:	8b 80 af f2 00 00    	mov    eax,DWORD PTR [eax+0xf2af]
  5f:	8b 4d e4             	mov    ecx,DWORD PTR [ebp-0x1c]
  62:	8b 8c 81 87 32 00 00 	mov    ecx,DWORD PTR [ecx+eax*4+0x3287]
  69:	e8 00 00 00 00       	call   0x6e
  6e:	85 c0                	test   eax,eax
  70:	0f 84 03 00 00 00    	je     0x79
  76:	d1 65 0c             	shl    DWORD PTR [ebp+0xc],1
  79:	c7 45 ec 00 00 00 00 	mov    DWORD PTR [ebp-0x14],0x0
  80:	e9 03 00 00 00       	jmp    0x88
  85:	ff 45 ec             	inc    DWORD PTR [ebp-0x14]
  88:	83 7d ec 03          	cmp    DWORD PTR [ebp-0x14],0x3
  8c:	0f 8d 6d 00 00 00    	jge    0xff
  92:	8b 45 e8             	mov    eax,DWORD PTR [ebp-0x18]
  95:	03 45 ec             	add    eax,DWORD PTR [ebp-0x14]
  98:	b9 03 00 00 00       	mov    ecx,0x3
  9d:	99                   	cdq
  9e:	f7 f9                	idiv   ecx
  a0:	8b 45 e4             	mov    eax,DWORD PTR [ebp-0x1c]
  a3:	8b 80 af f2 00 00    	mov    eax,DWORD PTR [eax+0xf2af]
  a9:	8d 04 40             	lea    eax,[eax+eax*2]
  ac:	03 c2                	add    eax,edx
  ae:	33 c9                	xor    ecx,ecx
  b0:	8a 4c 05 f4          	mov    cl,BYTE PTR [ebp+eax*1-0xc]
  b4:	8b c1                	mov    eax,ecx
  b6:	8d 0c 89             	lea    ecx,[ecx+ecx*4]
  b9:	8d 0c 48             	lea    ecx,[eax+ecx*2]
  bc:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  bf:	2b c8                	sub    ecx,eax
  c1:	8b 45 e4             	mov    eax,DWORD PTR [ebp-0x1c]
  c4:	0f be 84 01 24 05 00 	movsx  eax,BYTE PTR [ecx+eax*1+0x524]
  cb:	00
  cc:	83 f8 ff             	cmp    eax,0xffffffff
  cf:	0f 85 25 00 00 00    	jne    0xfa
  d5:	8b 45 e8             	mov    eax,DWORD PTR [ebp-0x18]
  d8:	03 45 ec             	add    eax,DWORD PTR [ebp-0x14]
  db:	b9 03 00 00 00       	mov    ecx,0x3
  e0:	99                   	cdq
  e1:	f7 f9                	idiv   ecx
  e3:	8b 45 e4             	mov    eax,DWORD PTR [ebp-0x1c]
  e6:	8b 80 af f2 00 00    	mov    eax,DWORD PTR [eax+0xf2af]
  ec:	8d 04 40             	lea    eax,[eax+eax*2]
  ef:	03 c2                	add    eax,edx
  f1:	33 c9                	xor    ecx,ecx
  f3:	8a 4c 05 f4          	mov    cl,BYTE PTR [ebp+eax*1-0xc]
  f7:	89 4d fc             	mov    DWORD PTR [ebp-0x4],ecx
  fa:	e9 86 ff ff ff       	jmp    0x85
  ff:	8a 45 08             	mov    al,BYTE PTR [ebp+0x8]
 102:	8b 4d e4             	mov    ecx,DWORD PTR [ebp-0x1c]
 105:	8b 89 af f2 00 00    	mov    ecx,DWORD PTR [ecx+0xf2af]
 10b:	8b 55 e4             	mov    edx,DWORD PTR [ebp-0x1c]
 10e:	88 84 11 1d 35 00 00 	mov    BYTE PTR [ecx+edx*1+0x351d],al
 115:	6a 01                	push   0x1
 117:	6a 00                	push   0x0
 119:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
 11c:	50                   	push   eax
 11d:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 120:	8d 04 40             	lea    eax,[eax+eax*2]
 123:	50                   	push   eax
 124:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 127:	50                   	push   eax
 128:	8b 45 e4             	mov    eax,DWORD PTR [ebp-0x1c]
 12b:	8b 80 af f2 00 00    	mov    eax,DWORD PTR [eax+0xf2af]
 131:	50                   	push   eax
 132:	8b 4d e4             	mov    ecx,DWORD PTR [ebp-0x1c]
 135:	e8 00 00 00 00       	call   0x13a
 13a:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
 13d:	8b c8                	mov    ecx,eax
 13f:	8d 04 80             	lea    eax,[eax+eax*4]
 142:	8d 04 41             	lea    eax,[ecx+eax*2]
 145:	8d 04 c0             	lea    eax,[eax+eax*8]
 148:	2b c1                	sub    eax,ecx
 14a:	8b 4d e4             	mov    ecx,DWORD PTR [ebp-0x1c]
 14d:	0f be 84 08 25 05 00 	movsx  eax,BYTE PTR [eax+ecx*1+0x525]
 154:	00
 155:	8b c8                	mov    ecx,eax
 157:	c1 e0 03             	shl    eax,0x3
 15a:	8d 04 c0             	lea    eax,[eax+eax*8]
 15d:	8d 04 c1             	lea    eax,[ecx+eax*8]
 160:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 163:	8b d1                	mov    edx,ecx
 165:	8d 0c 89             	lea    ecx,[ecx+ecx*4]
 168:	8d 0c 4a             	lea    ecx,[edx+ecx*2]
 16b:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
 16e:	2b ca                	sub    ecx,edx
 170:	8b 55 e4             	mov    edx,DWORD PTR [ebp-0x1c]
 173:	0f be 8c 11 24 05 00 	movsx  ecx,BYTE PTR [ecx+edx*1+0x524]
 17a:	00
 17b:	8b d1                	mov    edx,ecx
 17d:	c1 e1 06             	shl    ecx,0x6
 180:	2b ca                	sub    ecx,edx
 182:	8d 0c ca             	lea    ecx,[edx+ecx*8]
 185:	c1 e1 03             	shl    ecx,0x3
 188:	2b ca                	sub    ecx,edx
 18a:	8d 0c 49             	lea    ecx,[ecx+ecx*2]
 18d:	03 c9                	add    ecx,ecx
 18f:	8d 04 41             	lea    eax,[ecx+eax*2]
 192:	03 45 e4             	add    eax,DWORD PTR [ebp-0x1c]
 195:	05 4f 35 00 00       	add    eax,0x354f
 19a:	89 45 f0             	mov    DWORD PTR [ebp-0x10],eax
 19d:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 1a0:	81 88 d0 00 00 00 00 	or     DWORD PTR [eax+0xd0],0x800
 1a7:	08 00 00
 1aa:	8b 45 e4             	mov    eax,DWORD PTR [ebp-0x1c]
 1ad:	8b 80 af f2 00 00    	mov    eax,DWORD PTR [eax+0xf2af]
 1b3:	8b 4d e4             	mov    ecx,DWORD PTR [ebp-0x1c]
 1b6:	8b 84 81 89 33 00 00 	mov    eax,DWORD PTR [ecx+eax*4+0x3389]
 1bd:	89 45 0c             	mov    DWORD PTR [ebp+0xc],eax
 1c0:	6a 2a                	push   0x2a
 1c2:	8b 45 e4             	mov    eax,DWORD PTR [ebp-0x1c]
 1c5:	8b 80 af f2 00 00    	mov    eax,DWORD PTR [eax+0xf2af]
 1cb:	8b 4d e4             	mov    ecx,DWORD PTR [ebp-0x1c]
 1ce:	8b 8c 81 87 32 00 00 	mov    ecx,DWORD PTR [ecx+eax*4+0x3287]
 1d5:	e8 00 00 00 00       	call   0x1da
 1da:	85 c0                	test   eax,eax
 1dc:	0f 84 04 00 00 00    	je     0x1e6
 1e2:	83 45 0c 02          	add    DWORD PTR [ebp+0xc],0x2
 1e6:	6a 42                	push   0x42
 1e8:	8b 45 e4             	mov    eax,DWORD PTR [ebp-0x1c]
 1eb:	8b 80 af f2 00 00    	mov    eax,DWORD PTR [eax+0xf2af]
 1f1:	8b 4d e4             	mov    ecx,DWORD PTR [ebp-0x1c]
 1f4:	8b 8c 81 87 32 00 00 	mov    ecx,DWORD PTR [ecx+eax*4+0x3287]
 1fb:	e8 00 00 00 00       	call   0x200
 200:	85 c0                	test   eax,eax
 202:	0f 84 04 00 00 00    	je     0x20c
 208:	83 45 0c 0a          	add    DWORD PTR [ebp+0xc],0xa
 20c:	e9 00 00 00 00       	jmp    0x211
 211:	5f                   	pop    edi
 212:	5e                   	pop    esi
 213:	5b                   	pop    ebx
 214:	c9                   	leave
 215:	c2 08 00             	ret    0x8
