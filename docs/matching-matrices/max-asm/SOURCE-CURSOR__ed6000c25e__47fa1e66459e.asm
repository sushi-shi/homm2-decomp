; ?DrawCursorShadow@advManager@@QAEXXZ
; unit SOURCE/CURSOR  src_hash 47fa1e66459e  score 100.0000  seed 20260724  trial 1
; size 876  text_sha 6ba74f32b733d17d
; relocs ["0000000e:0006:?bShowIt@@3HA:00000000", "0000001b:0006:?bSpecialHideCursor@@3HA:00000000", "0000002d:0006:?gbDrawSavedCursor@@3HA:00000000", "00000039:0006:?S1cursorDirection@@3HA:00000000", "00000047:0006:?S1cursorBaseFrame@@3HA:00000000", "00000055:0006:?S1cursorFrameCount@@3HA:00000000", "00000063:0006:?S1cursorCycle@@3HA:00000000", "00000071:0006:?S1cursorTurning@@3HA:00000000", "00000152:0006:?gpWindowManager@@3PAVheroWindowManager@@A:00000000", "00000163:0014:?IconToBitmap@@YIXPAVicon@@PAVbitmap@@HHHHHHHHH@Z:00000000", "00000233:0006:?gpWindowManager@@3PAVheroWindowManager@@A:00000000", "00000244:0014:?IconToBitmap@@YIXPAVicon@@PAVbitmap@@HHHHHHHHH@Z:00000000", "000002a1:0006:?gpWindowManager@@3PAVheroWindowManager@@A:00000000", "000002b2:0014:?IconToBitmap@@YIXPAVicon@@PAVbitmap@@HHHHHHHHH@Z:00000000", "000002fa:0006:?gpWindowManager@@3PAVheroWindowManager@@A:00000000", "0000030b:0014:?IconToBitmap@@YIXPAVicon@@PAVbitmap@@HHHHHHHHH@Z:00000000", "00000311:0006:?gbDrawSavedCursor@@3HA:00000000", "00000326:0006:?S1cursorDirection@@3HA:00000000", "00000334:0006:?S1cursorBaseFrame@@3HA:00000000", "00000342:0006:?S1cursorFrameCount@@3HA:00000000", "00000350:0006:?S1cursorCycle@@3HA:00000000", "0000035e:0006:?S1cursorTurning@@3HA:00000000"]
; hex 558bec83ec20535657894de0833d00000000000f840d000000833d00000000000f8405000000e93c030000833d00000000000f8446000000a1000000008b4de089817e020000a1000000008b4de0898182020000a1000000008b4de0898186020000a1000000008b4de089818a020000a1000000008b4de089818e0200008b45e08b80f601000005e00000008945fc8b45e08b80fa01000005ff0000008945f48b45e083b87a020000060f8504000000836df40a8b45e0f68082020000800f84890100008345fc208b45e08b808202000083e07f8b4de00381860200008945f88b45e083b876020000000f847c0000008b45e083b87a020000060f856c0000008b45f88945f0837df0090f8c16000000837df0240f8d0c000000c745e824000000e907000000c745e8000000006a0068e001000068e00100006a006a006a018b45f00345e8508b45f4508b45fc83e82050a1000000008b4de08b50468b893e020000e800000000e9dc0000008b45e083b876020000000f84cc0000008b45e083b87a020000060f84bc0000008b45f88945ec837dec330f8507000000c745ec38000000837dec320f8507000000c745ec39000000837dec310f8507000000c745ec3a000000837dec2f0f8507000000c745ec37000000837dec2e0f8507000000c745ec37000000837dec090f8c16000000837dec240f8d0c000000c745e432000000e907000000c745e4000000006a0068e001000068e00100006a006a006a018b45ec0345e4508b45f4508b45fc83e82050a1000000008b4de08b50468b893a020000e800000000e9c20000008b45e08b80820200008b4de00381860200008945f88b45e083b876020000000f84490000008b45e083b87a020000060f85390000006a0068e001000068e00100006a006a006a018b45f8508b45f4508b45fc50a1000000008b4de08b50468b893e020000e800000000e9540000008b45e083b876020000000f84440000008b45e083b87a020000060f84340000006a0068e001000068e00100006a006a006a018b45f8508b45f4508b45fc50a1000000008b4de08b50468b893a020000e800000000833d00000000000f85460000008b45e08b807e020000a3000000008b45e08b8082020000a3000000008b45e08b8086020000a3000000008b45e08b808a020000a3000000008b45e08b808e020000a300000000e9000000005f5e5bc9c3

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 20             	sub    esp,0x20
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 4d e0             	mov    DWORD PTR [ebp-0x20],ecx
   c:	83 3d 00 00 00 00 00 	cmp    DWORD PTR ds:0x0,0x0
  13:	0f 84 0d 00 00 00    	je     0x26
  19:	83 3d 00 00 00 00 00 	cmp    DWORD PTR ds:0x0,0x0
  20:	0f 84 05 00 00 00    	je     0x2b
  26:	e9 3c 03 00 00       	jmp    0x367
  2b:	83 3d 00 00 00 00 00 	cmp    DWORD PTR ds:0x0,0x0
  32:	0f 84 46 00 00 00    	je     0x7e
  38:	a1 00 00 00 00       	mov    eax,ds:0x0
  3d:	8b 4d e0             	mov    ecx,DWORD PTR [ebp-0x20]
  40:	89 81 7e 02 00 00    	mov    DWORD PTR [ecx+0x27e],eax
  46:	a1 00 00 00 00       	mov    eax,ds:0x0
  4b:	8b 4d e0             	mov    ecx,DWORD PTR [ebp-0x20]
  4e:	89 81 82 02 00 00    	mov    DWORD PTR [ecx+0x282],eax
  54:	a1 00 00 00 00       	mov    eax,ds:0x0
  59:	8b 4d e0             	mov    ecx,DWORD PTR [ebp-0x20]
  5c:	89 81 86 02 00 00    	mov    DWORD PTR [ecx+0x286],eax
  62:	a1 00 00 00 00       	mov    eax,ds:0x0
  67:	8b 4d e0             	mov    ecx,DWORD PTR [ebp-0x20]
  6a:	89 81 8a 02 00 00    	mov    DWORD PTR [ecx+0x28a],eax
  70:	a1 00 00 00 00       	mov    eax,ds:0x0
  75:	8b 4d e0             	mov    ecx,DWORD PTR [ebp-0x20]
  78:	89 81 8e 02 00 00    	mov    DWORD PTR [ecx+0x28e],eax
  7e:	8b 45 e0             	mov    eax,DWORD PTR [ebp-0x20]
  81:	8b 80 f6 01 00 00    	mov    eax,DWORD PTR [eax+0x1f6]
  87:	05 e0 00 00 00       	add    eax,0xe0
  8c:	89 45 fc             	mov    DWORD PTR [ebp-0x4],eax
  8f:	8b 45 e0             	mov    eax,DWORD PTR [ebp-0x20]
  92:	8b 80 fa 01 00 00    	mov    eax,DWORD PTR [eax+0x1fa]
  98:	05 ff 00 00 00       	add    eax,0xff
  9d:	89 45 f4             	mov    DWORD PTR [ebp-0xc],eax
  a0:	8b 45 e0             	mov    eax,DWORD PTR [ebp-0x20]
  a3:	83 b8 7a 02 00 00 06 	cmp    DWORD PTR [eax+0x27a],0x6
  aa:	0f 85 04 00 00 00    	jne    0xb4
  b0:	83 6d f4 0a          	sub    DWORD PTR [ebp-0xc],0xa
  b4:	8b 45 e0             	mov    eax,DWORD PTR [ebp-0x20]
  b7:	f6 80 82 02 00 00 80 	test   BYTE PTR [eax+0x282],0x80
  be:	0f 84 89 01 00 00    	je     0x24d
  c4:	83 45 fc 20          	add    DWORD PTR [ebp-0x4],0x20
  c8:	8b 45 e0             	mov    eax,DWORD PTR [ebp-0x20]
  cb:	8b 80 82 02 00 00    	mov    eax,DWORD PTR [eax+0x282]
  d1:	83 e0 7f             	and    eax,0x7f
  d4:	8b 4d e0             	mov    ecx,DWORD PTR [ebp-0x20]
  d7:	03 81 86 02 00 00    	add    eax,DWORD PTR [ecx+0x286]
  dd:	89 45 f8             	mov    DWORD PTR [ebp-0x8],eax
  e0:	8b 45 e0             	mov    eax,DWORD PTR [ebp-0x20]
  e3:	83 b8 76 02 00 00 00 	cmp    DWORD PTR [eax+0x276],0x0
  ea:	0f 84 7c 00 00 00    	je     0x16c
  f0:	8b 45 e0             	mov    eax,DWORD PTR [ebp-0x20]
  f3:	83 b8 7a 02 00 00 06 	cmp    DWORD PTR [eax+0x27a],0x6
  fa:	0f 85 6c 00 00 00    	jne    0x16c
 100:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
 103:	89 45 f0             	mov    DWORD PTR [ebp-0x10],eax
 106:	83 7d f0 09          	cmp    DWORD PTR [ebp-0x10],0x9
 10a:	0f 8c 16 00 00 00    	jl     0x126
 110:	83 7d f0 24          	cmp    DWORD PTR [ebp-0x10],0x24
 114:	0f 8d 0c 00 00 00    	jge    0x126
 11a:	c7 45 e8 24 00 00 00 	mov    DWORD PTR [ebp-0x18],0x24
 121:	e9 07 00 00 00       	jmp    0x12d
 126:	c7 45 e8 00 00 00 00 	mov    DWORD PTR [ebp-0x18],0x0
 12d:	6a 00                	push   0x0
 12f:	68 e0 01 00 00       	push   0x1e0
 134:	68 e0 01 00 00       	push   0x1e0
 139:	6a 00                	push   0x0
 13b:	6a 00                	push   0x0
 13d:	6a 01                	push   0x1
 13f:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 142:	03 45 e8             	add    eax,DWORD PTR [ebp-0x18]
 145:	50                   	push   eax
 146:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
 149:	50                   	push   eax
 14a:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
 14d:	83 e8 20             	sub    eax,0x20
 150:	50                   	push   eax
 151:	a1 00 00 00 00       	mov    eax,ds:0x0
 156:	8b 4d e0             	mov    ecx,DWORD PTR [ebp-0x20]
 159:	8b 50 46             	mov    edx,DWORD PTR [eax+0x46]
 15c:	8b 89 3e 02 00 00    	mov    ecx,DWORD PTR [ecx+0x23e]
 162:	e8 00 00 00 00       	call   0x167
 167:	e9 dc 00 00 00       	jmp    0x248
 16c:	8b 45 e0             	mov    eax,DWORD PTR [ebp-0x20]
 16f:	83 b8 76 02 00 00 00 	cmp    DWORD PTR [eax+0x276],0x0
 176:	0f 84 cc 00 00 00    	je     0x248
 17c:	8b 45 e0             	mov    eax,DWORD PTR [ebp-0x20]
 17f:	83 b8 7a 02 00 00 06 	cmp    DWORD PTR [eax+0x27a],0x6
 186:	0f 84 bc 00 00 00    	je     0x248
 18c:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
 18f:	89 45 ec             	mov    DWORD PTR [ebp-0x14],eax
 192:	83 7d ec 33          	cmp    DWORD PTR [ebp-0x14],0x33
 196:	0f 85 07 00 00 00    	jne    0x1a3
 19c:	c7 45 ec 38 00 00 00 	mov    DWORD PTR [ebp-0x14],0x38
 1a3:	83 7d ec 32          	cmp    DWORD PTR [ebp-0x14],0x32
 1a7:	0f 85 07 00 00 00    	jne    0x1b4
 1ad:	c7 45 ec 39 00 00 00 	mov    DWORD PTR [ebp-0x14],0x39
 1b4:	83 7d ec 31          	cmp    DWORD PTR [ebp-0x14],0x31
 1b8:	0f 85 07 00 00 00    	jne    0x1c5
 1be:	c7 45 ec 3a 00 00 00 	mov    DWORD PTR [ebp-0x14],0x3a
 1c5:	83 7d ec 2f          	cmp    DWORD PTR [ebp-0x14],0x2f
 1c9:	0f 85 07 00 00 00    	jne    0x1d6
 1cf:	c7 45 ec 37 00 00 00 	mov    DWORD PTR [ebp-0x14],0x37
 1d6:	83 7d ec 2e          	cmp    DWORD PTR [ebp-0x14],0x2e
 1da:	0f 85 07 00 00 00    	jne    0x1e7
 1e0:	c7 45 ec 37 00 00 00 	mov    DWORD PTR [ebp-0x14],0x37
 1e7:	83 7d ec 09          	cmp    DWORD PTR [ebp-0x14],0x9
 1eb:	0f 8c 16 00 00 00    	jl     0x207
 1f1:	83 7d ec 24          	cmp    DWORD PTR [ebp-0x14],0x24
 1f5:	0f 8d 0c 00 00 00    	jge    0x207
 1fb:	c7 45 e4 32 00 00 00 	mov    DWORD PTR [ebp-0x1c],0x32
 202:	e9 07 00 00 00       	jmp    0x20e
 207:	c7 45 e4 00 00 00 00 	mov    DWORD PTR [ebp-0x1c],0x0
 20e:	6a 00                	push   0x0
 210:	68 e0 01 00 00       	push   0x1e0
 215:	68 e0 01 00 00       	push   0x1e0
 21a:	6a 00                	push   0x0
 21c:	6a 00                	push   0x0
 21e:	6a 01                	push   0x1
 220:	8b 45 ec             	mov    eax,DWORD PTR [ebp-0x14]
 223:	03 45 e4             	add    eax,DWORD PTR [ebp-0x1c]
 226:	50                   	push   eax
 227:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
 22a:	50                   	push   eax
 22b:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
 22e:	83 e8 20             	sub    eax,0x20
 231:	50                   	push   eax
 232:	a1 00 00 00 00       	mov    eax,ds:0x0
 237:	8b 4d e0             	mov    ecx,DWORD PTR [ebp-0x20]
 23a:	8b 50 46             	mov    edx,DWORD PTR [eax+0x46]
 23d:	8b 89 3a 02 00 00    	mov    ecx,DWORD PTR [ecx+0x23a]
 243:	e8 00 00 00 00       	call   0x248
 248:	e9 c2 00 00 00       	jmp    0x30f
 24d:	8b 45 e0             	mov    eax,DWORD PTR [ebp-0x20]
 250:	8b 80 82 02 00 00    	mov    eax,DWORD PTR [eax+0x282]
 256:	8b 4d e0             	mov    ecx,DWORD PTR [ebp-0x20]
 259:	03 81 86 02 00 00    	add    eax,DWORD PTR [ecx+0x286]
 25f:	89 45 f8             	mov    DWORD PTR [ebp-0x8],eax
 262:	8b 45 e0             	mov    eax,DWORD PTR [ebp-0x20]
 265:	83 b8 76 02 00 00 00 	cmp    DWORD PTR [eax+0x276],0x0
 26c:	0f 84 49 00 00 00    	je     0x2bb
 272:	8b 45 e0             	mov    eax,DWORD PTR [ebp-0x20]
 275:	83 b8 7a 02 00 00 06 	cmp    DWORD PTR [eax+0x27a],0x6
 27c:	0f 85 39 00 00 00    	jne    0x2bb
 282:	6a 00                	push   0x0
 284:	68 e0 01 00 00       	push   0x1e0
 289:	68 e0 01 00 00       	push   0x1e0
 28e:	6a 00                	push   0x0
 290:	6a 00                	push   0x0
 292:	6a 01                	push   0x1
 294:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
 297:	50                   	push   eax
 298:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
 29b:	50                   	push   eax
 29c:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
 29f:	50                   	push   eax
 2a0:	a1 00 00 00 00       	mov    eax,ds:0x0
 2a5:	8b 4d e0             	mov    ecx,DWORD PTR [ebp-0x20]
 2a8:	8b 50 46             	mov    edx,DWORD PTR [eax+0x46]
 2ab:	8b 89 3e 02 00 00    	mov    ecx,DWORD PTR [ecx+0x23e]
 2b1:	e8 00 00 00 00       	call   0x2b6
 2b6:	e9 54 00 00 00       	jmp    0x30f
 2bb:	8b 45 e0             	mov    eax,DWORD PTR [ebp-0x20]
 2be:	83 b8 76 02 00 00 00 	cmp    DWORD PTR [eax+0x276],0x0
 2c5:	0f 84 44 00 00 00    	je     0x30f
 2cb:	8b 45 e0             	mov    eax,DWORD PTR [ebp-0x20]
 2ce:	83 b8 7a 02 00 00 06 	cmp    DWORD PTR [eax+0x27a],0x6
 2d5:	0f 84 34 00 00 00    	je     0x30f
 2db:	6a 00                	push   0x0
 2dd:	68 e0 01 00 00       	push   0x1e0
 2e2:	68 e0 01 00 00       	push   0x1e0
 2e7:	6a 00                	push   0x0
 2e9:	6a 00                	push   0x0
 2eb:	6a 01                	push   0x1
 2ed:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
 2f0:	50                   	push   eax
 2f1:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
 2f4:	50                   	push   eax
 2f5:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
 2f8:	50                   	push   eax
 2f9:	a1 00 00 00 00       	mov    eax,ds:0x0
 2fe:	8b 4d e0             	mov    ecx,DWORD PTR [ebp-0x20]
 301:	8b 50 46             	mov    edx,DWORD PTR [eax+0x46]
 304:	8b 89 3a 02 00 00    	mov    ecx,DWORD PTR [ecx+0x23a]
 30a:	e8 00 00 00 00       	call   0x30f
 30f:	83 3d 00 00 00 00 00 	cmp    DWORD PTR ds:0x0,0x0
 316:	0f 85 46 00 00 00    	jne    0x362
 31c:	8b 45 e0             	mov    eax,DWORD PTR [ebp-0x20]
 31f:	8b 80 7e 02 00 00    	mov    eax,DWORD PTR [eax+0x27e]
 325:	a3 00 00 00 00       	mov    ds:0x0,eax
 32a:	8b 45 e0             	mov    eax,DWORD PTR [ebp-0x20]
 32d:	8b 80 82 02 00 00    	mov    eax,DWORD PTR [eax+0x282]
 333:	a3 00 00 00 00       	mov    ds:0x0,eax
 338:	8b 45 e0             	mov    eax,DWORD PTR [ebp-0x20]
 33b:	8b 80 86 02 00 00    	mov    eax,DWORD PTR [eax+0x286]
 341:	a3 00 00 00 00       	mov    ds:0x0,eax
 346:	8b 45 e0             	mov    eax,DWORD PTR [ebp-0x20]
 349:	8b 80 8a 02 00 00    	mov    eax,DWORD PTR [eax+0x28a]
 34f:	a3 00 00 00 00       	mov    ds:0x0,eax
 354:	8b 45 e0             	mov    eax,DWORD PTR [ebp-0x20]
 357:	8b 80 8e 02 00 00    	mov    eax,DWORD PTR [eax+0x28e]
 35d:	a3 00 00 00 00       	mov    ds:0x0,eax
 362:	e9 00 00 00 00       	jmp    0x367
 367:	5f                   	pop    edi
 368:	5e                   	pop    esi
 369:	5b                   	pop    ebx
 36a:	c9                   	leave
 36b:	c3                   	ret
