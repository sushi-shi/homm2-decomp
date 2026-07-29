; ?Open@recruitUnit@@UAEHH@Z
; unit SOURCE/RECRUIT  src_hash e6ec0a1c1561  score 100.0000  seed 1380929614  trial 4
; size 587  text_sha 39b07b66a5cffe99
; relocs ["0000000f:0014:??2@YAPAXI@Z:00000000", "00000024:0006:__h2cg$SOURCE$RECRUIT$data$openRecruit0Bin:00000000", "00000029:0006:__h2cg$SOURCE$RECRUIT$data$openRecruit1Bin:00000000", "0000004c:0014:??0heroWindow@@QAE@HHPAD@Z:00000000", "00000073:0014:?MemError@@YIXXZ:00000000", "000000c1:0014:?SetupRecruitWin@@YIXPAVheroWindow@@HHHHH@Z:00000000", "000000cd:0006:__h2cg$SOURCE$RECRUIT$data$openAdvmiceMse:00000000", "000000d3:0006:?gpMouseManager@@3PAVmouseManager@@A:00000000", "000000d8:0014:?SetPointer@mouseManager@@QAEXPADHH@Z:00000000", "000000e0:0014:?Update@recruitUnit@@QAEXXZ:00000000", "000000f7:0006:?gpWindowManager@@3PAVheroWindowManager@@A:00000000", "000000fc:0014:?BroadcastMessage@heroWindowManager@@QAEHHHHH@Z:00000000", "0000010d:0006:?gpWindowManager@@3PAVheroWindowManager@@A:00000000", "00000112:0014:?AddWindow@heroWindowManager@@QAEXPAVheroWindow@@HH@Z:00000000", "00000117:0006:?gpCurPlayer@@3PAVplayerData@@A:00000000", "00000140:0006:?gpCurPlayer@@3PAVplayerData@@A:00000000", "000001d2:0006:?gpWindowManager@@3PAVheroWindowManager@@A:00000000", "000001d7:0014:?BroadcastMessage@heroWindowManager@@QAEHHHHH@Z:00000000", "000001ee:0006:?gpWindowManager@@3PAVheroWindowManager@@A:00000000", "000001f3:0014:?BroadcastMessage@heroWindowManager@@QAEHHHHH@Z:00000000", "000001f8:0006:?hmnuCurrent@@3PAXA:00000000", "000001fd:0006:?hmnuRecruitSave@@3PAXA:00000000", "00000203:0006:?hmnuDflt@@3PAXA:00000000", "00000208:0014:?KBChangeMenu@@YIXPAX@Z:00000000", "0000022a:0006:__h2cg$SOURCE$RECRUIT$data$openRecruitManager:00000000", "00000236:0014:_strcpy:00000000"]
; hex 558bec83ec10535657894df06a44e80000000083c4048945f4837df4000f8438000000b800000000b9000000008b55f033db837a46ff0f94c34b2bc823d903d8536a10688f0000008b4df4e8000000008b4df0894152e90a0000008b45f0c74052000000008b45f0837852000f8505000000e8000000008b45f0c7407a000000008b45f0c74072000000008b45f0c74076000000008b45f08b406a0fbf00508b45f08b404a508b45f08b4046508b45f08b4042508b45f08b4df08b503a8b4952e8000000006819fcffff6a0068000000008b0d00000000e8000000008b4df0e800000000680840000068007800006a0568000200008b0d00000000e8000000006a016aff8b45f08b4052508b0d00000000e800000000a1000000008b4df08b80a700000099f779428945f88b45f0837846ff0f84380000008b45f08b40468b0d000000008b5df08b84818f00000099f77b4a8945fc8b45f88b4dfc3bc10f8c020000008bc18b4df089416ee9090000008b45f88b4df089416e8b45f08b406a0fbf008b4df03b416e0f8d0f0000008b45f08b406a0fbf008b4df089416e8b45f0c74062000000008b45f0c74066000000008b45f08b406a0fbf0085c00f85350000006a0268027800006a0668000200008b0d00000000e800000000680840000068027800006a0568000200008b0d00000000e800000000a100000000a3000000008b0d00000000e8000000008b45f0c7400c004000008b45088b4df08941108b45f0c740320100000068000000008b45f083c01450e80000000083c40833c0e9000000005f5e5bc9c20400

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 10             	sub    esp,0x10
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 4d f0             	mov    DWORD PTR [ebp-0x10],ecx
   c:	6a 44                	push   0x44
   e:	e8 00 00 00 00       	call   0x13
  13:	83 c4 04             	add    esp,0x4
  16:	89 45 f4             	mov    DWORD PTR [ebp-0xc],eax
  19:	83 7d f4 00          	cmp    DWORD PTR [ebp-0xc],0x0
  1d:	0f 84 38 00 00 00    	je     0x5b
  23:	b8 00 00 00 00       	mov    eax,0x0
  28:	b9 00 00 00 00       	mov    ecx,0x0
  2d:	8b 55 f0             	mov    edx,DWORD PTR [ebp-0x10]
  30:	33 db                	xor    ebx,ebx
  32:	83 7a 46 ff          	cmp    DWORD PTR [edx+0x46],0xffffffff
  36:	0f 94 c3             	sete   bl
  39:	4b                   	dec    ebx
  3a:	2b c8                	sub    ecx,eax
  3c:	23 d9                	and    ebx,ecx
  3e:	03 d8                	add    ebx,eax
  40:	53                   	push   ebx
  41:	6a 10                	push   0x10
  43:	68 8f 00 00 00       	push   0x8f
  48:	8b 4d f4             	mov    ecx,DWORD PTR [ebp-0xc]
  4b:	e8 00 00 00 00       	call   0x50
  50:	8b 4d f0             	mov    ecx,DWORD PTR [ebp-0x10]
  53:	89 41 52             	mov    DWORD PTR [ecx+0x52],eax
  56:	e9 0a 00 00 00       	jmp    0x65
  5b:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  5e:	c7 40 52 00 00 00 00 	mov    DWORD PTR [eax+0x52],0x0
  65:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  68:	83 78 52 00          	cmp    DWORD PTR [eax+0x52],0x0
  6c:	0f 85 05 00 00 00    	jne    0x77
  72:	e8 00 00 00 00       	call   0x77
  77:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  7a:	c7 40 7a 00 00 00 00 	mov    DWORD PTR [eax+0x7a],0x0
  81:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  84:	c7 40 72 00 00 00 00 	mov    DWORD PTR [eax+0x72],0x0
  8b:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  8e:	c7 40 76 00 00 00 00 	mov    DWORD PTR [eax+0x76],0x0
  95:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  98:	8b 40 6a             	mov    eax,DWORD PTR [eax+0x6a]
  9b:	0f bf 00             	movsx  eax,WORD PTR [eax]
  9e:	50                   	push   eax
  9f:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  a2:	8b 40 4a             	mov    eax,DWORD PTR [eax+0x4a]
  a5:	50                   	push   eax
  a6:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  a9:	8b 40 46             	mov    eax,DWORD PTR [eax+0x46]
  ac:	50                   	push   eax
  ad:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  b0:	8b 40 42             	mov    eax,DWORD PTR [eax+0x42]
  b3:	50                   	push   eax
  b4:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  b7:	8b 4d f0             	mov    ecx,DWORD PTR [ebp-0x10]
  ba:	8b 50 3a             	mov    edx,DWORD PTR [eax+0x3a]
  bd:	8b 49 52             	mov    ecx,DWORD PTR [ecx+0x52]
  c0:	e8 00 00 00 00       	call   0xc5
  c5:	68 19 fc ff ff       	push   0xfffffc19
  ca:	6a 00                	push   0x0
  cc:	68 00 00 00 00       	push   0x0
  d1:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
  d7:	e8 00 00 00 00       	call   0xdc
  dc:	8b 4d f0             	mov    ecx,DWORD PTR [ebp-0x10]
  df:	e8 00 00 00 00       	call   0xe4
  e4:	68 08 40 00 00       	push   0x4008
  e9:	68 00 78 00 00       	push   0x7800
  ee:	6a 05                	push   0x5
  f0:	68 00 02 00 00       	push   0x200
  f5:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
  fb:	e8 00 00 00 00       	call   0x100
 100:	6a 01                	push   0x1
 102:	6a ff                	push   0xffffffff
 104:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 107:	8b 40 52             	mov    eax,DWORD PTR [eax+0x52]
 10a:	50                   	push   eax
 10b:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
 111:	e8 00 00 00 00       	call   0x116
 116:	a1 00 00 00 00       	mov    eax,ds:0x0
 11b:	8b 4d f0             	mov    ecx,DWORD PTR [ebp-0x10]
 11e:	8b 80 a7 00 00 00    	mov    eax,DWORD PTR [eax+0xa7]
 124:	99                   	cdq
 125:	f7 79 42             	idiv   DWORD PTR [ecx+0x42]
 128:	89 45 f8             	mov    DWORD PTR [ebp-0x8],eax
 12b:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 12e:	83 78 46 ff          	cmp    DWORD PTR [eax+0x46],0xffffffff
 132:	0f 84 38 00 00 00    	je     0x170
 138:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 13b:	8b 40 46             	mov    eax,DWORD PTR [eax+0x46]
 13e:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
 144:	8b 5d f0             	mov    ebx,DWORD PTR [ebp-0x10]
 147:	8b 84 81 8f 00 00 00 	mov    eax,DWORD PTR [ecx+eax*4+0x8f]
 14e:	99                   	cdq
 14f:	f7 7b 4a             	idiv   DWORD PTR [ebx+0x4a]
 152:	89 45 fc             	mov    DWORD PTR [ebp-0x4],eax
 155:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
 158:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 15b:	3b c1                	cmp    eax,ecx
 15d:	0f 8c 02 00 00 00    	jl     0x165
 163:	8b c1                	mov    eax,ecx
 165:	8b 4d f0             	mov    ecx,DWORD PTR [ebp-0x10]
 168:	89 41 6e             	mov    DWORD PTR [ecx+0x6e],eax
 16b:	e9 09 00 00 00       	jmp    0x179
 170:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
 173:	8b 4d f0             	mov    ecx,DWORD PTR [ebp-0x10]
 176:	89 41 6e             	mov    DWORD PTR [ecx+0x6e],eax
 179:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 17c:	8b 40 6a             	mov    eax,DWORD PTR [eax+0x6a]
 17f:	0f bf 00             	movsx  eax,WORD PTR [eax]
 182:	8b 4d f0             	mov    ecx,DWORD PTR [ebp-0x10]
 185:	3b 41 6e             	cmp    eax,DWORD PTR [ecx+0x6e]
 188:	0f 8d 0f 00 00 00    	jge    0x19d
 18e:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 191:	8b 40 6a             	mov    eax,DWORD PTR [eax+0x6a]
 194:	0f bf 00             	movsx  eax,WORD PTR [eax]
 197:	8b 4d f0             	mov    ecx,DWORD PTR [ebp-0x10]
 19a:	89 41 6e             	mov    DWORD PTR [ecx+0x6e],eax
 19d:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 1a0:	c7 40 62 00 00 00 00 	mov    DWORD PTR [eax+0x62],0x0
 1a7:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 1aa:	c7 40 66 00 00 00 00 	mov    DWORD PTR [eax+0x66],0x0
 1b1:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 1b4:	8b 40 6a             	mov    eax,DWORD PTR [eax+0x6a]
 1b7:	0f bf 00             	movsx  eax,WORD PTR [eax]
 1ba:	85 c0                	test   eax,eax
 1bc:	0f 85 35 00 00 00    	jne    0x1f7
 1c2:	6a 02                	push   0x2
 1c4:	68 02 78 00 00       	push   0x7802
 1c9:	6a 06                	push   0x6
 1cb:	68 00 02 00 00       	push   0x200
 1d0:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
 1d6:	e8 00 00 00 00       	call   0x1db
 1db:	68 08 40 00 00       	push   0x4008
 1e0:	68 02 78 00 00       	push   0x7802
 1e5:	6a 05                	push   0x5
 1e7:	68 00 02 00 00       	push   0x200
 1ec:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
 1f2:	e8 00 00 00 00       	call   0x1f7
 1f7:	a1 00 00 00 00       	mov    eax,ds:0x0
 1fc:	a3 00 00 00 00       	mov    ds:0x0,eax
 201:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
 207:	e8 00 00 00 00       	call   0x20c
 20c:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 20f:	c7 40 0c 00 40 00 00 	mov    DWORD PTR [eax+0xc],0x4000
 216:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 219:	8b 4d f0             	mov    ecx,DWORD PTR [ebp-0x10]
 21c:	89 41 10             	mov    DWORD PTR [ecx+0x10],eax
 21f:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 222:	c7 40 32 01 00 00 00 	mov    DWORD PTR [eax+0x32],0x1
 229:	68 00 00 00 00       	push   0x0
 22e:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 231:	83 c0 14             	add    eax,0x14
 234:	50                   	push   eax
 235:	e8 00 00 00 00       	call   0x23a
 23a:	83 c4 08             	add    esp,0x8
 23d:	33 c0                	xor    eax,eax
 23f:	e9 00 00 00 00       	jmp    0x244
 244:	5f                   	pop    edi
 245:	5e                   	pop    esi
 246:	5b                   	pop    ebx
 247:	c9                   	leave
 248:	c2 04 00             	ret    0x4
