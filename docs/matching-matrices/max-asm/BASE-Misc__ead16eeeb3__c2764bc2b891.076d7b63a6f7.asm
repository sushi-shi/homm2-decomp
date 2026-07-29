; ?CreatePCXFile@@YIXPADPAEHH1@Z
; unit BASE/Misc  src_hash c2764bc2b891.076d7b63a6f7  score 98.3784  seed 1213156658  trial 5
; size 493  text_sha 691fb6c6be3a9997
; relocs ["0000006c:0014:_open:00000000", "00000090:0014:_write:00000000", "0000009f:0006:_gMiscText$Sdata_data_6163021afbe567546dfbe3447cda28ff79fc46810a3e0fadaec6446606828b4a_0:540b0000", "000000ab:0014:?BaseAlloc@@YIPAXIPADH@Z:00000000", "00000132:0014:_write:00000000", "00000157:0006:_gMiscText$Sdata_data_6163021afbe567546dfbe3447cda28ff79fc46810a3e0fadaec6446606828b4a_0:7c0b0000", "0000015e:0014:?BaseFree@@YIXPAXPADH@Z:00000000", "00000174:0014:_write:00000000", "0000017c:0006:_gMiscText$Sdata_data_6163021afbe567546dfbe3447cda28ff79fc46810a3e0fadaec6446606828b4a_0:a40b0000", "0000018c:0014:?BaseAlloc@@YIPAXIPADH@Z:00000000", "000001ba:0014:_write:00000000", "000001c2:0006:_gMiscText$Sdata_data_6163021afbe567546dfbe3447cda28ff79fc46810a3e0fadaec6446606828b4a_0:cc0b0000", "000001cf:0014:?BaseFree@@YIXPAXPADH@Z:00000000", "000001d9:0014:_close:00000000"]
; hex 81ec8c0000005356578bf2558bd18d7c241c33c0b920000000f3ab668b9c24a0000000b801000000c644241c0a8d4bff8bbc24a400000066894c24248844241e66895c245e8d6fff66896c24268844245d66894424606880000000c644242105c644242308680183000052e8000000008944242083c40c83f8ff0f84600100008d44241c68800000008b4c24185051e8000000008b8c24ac00000083c40cba540b000003c968c8050000e8000000008be885ff0f8e98000000897c241833c033ff398424a00000007e608a0c068bd83b8424a00000007d1d8a141e3ad175168bd32bd04283fa407d0c438b9424a00000003bda7ce32bd883fb017f128ad180e2c080fac07408884c3d004740eb118bd34780cac04703c388543dfe884c3dff8b8c24a00000003bc17ca08b442414575550e8000000008b8424ac0000008b4c242483c40c03f049894c24180f856cffffff68f0050000ba7c0b00008bcde8000000008d4424136a018b4c241850c644241b0c51e80000000083c40cbaa40b0000b9000300009068f6050000e8000000008bf033c98b8424a80000008a140841c0e20281f90003000088540eff7ced8b44241468000300005650e80000000083c40cbacc0b00008bce9068fb050000e8000000008b44241450e80000000083c4045d5f5e5b81c48c000000c20c00

   0:	81 ec 8c 00 00 00    	sub    esp,0x8c
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	8b f2                	mov    esi,edx
   b:	55                   	push   ebp
   c:	8b d1                	mov    edx,ecx
   e:	8d 7c 24 1c          	lea    edi,[esp+0x1c]
  12:	33 c0                	xor    eax,eax
  14:	b9 20 00 00 00       	mov    ecx,0x20
  19:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  1b:	66 8b 9c 24 a0 00 00 	mov    bx,WORD PTR [esp+0xa0]
  22:	00
  23:	b8 01 00 00 00       	mov    eax,0x1
  28:	c6 44 24 1c 0a       	mov    BYTE PTR [esp+0x1c],0xa
  2d:	8d 4b ff             	lea    ecx,[ebx-0x1]
  30:	8b bc 24 a4 00 00 00 	mov    edi,DWORD PTR [esp+0xa4]
  37:	66 89 4c 24 24       	mov    WORD PTR [esp+0x24],cx
  3c:	88 44 24 1e          	mov    BYTE PTR [esp+0x1e],al
  40:	66 89 5c 24 5e       	mov    WORD PTR [esp+0x5e],bx
  45:	8d 6f ff             	lea    ebp,[edi-0x1]
  48:	66 89 6c 24 26       	mov    WORD PTR [esp+0x26],bp
  4d:	88 44 24 5d          	mov    BYTE PTR [esp+0x5d],al
  51:	66 89 44 24 60       	mov    WORD PTR [esp+0x60],ax
  56:	68 80 00 00 00       	push   0x80
  5b:	c6 44 24 21 05       	mov    BYTE PTR [esp+0x21],0x5
  60:	c6 44 24 23 08       	mov    BYTE PTR [esp+0x23],0x8
  65:	68 01 83 00 00       	push   0x8301
  6a:	52                   	push   edx
  6b:	e8 00 00 00 00       	call   0x70
  70:	89 44 24 20          	mov    DWORD PTR [esp+0x20],eax
  74:	83 c4 0c             	add    esp,0xc
  77:	83 f8 ff             	cmp    eax,0xffffffff
  7a:	0f 84 60 01 00 00    	je     0x1e0
  80:	8d 44 24 1c          	lea    eax,[esp+0x1c]
  84:	68 80 00 00 00       	push   0x80
  89:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  8d:	50                   	push   eax
  8e:	51                   	push   ecx
  8f:	e8 00 00 00 00       	call   0x94
  94:	8b 8c 24 ac 00 00 00 	mov    ecx,DWORD PTR [esp+0xac]
  9b:	83 c4 0c             	add    esp,0xc
  9e:	ba 54 0b 00 00       	mov    edx,0xb54
  a3:	03 c9                	add    ecx,ecx
  a5:	68 c8 05 00 00       	push   0x5c8
  aa:	e8 00 00 00 00       	call   0xaf
  af:	8b e8                	mov    ebp,eax
  b1:	85 ff                	test   edi,edi
  b3:	0f 8e 98 00 00 00    	jle    0x151
  b9:	89 7c 24 18          	mov    DWORD PTR [esp+0x18],edi
  bd:	33 c0                	xor    eax,eax
  bf:	33 ff                	xor    edi,edi
  c1:	39 84 24 a0 00 00 00 	cmp    DWORD PTR [esp+0xa0],eax
  c8:	7e 60                	jle    0x12a
  ca:	8a 0c 06             	mov    cl,BYTE PTR [esi+eax*1]
  cd:	8b d8                	mov    ebx,eax
  cf:	3b 84 24 a0 00 00 00 	cmp    eax,DWORD PTR [esp+0xa0]
  d6:	7d 1d                	jge    0xf5
  d8:	8a 14 1e             	mov    dl,BYTE PTR [esi+ebx*1]
  db:	3a d1                	cmp    dl,cl
  dd:	75 16                	jne    0xf5
  df:	8b d3                	mov    edx,ebx
  e1:	2b d0                	sub    edx,eax
  e3:	42                   	inc    edx
  e4:	83 fa 40             	cmp    edx,0x40
  e7:	7d 0c                	jge    0xf5
  e9:	43                   	inc    ebx
  ea:	8b 94 24 a0 00 00 00 	mov    edx,DWORD PTR [esp+0xa0]
  f1:	3b da                	cmp    ebx,edx
  f3:	7c e3                	jl     0xd8
  f5:	2b d8                	sub    ebx,eax
  f7:	83 fb 01             	cmp    ebx,0x1
  fa:	7f 12                	jg     0x10e
  fc:	8a d1                	mov    dl,cl
  fe:	80 e2 c0             	and    dl,0xc0
 101:	80 fa c0             	cmp    dl,0xc0
 104:	74 08                	je     0x10e
 106:	88 4c 3d 00          	mov    BYTE PTR [ebp+edi*1+0x0],cl
 10a:	47                   	inc    edi
 10b:	40                   	inc    eax
 10c:	eb 11                	jmp    0x11f
 10e:	8b d3                	mov    edx,ebx
 110:	47                   	inc    edi
 111:	80 ca c0             	or     dl,0xc0
 114:	47                   	inc    edi
 115:	03 c3                	add    eax,ebx
 117:	88 54 3d fe          	mov    BYTE PTR [ebp+edi*1-0x2],dl
 11b:	88 4c 3d ff          	mov    BYTE PTR [ebp+edi*1-0x1],cl
 11f:	8b 8c 24 a0 00 00 00 	mov    ecx,DWORD PTR [esp+0xa0]
 126:	3b c1                	cmp    eax,ecx
 128:	7c a0                	jl     0xca
 12a:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
 12e:	57                   	push   edi
 12f:	55                   	push   ebp
 130:	50                   	push   eax
 131:	e8 00 00 00 00       	call   0x136
 136:	8b 84 24 ac 00 00 00 	mov    eax,DWORD PTR [esp+0xac]
 13d:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
 141:	83 c4 0c             	add    esp,0xc
 144:	03 f0                	add    esi,eax
 146:	49                   	dec    ecx
 147:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
 14b:	0f 85 6c ff ff ff    	jne    0xbd
 151:	68 f0 05 00 00       	push   0x5f0
 156:	ba 7c 0b 00 00       	mov    edx,0xb7c
 15b:	8b cd                	mov    ecx,ebp
 15d:	e8 00 00 00 00       	call   0x162
 162:	8d 44 24 13          	lea    eax,[esp+0x13]
 166:	6a 01                	push   0x1
 168:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
 16c:	50                   	push   eax
 16d:	c6 44 24 1b 0c       	mov    BYTE PTR [esp+0x1b],0xc
 172:	51                   	push   ecx
 173:	e8 00 00 00 00       	call   0x178
 178:	83 c4 0c             	add    esp,0xc
 17b:	ba a4 0b 00 00       	mov    edx,0xba4
 180:	b9 00 03 00 00       	mov    ecx,0x300
 185:	90                   	nop
 186:	68 f6 05 00 00       	push   0x5f6
 18b:	e8 00 00 00 00       	call   0x190
 190:	8b f0                	mov    esi,eax
 192:	33 c9                	xor    ecx,ecx
 194:	8b 84 24 a8 00 00 00 	mov    eax,DWORD PTR [esp+0xa8]
 19b:	8a 14 08             	mov    dl,BYTE PTR [eax+ecx*1]
 19e:	41                   	inc    ecx
 19f:	c0 e2 02             	shl    dl,0x2
 1a2:	81 f9 00 03 00 00    	cmp    ecx,0x300
 1a8:	88 54 0e ff          	mov    BYTE PTR [esi+ecx*1-0x1],dl
 1ac:	7c ed                	jl     0x19b
 1ae:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
 1b2:	68 00 03 00 00       	push   0x300
 1b7:	56                   	push   esi
 1b8:	50                   	push   eax
 1b9:	e8 00 00 00 00       	call   0x1be
 1be:	83 c4 0c             	add    esp,0xc
 1c1:	ba cc 0b 00 00       	mov    edx,0xbcc
 1c6:	8b ce                	mov    ecx,esi
 1c8:	90                   	nop
 1c9:	68 fb 05 00 00       	push   0x5fb
 1ce:	e8 00 00 00 00       	call   0x1d3
 1d3:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
 1d7:	50                   	push   eax
 1d8:	e8 00 00 00 00       	call   0x1dd
 1dd:	83 c4 04             	add    esp,0x4
 1e0:	5d                   	pop    ebp
 1e1:	5f                   	pop    edi
 1e2:	5e                   	pop    esi
 1e3:	5b                   	pop    ebx
 1e4:	81 c4 8c 00 00 00    	add    esp,0x8c
 1ea:	c2 0c 00             	ret    0xc
