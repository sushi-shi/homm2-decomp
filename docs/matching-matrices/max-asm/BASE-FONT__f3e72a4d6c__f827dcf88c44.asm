; ?DrawBoundedString@font@@QAEXPADHHHHHH@Z
; unit BASE/FONT  src_hash f827dcf88c44  score 100.0000  seed 1112495438  trial 6
; size 787  text_sha c8ea4726c53f4ea5
; relocs ["00000087:0014:?LineLength@font@@QAEHPADH@Z:00000000", "00000138:0014:?GetCharacterWidth@font@@QAEHE@Z:00000000", "00000192:0014:?GetCharacterWidth@font@@QAEHE@Z:00000000", "00000221:0014:?GetCharacterWidth@font@@QAEHE@Z:00000000", "000002d5:0014:?DrawStringExecute@font@@IAEXPADHHHHHHH@Z:00000000"]
; hex 558bec83ec4c535657894db88b7d08b9ffffffff2bc0f2aef7d18d41ff8945ec8b45b88b401c8b40128945d4c645cc20c745e800000000c745e400000000c745f000000000c745d000000000c745f800000000c745d800000000c745c8000000008b45088945dc8b451c8945fcf64520040f843e000000836d20048b4514508b4508508b4db8e8000000008945c08b45b88b40100faf45c08945c48b45183945c40f8d0e0000008b45182b45c4992bc2d1f88945e48b45b8c74018000000008b45f83945ec0f8e3c0200008b45f88b4ddc0fbe040885c00f842a0200008b45b88b40100345e43b45180f8e0a000000837de4000f850e0200008b45f88b4ddc0fbe040885c00f843c0000008b45f88b4ddc0fbe040883f80a0f84290000008b45d83945140f8c1d0000008b45dc8b4df88a0408508b4db8e8000000000145d8ff45f8e9b2ffffff8b45d88945bc8b45d83945140f8dd3000000ff4df8c745c8000000008b45f88b4ddc0fbe040883f8200f84700000008b45f03945f80f8c6400000033db8b45dc8b4df88a0408508b4db8e8000000002bd8f7db295dd88b45b88b401003c00345e43b45180f8e110000008b45d83945140f8e05000000e924000000837dc8000f85120000008b45d83945140f8e060000008b45f88945c8ff4df8e97dffffff8b45f03945f80f8f0c0000008b45c88945f88b45bc8945d88b45f88b4ddc0fbe040883f8200f851b00000033db8b45dc8b4df88a0408508b4db8e8000000002bd8f7db295dd88b45f88945d08b45d08b4ddc8a04088845e08b45d08b4ddcc60408008b45208945b4e932000000c745e800000000e9490000008b45142b45d8992bc2d1f88945e8e9360000008b45142b45d88945e8e928000000e923000000837db4000f84c4ffffff837db4010f84c6ffffff837db4020f84cfffffffe9000000008b4518508b4514508b4510508b450c508b45fc508b45e4034510508b45e803450c508b45f00345dc508b4db8e8000000008a45e08b4dd08b55dc8804118b45b88b40100145e48b45d0408945f08b45f08945f8c745d800000000e9b8fdffffe9000000005f5e5bc9c21c00

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 4c             	sub    esp,0x4c
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 4d b8             	mov    DWORD PTR [ebp-0x48],ecx
   c:	8b 7d 08             	mov    edi,DWORD PTR [ebp+0x8]
   f:	b9 ff ff ff ff       	mov    ecx,0xffffffff
  14:	2b c0                	sub    eax,eax
  16:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  18:	f7 d1                	not    ecx
  1a:	8d 41 ff             	lea    eax,[ecx-0x1]
  1d:	89 45 ec             	mov    DWORD PTR [ebp-0x14],eax
  20:	8b 45 b8             	mov    eax,DWORD PTR [ebp-0x48]
  23:	8b 40 1c             	mov    eax,DWORD PTR [eax+0x1c]
  26:	8b 40 12             	mov    eax,DWORD PTR [eax+0x12]
  29:	89 45 d4             	mov    DWORD PTR [ebp-0x2c],eax
  2c:	c6 45 cc 20          	mov    BYTE PTR [ebp-0x34],0x20
  30:	c7 45 e8 00 00 00 00 	mov    DWORD PTR [ebp-0x18],0x0
  37:	c7 45 e4 00 00 00 00 	mov    DWORD PTR [ebp-0x1c],0x0
  3e:	c7 45 f0 00 00 00 00 	mov    DWORD PTR [ebp-0x10],0x0
  45:	c7 45 d0 00 00 00 00 	mov    DWORD PTR [ebp-0x30],0x0
  4c:	c7 45 f8 00 00 00 00 	mov    DWORD PTR [ebp-0x8],0x0
  53:	c7 45 d8 00 00 00 00 	mov    DWORD PTR [ebp-0x28],0x0
  5a:	c7 45 c8 00 00 00 00 	mov    DWORD PTR [ebp-0x38],0x0
  61:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  64:	89 45 dc             	mov    DWORD PTR [ebp-0x24],eax
  67:	8b 45 1c             	mov    eax,DWORD PTR [ebp+0x1c]
  6a:	89 45 fc             	mov    DWORD PTR [ebp-0x4],eax
  6d:	f6 45 20 04          	test   BYTE PTR [ebp+0x20],0x4
  71:	0f 84 3e 00 00 00    	je     0xb5
  77:	83 6d 20 04          	sub    DWORD PTR [ebp+0x20],0x4
  7b:	8b 45 14             	mov    eax,DWORD PTR [ebp+0x14]
  7e:	50                   	push   eax
  7f:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  82:	50                   	push   eax
  83:	8b 4d b8             	mov    ecx,DWORD PTR [ebp-0x48]
  86:	e8 00 00 00 00       	call   0x8b
  8b:	89 45 c0             	mov    DWORD PTR [ebp-0x40],eax
  8e:	8b 45 b8             	mov    eax,DWORD PTR [ebp-0x48]
  91:	8b 40 10             	mov    eax,DWORD PTR [eax+0x10]
  94:	0f af 45 c0          	imul   eax,DWORD PTR [ebp-0x40]
  98:	89 45 c4             	mov    DWORD PTR [ebp-0x3c],eax
  9b:	8b 45 18             	mov    eax,DWORD PTR [ebp+0x18]
  9e:	39 45 c4             	cmp    DWORD PTR [ebp-0x3c],eax
  a1:	0f 8d 0e 00 00 00    	jge    0xb5
  a7:	8b 45 18             	mov    eax,DWORD PTR [ebp+0x18]
  aa:	2b 45 c4             	sub    eax,DWORD PTR [ebp-0x3c]
  ad:	99                   	cdq
  ae:	2b c2                	sub    eax,edx
  b0:	d1 f8                	sar    eax,1
  b2:	89 45 e4             	mov    DWORD PTR [ebp-0x1c],eax
  b5:	8b 45 b8             	mov    eax,DWORD PTR [ebp-0x48]
  b8:	c7 40 18 00 00 00 00 	mov    DWORD PTR [eax+0x18],0x0
  bf:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
  c2:	39 45 ec             	cmp    DWORD PTR [ebp-0x14],eax
  c5:	0f 8e 3c 02 00 00    	jle    0x307
  cb:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
  ce:	8b 4d dc             	mov    ecx,DWORD PTR [ebp-0x24]
  d1:	0f be 04 08          	movsx  eax,BYTE PTR [eax+ecx*1]
  d5:	85 c0                	test   eax,eax
  d7:	0f 84 2a 02 00 00    	je     0x307
  dd:	8b 45 b8             	mov    eax,DWORD PTR [ebp-0x48]
  e0:	8b 40 10             	mov    eax,DWORD PTR [eax+0x10]
  e3:	03 45 e4             	add    eax,DWORD PTR [ebp-0x1c]
  e6:	3b 45 18             	cmp    eax,DWORD PTR [ebp+0x18]
  e9:	0f 8e 0a 00 00 00    	jle    0xf9
  ef:	83 7d e4 00          	cmp    DWORD PTR [ebp-0x1c],0x0
  f3:	0f 85 0e 02 00 00    	jne    0x307
  f9:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
  fc:	8b 4d dc             	mov    ecx,DWORD PTR [ebp-0x24]
  ff:	0f be 04 08          	movsx  eax,BYTE PTR [eax+ecx*1]
 103:	85 c0                	test   eax,eax
 105:	0f 84 3c 00 00 00    	je     0x147
 10b:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
 10e:	8b 4d dc             	mov    ecx,DWORD PTR [ebp-0x24]
 111:	0f be 04 08          	movsx  eax,BYTE PTR [eax+ecx*1]
 115:	83 f8 0a             	cmp    eax,0xa
 118:	0f 84 29 00 00 00    	je     0x147
 11e:	8b 45 d8             	mov    eax,DWORD PTR [ebp-0x28]
 121:	39 45 14             	cmp    DWORD PTR [ebp+0x14],eax
 124:	0f 8c 1d 00 00 00    	jl     0x147
 12a:	8b 45 dc             	mov    eax,DWORD PTR [ebp-0x24]
 12d:	8b 4d f8             	mov    ecx,DWORD PTR [ebp-0x8]
 130:	8a 04 08             	mov    al,BYTE PTR [eax+ecx*1]
 133:	50                   	push   eax
 134:	8b 4d b8             	mov    ecx,DWORD PTR [ebp-0x48]
 137:	e8 00 00 00 00       	call   0x13c
 13c:	01 45 d8             	add    DWORD PTR [ebp-0x28],eax
 13f:	ff 45 f8             	inc    DWORD PTR [ebp-0x8]
 142:	e9 b2 ff ff ff       	jmp    0xf9
 147:	8b 45 d8             	mov    eax,DWORD PTR [ebp-0x28]
 14a:	89 45 bc             	mov    DWORD PTR [ebp-0x44],eax
 14d:	8b 45 d8             	mov    eax,DWORD PTR [ebp-0x28]
 150:	39 45 14             	cmp    DWORD PTR [ebp+0x14],eax
 153:	0f 8d d3 00 00 00    	jge    0x22c
 159:	ff 4d f8             	dec    DWORD PTR [ebp-0x8]
 15c:	c7 45 c8 00 00 00 00 	mov    DWORD PTR [ebp-0x38],0x0
 163:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
 166:	8b 4d dc             	mov    ecx,DWORD PTR [ebp-0x24]
 169:	0f be 04 08          	movsx  eax,BYTE PTR [eax+ecx*1]
 16d:	83 f8 20             	cmp    eax,0x20
 170:	0f 84 70 00 00 00    	je     0x1e6
 176:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 179:	39 45 f8             	cmp    DWORD PTR [ebp-0x8],eax
 17c:	0f 8c 64 00 00 00    	jl     0x1e6
 182:	33 db                	xor    ebx,ebx
 184:	8b 45 dc             	mov    eax,DWORD PTR [ebp-0x24]
 187:	8b 4d f8             	mov    ecx,DWORD PTR [ebp-0x8]
 18a:	8a 04 08             	mov    al,BYTE PTR [eax+ecx*1]
 18d:	50                   	push   eax
 18e:	8b 4d b8             	mov    ecx,DWORD PTR [ebp-0x48]
 191:	e8 00 00 00 00       	call   0x196
 196:	2b d8                	sub    ebx,eax
 198:	f7 db                	neg    ebx
 19a:	29 5d d8             	sub    DWORD PTR [ebp-0x28],ebx
 19d:	8b 45 b8             	mov    eax,DWORD PTR [ebp-0x48]
 1a0:	8b 40 10             	mov    eax,DWORD PTR [eax+0x10]
 1a3:	03 c0                	add    eax,eax
 1a5:	03 45 e4             	add    eax,DWORD PTR [ebp-0x1c]
 1a8:	3b 45 18             	cmp    eax,DWORD PTR [ebp+0x18]
 1ab:	0f 8e 11 00 00 00    	jle    0x1c2
 1b1:	8b 45 d8             	mov    eax,DWORD PTR [ebp-0x28]
 1b4:	39 45 14             	cmp    DWORD PTR [ebp+0x14],eax
 1b7:	0f 8e 05 00 00 00    	jle    0x1c2
 1bd:	e9 24 00 00 00       	jmp    0x1e6
 1c2:	83 7d c8 00          	cmp    DWORD PTR [ebp-0x38],0x0
 1c6:	0f 85 12 00 00 00    	jne    0x1de
 1cc:	8b 45 d8             	mov    eax,DWORD PTR [ebp-0x28]
 1cf:	39 45 14             	cmp    DWORD PTR [ebp+0x14],eax
 1d2:	0f 8e 06 00 00 00    	jle    0x1de
 1d8:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
 1db:	89 45 c8             	mov    DWORD PTR [ebp-0x38],eax
 1de:	ff 4d f8             	dec    DWORD PTR [ebp-0x8]
 1e1:	e9 7d ff ff ff       	jmp    0x163
 1e6:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 1e9:	39 45 f8             	cmp    DWORD PTR [ebp-0x8],eax
 1ec:	0f 8f 0c 00 00 00    	jg     0x1fe
 1f2:	8b 45 c8             	mov    eax,DWORD PTR [ebp-0x38]
 1f5:	89 45 f8             	mov    DWORD PTR [ebp-0x8],eax
 1f8:	8b 45 bc             	mov    eax,DWORD PTR [ebp-0x44]
 1fb:	89 45 d8             	mov    DWORD PTR [ebp-0x28],eax
 1fe:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
 201:	8b 4d dc             	mov    ecx,DWORD PTR [ebp-0x24]
 204:	0f be 04 08          	movsx  eax,BYTE PTR [eax+ecx*1]
 208:	83 f8 20             	cmp    eax,0x20
 20b:	0f 85 1b 00 00 00    	jne    0x22c
 211:	33 db                	xor    ebx,ebx
 213:	8b 45 dc             	mov    eax,DWORD PTR [ebp-0x24]
 216:	8b 4d f8             	mov    ecx,DWORD PTR [ebp-0x8]
 219:	8a 04 08             	mov    al,BYTE PTR [eax+ecx*1]
 21c:	50                   	push   eax
 21d:	8b 4d b8             	mov    ecx,DWORD PTR [ebp-0x48]
 220:	e8 00 00 00 00       	call   0x225
 225:	2b d8                	sub    ebx,eax
 227:	f7 db                	neg    ebx
 229:	29 5d d8             	sub    DWORD PTR [ebp-0x28],ebx
 22c:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
 22f:	89 45 d0             	mov    DWORD PTR [ebp-0x30],eax
 232:	8b 45 d0             	mov    eax,DWORD PTR [ebp-0x30]
 235:	8b 4d dc             	mov    ecx,DWORD PTR [ebp-0x24]
 238:	8a 04 08             	mov    al,BYTE PTR [eax+ecx*1]
 23b:	88 45 e0             	mov    BYTE PTR [ebp-0x20],al
 23e:	8b 45 d0             	mov    eax,DWORD PTR [ebp-0x30]
 241:	8b 4d dc             	mov    ecx,DWORD PTR [ebp-0x24]
 244:	c6 04 08 00          	mov    BYTE PTR [eax+ecx*1],0x0
 248:	8b 45 20             	mov    eax,DWORD PTR [ebp+0x20]
 24b:	89 45 b4             	mov    DWORD PTR [ebp-0x4c],eax
 24e:	e9 32 00 00 00       	jmp    0x285
 253:	c7 45 e8 00 00 00 00 	mov    DWORD PTR [ebp-0x18],0x0
 25a:	e9 49 00 00 00       	jmp    0x2a8
 25f:	8b 45 14             	mov    eax,DWORD PTR [ebp+0x14]
 262:	2b 45 d8             	sub    eax,DWORD PTR [ebp-0x28]
 265:	99                   	cdq
 266:	2b c2                	sub    eax,edx
 268:	d1 f8                	sar    eax,1
 26a:	89 45 e8             	mov    DWORD PTR [ebp-0x18],eax
 26d:	e9 36 00 00 00       	jmp    0x2a8
 272:	8b 45 14             	mov    eax,DWORD PTR [ebp+0x14]
 275:	2b 45 d8             	sub    eax,DWORD PTR [ebp-0x28]
 278:	89 45 e8             	mov    DWORD PTR [ebp-0x18],eax
 27b:	e9 28 00 00 00       	jmp    0x2a8
 280:	e9 23 00 00 00       	jmp    0x2a8
 285:	83 7d b4 00          	cmp    DWORD PTR [ebp-0x4c],0x0
 289:	0f 84 c4 ff ff ff    	je     0x253
 28f:	83 7d b4 01          	cmp    DWORD PTR [ebp-0x4c],0x1
 293:	0f 84 c6 ff ff ff    	je     0x25f
 299:	83 7d b4 02          	cmp    DWORD PTR [ebp-0x4c],0x2
 29d:	0f 84 cf ff ff ff    	je     0x272
 2a3:	e9 00 00 00 00       	jmp    0x2a8
 2a8:	8b 45 18             	mov    eax,DWORD PTR [ebp+0x18]
 2ab:	50                   	push   eax
 2ac:	8b 45 14             	mov    eax,DWORD PTR [ebp+0x14]
 2af:	50                   	push   eax
 2b0:	8b 45 10             	mov    eax,DWORD PTR [ebp+0x10]
 2b3:	50                   	push   eax
 2b4:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 2b7:	50                   	push   eax
 2b8:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
 2bb:	50                   	push   eax
 2bc:	8b 45 e4             	mov    eax,DWORD PTR [ebp-0x1c]
 2bf:	03 45 10             	add    eax,DWORD PTR [ebp+0x10]
 2c2:	50                   	push   eax
 2c3:	8b 45 e8             	mov    eax,DWORD PTR [ebp-0x18]
 2c6:	03 45 0c             	add    eax,DWORD PTR [ebp+0xc]
 2c9:	50                   	push   eax
 2ca:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 2cd:	03 45 dc             	add    eax,DWORD PTR [ebp-0x24]
 2d0:	50                   	push   eax
 2d1:	8b 4d b8             	mov    ecx,DWORD PTR [ebp-0x48]
 2d4:	e8 00 00 00 00       	call   0x2d9
 2d9:	8a 45 e0             	mov    al,BYTE PTR [ebp-0x20]
 2dc:	8b 4d d0             	mov    ecx,DWORD PTR [ebp-0x30]
 2df:	8b 55 dc             	mov    edx,DWORD PTR [ebp-0x24]
 2e2:	88 04 11             	mov    BYTE PTR [ecx+edx*1],al
 2e5:	8b 45 b8             	mov    eax,DWORD PTR [ebp-0x48]
 2e8:	8b 40 10             	mov    eax,DWORD PTR [eax+0x10]
 2eb:	01 45 e4             	add    DWORD PTR [ebp-0x1c],eax
 2ee:	8b 45 d0             	mov    eax,DWORD PTR [ebp-0x30]
 2f1:	40                   	inc    eax
 2f2:	89 45 f0             	mov    DWORD PTR [ebp-0x10],eax
 2f5:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 2f8:	89 45 f8             	mov    DWORD PTR [ebp-0x8],eax
 2fb:	c7 45 d8 00 00 00 00 	mov    DWORD PTR [ebp-0x28],0x0
 302:	e9 b8 fd ff ff       	jmp    0xbf
 307:	e9 00 00 00 00       	jmp    0x30c
 30c:	5f                   	pop    edi
 30d:	5e                   	pop    esi
 30e:	5b                   	pop    ebx
 30f:	c9                   	leave
 310:	c2 1c 00             	ret    0x1c
