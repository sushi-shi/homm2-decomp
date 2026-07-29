; ?CheckForCreatureUpgrades@philAI@@QAEXXZ
; unit SOURCE/PHILAI  src_hash 3cc449312877  score 99.5516  seed 1414  trial 2
; size 899  text_sha 0f8277abf23e4850
; relocs ["00000023:0006:?gpCurPlayer@@3PAVplayerData@@A:00000000", "00000043:0006:?gpCurPlayer@@3PAVplayerData@@A:00000000", "00000057:0006:?gpGame@@3PAVgame@@A:00000000", "000000c2:0006:?gpGame@@3PAVgame@@A:00000000", "00000130:0006:?gDwellingType@@3PAY0M@CA:edffffff", "000001d3:0006:?gMonsterDatabase@@3PAUtag_monsterInfo@@A:00000000", "000001ed:0006:?gMonsterDatabase@@3PAUtag_monsterInfo@@A:00000000", "00000261:0006:?gpCurPlayer@@3PAVplayerData@@A:00000000", "00000283:0006:?gpCurPlayer@@3PAVplayerData@@A:00000000", "000002a0:0006:?gpCurPlayer@@3PAVplayerData@@A:00000000", "000002c0:0006:?gpCurPlayer@@3PAVplayerData@@A:00000000"]
; hex 558bec83ec34535657894dccc745e8ffffffffc745f800000000e903000000ff45f8a1000000000fbe40443b45f80f8e45030000e900000000e9000000008b45f88b0d000000000fbe440847c1e0028d04808d048003050000000005530b00008945dcc745f400000000e903000000ff45f4837df4020f8df8020000837df4000f850e0000008b45dc83c0088945fce93a0000008b45dc0fbe401783f8ff0f8505000000e9c6ffffffe9000000008b45dc0fbe40178d04808d04808d048003c003050000000005292800008945fcc745f000000000e903000000ff45f0837df0050f8d880200008b45f08b4dfc0fbe040883f8ff0f8505000000e9dbffffffc745e000000000c745ec14000000e903000000ff45ec837dec180f8f5a0000008b45dc0fbe40038d04408b4dec0fbe8481edffffff8b4df08b55fc0fbe0c113bc10f852e000000b8010000008b4dec83c105d3e08b4ddc8541180f8415000000c745e0010000008b45f08b4dfc0fbe0408408945e8e999ffffff8b45f08b4dfc0fbe040883f8230f84130000008b45f08b4dfc0fbe040883f8240f851b0000008b45dcf6401b400f840e000000c745e001000000c745e825000000837de0000f84a60100008b45e88bc88d04808d04800fbf8408000000008b4df08b55fc0fbe0c118bd18d0c898d0c890fbf8c11000000002bc18b4df08b55fc0fbf4c4a050fafc103c08945d8837de8250f851c000000c745d4030000008b45f08b4dfc0fbf44410503c08945d0e934000000837de82e0f851c000000c745d4050000008b45f08b4dfc0fbf44410503c08945d0e90e000000c745d4ffffffffc745d000000000a1000000008b4dd83988a70000000f8cf6000000837dd4ff0f84190000008b45d48b0d000000008b55d03994818f0000000f8cd300000033c02b45d8f7d88b0d000000002981a7000000837dd4ff0f841700000033c02b45d0f7d88b4dd48b150000000029848a8f0000008a45e88b4df08b55fc880411c745e400000000e903000000ff45e4837de4050f8d7a0000008b45f03945e40f84690000008b45e48b4dfc0fbe04088b4df08b55fc0fbe0c113bc10f854d0000008b45e48b4dfc0fbf44410585c00f8e3a0000008b45f08b4dfc0fbf4441058b4de48b55fc0fbf4c4a0503c18b4de48b55fc6689444a058b45f08b4dfc66c744410500008b45f08b4dfcc60408ffe979ffffffe96bfdffffe9fbfcffffe9a6fcffffe9000000005f5e5bc9c3

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 34             	sub    esp,0x34
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 4d cc             	mov    DWORD PTR [ebp-0x34],ecx
   c:	c7 45 e8 ff ff ff ff 	mov    DWORD PTR [ebp-0x18],0xffffffff
  13:	c7 45 f8 00 00 00 00 	mov    DWORD PTR [ebp-0x8],0x0
  1a:	e9 03 00 00 00       	jmp    0x22
  1f:	ff 45 f8             	inc    DWORD PTR [ebp-0x8]
  22:	a1 00 00 00 00       	mov    eax,ds:0x0
  27:	0f be 40 44          	movsx  eax,BYTE PTR [eax+0x44]
  2b:	3b 45 f8             	cmp    eax,DWORD PTR [ebp-0x8]
  2e:	0f 8e 45 03 00 00    	jle    0x379
  34:	e9 00 00 00 00       	jmp    0x39
  39:	e9 00 00 00 00       	jmp    0x3e
  3e:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
  41:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
  47:	0f be 44 08 47       	movsx  eax,BYTE PTR [eax+ecx*1+0x47]
  4c:	c1 e0 02             	shl    eax,0x2
  4f:	8d 04 80             	lea    eax,[eax+eax*4]
  52:	8d 04 80             	lea    eax,[eax+eax*4]
  55:	03 05 00 00 00 00    	add    eax,DWORD PTR ds:0x0
  5b:	05 53 0b 00 00       	add    eax,0xb53
  60:	89 45 dc             	mov    DWORD PTR [ebp-0x24],eax
  63:	c7 45 f4 00 00 00 00 	mov    DWORD PTR [ebp-0xc],0x0
  6a:	e9 03 00 00 00       	jmp    0x72
  6f:	ff 45 f4             	inc    DWORD PTR [ebp-0xc]
  72:	83 7d f4 02          	cmp    DWORD PTR [ebp-0xc],0x2
  76:	0f 8d f8 02 00 00    	jge    0x374
  7c:	83 7d f4 00          	cmp    DWORD PTR [ebp-0xc],0x0
  80:	0f 85 0e 00 00 00    	jne    0x94
  86:	8b 45 dc             	mov    eax,DWORD PTR [ebp-0x24]
  89:	83 c0 08             	add    eax,0x8
  8c:	89 45 fc             	mov    DWORD PTR [ebp-0x4],eax
  8f:	e9 3a 00 00 00       	jmp    0xce
  94:	8b 45 dc             	mov    eax,DWORD PTR [ebp-0x24]
  97:	0f be 40 17          	movsx  eax,BYTE PTR [eax+0x17]
  9b:	83 f8 ff             	cmp    eax,0xffffffff
  9e:	0f 85 05 00 00 00    	jne    0xa9
  a4:	e9 c6 ff ff ff       	jmp    0x6f
  a9:	e9 00 00 00 00       	jmp    0xae
  ae:	8b 45 dc             	mov    eax,DWORD PTR [ebp-0x24]
  b1:	0f be 40 17          	movsx  eax,BYTE PTR [eax+0x17]
  b5:	8d 04 80             	lea    eax,[eax+eax*4]
  b8:	8d 04 80             	lea    eax,[eax+eax*4]
  bb:	8d 04 80             	lea    eax,[eax+eax*4]
  be:	03 c0                	add    eax,eax
  c0:	03 05 00 00 00 00    	add    eax,DWORD PTR ds:0x0
  c6:	05 29 28 00 00       	add    eax,0x2829
  cb:	89 45 fc             	mov    DWORD PTR [ebp-0x4],eax
  ce:	c7 45 f0 00 00 00 00 	mov    DWORD PTR [ebp-0x10],0x0
  d5:	e9 03 00 00 00       	jmp    0xdd
  da:	ff 45 f0             	inc    DWORD PTR [ebp-0x10]
  dd:	83 7d f0 05          	cmp    DWORD PTR [ebp-0x10],0x5
  e1:	0f 8d 88 02 00 00    	jge    0x36f
  e7:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  ea:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
  ed:	0f be 04 08          	movsx  eax,BYTE PTR [eax+ecx*1]
  f1:	83 f8 ff             	cmp    eax,0xffffffff
  f4:	0f 85 05 00 00 00    	jne    0xff
  fa:	e9 db ff ff ff       	jmp    0xda
  ff:	c7 45 e0 00 00 00 00 	mov    DWORD PTR [ebp-0x20],0x0
 106:	c7 45 ec 14 00 00 00 	mov    DWORD PTR [ebp-0x14],0x14
 10d:	e9 03 00 00 00       	jmp    0x115
 112:	ff 45 ec             	inc    DWORD PTR [ebp-0x14]
 115:	83 7d ec 18          	cmp    DWORD PTR [ebp-0x14],0x18
 119:	0f 8f 5a 00 00 00    	jg     0x179
 11f:	8b 45 dc             	mov    eax,DWORD PTR [ebp-0x24]
 122:	0f be 40 03          	movsx  eax,BYTE PTR [eax+0x3]
 126:	8d 04 40             	lea    eax,[eax+eax*2]
 129:	8b 4d ec             	mov    ecx,DWORD PTR [ebp-0x14]
 12c:	0f be 84 81 ed ff ff 	movsx  eax,BYTE PTR [ecx+eax*4-0x13]
 133:	ff
 134:	8b 4d f0             	mov    ecx,DWORD PTR [ebp-0x10]
 137:	8b 55 fc             	mov    edx,DWORD PTR [ebp-0x4]
 13a:	0f be 0c 11          	movsx  ecx,BYTE PTR [ecx+edx*1]
 13e:	3b c1                	cmp    eax,ecx
 140:	0f 85 2e 00 00 00    	jne    0x174
 146:	b8 01 00 00 00       	mov    eax,0x1
 14b:	8b 4d ec             	mov    ecx,DWORD PTR [ebp-0x14]
 14e:	83 c1 05             	add    ecx,0x5
 151:	d3 e0                	shl    eax,cl
 153:	8b 4d dc             	mov    ecx,DWORD PTR [ebp-0x24]
 156:	85 41 18             	test   DWORD PTR [ecx+0x18],eax
 159:	0f 84 15 00 00 00    	je     0x174
 15f:	c7 45 e0 01 00 00 00 	mov    DWORD PTR [ebp-0x20],0x1
 166:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 169:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 16c:	0f be 04 08          	movsx  eax,BYTE PTR [eax+ecx*1]
 170:	40                   	inc    eax
 171:	89 45 e8             	mov    DWORD PTR [ebp-0x18],eax
 174:	e9 99 ff ff ff       	jmp    0x112
 179:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 17c:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 17f:	0f be 04 08          	movsx  eax,BYTE PTR [eax+ecx*1]
 183:	83 f8 23             	cmp    eax,0x23
 186:	0f 84 13 00 00 00    	je     0x19f
 18c:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 18f:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 192:	0f be 04 08          	movsx  eax,BYTE PTR [eax+ecx*1]
 196:	83 f8 24             	cmp    eax,0x24
 199:	0f 85 1b 00 00 00    	jne    0x1ba
 19f:	8b 45 dc             	mov    eax,DWORD PTR [ebp-0x24]
 1a2:	f6 40 1b 40          	test   BYTE PTR [eax+0x1b],0x40
 1a6:	0f 84 0e 00 00 00    	je     0x1ba
 1ac:	c7 45 e0 01 00 00 00 	mov    DWORD PTR [ebp-0x20],0x1
 1b3:	c7 45 e8 25 00 00 00 	mov    DWORD PTR [ebp-0x18],0x25
 1ba:	83 7d e0 00          	cmp    DWORD PTR [ebp-0x20],0x0
 1be:	0f 84 a6 01 00 00    	je     0x36a
 1c4:	8b 45 e8             	mov    eax,DWORD PTR [ebp-0x18]
 1c7:	8b c8                	mov    ecx,eax
 1c9:	8d 04 80             	lea    eax,[eax+eax*4]
 1cc:	8d 04 80             	lea    eax,[eax+eax*4]
 1cf:	0f bf 84 08 00 00 00 	movsx  eax,WORD PTR [eax+ecx*1+0x0]
 1d6:	00
 1d7:	8b 4d f0             	mov    ecx,DWORD PTR [ebp-0x10]
 1da:	8b 55 fc             	mov    edx,DWORD PTR [ebp-0x4]
 1dd:	0f be 0c 11          	movsx  ecx,BYTE PTR [ecx+edx*1]
 1e1:	8b d1                	mov    edx,ecx
 1e3:	8d 0c 89             	lea    ecx,[ecx+ecx*4]
 1e6:	8d 0c 89             	lea    ecx,[ecx+ecx*4]
 1e9:	0f bf 8c 11 00 00 00 	movsx  ecx,WORD PTR [ecx+edx*1+0x0]
 1f0:	00
 1f1:	2b c1                	sub    eax,ecx
 1f3:	8b 4d f0             	mov    ecx,DWORD PTR [ebp-0x10]
 1f6:	8b 55 fc             	mov    edx,DWORD PTR [ebp-0x4]
 1f9:	0f bf 4c 4a 05       	movsx  ecx,WORD PTR [edx+ecx*2+0x5]
 1fe:	0f af c1             	imul   eax,ecx
 201:	03 c0                	add    eax,eax
 203:	89 45 d8             	mov    DWORD PTR [ebp-0x28],eax
 206:	83 7d e8 25          	cmp    DWORD PTR [ebp-0x18],0x25
 20a:	0f 85 1c 00 00 00    	jne    0x22c
 210:	c7 45 d4 03 00 00 00 	mov    DWORD PTR [ebp-0x2c],0x3
 217:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 21a:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 21d:	0f bf 44 41 05       	movsx  eax,WORD PTR [ecx+eax*2+0x5]
 222:	03 c0                	add    eax,eax
 224:	89 45 d0             	mov    DWORD PTR [ebp-0x30],eax
 227:	e9 34 00 00 00       	jmp    0x260
 22c:	83 7d e8 2e          	cmp    DWORD PTR [ebp-0x18],0x2e
 230:	0f 85 1c 00 00 00    	jne    0x252
 236:	c7 45 d4 05 00 00 00 	mov    DWORD PTR [ebp-0x2c],0x5
 23d:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 240:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 243:	0f bf 44 41 05       	movsx  eax,WORD PTR [ecx+eax*2+0x5]
 248:	03 c0                	add    eax,eax
 24a:	89 45 d0             	mov    DWORD PTR [ebp-0x30],eax
 24d:	e9 0e 00 00 00       	jmp    0x260
 252:	c7 45 d4 ff ff ff ff 	mov    DWORD PTR [ebp-0x2c],0xffffffff
 259:	c7 45 d0 00 00 00 00 	mov    DWORD PTR [ebp-0x30],0x0
 260:	a1 00 00 00 00       	mov    eax,ds:0x0
 265:	8b 4d d8             	mov    ecx,DWORD PTR [ebp-0x28]
 268:	39 88 a7 00 00 00    	cmp    DWORD PTR [eax+0xa7],ecx
 26e:	0f 8c f6 00 00 00    	jl     0x36a
 274:	83 7d d4 ff          	cmp    DWORD PTR [ebp-0x2c],0xffffffff
 278:	0f 84 19 00 00 00    	je     0x297
 27e:	8b 45 d4             	mov    eax,DWORD PTR [ebp-0x2c]
 281:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
 287:	8b 55 d0             	mov    edx,DWORD PTR [ebp-0x30]
 28a:	39 94 81 8f 00 00 00 	cmp    DWORD PTR [ecx+eax*4+0x8f],edx
 291:	0f 8c d3 00 00 00    	jl     0x36a
 297:	33 c0                	xor    eax,eax
 299:	2b 45 d8             	sub    eax,DWORD PTR [ebp-0x28]
 29c:	f7 d8                	neg    eax
 29e:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
 2a4:	29 81 a7 00 00 00    	sub    DWORD PTR [ecx+0xa7],eax
 2aa:	83 7d d4 ff          	cmp    DWORD PTR [ebp-0x2c],0xffffffff
 2ae:	0f 84 17 00 00 00    	je     0x2cb
 2b4:	33 c0                	xor    eax,eax
 2b6:	2b 45 d0             	sub    eax,DWORD PTR [ebp-0x30]
 2b9:	f7 d8                	neg    eax
 2bb:	8b 4d d4             	mov    ecx,DWORD PTR [ebp-0x2c]
 2be:	8b 15 00 00 00 00    	mov    edx,DWORD PTR ds:0x0
 2c4:	29 84 8a 8f 00 00 00 	sub    DWORD PTR [edx+ecx*4+0x8f],eax
 2cb:	8a 45 e8             	mov    al,BYTE PTR [ebp-0x18]
 2ce:	8b 4d f0             	mov    ecx,DWORD PTR [ebp-0x10]
 2d1:	8b 55 fc             	mov    edx,DWORD PTR [ebp-0x4]
 2d4:	88 04 11             	mov    BYTE PTR [ecx+edx*1],al
 2d7:	c7 45 e4 00 00 00 00 	mov    DWORD PTR [ebp-0x1c],0x0
 2de:	e9 03 00 00 00       	jmp    0x2e6
 2e3:	ff 45 e4             	inc    DWORD PTR [ebp-0x1c]
 2e6:	83 7d e4 05          	cmp    DWORD PTR [ebp-0x1c],0x5
 2ea:	0f 8d 7a 00 00 00    	jge    0x36a
 2f0:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 2f3:	39 45 e4             	cmp    DWORD PTR [ebp-0x1c],eax
 2f6:	0f 84 69 00 00 00    	je     0x365
 2fc:	8b 45 e4             	mov    eax,DWORD PTR [ebp-0x1c]
 2ff:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 302:	0f be 04 08          	movsx  eax,BYTE PTR [eax+ecx*1]
 306:	8b 4d f0             	mov    ecx,DWORD PTR [ebp-0x10]
 309:	8b 55 fc             	mov    edx,DWORD PTR [ebp-0x4]
 30c:	0f be 0c 11          	movsx  ecx,BYTE PTR [ecx+edx*1]
 310:	3b c1                	cmp    eax,ecx
 312:	0f 85 4d 00 00 00    	jne    0x365
 318:	8b 45 e4             	mov    eax,DWORD PTR [ebp-0x1c]
 31b:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 31e:	0f bf 44 41 05       	movsx  eax,WORD PTR [ecx+eax*2+0x5]
 323:	85 c0                	test   eax,eax
 325:	0f 8e 3a 00 00 00    	jle    0x365
 32b:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 32e:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 331:	0f bf 44 41 05       	movsx  eax,WORD PTR [ecx+eax*2+0x5]
 336:	8b 4d e4             	mov    ecx,DWORD PTR [ebp-0x1c]
 339:	8b 55 fc             	mov    edx,DWORD PTR [ebp-0x4]
 33c:	0f bf 4c 4a 05       	movsx  ecx,WORD PTR [edx+ecx*2+0x5]
 341:	03 c1                	add    eax,ecx
 343:	8b 4d e4             	mov    ecx,DWORD PTR [ebp-0x1c]
 346:	8b 55 fc             	mov    edx,DWORD PTR [ebp-0x4]
 349:	66 89 44 4a 05       	mov    WORD PTR [edx+ecx*2+0x5],ax
 34e:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 351:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 354:	66 c7 44 41 05 00 00 	mov    WORD PTR [ecx+eax*2+0x5],0x0
 35b:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 35e:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
 361:	c6 04 08 ff          	mov    BYTE PTR [eax+ecx*1],0xff
 365:	e9 79 ff ff ff       	jmp    0x2e3
 36a:	e9 6b fd ff ff       	jmp    0xda
 36f:	e9 fb fc ff ff       	jmp    0x6f
 374:	e9 a6 fc ff ff       	jmp    0x1f
 379:	e9 00 00 00 00       	jmp    0x37e
 37e:	5f                   	pop    edi
 37f:	5e                   	pop    esi
 380:	5b                   	pop    ebx
 381:	c9                   	leave
 382:	c3                   	ret
