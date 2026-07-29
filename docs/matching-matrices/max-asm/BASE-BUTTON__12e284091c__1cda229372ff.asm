; ?Select@button@@QAEFAAUtag_message@@@Z
; unit BASE/BUTTON  src_hash 1cda229372ff  score 100.0000  seed 1213156658  trial 20
; size 150  text_sha a8957aa9e7445a0f
; relocs ["0000002a:0014:?DrawToBuffer@icon@@QAEXHHHH@Z:00000000", "0000003b:0006:?gpWindowManager@@3PAVheroWindowManager@@A:00000000", "00000041:0014:?UpdateScreenRegion@heroWindowManager@@QAEXHHHH@Z:00000000", "00000070:0014:?KBTickCount@@YIJXZ:00000000", "0000007d:0006:?glTimers@@3PAHA:08000000", "0000008f:0006:?iLeftRightSave@@3HA:00000000"]
; hex 53568b4104578bf16a00668b4e1a8b5028660356180fbffa6603482c0fbfd90fbf4626508b4e205357e8000000000fbf461e0fbf4e1c5051538b0d0000000057e8000000008b7c2410804e1601c707000200000fbf4e10894f0866837e2801c747040a0000007407c747040c000000e8000000008d483c66b80200890d080000008b4f0c5f81e1000300005e5b890d00000000c20400

   0:	53                   	push   ebx
   1:	56                   	push   esi
   2:	8b 41 04             	mov    eax,DWORD PTR [ecx+0x4]
   5:	57                   	push   edi
   6:	8b f1                	mov    esi,ecx
   8:	6a 00                	push   0x0
   a:	66 8b 4e 1a          	mov    cx,WORD PTR [esi+0x1a]
   e:	8b 50 28             	mov    edx,DWORD PTR [eax+0x28]
  11:	66 03 56 18          	add    dx,WORD PTR [esi+0x18]
  15:	0f bf fa             	movsx  edi,dx
  18:	66 03 48 2c          	add    cx,WORD PTR [eax+0x2c]
  1c:	0f bf d9             	movsx  ebx,cx
  1f:	0f bf 46 26          	movsx  eax,WORD PTR [esi+0x26]
  23:	50                   	push   eax
  24:	8b 4e 20             	mov    ecx,DWORD PTR [esi+0x20]
  27:	53                   	push   ebx
  28:	57                   	push   edi
  29:	e8 00 00 00 00       	call   0x2e
  2e:	0f bf 46 1e          	movsx  eax,WORD PTR [esi+0x1e]
  32:	0f bf 4e 1c          	movsx  ecx,WORD PTR [esi+0x1c]
  36:	50                   	push   eax
  37:	51                   	push   ecx
  38:	53                   	push   ebx
  39:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
  3f:	57                   	push   edi
  40:	e8 00 00 00 00       	call   0x45
  45:	8b 7c 24 10          	mov    edi,DWORD PTR [esp+0x10]
  49:	80 4e 16 01          	or     BYTE PTR [esi+0x16],0x1
  4d:	c7 07 00 02 00 00    	mov    DWORD PTR [edi],0x200
  53:	0f bf 4e 10          	movsx  ecx,WORD PTR [esi+0x10]
  57:	89 4f 08             	mov    DWORD PTR [edi+0x8],ecx
  5a:	66 83 7e 28 01       	cmp    WORD PTR [esi+0x28],0x1
  5f:	c7 47 04 0a 00 00 00 	mov    DWORD PTR [edi+0x4],0xa
  66:	74 07                	je     0x6f
  68:	c7 47 04 0c 00 00 00 	mov    DWORD PTR [edi+0x4],0xc
  6f:	e8 00 00 00 00       	call   0x74
  74:	8d 48 3c             	lea    ecx,[eax+0x3c]
  77:	66 b8 02 00          	mov    ax,0x2
  7b:	89 0d 08 00 00 00    	mov    DWORD PTR ds:0x8,ecx
  81:	8b 4f 0c             	mov    ecx,DWORD PTR [edi+0xc]
  84:	5f                   	pop    edi
  85:	81 e1 00 03 00 00    	and    ecx,0x300
  8b:	5e                   	pop    esi
  8c:	5b                   	pop    ebx
  8d:	89 0d 00 00 00 00    	mov    DWORD PTR ds:0x0,ecx
  93:	c2 04 00             	ret    0x4
