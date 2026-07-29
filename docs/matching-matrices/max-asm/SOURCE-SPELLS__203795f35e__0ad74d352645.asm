; ?ResetBoltAngle@combatManager@@QAEXPAUSBolt@@@Z
; unit SOURCE/SPELLS  src_hash 0ad74d352645  score 100.0000  seed 1213156658  trial 8
; size 800  text_sha b9bb25de5d6b4c06
; relocs ["0000002c:0014:_abs:00000000", "00000044:0014:_abs:00000000", "0000006b:0014:_sqrt:00000000", "00000073:0014:__ftol:00000000", "000000b5:0006:__adjust_fdiv:00000000", "000000c4:0014:__adj_fdiv_r:00000000", "00000115:0014:__ftol:00000000", "0000019e:0014:_atan2:00000000", "000001ca:0006:__h2cg$SOURCE$SPELLS$data$aNGLEBIASConstant:00000000", "000001d6:0006:__adjust_fdiv:00000000", "000001df:0006:__h2cg$SOURCE$SPELLS$data$aNGLEDIVISORConstant:00000000", "000001e7:0006:__h2cg$SOURCE$SPELLS$data$aNGLEDIVISORConstant:04000000", "000001ed:0006:__h2cg$SOURCE$SPELLS$data$aNGLEDIVISORConstant:00000000", "000001f2:0014:__adj_fdiv_m64:00000000", "00000235:0006:__h2cg$SOURCE$SPELLS$data$resetBoltAngleConstant:00000000", "0000027c:0006:__adjust_fdiv:00000000", "00000285:0006:__h2cg$SOURCE$SPELLS$data$fRAMEDELAYConstant2:00000000", "0000028d:0006:__h2cg$SOURCE$SPELLS$data$fRAMEDELAYConstant2:00000000", "00000292:0014:__adj_fdiv_m32:00000000", "000002ab:0014:?Random@@YIHHH@Z:00000000", "000002b7:0006:__adjust_fdiv:00000000", "000002c0:0006:__h2cg$SOURCE$SPELLS$data$fRAMEDELAYConstant2:00000000", "000002c8:0006:__h2cg$SOURCE$SPELLS$data$fRAMEDELAYConstant2:00000000", "000002cd:0014:__adj_fdiv_m32:00000000", "000002d6:0006:__h2cg$SOURCE$SPELLS$data$aNGLEBIASConstant2:00000000", "000002e2:0006:__adjust_fdiv:00000000", "000002eb:0006:__h2cg$SOURCE$SPELLS$data$resetBoltAngleConstant:00000000", "000002f3:0006:__h2cg$SOURCE$SPELLS$data$resetBoltAngleConstant:04000000", "000002f9:0006:__h2cg$SOURCE$SPELLS$data$resetBoltAngleConstant:00000000", "000002fe:0014:__adj_fdiv_m64:00000000"]
; hex 558bec83ec58535657894dd48b4508837844000f8405000000e9fb0200008b45088b40088b4d082b412c50e80000000083c4048945f08b45088b400c8b4d082b413050e80000000083c4048945ec8b45f00faf45f08b4dec0faf4dec03c18945d0db45d083ec08dd1c24e80000000083c408e8000000008945f88b45088b4df83948640f8d0f0000008b4508c7407000000000e9370000008b45088b40642b45f88945ccdb45cc8b45088b40648945c8db45c8833d00000000007504def9eb0950b00de800000000588b4508d958708b45088b4d088b496039485c0f845d0000008b45088b40608b4d082b415c8945f4837df4000f8e08000000ff45f4e903000000ff4df48b45088b4df4894dc4db45c4d84870e8000000008b4d088b495c03c8894de8837de8010f8d07000000c745e8010000008b45e88b4d088941148b45088b4014d1f8f7d88b4d0889414c8b45088b40148b4d0803414c488b4d088941508b45088b40088b4d082b412c8945dc8b45088b400c8b4d082b41308945fc8b45fc8945c0db45c083ec08dd1c248b45dc8945bcdb45bc83ec08dd1c24e80000000083c4108b4508d958388b45088b40688b4d0803416cb9c800000099f7f98945b8db45b8d95de0dd05000000008b4508d86070833d00000000007508dc3500000000eb11ff3504000000ff3500000000e800000000d84de0d955e08b4508d840388b4508d9583c8b4508837868000f85120000008b450883786c000f8505000000e9f20000008b45088b40208945b4db45b4dc0d000000008b45f88945b0db45b0ded9dfe0f6c4410f840d0000008b4508837874000f84b80000008b45088b4d088b496c3948680f85300000008b45088b40688945acdb45ac833d00000000007508d83500000000eb0bff3500000000e800000000d95dd8e9360000008b45088b4d088b506c8b4968e8000000008945a8db45a8833d00000000007508d83500000000eb0bff3500000000e800000000d95dd8d905000000008b4508d86070833d00000000007508dc3500000000eb11ff3504000000ff3500000000e800000000d84dd8d955d88b4508d840388b4508d95838e9000000005f5e5bc9c20400

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 58             	sub    esp,0x58
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 4d d4             	mov    DWORD PTR [ebp-0x2c],ecx
   c:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
   f:	83 78 44 00          	cmp    DWORD PTR [eax+0x44],0x0
  13:	0f 84 05 00 00 00    	je     0x1e
  19:	e9 fb 02 00 00       	jmp    0x319
  1e:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  21:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
  24:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  27:	2b 41 2c             	sub    eax,DWORD PTR [ecx+0x2c]
  2a:	50                   	push   eax
  2b:	e8 00 00 00 00       	call   0x30
  30:	83 c4 04             	add    esp,0x4
  33:	89 45 f0             	mov    DWORD PTR [ebp-0x10],eax
  36:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  39:	8b 40 0c             	mov    eax,DWORD PTR [eax+0xc]
  3c:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  3f:	2b 41 30             	sub    eax,DWORD PTR [ecx+0x30]
  42:	50                   	push   eax
  43:	e8 00 00 00 00       	call   0x48
  48:	83 c4 04             	add    esp,0x4
  4b:	89 45 ec             	mov    DWORD PTR [ebp-0x14],eax
  4e:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  51:	0f af 45 f0          	imul   eax,DWORD PTR [ebp-0x10]
  55:	8b 4d ec             	mov    ecx,DWORD PTR [ebp-0x14]
  58:	0f af 4d ec          	imul   ecx,DWORD PTR [ebp-0x14]
  5c:	03 c1                	add    eax,ecx
  5e:	89 45 d0             	mov    DWORD PTR [ebp-0x30],eax
  61:	db 45 d0             	fild   DWORD PTR [ebp-0x30]
  64:	83 ec 08             	sub    esp,0x8
  67:	dd 1c 24             	fstp   QWORD PTR [esp]
  6a:	e8 00 00 00 00       	call   0x6f
  6f:	83 c4 08             	add    esp,0x8
  72:	e8 00 00 00 00       	call   0x77
  77:	89 45 f8             	mov    DWORD PTR [ebp-0x8],eax
  7a:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  7d:	8b 4d f8             	mov    ecx,DWORD PTR [ebp-0x8]
  80:	39 48 64             	cmp    DWORD PTR [eax+0x64],ecx
  83:	0f 8d 0f 00 00 00    	jge    0x98
  89:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  8c:	c7 40 70 00 00 00 00 	mov    DWORD PTR [eax+0x70],0x0
  93:	e9 37 00 00 00       	jmp    0xcf
  98:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  9b:	8b 40 64             	mov    eax,DWORD PTR [eax+0x64]
  9e:	2b 45 f8             	sub    eax,DWORD PTR [ebp-0x8]
  a1:	89 45 cc             	mov    DWORD PTR [ebp-0x34],eax
  a4:	db 45 cc             	fild   DWORD PTR [ebp-0x34]
  a7:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  aa:	8b 40 64             	mov    eax,DWORD PTR [eax+0x64]
  ad:	89 45 c8             	mov    DWORD PTR [ebp-0x38],eax
  b0:	db 45 c8             	fild   DWORD PTR [ebp-0x38]
  b3:	83 3d 00 00 00 00 00 	cmp    DWORD PTR ds:0x0,0x0
  ba:	75 04                	jne    0xc0
  bc:	de f9                	fdivp  st(1),st
  be:	eb 09                	jmp    0xc9
  c0:	50                   	push   eax
  c1:	b0 0d                	mov    al,0xd
  c3:	e8 00 00 00 00       	call   0xc8
  c8:	58                   	pop    eax
  c9:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  cc:	d9 58 70             	fstp   DWORD PTR [eax+0x70]
  cf:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  d2:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  d5:	8b 49 60             	mov    ecx,DWORD PTR [ecx+0x60]
  d8:	39 48 5c             	cmp    DWORD PTR [eax+0x5c],ecx
  db:	0f 84 5d 00 00 00    	je     0x13e
  e1:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  e4:	8b 40 60             	mov    eax,DWORD PTR [eax+0x60]
  e7:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  ea:	2b 41 5c             	sub    eax,DWORD PTR [ecx+0x5c]
  ed:	89 45 f4             	mov    DWORD PTR [ebp-0xc],eax
  f0:	83 7d f4 00          	cmp    DWORD PTR [ebp-0xc],0x0
  f4:	0f 8e 08 00 00 00    	jle    0x102
  fa:	ff 45 f4             	inc    DWORD PTR [ebp-0xc]
  fd:	e9 03 00 00 00       	jmp    0x105
 102:	ff 4d f4             	dec    DWORD PTR [ebp-0xc]
 105:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 108:	8b 4d f4             	mov    ecx,DWORD PTR [ebp-0xc]
 10b:	89 4d c4             	mov    DWORD PTR [ebp-0x3c],ecx
 10e:	db 45 c4             	fild   DWORD PTR [ebp-0x3c]
 111:	d8 48 70             	fmul   DWORD PTR [eax+0x70]
 114:	e8 00 00 00 00       	call   0x119
 119:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
 11c:	8b 49 5c             	mov    ecx,DWORD PTR [ecx+0x5c]
 11f:	03 c8                	add    ecx,eax
 121:	89 4d e8             	mov    DWORD PTR [ebp-0x18],ecx
 124:	83 7d e8 01          	cmp    DWORD PTR [ebp-0x18],0x1
 128:	0f 8d 07 00 00 00    	jge    0x135
 12e:	c7 45 e8 01 00 00 00 	mov    DWORD PTR [ebp-0x18],0x1
 135:	8b 45 e8             	mov    eax,DWORD PTR [ebp-0x18]
 138:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
 13b:	89 41 14             	mov    DWORD PTR [ecx+0x14],eax
 13e:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 141:	8b 40 14             	mov    eax,DWORD PTR [eax+0x14]
 144:	d1 f8                	sar    eax,1
 146:	f7 d8                	neg    eax
 148:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
 14b:	89 41 4c             	mov    DWORD PTR [ecx+0x4c],eax
 14e:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 151:	8b 40 14             	mov    eax,DWORD PTR [eax+0x14]
 154:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
 157:	03 41 4c             	add    eax,DWORD PTR [ecx+0x4c]
 15a:	48                   	dec    eax
 15b:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
 15e:	89 41 50             	mov    DWORD PTR [ecx+0x50],eax
 161:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 164:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
 167:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
 16a:	2b 41 2c             	sub    eax,DWORD PTR [ecx+0x2c]
 16d:	89 45 dc             	mov    DWORD PTR [ebp-0x24],eax
 170:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 173:	8b 40 0c             	mov    eax,DWORD PTR [eax+0xc]
 176:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
 179:	2b 41 30             	sub    eax,DWORD PTR [ecx+0x30]
 17c:	89 45 fc             	mov    DWORD PTR [ebp-0x4],eax
 17f:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
 182:	89 45 c0             	mov    DWORD PTR [ebp-0x40],eax
 185:	db 45 c0             	fild   DWORD PTR [ebp-0x40]
 188:	83 ec 08             	sub    esp,0x8
 18b:	dd 1c 24             	fstp   QWORD PTR [esp]
 18e:	8b 45 dc             	mov    eax,DWORD PTR [ebp-0x24]
 191:	89 45 bc             	mov    DWORD PTR [ebp-0x44],eax
 194:	db 45 bc             	fild   DWORD PTR [ebp-0x44]
 197:	83 ec 08             	sub    esp,0x8
 19a:	dd 1c 24             	fstp   QWORD PTR [esp]
 19d:	e8 00 00 00 00       	call   0x1a2
 1a2:	83 c4 10             	add    esp,0x10
 1a5:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 1a8:	d9 58 38             	fstp   DWORD PTR [eax+0x38]
 1ab:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 1ae:	8b 40 68             	mov    eax,DWORD PTR [eax+0x68]
 1b1:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
 1b4:	03 41 6c             	add    eax,DWORD PTR [ecx+0x6c]
 1b7:	b9 c8 00 00 00       	mov    ecx,0xc8
 1bc:	99                   	cdq
 1bd:	f7 f9                	idiv   ecx
 1bf:	89 45 b8             	mov    DWORD PTR [ebp-0x48],eax
 1c2:	db 45 b8             	fild   DWORD PTR [ebp-0x48]
 1c5:	d9 5d e0             	fstp   DWORD PTR [ebp-0x20]
 1c8:	dd 05 00 00 00 00    	fld    QWORD PTR ds:0x0
 1ce:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 1d1:	d8 60 70             	fsub   DWORD PTR [eax+0x70]
 1d4:	83 3d 00 00 00 00 00 	cmp    DWORD PTR ds:0x0,0x0
 1db:	75 08                	jne    0x1e5
 1dd:	dc 35 00 00 00 00    	fdiv   QWORD PTR ds:0x0
 1e3:	eb 11                	jmp    0x1f6
 1e5:	ff 35 04 00 00 00    	push   DWORD PTR ds:0x4
 1eb:	ff 35 00 00 00 00    	push   DWORD PTR ds:0x0
 1f1:	e8 00 00 00 00       	call   0x1f6
 1f6:	d8 4d e0             	fmul   DWORD PTR [ebp-0x20]
 1f9:	d9 55 e0             	fst    DWORD PTR [ebp-0x20]
 1fc:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 1ff:	d8 40 38             	fadd   DWORD PTR [eax+0x38]
 202:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 205:	d9 58 3c             	fstp   DWORD PTR [eax+0x3c]
 208:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 20b:	83 78 68 00          	cmp    DWORD PTR [eax+0x68],0x0
 20f:	0f 85 12 00 00 00    	jne    0x227
 215:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 218:	83 78 6c 00          	cmp    DWORD PTR [eax+0x6c],0x0
 21c:	0f 85 05 00 00 00    	jne    0x227
 222:	e9 f2 00 00 00       	jmp    0x319
 227:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 22a:	8b 40 20             	mov    eax,DWORD PTR [eax+0x20]
 22d:	89 45 b4             	mov    DWORD PTR [ebp-0x4c],eax
 230:	db 45 b4             	fild   DWORD PTR [ebp-0x4c]
 233:	dc 0d 00 00 00 00    	fmul   QWORD PTR ds:0x0
 239:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
 23c:	89 45 b0             	mov    DWORD PTR [ebp-0x50],eax
 23f:	db 45 b0             	fild   DWORD PTR [ebp-0x50]
 242:	de d9                	fcompp
 244:	df e0                	fnstsw ax
 246:	f6 c4 41             	test   ah,0x41
 249:	0f 84 0d 00 00 00    	je     0x25c
 24f:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 252:	83 78 74 00          	cmp    DWORD PTR [eax+0x74],0x0
 256:	0f 84 b8 00 00 00    	je     0x314
 25c:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 25f:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
 262:	8b 49 6c             	mov    ecx,DWORD PTR [ecx+0x6c]
 265:	39 48 68             	cmp    DWORD PTR [eax+0x68],ecx
 268:	0f 85 30 00 00 00    	jne    0x29e
 26e:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 271:	8b 40 68             	mov    eax,DWORD PTR [eax+0x68]
 274:	89 45 ac             	mov    DWORD PTR [ebp-0x54],eax
 277:	db 45 ac             	fild   DWORD PTR [ebp-0x54]
 27a:	83 3d 00 00 00 00 00 	cmp    DWORD PTR ds:0x0,0x0
 281:	75 08                	jne    0x28b
 283:	d8 35 00 00 00 00    	fdiv   DWORD PTR ds:0x0
 289:	eb 0b                	jmp    0x296
 28b:	ff 35 00 00 00 00    	push   DWORD PTR ds:0x0
 291:	e8 00 00 00 00       	call   0x296
 296:	d9 5d d8             	fstp   DWORD PTR [ebp-0x28]
 299:	e9 36 00 00 00       	jmp    0x2d4
 29e:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 2a1:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
 2a4:	8b 50 6c             	mov    edx,DWORD PTR [eax+0x6c]
 2a7:	8b 49 68             	mov    ecx,DWORD PTR [ecx+0x68]
 2aa:	e8 00 00 00 00       	call   0x2af
 2af:	89 45 a8             	mov    DWORD PTR [ebp-0x58],eax
 2b2:	db 45 a8             	fild   DWORD PTR [ebp-0x58]
 2b5:	83 3d 00 00 00 00 00 	cmp    DWORD PTR ds:0x0,0x0
 2bc:	75 08                	jne    0x2c6
 2be:	d8 35 00 00 00 00    	fdiv   DWORD PTR ds:0x0
 2c4:	eb 0b                	jmp    0x2d1
 2c6:	ff 35 00 00 00 00    	push   DWORD PTR ds:0x0
 2cc:	e8 00 00 00 00       	call   0x2d1
 2d1:	d9 5d d8             	fstp   DWORD PTR [ebp-0x28]
 2d4:	d9 05 00 00 00 00    	fld    DWORD PTR ds:0x0
 2da:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 2dd:	d8 60 70             	fsub   DWORD PTR [eax+0x70]
 2e0:	83 3d 00 00 00 00 00 	cmp    DWORD PTR ds:0x0,0x0
 2e7:	75 08                	jne    0x2f1
 2e9:	dc 35 00 00 00 00    	fdiv   QWORD PTR ds:0x0
 2ef:	eb 11                	jmp    0x302
 2f1:	ff 35 04 00 00 00    	push   DWORD PTR ds:0x4
 2f7:	ff 35 00 00 00 00    	push   DWORD PTR ds:0x0
 2fd:	e8 00 00 00 00       	call   0x302
 302:	d8 4d d8             	fmul   DWORD PTR [ebp-0x28]
 305:	d9 55 d8             	fst    DWORD PTR [ebp-0x28]
 308:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 30b:	d8 40 38             	fadd   DWORD PTR [eax+0x38]
 30e:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 311:	d9 58 38             	fstp   DWORD PTR [eax+0x38]
 314:	e9 00 00 00 00       	jmp    0x319
 319:	5f                   	pop    edi
 31a:	5e                   	pop    esi
 31b:	5b                   	pop    ebx
 31c:	c9                   	leave
 31d:	c2 04 00             	ret    0x4
