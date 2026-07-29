; ?GetBestHero@philAI@@QAEXPAVtown@@AAUBHC@@AAM@Z
; unit SOURCE/PHILAI  src_hash 97ebf934a5cb  score 100.0000  seed 1291  trial 4
; size 433  text_sha cc8760c133a01110
; relocs ["0000003f:0006:?gpCurPlayer@@3PAVplayerData@@A:00000000", "00000055:0006:?gpGame@@3PAVgame@@A:00000000", "00000075:0014:?ValueOfBuyingHero@philAI@@QAEXPAVtown@@PAVhero@@AAHAAM@Z:00000000", "00000084:0014:?Random@@YIHHH@Z:00000000", "00000090:0006:__h2cg$SOURCE$PHILAI$data$rANDOMBASEConstant:00000000", "00000099:0006:__adjust_fdiv:00000000", "000000a2:0006:__h2cg$SOURCE$PHILAI$data$getBestBHCConstant2:00000000", "000000aa:0006:__h2cg$SOURCE$PHILAI$data$getBestBHCConstant2:04000000", "000000b0:0006:__h2cg$SOURCE$PHILAI$data$getBestBHCConstant2:00000000", "000000b5:0014:__adj_fdiv_m64:00000000", "000000de:0006:?giDebugLevel@@3HA:00000000", "00000107:0006:__h2cg$SOURCE$PHILAI$data$getBestHeroTown2dHero15iRawBC:00000000", "0000010c:0006:?gText@@3PADA:00000000", "00000111:0014:_sprintf:00000000", "00000119:0006:?gText@@3PADA:00000000", "0000011e:0014:?LogStr@@YIXPAD@Z:00000000", "00000158:0006:?gpGame@@3PAVgame@@A:00000000", "00000179:0006:?gpGame@@3PAVgame@@A:00000000", "0000019c:0006:__h2cg$SOURCE$PHILAI$data$valueOfEventAtPositionConstant2:00000000"]
; hex 558bec83ec28535657894ddcc745f8ffffffffc745ec0000c6c2c745f00000c6c2c745f400000000e903000000ff45f4837df4020f8ded0000008b45f48b0d000000000fbe44080c8d04808d04808d048003c003050000000005c42700008945e48d45fc508d45e0508b45e4508b4508508b4ddce800000000ba0a000000b901000000e8000000008945d8db45d8dc0500000000d84dfc833d00000000007508dc3500000000eb11ff3504000000ff3500000000e800000000d955e8d85df0dfe0f6c4410f85120000008b45f48945f88b45fc8945ec8b45e88945f0833d00000000050f8c39000000d945e883ec08dd1c24d945fc83ec08dd1c248b45f4508b45080fbe005068000000006800000000e80000000083c420b900000000e800000000e906ffffff8b45088b4d0c89018b450cc74004010000008b45f88b4d0c8941088b45108b4dec8908e9000000008b450833c98a4805a1000000000faf88460b00008d04498b4d0833d28a51048d0c52c1e1028d04818b0d000000008b893e0b000033d28a54080981faaa0000000f85100000008b4510d900d825000000008b4510d918e9000000005f5e5bc9c20c00

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 28             	sub    esp,0x28
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 4d dc             	mov    DWORD PTR [ebp-0x24],ecx
   c:	c7 45 f8 ff ff ff ff 	mov    DWORD PTR [ebp-0x8],0xffffffff
  13:	c7 45 ec 00 00 c6 c2 	mov    DWORD PTR [ebp-0x14],0xc2c60000
  1a:	c7 45 f0 00 00 c6 c2 	mov    DWORD PTR [ebp-0x10],0xc2c60000
  21:	c7 45 f4 00 00 00 00 	mov    DWORD PTR [ebp-0xc],0x0
  28:	e9 03 00 00 00       	jmp    0x30
  2d:	ff 45 f4             	inc    DWORD PTR [ebp-0xc]
  30:	83 7d f4 02          	cmp    DWORD PTR [ebp-0xc],0x2
  34:	0f 8d ed 00 00 00    	jge    0x127
  3a:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
  3d:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
  43:	0f be 44 08 0c       	movsx  eax,BYTE PTR [eax+ecx*1+0xc]
  48:	8d 04 80             	lea    eax,[eax+eax*4]
  4b:	8d 04 80             	lea    eax,[eax+eax*4]
  4e:	8d 04 80             	lea    eax,[eax+eax*4]
  51:	03 c0                	add    eax,eax
  53:	03 05 00 00 00 00    	add    eax,DWORD PTR ds:0x0
  59:	05 c4 27 00 00       	add    eax,0x27c4
  5e:	89 45 e4             	mov    DWORD PTR [ebp-0x1c],eax
  61:	8d 45 fc             	lea    eax,[ebp-0x4]
  64:	50                   	push   eax
  65:	8d 45 e0             	lea    eax,[ebp-0x20]
  68:	50                   	push   eax
  69:	8b 45 e4             	mov    eax,DWORD PTR [ebp-0x1c]
  6c:	50                   	push   eax
  6d:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  70:	50                   	push   eax
  71:	8b 4d dc             	mov    ecx,DWORD PTR [ebp-0x24]
  74:	e8 00 00 00 00       	call   0x79
  79:	ba 0a 00 00 00       	mov    edx,0xa
  7e:	b9 01 00 00 00       	mov    ecx,0x1
  83:	e8 00 00 00 00       	call   0x88
  88:	89 45 d8             	mov    DWORD PTR [ebp-0x28],eax
  8b:	db 45 d8             	fild   DWORD PTR [ebp-0x28]
  8e:	dc 05 00 00 00 00    	fadd   QWORD PTR ds:0x0
  94:	d8 4d fc             	fmul   DWORD PTR [ebp-0x4]
  97:	83 3d 00 00 00 00 00 	cmp    DWORD PTR ds:0x0,0x0
  9e:	75 08                	jne    0xa8
  a0:	dc 35 00 00 00 00    	fdiv   QWORD PTR ds:0x0
  a6:	eb 11                	jmp    0xb9
  a8:	ff 35 04 00 00 00    	push   DWORD PTR ds:0x4
  ae:	ff 35 00 00 00 00    	push   DWORD PTR ds:0x0
  b4:	e8 00 00 00 00       	call   0xb9
  b9:	d9 55 e8             	fst    DWORD PTR [ebp-0x18]
  bc:	d8 5d f0             	fcomp  DWORD PTR [ebp-0x10]
  bf:	df e0                	fnstsw ax
  c1:	f6 c4 41             	test   ah,0x41
  c4:	0f 85 12 00 00 00    	jne    0xdc
  ca:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
  cd:	89 45 f8             	mov    DWORD PTR [ebp-0x8],eax
  d0:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
  d3:	89 45 ec             	mov    DWORD PTR [ebp-0x14],eax
  d6:	8b 45 e8             	mov    eax,DWORD PTR [ebp-0x18]
  d9:	89 45 f0             	mov    DWORD PTR [ebp-0x10],eax
  dc:	83 3d 00 00 00 00 05 	cmp    DWORD PTR ds:0x0,0x5
  e3:	0f 8c 39 00 00 00    	jl     0x122
  e9:	d9 45 e8             	fld    DWORD PTR [ebp-0x18]
  ec:	83 ec 08             	sub    esp,0x8
  ef:	dd 1c 24             	fstp   QWORD PTR [esp]
  f2:	d9 45 fc             	fld    DWORD PTR [ebp-0x4]
  f5:	83 ec 08             	sub    esp,0x8
  f8:	dd 1c 24             	fstp   QWORD PTR [esp]
  fb:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
  fe:	50                   	push   eax
  ff:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 102:	0f be 00             	movsx  eax,BYTE PTR [eax]
 105:	50                   	push   eax
 106:	68 00 00 00 00       	push   0x0
 10b:	68 00 00 00 00       	push   0x0
 110:	e8 00 00 00 00       	call   0x115
 115:	83 c4 20             	add    esp,0x20
 118:	b9 00 00 00 00       	mov    ecx,0x0
 11d:	e8 00 00 00 00       	call   0x122
 122:	e9 06 ff ff ff       	jmp    0x2d
 127:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 12a:	8b 4d 0c             	mov    ecx,DWORD PTR [ebp+0xc]
 12d:	89 01                	mov    DWORD PTR [ecx],eax
 12f:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 132:	c7 40 04 01 00 00 00 	mov    DWORD PTR [eax+0x4],0x1
 139:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
 13c:	8b 4d 0c             	mov    ecx,DWORD PTR [ebp+0xc]
 13f:	89 41 08             	mov    DWORD PTR [ecx+0x8],eax
 142:	8b 45 10             	mov    eax,DWORD PTR [ebp+0x10]
 145:	8b 4d ec             	mov    ecx,DWORD PTR [ebp-0x14]
 148:	89 08                	mov    DWORD PTR [eax],ecx
 14a:	e9 00 00 00 00       	jmp    0x14f
 14f:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 152:	33 c9                	xor    ecx,ecx
 154:	8a 48 05             	mov    cl,BYTE PTR [eax+0x5]
 157:	a1 00 00 00 00       	mov    eax,ds:0x0
 15c:	0f af 88 46 0b 00 00 	imul   ecx,DWORD PTR [eax+0xb46]
 163:	8d 04 49             	lea    eax,[ecx+ecx*2]
 166:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
 169:	33 d2                	xor    edx,edx
 16b:	8a 51 04             	mov    dl,BYTE PTR [ecx+0x4]
 16e:	8d 0c 52             	lea    ecx,[edx+edx*2]
 171:	c1 e1 02             	shl    ecx,0x2
 174:	8d 04 81             	lea    eax,[ecx+eax*4]
 177:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
 17d:	8b 89 3e 0b 00 00    	mov    ecx,DWORD PTR [ecx+0xb3e]
 183:	33 d2                	xor    edx,edx
 185:	8a 54 08 09          	mov    dl,BYTE PTR [eax+ecx*1+0x9]
 189:	81 fa aa 00 00 00    	cmp    edx,0xaa
 18f:	0f 85 10 00 00 00    	jne    0x1a5
 195:	8b 45 10             	mov    eax,DWORD PTR [ebp+0x10]
 198:	d9 00                	fld    DWORD PTR [eax]
 19a:	d8 25 00 00 00 00    	fsub   DWORD PTR ds:0x0
 1a0:	8b 45 10             	mov    eax,DWORD PTR [ebp+0x10]
 1a3:	d9 18                	fstp   DWORD PTR [eax]
 1a5:	e9 00 00 00 00       	jmp    0x1aa
 1aa:	5f                   	pop    edi
 1ab:	5e                   	pop    esi
 1ac:	5b                   	pop    ebx
 1ad:	c9                   	leave
 1ae:	c2 0c 00             	ret    0xc
