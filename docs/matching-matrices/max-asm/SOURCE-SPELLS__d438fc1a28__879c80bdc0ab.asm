; ?ElementalStorm@combatManager@@QAEXXZ
; unit SOURCE/SPELLS  src_hash 879c80bdc0ab  score 100.0000  seed 777  trial 4
; size 763  text_sha 3a1fbf9c050584e4
; relocs ["0000000e:0006:?gbNoShowCombat@@3HA:00000000", "0000001a:0006:__h2cg$SOURCE$SPELLS$data$elementalStormStormIcn:00000000", "00000020:0006:?gpResourceManager@@3PAVresourceManager@@A:00000000", "00000025:0014:?GetIcon@resourceManager@@QAEPAVicon@@PAD@Z:00000000", "0000005f:0014:?KBTickCount@@YIJXZ:00000000", "0000006a:0006:?gConfig@@3UconfigStruct@@A:2e010000", "00000071:0006:?gfCombatSpeedMod@@3PAMA:00000000", "00000077:0006:__h2cg$SOURCE$SPELLS$data$aNIMATIONDELAYConstant2:00000000", "0000007e:0014:__ftol:00000000", "00000083:0006:?glTimers@@3PAHA:00000000", "00000099:0014:?DrawFrame@combatManager@@QAEXHHHHHHH@Z:00000000", "0000010c:0014:?CombatClipDrawToBuffer@icon@@QAEHHHHPAUSLimitData@@HHPAEPAC@Z:00000000", "0000011e:0014:?UpdateCombatArea@combatManager@@QAEXXZ:00000000", "00000123:0006:?glTimers@@3PAHA:00000000", "00000128:0014:?DelayTil@@YIXPAH@Z:00000000", "0000013c:0006:?gpResourceManager@@3PAVresourceManager@@A:00000000", "00000141:0014:?Dispose@resourceManager@@QAEXPAVresource@@@Z:00000000", "00000157:0014:?DrawFrame@combatManager@@QAEXHHHHHHH@Z:00000000", "000001f5:0014:?SpellCastWorks@army@@QAEHH@Z:00000000", "0000023c:0006:__h2cg$SOURCE$SPELLS$data$castSpellConstant:00000000", "00000241:0014:__ftol:00000000", "0000026c:0014:?HasArtifact@hero@@QAEHH@Z:00000000", "00000283:0006:__h2cg$SOURCE$SPELLS$data$castSpellConstant:00000000", "00000288:0014:__ftol:00000000", "00000299:0014:?Damage@army@@QAEHJH@Z:00000000", "000002bd:0006:__h2cg$SOURCE$SPELLS$data$elementalStormTheElementalStormDoesDDamage:00000000", "000002c2:0006:?gText@@3PADA:00000000", "000002c7:0014:_sprintf:00000000", "000002d5:0006:?gText@@3PADA:00000000", "000002dd:0014:?CombatMessage@combatManager@@QAEXPADHHH@Z:00000000", "000002ed:0014:?PowEffect@army@@QAEXHHHH@Z:00000000"]
; hex 558bec83ec4c535657894dc0833d00000000000f852c01000068000000008b0d00000000e8000000008945d4c745e400000000e903000000ff45e4837de4060f8df1000000c745e800000000e903000000ff45e8837de80a0f8dd3000000e8000000008945bcdb45bca12e010000d9048500000000d80d00000000dec1e800000000a3000000006a016a016a4b6a006a006a006a008b4dc0e800000000c745ec00000000e903000000ff45ec837dec0a0f8d64000000c745f000000000e903000000ff45f0837df00c0f8d460000006a006a006a006a008d45c4508b45f08d04400345e80345ecb90a00000099f7f9528b45ec8d044003c08d04c0508b45f08d044003c08d04c0508b4dd4e800000000e9adffffffe98fffffff8b4dc0e800000000b900000000e800000000e920ffffffe902ffffff8b45d4508b0d00000000e8000000006a016a016a4b6a006a006a006a018b4dc0e800000000c745e0000000008b45c08b80aff200008b4dc08b8481893300008d04808d04808945fcc745dc00000000e903000000ff45dc837ddc020f8d17010000c745f800000000e903000000ff45f88b45dc8b4dc08b55f8399481473500000f8eed0000008b45dc8bc8c1e0062bc18d04c1c1e0032bc18d04408b4df88bd1c1e1038d0cc98d0cca03c98d04410345c0054f3500008945f46a1c8b4df4e80000000085c00f84a30000008b45fc8945d88b45f48378763f0f8503000000d165d88b45f4837876280f840d0000008b45f4837876290f85170000008b45d88945b8db45b8dc0d00000000e8000000008945d88b45dc8b4dc083bc8187320000000f84330000006a598b45dc8b4dc08b8c8187320000e80000000085c00f84170000008b45d88945b4db45b4dc0d00000000e8000000008945d86a1c8b45d8508b4df4e800000000c745e001000000e9fafeffffe9dcfeffff837de0000f84390000008b45fc5068000000006800000000e80000000083c40c6a006a016a0168000000008b4dc0e8000000006aff6aff6a016aff8b4df4e800000000e9000000005f5e5bc9c3

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 4c             	sub    esp,0x4c
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 4d c0             	mov    DWORD PTR [ebp-0x40],ecx
   c:	83 3d 00 00 00 00 00 	cmp    DWORD PTR ds:0x0,0x0
  13:	0f 85 2c 01 00 00    	jne    0x145
  19:	68 00 00 00 00       	push   0x0
  1e:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
  24:	e8 00 00 00 00       	call   0x29
  29:	89 45 d4             	mov    DWORD PTR [ebp-0x2c],eax
  2c:	c7 45 e4 00 00 00 00 	mov    DWORD PTR [ebp-0x1c],0x0
  33:	e9 03 00 00 00       	jmp    0x3b
  38:	ff 45 e4             	inc    DWORD PTR [ebp-0x1c]
  3b:	83 7d e4 06          	cmp    DWORD PTR [ebp-0x1c],0x6
  3f:	0f 8d f1 00 00 00    	jge    0x136
  45:	c7 45 e8 00 00 00 00 	mov    DWORD PTR [ebp-0x18],0x0
  4c:	e9 03 00 00 00       	jmp    0x54
  51:	ff 45 e8             	inc    DWORD PTR [ebp-0x18]
  54:	83 7d e8 0a          	cmp    DWORD PTR [ebp-0x18],0xa
  58:	0f 8d d3 00 00 00    	jge    0x131
  5e:	e8 00 00 00 00       	call   0x63
  63:	89 45 bc             	mov    DWORD PTR [ebp-0x44],eax
  66:	db 45 bc             	fild   DWORD PTR [ebp-0x44]
  69:	a1 2e 01 00 00       	mov    eax,ds:0x12e
  6e:	d9 04 85 00 00 00 00 	fld    DWORD PTR [eax*4+0x0]
  75:	d8 0d 00 00 00 00    	fmul   DWORD PTR ds:0x0
  7b:	de c1                	faddp  st(1),st
  7d:	e8 00 00 00 00       	call   0x82
  82:	a3 00 00 00 00       	mov    ds:0x0,eax
  87:	6a 01                	push   0x1
  89:	6a 01                	push   0x1
  8b:	6a 4b                	push   0x4b
  8d:	6a 00                	push   0x0
  8f:	6a 00                	push   0x0
  91:	6a 00                	push   0x0
  93:	6a 00                	push   0x0
  95:	8b 4d c0             	mov    ecx,DWORD PTR [ebp-0x40]
  98:	e8 00 00 00 00       	call   0x9d
  9d:	c7 45 ec 00 00 00 00 	mov    DWORD PTR [ebp-0x14],0x0
  a4:	e9 03 00 00 00       	jmp    0xac
  a9:	ff 45 ec             	inc    DWORD PTR [ebp-0x14]
  ac:	83 7d ec 0a          	cmp    DWORD PTR [ebp-0x14],0xa
  b0:	0f 8d 64 00 00 00    	jge    0x11a
  b6:	c7 45 f0 00 00 00 00 	mov    DWORD PTR [ebp-0x10],0x0
  bd:	e9 03 00 00 00       	jmp    0xc5
  c2:	ff 45 f0             	inc    DWORD PTR [ebp-0x10]
  c5:	83 7d f0 0c          	cmp    DWORD PTR [ebp-0x10],0xc
  c9:	0f 8d 46 00 00 00    	jge    0x115
  cf:	6a 00                	push   0x0
  d1:	6a 00                	push   0x0
  d3:	6a 00                	push   0x0
  d5:	6a 00                	push   0x0
  d7:	8d 45 c4             	lea    eax,[ebp-0x3c]
  da:	50                   	push   eax
  db:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  de:	8d 04 40             	lea    eax,[eax+eax*2]
  e1:	03 45 e8             	add    eax,DWORD PTR [ebp-0x18]
  e4:	03 45 ec             	add    eax,DWORD PTR [ebp-0x14]
  e7:	b9 0a 00 00 00       	mov    ecx,0xa
  ec:	99                   	cdq
  ed:	f7 f9                	idiv   ecx
  ef:	52                   	push   edx
  f0:	8b 45 ec             	mov    eax,DWORD PTR [ebp-0x14]
  f3:	8d 04 40             	lea    eax,[eax+eax*2]
  f6:	03 c0                	add    eax,eax
  f8:	8d 04 c0             	lea    eax,[eax+eax*8]
  fb:	50                   	push   eax
  fc:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  ff:	8d 04 40             	lea    eax,[eax+eax*2]
 102:	03 c0                	add    eax,eax
 104:	8d 04 c0             	lea    eax,[eax+eax*8]
 107:	50                   	push   eax
 108:	8b 4d d4             	mov    ecx,DWORD PTR [ebp-0x2c]
 10b:	e8 00 00 00 00       	call   0x110
 110:	e9 ad ff ff ff       	jmp    0xc2
 115:	e9 8f ff ff ff       	jmp    0xa9
 11a:	8b 4d c0             	mov    ecx,DWORD PTR [ebp-0x40]
 11d:	e8 00 00 00 00       	call   0x122
 122:	b9 00 00 00 00       	mov    ecx,0x0
 127:	e8 00 00 00 00       	call   0x12c
 12c:	e9 20 ff ff ff       	jmp    0x51
 131:	e9 02 ff ff ff       	jmp    0x38
 136:	8b 45 d4             	mov    eax,DWORD PTR [ebp-0x2c]
 139:	50                   	push   eax
 13a:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
 140:	e8 00 00 00 00       	call   0x145
 145:	6a 01                	push   0x1
 147:	6a 01                	push   0x1
 149:	6a 4b                	push   0x4b
 14b:	6a 00                	push   0x0
 14d:	6a 00                	push   0x0
 14f:	6a 00                	push   0x0
 151:	6a 01                	push   0x1
 153:	8b 4d c0             	mov    ecx,DWORD PTR [ebp-0x40]
 156:	e8 00 00 00 00       	call   0x15b
 15b:	c7 45 e0 00 00 00 00 	mov    DWORD PTR [ebp-0x20],0x0
 162:	8b 45 c0             	mov    eax,DWORD PTR [ebp-0x40]
 165:	8b 80 af f2 00 00    	mov    eax,DWORD PTR [eax+0xf2af]
 16b:	8b 4d c0             	mov    ecx,DWORD PTR [ebp-0x40]
 16e:	8b 84 81 89 33 00 00 	mov    eax,DWORD PTR [ecx+eax*4+0x3389]
 175:	8d 04 80             	lea    eax,[eax+eax*4]
 178:	8d 04 80             	lea    eax,[eax+eax*4]
 17b:	89 45 fc             	mov    DWORD PTR [ebp-0x4],eax
 17e:	c7 45 dc 00 00 00 00 	mov    DWORD PTR [ebp-0x24],0x0
 185:	e9 03 00 00 00       	jmp    0x18d
 18a:	ff 45 dc             	inc    DWORD PTR [ebp-0x24]
 18d:	83 7d dc 02          	cmp    DWORD PTR [ebp-0x24],0x2
 191:	0f 8d 17 01 00 00    	jge    0x2ae
 197:	c7 45 f8 00 00 00 00 	mov    DWORD PTR [ebp-0x8],0x0
 19e:	e9 03 00 00 00       	jmp    0x1a6
 1a3:	ff 45 f8             	inc    DWORD PTR [ebp-0x8]
 1a6:	8b 45 dc             	mov    eax,DWORD PTR [ebp-0x24]
 1a9:	8b 4d c0             	mov    ecx,DWORD PTR [ebp-0x40]
 1ac:	8b 55 f8             	mov    edx,DWORD PTR [ebp-0x8]
 1af:	39 94 81 47 35 00 00 	cmp    DWORD PTR [ecx+eax*4+0x3547],edx
 1b6:	0f 8e ed 00 00 00    	jle    0x2a9
 1bc:	8b 45 dc             	mov    eax,DWORD PTR [ebp-0x24]
 1bf:	8b c8                	mov    ecx,eax
 1c1:	c1 e0 06             	shl    eax,0x6
 1c4:	2b c1                	sub    eax,ecx
 1c6:	8d 04 c1             	lea    eax,[ecx+eax*8]
 1c9:	c1 e0 03             	shl    eax,0x3
 1cc:	2b c1                	sub    eax,ecx
 1ce:	8d 04 40             	lea    eax,[eax+eax*2]
 1d1:	8b 4d f8             	mov    ecx,DWORD PTR [ebp-0x8]
 1d4:	8b d1                	mov    edx,ecx
 1d6:	c1 e1 03             	shl    ecx,0x3
 1d9:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
 1dc:	8d 0c ca             	lea    ecx,[edx+ecx*8]
 1df:	03 c9                	add    ecx,ecx
 1e1:	8d 04 41             	lea    eax,[ecx+eax*2]
 1e4:	03 45 c0             	add    eax,DWORD PTR [ebp-0x40]
 1e7:	05 4f 35 00 00       	add    eax,0x354f
 1ec:	89 45 f4             	mov    DWORD PTR [ebp-0xc],eax
 1ef:	6a 1c                	push   0x1c
 1f1:	8b 4d f4             	mov    ecx,DWORD PTR [ebp-0xc]
 1f4:	e8 00 00 00 00       	call   0x1f9
 1f9:	85 c0                	test   eax,eax
 1fb:	0f 84 a3 00 00 00    	je     0x2a4
 201:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
 204:	89 45 d8             	mov    DWORD PTR [ebp-0x28],eax
 207:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
 20a:	83 78 76 3f          	cmp    DWORD PTR [eax+0x76],0x3f
 20e:	0f 85 03 00 00 00    	jne    0x217
 214:	d1 65 d8             	shl    DWORD PTR [ebp-0x28],1
 217:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
 21a:	83 78 76 28          	cmp    DWORD PTR [eax+0x76],0x28
 21e:	0f 84 0d 00 00 00    	je     0x231
 224:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
 227:	83 78 76 29          	cmp    DWORD PTR [eax+0x76],0x29
 22b:	0f 85 17 00 00 00    	jne    0x248
 231:	8b 45 d8             	mov    eax,DWORD PTR [ebp-0x28]
 234:	89 45 b8             	mov    DWORD PTR [ebp-0x48],eax
 237:	db 45 b8             	fild   DWORD PTR [ebp-0x48]
 23a:	dc 0d 00 00 00 00    	fmul   QWORD PTR ds:0x0
 240:	e8 00 00 00 00       	call   0x245
 245:	89 45 d8             	mov    DWORD PTR [ebp-0x28],eax
 248:	8b 45 dc             	mov    eax,DWORD PTR [ebp-0x24]
 24b:	8b 4d c0             	mov    ecx,DWORD PTR [ebp-0x40]
 24e:	83 bc 81 87 32 00 00 	cmp    DWORD PTR [ecx+eax*4+0x3287],0x0
 255:	00
 256:	0f 84 33 00 00 00    	je     0x28f
 25c:	6a 59                	push   0x59
 25e:	8b 45 dc             	mov    eax,DWORD PTR [ebp-0x24]
 261:	8b 4d c0             	mov    ecx,DWORD PTR [ebp-0x40]
 264:	8b 8c 81 87 32 00 00 	mov    ecx,DWORD PTR [ecx+eax*4+0x3287]
 26b:	e8 00 00 00 00       	call   0x270
 270:	85 c0                	test   eax,eax
 272:	0f 84 17 00 00 00    	je     0x28f
 278:	8b 45 d8             	mov    eax,DWORD PTR [ebp-0x28]
 27b:	89 45 b4             	mov    DWORD PTR [ebp-0x4c],eax
 27e:	db 45 b4             	fild   DWORD PTR [ebp-0x4c]
 281:	dc 0d 00 00 00 00    	fmul   QWORD PTR ds:0x0
 287:	e8 00 00 00 00       	call   0x28c
 28c:	89 45 d8             	mov    DWORD PTR [ebp-0x28],eax
 28f:	6a 1c                	push   0x1c
 291:	8b 45 d8             	mov    eax,DWORD PTR [ebp-0x28]
 294:	50                   	push   eax
 295:	8b 4d f4             	mov    ecx,DWORD PTR [ebp-0xc]
 298:	e8 00 00 00 00       	call   0x29d
 29d:	c7 45 e0 01 00 00 00 	mov    DWORD PTR [ebp-0x20],0x1
 2a4:	e9 fa fe ff ff       	jmp    0x1a3
 2a9:	e9 dc fe ff ff       	jmp    0x18a
 2ae:	83 7d e0 00          	cmp    DWORD PTR [ebp-0x20],0x0
 2b2:	0f 84 39 00 00 00    	je     0x2f1
 2b8:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
 2bb:	50                   	push   eax
 2bc:	68 00 00 00 00       	push   0x0
 2c1:	68 00 00 00 00       	push   0x0
 2c6:	e8 00 00 00 00       	call   0x2cb
 2cb:	83 c4 0c             	add    esp,0xc
 2ce:	6a 00                	push   0x0
 2d0:	6a 01                	push   0x1
 2d2:	6a 01                	push   0x1
 2d4:	68 00 00 00 00       	push   0x0
 2d9:	8b 4d c0             	mov    ecx,DWORD PTR [ebp-0x40]
 2dc:	e8 00 00 00 00       	call   0x2e1
 2e1:	6a ff                	push   0xffffffff
 2e3:	6a ff                	push   0xffffffff
 2e5:	6a 01                	push   0x1
 2e7:	6a ff                	push   0xffffffff
 2e9:	8b 4d f4             	mov    ecx,DWORD PTR [ebp-0xc]
 2ec:	e8 00 00 00 00       	call   0x2f1
 2f1:	e9 00 00 00 00       	jmp    0x2f6
 2f6:	5f                   	pop    edi
 2f7:	5e                   	pop    esi
 2f8:	5b                   	pop    ebx
 2f9:	c9                   	leave
 2fa:	c3                   	ret
