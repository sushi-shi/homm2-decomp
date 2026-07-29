; ?PerMonth@game@@QAEXXZ
; unit SOURCE/GAME  src_hash 54301e5d8082  score 100.0000  seed 31337  trial 4
; size 885  text_sha 11004fc51f5160b8
; relocs ["00000021:0014:?Random@@YIHHH@Z:00000000", "00000034:0006:?giMonthType@@3HA:00000000", "00000044:0014:?Random@@YIHHH@Z:00000000", "00000049:0006:?giMonthTypeExtra@@3HA:00000000", "0000005e:0006:?giMonthType@@3HA:00000000", "0000006e:0014:?Random@@YIHHH@Z:00000000", "00000075:0006:?giMonType@@3PACA:00000000", "0000007a:0006:?giMonthTypeExtra@@3HA:00000000", "00000085:0006:?giMonthType@@3HA:00000000", "00000102:0006:?gDwellingType@@3PAY0M@CA:edffffff", "00000112:0006:?gMonsterDatabase@@3PAUtag_monsterInfo@@A:07000000", "00000147:0006:?giMonthType@@3HA:00000000", "00000163:0006:?gDwellingType@@3PAY0M@CA:edffffff", "00000169:0006:?giMonthTypeExtra@@3HA:00000000", "0000018d:0006:?giMonthType@@3HA:00000000", "000001f5:0006:?giMonthType@@3HA:00000000", "00000210:0006:?MAP_WIDTH@@3HA:00000000", "0000022d:0006:?MAP_HEIGHT@@3HA:00000000", "00000244:0006:?gpAdvManager@@3PAVadvManager@@A:00000000", "00000249:0014:?GetCell@advManager@@QAEPAVmapCell@@HH@Z:00000000", "0000028d:0006:?giGroundToTerrain@@3PAEA:00000000", "000002a1:0014:?Random@@YIHHH@Z:00000000", "000002c6:0006:?giMonthTypeExtra@@3HA:00000000", "000002d1:0006:?giMonthTypeExtra@@3HA:00000000", "000002da:0014:?GetRandomNumTroops@game@@QAEHH@Z:00000000", "000002e2:0006:?giMonthTypeExtra@@3HA:00000000", "000002eb:0014:?GetRandomNumTroops@game@@QAEHH@Z:00000000", "00000319:0014:?Random@@YIHHH@Z:00000000", "00000362:0006:?gpAdvManager@@3PAVadvManager@@A:00000000", "00000367:0014:?CompleteDraw@advManager@@QAEXH@Z:00000000"]
; hex 558bec83ec28535657894dd88b45d866ff809a040000ba0a000000b901000000e8000000008945f0837df0050f8f20000000c7050000000000000000ba0900000033c9e800000000a300000000e93b000000837df0090f8f27000000c7050000000001000000ba0b00000033c9e8000000000fbe8000000000a300000000e90a000000c7050000000002000000c745f000000000e903000000ff45f0837df0480f8d4d010000c745ec13000000e903000000ff45ec837dec1e0f8f2f010000e9000000008b45f0c1e0028d04808d04800345d805530b00008945e48b45e4ba010000008a4decd3e28550180f84f80000008b45e40fbe40038d04408b4dec0fbe8481edffffff8bc88d04808d04800fbe8408070000008945e88b45e4f64018100f84040000008345e802837dec130f85110000008b45e4f64019080f84040000008345e808833d00000000010f85390000008b45e40fbe40038d04408b4dec0fbe8481edffffff3b05000000000f85180000008b45ec8b4de40fbf4441f803c08b4dec8b55e46689444af8833d00000000020f85510000008b45ec8b4de40fbf4441f82b45e88b4dec8b55e46689444af88b45ec8b4de40fbf4441f885c00f8d0d0000008b45ec8b4de466c74441f800008b45ec8b4de40fbf4441f8d1f88b4dec8b55e46689444af8e9c4feffffe9a6feffff833d00000000010f855e010000c745f800000000e903000000ff45f8a1000000003945f80f8d41010000c745f400000000e903000000ff45f4a1000000003945f40f8d1f0100008b45f4508b45f8508b0d00000000e8000000008945fc8b45fc33c98a480985c90f85f40000008b45fc668b400466d1e8a8010f85e20000008b45fc668b4004a8010f85d30000008b45fc33c9668b0833c08a810000000085c00f84bb000000ba6801000033c9e80000000083f80a0f85a60000008b45fcc64009988b45fc8a400224030c308b4dfc884102a0000000008b4dfc884103a100000000508b4dd8e8000000008945e0a100000000508b4dd8e8000000008945dc8b45e00345dc66c1e0038b4dfc668b49046683e107660bc18b4dfc66894104ba6400000033c9e80000000083f8140f8d2e0000008b45fc668b400466c1e80325ff1f00000d0010000066c1e0038b4dfc668b49046683e107660bc18b4dfc66894104e9d0feffffe9aefeffff6a008b0d00000000e800000000e9000000005f5e5bc9c3

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 28             	sub    esp,0x28
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 4d d8             	mov    DWORD PTR [ebp-0x28],ecx
   c:	8b 45 d8             	mov    eax,DWORD PTR [ebp-0x28]
   f:	66 ff 80 9a 04 00 00 	inc    WORD PTR [eax+0x49a]
  16:	ba 0a 00 00 00       	mov    edx,0xa
  1b:	b9 01 00 00 00       	mov    ecx,0x1
  20:	e8 00 00 00 00       	call   0x25
  25:	89 45 f0             	mov    DWORD PTR [ebp-0x10],eax
  28:	83 7d f0 05          	cmp    DWORD PTR [ebp-0x10],0x5
  2c:	0f 8f 20 00 00 00    	jg     0x52
  32:	c7 05 00 00 00 00 00 	mov    DWORD PTR ds:0x0,0x0
  39:	00 00 00
  3c:	ba 09 00 00 00       	mov    edx,0x9
  41:	33 c9                	xor    ecx,ecx
  43:	e8 00 00 00 00       	call   0x48
  48:	a3 00 00 00 00       	mov    ds:0x0,eax
  4d:	e9 3b 00 00 00       	jmp    0x8d
  52:	83 7d f0 09          	cmp    DWORD PTR [ebp-0x10],0x9
  56:	0f 8f 27 00 00 00    	jg     0x83
  5c:	c7 05 00 00 00 00 01 	mov    DWORD PTR ds:0x0,0x1
  63:	00 00 00
  66:	ba 0b 00 00 00       	mov    edx,0xb
  6b:	33 c9                	xor    ecx,ecx
  6d:	e8 00 00 00 00       	call   0x72
  72:	0f be 80 00 00 00 00 	movsx  eax,BYTE PTR [eax+0x0]
  79:	a3 00 00 00 00       	mov    ds:0x0,eax
  7e:	e9 0a 00 00 00       	jmp    0x8d
  83:	c7 05 00 00 00 00 02 	mov    DWORD PTR ds:0x0,0x2
  8a:	00 00 00
  8d:	c7 45 f0 00 00 00 00 	mov    DWORD PTR [ebp-0x10],0x0
  94:	e9 03 00 00 00       	jmp    0x9c
  99:	ff 45 f0             	inc    DWORD PTR [ebp-0x10]
  9c:	83 7d f0 48          	cmp    DWORD PTR [ebp-0x10],0x48
  a0:	0f 8d 4d 01 00 00    	jge    0x1f3
  a6:	c7 45 ec 13 00 00 00 	mov    DWORD PTR [ebp-0x14],0x13
  ad:	e9 03 00 00 00       	jmp    0xb5
  b2:	ff 45 ec             	inc    DWORD PTR [ebp-0x14]
  b5:	83 7d ec 1e          	cmp    DWORD PTR [ebp-0x14],0x1e
  b9:	0f 8f 2f 01 00 00    	jg     0x1ee
  bf:	e9 00 00 00 00       	jmp    0xc4
  c4:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  c7:	c1 e0 02             	shl    eax,0x2
  ca:	8d 04 80             	lea    eax,[eax+eax*4]
  cd:	8d 04 80             	lea    eax,[eax+eax*4]
  d0:	03 45 d8             	add    eax,DWORD PTR [ebp-0x28]
  d3:	05 53 0b 00 00       	add    eax,0xb53
  d8:	89 45 e4             	mov    DWORD PTR [ebp-0x1c],eax
  db:	8b 45 e4             	mov    eax,DWORD PTR [ebp-0x1c]
  de:	ba 01 00 00 00       	mov    edx,0x1
  e3:	8a 4d ec             	mov    cl,BYTE PTR [ebp-0x14]
  e6:	d3 e2                	shl    edx,cl
  e8:	85 50 18             	test   DWORD PTR [eax+0x18],edx
  eb:	0f 84 f8 00 00 00    	je     0x1e9
  f1:	8b 45 e4             	mov    eax,DWORD PTR [ebp-0x1c]
  f4:	0f be 40 03          	movsx  eax,BYTE PTR [eax+0x3]
  f8:	8d 04 40             	lea    eax,[eax+eax*2]
  fb:	8b 4d ec             	mov    ecx,DWORD PTR [ebp-0x14]
  fe:	0f be 84 81 ed ff ff 	movsx  eax,BYTE PTR [ecx+eax*4-0x13]
 105:	ff
 106:	8b c8                	mov    ecx,eax
 108:	8d 04 80             	lea    eax,[eax+eax*4]
 10b:	8d 04 80             	lea    eax,[eax+eax*4]
 10e:	0f be 84 08 07 00 00 	movsx  eax,BYTE PTR [eax+ecx*1+0x7]
 115:	00
 116:	89 45 e8             	mov    DWORD PTR [ebp-0x18],eax
 119:	8b 45 e4             	mov    eax,DWORD PTR [ebp-0x1c]
 11c:	f6 40 18 10          	test   BYTE PTR [eax+0x18],0x10
 120:	0f 84 04 00 00 00    	je     0x12a
 126:	83 45 e8 02          	add    DWORD PTR [ebp-0x18],0x2
 12a:	83 7d ec 13          	cmp    DWORD PTR [ebp-0x14],0x13
 12e:	0f 85 11 00 00 00    	jne    0x145
 134:	8b 45 e4             	mov    eax,DWORD PTR [ebp-0x1c]
 137:	f6 40 19 08          	test   BYTE PTR [eax+0x19],0x8
 13b:	0f 84 04 00 00 00    	je     0x145
 141:	83 45 e8 08          	add    DWORD PTR [ebp-0x18],0x8
 145:	83 3d 00 00 00 00 01 	cmp    DWORD PTR ds:0x0,0x1
 14c:	0f 85 39 00 00 00    	jne    0x18b
 152:	8b 45 e4             	mov    eax,DWORD PTR [ebp-0x1c]
 155:	0f be 40 03          	movsx  eax,BYTE PTR [eax+0x3]
 159:	8d 04 40             	lea    eax,[eax+eax*2]
 15c:	8b 4d ec             	mov    ecx,DWORD PTR [ebp-0x14]
 15f:	0f be 84 81 ed ff ff 	movsx  eax,BYTE PTR [ecx+eax*4-0x13]
 166:	ff
 167:	3b 05 00 00 00 00    	cmp    eax,DWORD PTR ds:0x0
 16d:	0f 85 18 00 00 00    	jne    0x18b
 173:	8b 45 ec             	mov    eax,DWORD PTR [ebp-0x14]
 176:	8b 4d e4             	mov    ecx,DWORD PTR [ebp-0x1c]
 179:	0f bf 44 41 f8       	movsx  eax,WORD PTR [ecx+eax*2-0x8]
 17e:	03 c0                	add    eax,eax
 180:	8b 4d ec             	mov    ecx,DWORD PTR [ebp-0x14]
 183:	8b 55 e4             	mov    edx,DWORD PTR [ebp-0x1c]
 186:	66 89 44 4a f8       	mov    WORD PTR [edx+ecx*2-0x8],ax
 18b:	83 3d 00 00 00 00 02 	cmp    DWORD PTR ds:0x0,0x2
 192:	0f 85 51 00 00 00    	jne    0x1e9
 198:	8b 45 ec             	mov    eax,DWORD PTR [ebp-0x14]
 19b:	8b 4d e4             	mov    ecx,DWORD PTR [ebp-0x1c]
 19e:	0f bf 44 41 f8       	movsx  eax,WORD PTR [ecx+eax*2-0x8]
 1a3:	2b 45 e8             	sub    eax,DWORD PTR [ebp-0x18]
 1a6:	8b 4d ec             	mov    ecx,DWORD PTR [ebp-0x14]
 1a9:	8b 55 e4             	mov    edx,DWORD PTR [ebp-0x1c]
 1ac:	66 89 44 4a f8       	mov    WORD PTR [edx+ecx*2-0x8],ax
 1b1:	8b 45 ec             	mov    eax,DWORD PTR [ebp-0x14]
 1b4:	8b 4d e4             	mov    ecx,DWORD PTR [ebp-0x1c]
 1b7:	0f bf 44 41 f8       	movsx  eax,WORD PTR [ecx+eax*2-0x8]
 1bc:	85 c0                	test   eax,eax
 1be:	0f 8d 0d 00 00 00    	jge    0x1d1
 1c4:	8b 45 ec             	mov    eax,DWORD PTR [ebp-0x14]
 1c7:	8b 4d e4             	mov    ecx,DWORD PTR [ebp-0x1c]
 1ca:	66 c7 44 41 f8 00 00 	mov    WORD PTR [ecx+eax*2-0x8],0x0
 1d1:	8b 45 ec             	mov    eax,DWORD PTR [ebp-0x14]
 1d4:	8b 4d e4             	mov    ecx,DWORD PTR [ebp-0x1c]
 1d7:	0f bf 44 41 f8       	movsx  eax,WORD PTR [ecx+eax*2-0x8]
 1dc:	d1 f8                	sar    eax,1
 1de:	8b 4d ec             	mov    ecx,DWORD PTR [ebp-0x14]
 1e1:	8b 55 e4             	mov    edx,DWORD PTR [ebp-0x1c]
 1e4:	66 89 44 4a f8       	mov    WORD PTR [edx+ecx*2-0x8],ax
 1e9:	e9 c4 fe ff ff       	jmp    0xb2
 1ee:	e9 a6 fe ff ff       	jmp    0x99
 1f3:	83 3d 00 00 00 00 01 	cmp    DWORD PTR ds:0x0,0x1
 1fa:	0f 85 5e 01 00 00    	jne    0x35e
 200:	c7 45 f8 00 00 00 00 	mov    DWORD PTR [ebp-0x8],0x0
 207:	e9 03 00 00 00       	jmp    0x20f
 20c:	ff 45 f8             	inc    DWORD PTR [ebp-0x8]
 20f:	a1 00 00 00 00       	mov    eax,ds:0x0
 214:	39 45 f8             	cmp    DWORD PTR [ebp-0x8],eax
 217:	0f 8d 41 01 00 00    	jge    0x35e
 21d:	c7 45 f4 00 00 00 00 	mov    DWORD PTR [ebp-0xc],0x0
 224:	e9 03 00 00 00       	jmp    0x22c
 229:	ff 45 f4             	inc    DWORD PTR [ebp-0xc]
 22c:	a1 00 00 00 00       	mov    eax,ds:0x0
 231:	39 45 f4             	cmp    DWORD PTR [ebp-0xc],eax
 234:	0f 8d 1f 01 00 00    	jge    0x359
 23a:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
 23d:	50                   	push   eax
 23e:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
 241:	50                   	push   eax
 242:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
 248:	e8 00 00 00 00       	call   0x24d
 24d:	89 45 fc             	mov    DWORD PTR [ebp-0x4],eax
 250:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
 253:	33 c9                	xor    ecx,ecx
 255:	8a 48 09             	mov    cl,BYTE PTR [eax+0x9]
 258:	85 c9                	test   ecx,ecx
 25a:	0f 85 f4 00 00 00    	jne    0x354
 260:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
 263:	66 8b 40 04          	mov    ax,WORD PTR [eax+0x4]
 267:	66 d1 e8             	shr    ax,1
 26a:	a8 01                	test   al,0x1
 26c:	0f 85 e2 00 00 00    	jne    0x354
 272:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
 275:	66 8b 40 04          	mov    ax,WORD PTR [eax+0x4]
 279:	a8 01                	test   al,0x1
 27b:	0f 85 d3 00 00 00    	jne    0x354
 281:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
 284:	33 c9                	xor    ecx,ecx
 286:	66 8b 08             	mov    cx,WORD PTR [eax]
 289:	33 c0                	xor    eax,eax
 28b:	8a 81 00 00 00 00    	mov    al,BYTE PTR [ecx+0x0]
 291:	85 c0                	test   eax,eax
 293:	0f 84 bb 00 00 00    	je     0x354
 299:	ba 68 01 00 00       	mov    edx,0x168
 29e:	33 c9                	xor    ecx,ecx
 2a0:	e8 00 00 00 00       	call   0x2a5
 2a5:	83 f8 0a             	cmp    eax,0xa
 2a8:	0f 85 a6 00 00 00    	jne    0x354
 2ae:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
 2b1:	c6 40 09 98          	mov    BYTE PTR [eax+0x9],0x98
 2b5:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
 2b8:	8a 40 02             	mov    al,BYTE PTR [eax+0x2]
 2bb:	24 03                	and    al,0x3
 2bd:	0c 30                	or     al,0x30
 2bf:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 2c2:	88 41 02             	mov    BYTE PTR [ecx+0x2],al
 2c5:	a0 00 00 00 00       	mov    al,ds:0x0
 2ca:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 2cd:	88 41 03             	mov    BYTE PTR [ecx+0x3],al
 2d0:	a1 00 00 00 00       	mov    eax,ds:0x0
 2d5:	50                   	push   eax
 2d6:	8b 4d d8             	mov    ecx,DWORD PTR [ebp-0x28]
 2d9:	e8 00 00 00 00       	call   0x2de
 2de:	89 45 e0             	mov    DWORD PTR [ebp-0x20],eax
 2e1:	a1 00 00 00 00       	mov    eax,ds:0x0
 2e6:	50                   	push   eax
 2e7:	8b 4d d8             	mov    ecx,DWORD PTR [ebp-0x28]
 2ea:	e8 00 00 00 00       	call   0x2ef
 2ef:	89 45 dc             	mov    DWORD PTR [ebp-0x24],eax
 2f2:	8b 45 e0             	mov    eax,DWORD PTR [ebp-0x20]
 2f5:	03 45 dc             	add    eax,DWORD PTR [ebp-0x24]
 2f8:	66 c1 e0 03          	shl    ax,0x3
 2fc:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 2ff:	66 8b 49 04          	mov    cx,WORD PTR [ecx+0x4]
 303:	66 83 e1 07          	and    cx,0x7
 307:	66 0b c1             	or     ax,cx
 30a:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 30d:	66 89 41 04          	mov    WORD PTR [ecx+0x4],ax
 311:	ba 64 00 00 00       	mov    edx,0x64
 316:	33 c9                	xor    ecx,ecx
 318:	e8 00 00 00 00       	call   0x31d
 31d:	83 f8 14             	cmp    eax,0x14
 320:	0f 8d 2e 00 00 00    	jge    0x354
 326:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
 329:	66 8b 40 04          	mov    ax,WORD PTR [eax+0x4]
 32d:	66 c1 e8 03          	shr    ax,0x3
 331:	25 ff 1f 00 00       	and    eax,0x1fff
 336:	0d 00 10 00 00       	or     eax,0x1000
 33b:	66 c1 e0 03          	shl    ax,0x3
 33f:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 342:	66 8b 49 04          	mov    cx,WORD PTR [ecx+0x4]
 346:	66 83 e1 07          	and    cx,0x7
 34a:	66 0b c1             	or     ax,cx
 34d:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 350:	66 89 41 04          	mov    WORD PTR [ecx+0x4],ax
 354:	e9 d0 fe ff ff       	jmp    0x229
 359:	e9 ae fe ff ff       	jmp    0x20c
 35e:	6a 00                	push   0x0
 360:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
 366:	e8 00 00 00 00       	call   0x36b
 36b:	e9 00 00 00 00       	jmp    0x370
 370:	5f                   	pop    edi
 371:	5e                   	pop    esi
 372:	5b                   	pop    ebx
 373:	c9                   	leave
 374:	c3                   	ret
