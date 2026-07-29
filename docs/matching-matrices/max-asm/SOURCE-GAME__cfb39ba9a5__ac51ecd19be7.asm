; ?RestoreCell@game@@QAEXHHHHPAVmapCell@@H@Z
; unit SOURCE/GAME  src_hash ac51ecd19be7  score 100.0000  seed 1213156658  trial 1
; size 181  text_sha 5f70a09aa67a414a
; relocs ["0000002b:0006:?gpAdvManager@@3PAVadvManager@@A:00000000", "00000030:0014:?GetCell@advManager@@QAEPAVmapCell@@HH@Z:00000000", "00000059:0006:?gpAdvManager@@3PAVadvManager@@A:00000000", "0000005e:0014:?GetCell@advManager@@QAEPAVmapCell@@HH@Z:00000000"]
; hex 558bec83ec08535657894df8837d18000f840b0000008b45188945fce9160000008b450c508b4508508b0d00000000e8000000008945fc837d0c000f8e43000000817d10a30000000f85360000008b450c48508b4508508b0d00000000e80000000033c98a480983f9230f84140000008b45fcc64009008b45fc6683600407e92a0000008a45108b4dfc8841098b45fc668b4004662507008b4d1466c1e103660bc18b4dfc66894104e9000000005f5e5bc9c21800

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 08             	sub    esp,0x8
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 4d f8             	mov    DWORD PTR [ebp-0x8],ecx
   c:	83 7d 18 00          	cmp    DWORD PTR [ebp+0x18],0x0
  10:	0f 84 0b 00 00 00    	je     0x21
  16:	8b 45 18             	mov    eax,DWORD PTR [ebp+0x18]
  19:	89 45 fc             	mov    DWORD PTR [ebp-0x4],eax
  1c:	e9 16 00 00 00       	jmp    0x37
  21:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
  24:	50                   	push   eax
  25:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  28:	50                   	push   eax
  29:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
  2f:	e8 00 00 00 00       	call   0x34
  34:	89 45 fc             	mov    DWORD PTR [ebp-0x4],eax
  37:	83 7d 0c 00          	cmp    DWORD PTR [ebp+0xc],0x0
  3b:	0f 8e 43 00 00 00    	jle    0x84
  41:	81 7d 10 a3 00 00 00 	cmp    DWORD PTR [ebp+0x10],0xa3
  48:	0f 85 36 00 00 00    	jne    0x84
  4e:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
  51:	48                   	dec    eax
  52:	50                   	push   eax
  53:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  56:	50                   	push   eax
  57:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
  5d:	e8 00 00 00 00       	call   0x62
  62:	33 c9                	xor    ecx,ecx
  64:	8a 48 09             	mov    cl,BYTE PTR [eax+0x9]
  67:	83 f9 23             	cmp    ecx,0x23
  6a:	0f 84 14 00 00 00    	je     0x84
  70:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
  73:	c6 40 09 00          	mov    BYTE PTR [eax+0x9],0x0
  77:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
  7a:	66 83 60 04 07       	and    WORD PTR [eax+0x4],0x7
  7f:	e9 2a 00 00 00       	jmp    0xae
  84:	8a 45 10             	mov    al,BYTE PTR [ebp+0x10]
  87:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
  8a:	88 41 09             	mov    BYTE PTR [ecx+0x9],al
  8d:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
  90:	66 8b 40 04          	mov    ax,WORD PTR [eax+0x4]
  94:	66 25 07 00          	and    ax,0x7
  98:	8b 4d 14             	mov    ecx,DWORD PTR [ebp+0x14]
  9b:	66 c1 e1 03          	shl    cx,0x3
  9f:	66 0b c1             	or     ax,cx
  a2:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
  a5:	66 89 41 04          	mov    WORD PTR [ecx+0x4],ax
  a9:	e9 00 00 00 00       	jmp    0xae
  ae:	5f                   	pop    edi
  af:	5e                   	pop    esi
  b0:	5b                   	pop    ebx
  b1:	c9                   	leave
  b2:	c2 18 00             	ret    0x18
