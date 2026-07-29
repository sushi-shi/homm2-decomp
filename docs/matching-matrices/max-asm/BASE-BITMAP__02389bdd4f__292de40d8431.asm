; ?DrawToBufferCareful@bitmap@@QAEXFF@Z
; unit BASE/BITMAP  src_hash 292de40d8431  score 100.0000  seed 1112359248  trial 311
; size 255  text_sha 0823f47b5534d394
; relocs ["00000014:0006:?gpWindowManager@@3PAVheroWindowManager@@A:00000000", "00000048:0014:@PollSound@0:00000000", "0000004e:0006:?gpWindowManager@@3PAVheroWindowManager@@A:00000000", "00000068:0014:?BlitBitmap@@YIXPAVbitmap@@HHHH0HH@Z:00000000", "0000006d:0014:@PollSound@0:00000000", "000000ad:0006:?gpWindowManager@@3PAVheroWindowManager@@A:00000000", "000000f1:0014:?BlitBitmap@@YIXPAVbitmap@@HHHH0HH@Z:00000000"]
; hex 83ec085356578bf1668b4c2418556685c97c68a1000000000fbff98b50460fbf46120fbf5a1203c73bc3668b4424207f4f6685c07c4a0fbfd80fbf6e140fbf521403eb3bea7f39e80000000053a100000000578b48460fbf56140fbf461251525033d26a008bcee800000000e8000000005d5f5e5b83c408c20800668b4424200fbf56126685c90fbf7e140fbfc97d0403d133c96685c00fbfc07d0cc74424100000000003f8eb0489442410a1000000008d2c118b58460fbf43123be87e042bc18bd00fbf43148b6c241003ef3be87e088bf88b4424102bf885d27c1885ff7c148b4424105051538bce57526a0033d2e8000000005d5f5e5b83c408c20800

   0:	83 ec 08             	sub    esp,0x8
   3:	53                   	push   ebx
   4:	56                   	push   esi
   5:	57                   	push   edi
   6:	8b f1                	mov    esi,ecx
   8:	66 8b 4c 24 18       	mov    cx,WORD PTR [esp+0x18]
   d:	55                   	push   ebp
   e:	66 85 c9             	test   cx,cx
  11:	7c 68                	jl     0x7b
  13:	a1 00 00 00 00       	mov    eax,ds:0x0
  18:	0f bf f9             	movsx  edi,cx
  1b:	8b 50 46             	mov    edx,DWORD PTR [eax+0x46]
  1e:	0f bf 46 12          	movsx  eax,WORD PTR [esi+0x12]
  22:	0f bf 5a 12          	movsx  ebx,WORD PTR [edx+0x12]
  26:	03 c7                	add    eax,edi
  28:	3b c3                	cmp    eax,ebx
  2a:	66 8b 44 24 20       	mov    ax,WORD PTR [esp+0x20]
  2f:	7f 4f                	jg     0x80
  31:	66 85 c0             	test   ax,ax
  34:	7c 4a                	jl     0x80
  36:	0f bf d8             	movsx  ebx,ax
  39:	0f bf 6e 14          	movsx  ebp,WORD PTR [esi+0x14]
  3d:	0f bf 52 14          	movsx  edx,WORD PTR [edx+0x14]
  41:	03 eb                	add    ebp,ebx
  43:	3b ea                	cmp    ebp,edx
  45:	7f 39                	jg     0x80
  47:	e8 00 00 00 00       	call   0x4c
  4c:	53                   	push   ebx
  4d:	a1 00 00 00 00       	mov    eax,ds:0x0
  52:	57                   	push   edi
  53:	8b 48 46             	mov    ecx,DWORD PTR [eax+0x46]
  56:	0f bf 56 14          	movsx  edx,WORD PTR [esi+0x14]
  5a:	0f bf 46 12          	movsx  eax,WORD PTR [esi+0x12]
  5e:	51                   	push   ecx
  5f:	52                   	push   edx
  60:	50                   	push   eax
  61:	33 d2                	xor    edx,edx
  63:	6a 00                	push   0x0
  65:	8b ce                	mov    ecx,esi
  67:	e8 00 00 00 00       	call   0x6c
  6c:	e8 00 00 00 00       	call   0x71
  71:	5d                   	pop    ebp
  72:	5f                   	pop    edi
  73:	5e                   	pop    esi
  74:	5b                   	pop    ebx
  75:	83 c4 08             	add    esp,0x8
  78:	c2 08 00             	ret    0x8
  7b:	66 8b 44 24 20       	mov    ax,WORD PTR [esp+0x20]
  80:	0f bf 56 12          	movsx  edx,WORD PTR [esi+0x12]
  84:	66 85 c9             	test   cx,cx
  87:	0f bf 7e 14          	movsx  edi,WORD PTR [esi+0x14]
  8b:	0f bf c9             	movsx  ecx,cx
  8e:	7d 04                	jge    0x94
  90:	03 d1                	add    edx,ecx
  92:	33 c9                	xor    ecx,ecx
  94:	66 85 c0             	test   ax,ax
  97:	0f bf c0             	movsx  eax,ax
  9a:	7d 0c                	jge    0xa8
  9c:	c7 44 24 10 00 00 00 	mov    DWORD PTR [esp+0x10],0x0
  a3:	00 
  a4:	03 f8                	add    edi,eax
  a6:	eb 04                	jmp    0xac
  a8:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  ac:	a1 00 00 00 00       	mov    eax,ds:0x0
  b1:	8d 2c 11             	lea    ebp,[ecx+edx*1]
  b4:	8b 58 46             	mov    ebx,DWORD PTR [eax+0x46]
  b7:	0f bf 43 12          	movsx  eax,WORD PTR [ebx+0x12]
  bb:	3b e8                	cmp    ebp,eax
  bd:	7e 04                	jle    0xc3
  bf:	2b c1                	sub    eax,ecx
  c1:	8b d0                	mov    edx,eax
  c3:	0f bf 43 14          	movsx  eax,WORD PTR [ebx+0x14]
  c7:	8b 6c 24 10          	mov    ebp,DWORD PTR [esp+0x10]
  cb:	03 ef                	add    ebp,edi
  cd:	3b e8                	cmp    ebp,eax
  cf:	7e 08                	jle    0xd9
  d1:	8b f8                	mov    edi,eax
  d3:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  d7:	2b f8                	sub    edi,eax
  d9:	85 d2                	test   edx,edx
  db:	7c 18                	jl     0xf5
  dd:	85 ff                	test   edi,edi
  df:	7c 14                	jl     0xf5
  e1:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  e5:	50                   	push   eax
  e6:	51                   	push   ecx
  e7:	53                   	push   ebx
  e8:	8b ce                	mov    ecx,esi
  ea:	57                   	push   edi
  eb:	52                   	push   edx
  ec:	6a 00                	push   0x0
  ee:	33 d2                	xor    edx,edx
  f0:	e8 00 00 00 00       	call   0xf5
  f5:	5d                   	pop    ebp
  f6:	5f                   	pop    edi
  f7:	5e                   	pop    esi
  f8:	5b                   	pop    ebx
  f9:	83 c4 08             	add    esp,0x8
  fc:	c2 08 00             	ret    0x8
