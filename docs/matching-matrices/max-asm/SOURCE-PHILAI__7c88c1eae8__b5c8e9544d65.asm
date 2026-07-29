; ?GetBestBuilding@philAI@@QAEXPAVtown@@AAUBHC@@AAM@Z
; unit SOURCE/PHILAI  src_hash b5c8e9544d65  score 100.0000  seed 1213156658  trial 1
; size 411  text_sha 2bbbd898f138402c
; relocs ["00000071:0014:?CanBuild@@YIHPAVtown@@H@Z:00000000", "00000091:0014:?ValueOfBuyingBuilding@philAI@@QAEXPAVtown@@HAAHAAM@Z:00000000", "00000096:0006:?gpCurPlayer@@3PAVplayerData@@A:00000000", "000000af:0006:__h2cg$SOURCE$PHILAI$data$getBestBuildingConstant:00000000", "000000b4:0014:__ftol:00000000", "000000c6:0014:?Random@@YIHHH@Z:00000000", "000000d8:0006:__adjust_fdiv:00000000", "000000e1:0006:__h2cg$SOURCE$PHILAI$data$getBestBHCConstant3:00000000", "000000e9:0006:__h2cg$SOURCE$PHILAI$data$getBestBHCConstant3:00000000", "000000ee:0014:__adj_fdiv_m32:00000000", "00000117:0006:?giDebugLevel@@3HA:00000000", "0000013f:0014:?GetBuildingName@@YIPADHH@Z:00000000", "0000014c:0006:__h2cg$SOURCE$PHILAI$data$getBestBuildingTown2dBuilding18sRawBC:00000000", "00000151:0006:?gText@@3PADA:00000000", "00000156:0014:_sprintf:00000000", "0000015e:0006:?gText@@3PADA:00000000", "00000163:0014:?LogStr@@YIXPAD@Z:00000000"]
; hex 558bec83ec28535657894de0c745f00000c6c2c745f40000c6c2c745f8ffffffffc745ec00000000e903000000ff45ec837dec200f8d320100008b4508ba010000008a4decd3e28550180f841a000000837dec000f850d0100008b45080fbe401c83f8050f8dfd0000008b55ec8b4d08e80000000085c00f84ea0000008d45fc508d45e4508b45ec508b4508508b4de0e800000000a10000000083780f010f85170000008b45e48945dcdb45dcdc0d00000000e8000000008945e4ba05000000b901000000e80000000083c05f8945d8db45d8d84dfc833d00000000007508d83500000000eb0bff3500000000e800000000d955e8d85df4dfe0f6c4410f85120000008b45ec8945f88b45fc8945f08b45e88945f4833d00000000050f8c45000000d945e883ec08dd1c24d945fc83ec08dd1c248b45088b55ec0fbe4803e800000000508b45080fbe005068000000006800000000e80000000083c420b900000000e800000000e9c1feffff8b45088b4d0c89018b450cc74004000000008b45f88b4d0c8941088b45108b4df08908e9000000005f5e5bc9c20c00

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 28             	sub    esp,0x28
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 4d e0             	mov    DWORD PTR [ebp-0x20],ecx
   c:	c7 45 f0 00 00 c6 c2 	mov    DWORD PTR [ebp-0x10],0xc2c60000
  13:	c7 45 f4 00 00 c6 c2 	mov    DWORD PTR [ebp-0xc],0xc2c60000
  1a:	c7 45 f8 ff ff ff ff 	mov    DWORD PTR [ebp-0x8],0xffffffff
  21:	c7 45 ec 00 00 00 00 	mov    DWORD PTR [ebp-0x14],0x0
  28:	e9 03 00 00 00       	jmp    0x30
  2d:	ff 45 ec             	inc    DWORD PTR [ebp-0x14]
  30:	83 7d ec 20          	cmp    DWORD PTR [ebp-0x14],0x20
  34:	0f 8d 32 01 00 00    	jge    0x16c
  3a:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  3d:	ba 01 00 00 00       	mov    edx,0x1
  42:	8a 4d ec             	mov    cl,BYTE PTR [ebp-0x14]
  45:	d3 e2                	shl    edx,cl
  47:	85 50 18             	test   DWORD PTR [eax+0x18],edx
  4a:	0f 84 1a 00 00 00    	je     0x6a
  50:	83 7d ec 00          	cmp    DWORD PTR [ebp-0x14],0x0
  54:	0f 85 0d 01 00 00    	jne    0x167
  5a:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  5d:	0f be 40 1c          	movsx  eax,BYTE PTR [eax+0x1c]
  61:	83 f8 05             	cmp    eax,0x5
  64:	0f 8d fd 00 00 00    	jge    0x167
  6a:	8b 55 ec             	mov    edx,DWORD PTR [ebp-0x14]
  6d:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  70:	e8 00 00 00 00       	call   0x75
  75:	85 c0                	test   eax,eax
  77:	0f 84 ea 00 00 00    	je     0x167
  7d:	8d 45 fc             	lea    eax,[ebp-0x4]
  80:	50                   	push   eax
  81:	8d 45 e4             	lea    eax,[ebp-0x1c]
  84:	50                   	push   eax
  85:	8b 45 ec             	mov    eax,DWORD PTR [ebp-0x14]
  88:	50                   	push   eax
  89:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  8c:	50                   	push   eax
  8d:	8b 4d e0             	mov    ecx,DWORD PTR [ebp-0x20]
  90:	e8 00 00 00 00       	call   0x95
  95:	a1 00 00 00 00       	mov    eax,ds:0x0
  9a:	83 78 0f 01          	cmp    DWORD PTR [eax+0xf],0x1
  9e:	0f 85 17 00 00 00    	jne    0xbb
  a4:	8b 45 e4             	mov    eax,DWORD PTR [ebp-0x1c]
  a7:	89 45 dc             	mov    DWORD PTR [ebp-0x24],eax
  aa:	db 45 dc             	fild   DWORD PTR [ebp-0x24]
  ad:	dc 0d 00 00 00 00    	fmul   QWORD PTR ds:0x0
  b3:	e8 00 00 00 00       	call   0xb8
  b8:	89 45 e4             	mov    DWORD PTR [ebp-0x1c],eax
  bb:	ba 05 00 00 00       	mov    edx,0x5
  c0:	b9 01 00 00 00       	mov    ecx,0x1
  c5:	e8 00 00 00 00       	call   0xca
  ca:	83 c0 5f             	add    eax,0x5f
  cd:	89 45 d8             	mov    DWORD PTR [ebp-0x28],eax
  d0:	db 45 d8             	fild   DWORD PTR [ebp-0x28]
  d3:	d8 4d fc             	fmul   DWORD PTR [ebp-0x4]
  d6:	83 3d 00 00 00 00 00 	cmp    DWORD PTR ds:0x0,0x0
  dd:	75 08                	jne    0xe7
  df:	d8 35 00 00 00 00    	fdiv   DWORD PTR ds:0x0
  e5:	eb 0b                	jmp    0xf2
  e7:	ff 35 00 00 00 00    	push   DWORD PTR ds:0x0
  ed:	e8 00 00 00 00       	call   0xf2
  f2:	d9 55 e8             	fst    DWORD PTR [ebp-0x18]
  f5:	d8 5d f4             	fcomp  DWORD PTR [ebp-0xc]
  f8:	df e0                	fnstsw ax
  fa:	f6 c4 41             	test   ah,0x41
  fd:	0f 85 12 00 00 00    	jne    0x115
 103:	8b 45 ec             	mov    eax,DWORD PTR [ebp-0x14]
 106:	89 45 f8             	mov    DWORD PTR [ebp-0x8],eax
 109:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
 10c:	89 45 f0             	mov    DWORD PTR [ebp-0x10],eax
 10f:	8b 45 e8             	mov    eax,DWORD PTR [ebp-0x18]
 112:	89 45 f4             	mov    DWORD PTR [ebp-0xc],eax
 115:	83 3d 00 00 00 00 05 	cmp    DWORD PTR ds:0x0,0x5
 11c:	0f 8c 45 00 00 00    	jl     0x167
 122:	d9 45 e8             	fld    DWORD PTR [ebp-0x18]
 125:	83 ec 08             	sub    esp,0x8
 128:	dd 1c 24             	fstp   QWORD PTR [esp]
 12b:	d9 45 fc             	fld    DWORD PTR [ebp-0x4]
 12e:	83 ec 08             	sub    esp,0x8
 131:	dd 1c 24             	fstp   QWORD PTR [esp]
 134:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 137:	8b 55 ec             	mov    edx,DWORD PTR [ebp-0x14]
 13a:	0f be 48 03          	movsx  ecx,BYTE PTR [eax+0x3]
 13e:	e8 00 00 00 00       	call   0x143
 143:	50                   	push   eax
 144:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 147:	0f be 00             	movsx  eax,BYTE PTR [eax]
 14a:	50                   	push   eax
 14b:	68 00 00 00 00       	push   0x0
 150:	68 00 00 00 00       	push   0x0
 155:	e8 00 00 00 00       	call   0x15a
 15a:	83 c4 20             	add    esp,0x20
 15d:	b9 00 00 00 00       	mov    ecx,0x0
 162:	e8 00 00 00 00       	call   0x167
 167:	e9 c1 fe ff ff       	jmp    0x2d
 16c:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 16f:	8b 4d 0c             	mov    ecx,DWORD PTR [ebp+0xc]
 172:	89 01                	mov    DWORD PTR [ecx],eax
 174:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 177:	c7 40 04 00 00 00 00 	mov    DWORD PTR [eax+0x4],0x0
 17e:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
 181:	8b 4d 0c             	mov    ecx,DWORD PTR [ebp+0xc]
 184:	89 41 08             	mov    DWORD PTR [ecx+0x8],eax
 187:	8b 45 10             	mov    eax,DWORD PTR [ebp+0x10]
 18a:	8b 4d f0             	mov    ecx,DWORD PTR [ebp-0x10]
 18d:	89 08                	mov    DWORD PTR [eax],ecx
 18f:	e9 00 00 00 00       	jmp    0x194
 194:	5f                   	pop    edi
 195:	5e                   	pop    esi
 196:	5b                   	pop    ebx
 197:	c9                   	leave
 198:	c2 0c 00             	ret    0xc
