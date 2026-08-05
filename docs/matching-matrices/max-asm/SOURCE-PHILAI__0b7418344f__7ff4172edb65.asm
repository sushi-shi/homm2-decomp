; ?TurnCostResource@philAI@@QAEXH@Z
; unit SOURCE/PHILAI  src_hash 7ff4172edb65  score 100.0000  seed 1213156658  trial 1
; size 384  text_sha ff2bbdb57cd5e106
; relocs ["00000020:0006:?gpGame@@3PAVgame@@A:00000000", "00000061:0006:__h2cg$SOURCE$PHILAI$data$turnCostResourceConstant:00000000", "0000007f:0006:?gpGame@@3PAVgame@@A:00000000", "00000098:0006:?gResourceBaseValue@@3PAHA:00000000", "000000a5:0014:__ftol:00000000", "000000fb:0006:__adjust_fdiv:00000000", "0000010b:0014:__adj_fdiv_m32:00000000", "0000011f:0006:__adjust_fdiv:00000000", "00000128:0006:__h2cg$SOURCE$PHILAI$data$valueOfBuyingBuildingConstant3:00000000", "00000130:0006:__h2cg$SOURCE$PHILAI$data$valueOfBuyingBuildingConstant3:00000000", "00000135:0014:__adj_fdiv_m32:00000000", "0000013b:0006:__h2cg$SOURCE$PHILAI$data$getTurnAIVarsConstant3:00000000", "00000145:0006:?gResourceBaseValue@@3PAHA:00000000", "00000151:0006:__adjust_fdiv:00000000", "00000160:0014:__adj_fdiv_r:00000000", "0000016b:0006:?gafAITurnCostResource@@3PAMA:00000000"]
; hex 558bec83ec64535657894db48b45088bc8c1e0038d04c02bc1c1e0022bc1030500000000054f0500008945dcc745d400000000c745d800000000e903000000ff45d8837dd8070f8d730000008b45d88b4ddc8b4481348d04808945b0db45b0dc0d000000008b45d88b4d088bd1c1e1038d0cc92bcac1e1022bca8d04818b0d000000008b84082b0500008945acdb45acdec18b45d88b0485000000008945a8db45a8dec9e8000000008b4dd889448db88b45d88b4485b80145d4e980ffffffb9070000008b45d499f7f98945a4db45a4d95dfcc745d800000000e903000000ff45d8837dd8070f8d880000008b45d88b4485b88945a0db45a0833d00000000007505d875fceb08ff75fce8000000008b45d8d95c85e08b45d8d94485e0833d00000000007508d83500000000eb0bff3500000000e800000000dc05000000008b45d88b04850000000089459cdb459c833d00000000007504def1eb0950b00fe800000000588b45d8d91c8500000000e96bffffffe9000000005f5e5bc9c20400

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 64             	sub    esp,0x64
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 4d b4             	mov    DWORD PTR [ebp-0x4c],ecx
   c:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
   f:	8b c8                	mov    ecx,eax
  11:	c1 e0 03             	shl    eax,0x3
  14:	8d 04 c0             	lea    eax,[eax+eax*8]
  17:	2b c1                	sub    eax,ecx
  19:	c1 e0 02             	shl    eax,0x2
  1c:	2b c1                	sub    eax,ecx
  1e:	03 05 00 00 00 00    	add    eax,DWORD PTR ds:0x0
  24:	05 4f 05 00 00       	add    eax,0x54f
  29:	89 45 dc             	mov    DWORD PTR [ebp-0x24],eax
  2c:	c7 45 d4 00 00 00 00 	mov    DWORD PTR [ebp-0x2c],0x0
  33:	c7 45 d8 00 00 00 00 	mov    DWORD PTR [ebp-0x28],0x0
  3a:	e9 03 00 00 00       	jmp    0x42
  3f:	ff 45 d8             	inc    DWORD PTR [ebp-0x28]
  42:	83 7d d8 07          	cmp    DWORD PTR [ebp-0x28],0x7
  46:	0f 8d 73 00 00 00    	jge    0xbf
  4c:	8b 45 d8             	mov    eax,DWORD PTR [ebp-0x28]
  4f:	8b 4d dc             	mov    ecx,DWORD PTR [ebp-0x24]
  52:	8b 44 81 34          	mov    eax,DWORD PTR [ecx+eax*4+0x34]
  56:	8d 04 80             	lea    eax,[eax+eax*4]
  59:	89 45 b0             	mov    DWORD PTR [ebp-0x50],eax
  5c:	db 45 b0             	fild   DWORD PTR [ebp-0x50]
  5f:	dc 0d 00 00 00 00    	fmul   QWORD PTR ds:0x0
  65:	8b 45 d8             	mov    eax,DWORD PTR [ebp-0x28]
  68:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  6b:	8b d1                	mov    edx,ecx
  6d:	c1 e1 03             	shl    ecx,0x3
  70:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  73:	2b ca                	sub    ecx,edx
  75:	c1 e1 02             	shl    ecx,0x2
  78:	2b ca                	sub    ecx,edx
  7a:	8d 04 81             	lea    eax,[ecx+eax*4]
  7d:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
  83:	8b 84 08 2b 05 00 00 	mov    eax,DWORD PTR [eax+ecx*1+0x52b]
  8a:	89 45 ac             	mov    DWORD PTR [ebp-0x54],eax
  8d:	db 45 ac             	fild   DWORD PTR [ebp-0x54]
  90:	de c1                	faddp  st(1),st
  92:	8b 45 d8             	mov    eax,DWORD PTR [ebp-0x28]
  95:	8b 04 85 00 00 00 00 	mov    eax,DWORD PTR [eax*4+0x0]
  9c:	89 45 a8             	mov    DWORD PTR [ebp-0x58],eax
  9f:	db 45 a8             	fild   DWORD PTR [ebp-0x58]
  a2:	de c9                	fmulp  st(1),st
  a4:	e8 00 00 00 00       	call   0xa9
  a9:	8b 4d d8             	mov    ecx,DWORD PTR [ebp-0x28]
  ac:	89 44 8d b8          	mov    DWORD PTR [ebp+ecx*4-0x48],eax
  b0:	8b 45 d8             	mov    eax,DWORD PTR [ebp-0x28]
  b3:	8b 44 85 b8          	mov    eax,DWORD PTR [ebp+eax*4-0x48]
  b7:	01 45 d4             	add    DWORD PTR [ebp-0x2c],eax
  ba:	e9 80 ff ff ff       	jmp    0x3f
  bf:	b9 07 00 00 00       	mov    ecx,0x7
  c4:	8b 45 d4             	mov    eax,DWORD PTR [ebp-0x2c]
  c7:	99                   	cdq
  c8:	f7 f9                	idiv   ecx
  ca:	89 45 a4             	mov    DWORD PTR [ebp-0x5c],eax
  cd:	db 45 a4             	fild   DWORD PTR [ebp-0x5c]
  d0:	d9 5d fc             	fstp   DWORD PTR [ebp-0x4]
  d3:	c7 45 d8 00 00 00 00 	mov    DWORD PTR [ebp-0x28],0x0
  da:	e9 03 00 00 00       	jmp    0xe2
  df:	ff 45 d8             	inc    DWORD PTR [ebp-0x28]
  e2:	83 7d d8 07          	cmp    DWORD PTR [ebp-0x28],0x7
  e6:	0f 8d 88 00 00 00    	jge    0x174
  ec:	8b 45 d8             	mov    eax,DWORD PTR [ebp-0x28]
  ef:	8b 44 85 b8          	mov    eax,DWORD PTR [ebp+eax*4-0x48]
  f3:	89 45 a0             	mov    DWORD PTR [ebp-0x60],eax
  f6:	db 45 a0             	fild   DWORD PTR [ebp-0x60]
  f9:	83 3d 00 00 00 00 00 	cmp    DWORD PTR ds:0x0,0x0
 100:	75 05                	jne    0x107
 102:	d8 75 fc             	fdiv   DWORD PTR [ebp-0x4]
 105:	eb 08                	jmp    0x10f
 107:	ff 75 fc             	push   DWORD PTR [ebp-0x4]
 10a:	e8 00 00 00 00       	call   0x10f
 10f:	8b 45 d8             	mov    eax,DWORD PTR [ebp-0x28]
 112:	d9 5c 85 e0          	fstp   DWORD PTR [ebp+eax*4-0x20]
 116:	8b 45 d8             	mov    eax,DWORD PTR [ebp-0x28]
 119:	d9 44 85 e0          	fld    DWORD PTR [ebp+eax*4-0x20]
 11d:	83 3d 00 00 00 00 00 	cmp    DWORD PTR ds:0x0,0x0
 124:	75 08                	jne    0x12e
 126:	d8 35 00 00 00 00    	fdiv   DWORD PTR ds:0x0
 12c:	eb 0b                	jmp    0x139
 12e:	ff 35 00 00 00 00    	push   DWORD PTR ds:0x0
 134:	e8 00 00 00 00       	call   0x139
 139:	dc 05 00 00 00 00    	fadd   QWORD PTR ds:0x0
 13f:	8b 45 d8             	mov    eax,DWORD PTR [ebp-0x28]
 142:	8b 04 85 00 00 00 00 	mov    eax,DWORD PTR [eax*4+0x0]
 149:	89 45 9c             	mov    DWORD PTR [ebp-0x64],eax
 14c:	db 45 9c             	fild   DWORD PTR [ebp-0x64]
 14f:	83 3d 00 00 00 00 00 	cmp    DWORD PTR ds:0x0,0x0
 156:	75 04                	jne    0x15c
 158:	de f1                	fdivrp st(1),st
 15a:	eb 09                	jmp    0x165
 15c:	50                   	push   eax
 15d:	b0 0f                	mov    al,0xf
 15f:	e8 00 00 00 00       	call   0x164
 164:	58                   	pop    eax
 165:	8b 45 d8             	mov    eax,DWORD PTR [ebp-0x28]
 168:	d9 1c 85 00 00 00 00 	fstp   DWORD PTR [eax*4+0x0]
 16f:	e9 6b ff ff ff       	jmp    0xdf
 174:	e9 00 00 00 00       	jmp    0x179
 179:	5f                   	pop    edi
 17a:	5e                   	pop    esi
 17b:	5b                   	pop    ebx
 17c:	c9                   	leave
 17d:	c2 04 00             	ret    0x4
