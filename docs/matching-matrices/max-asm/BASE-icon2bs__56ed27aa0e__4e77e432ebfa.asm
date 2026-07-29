; ?IconToBitmapScale@@YIXPAVicon@@PAVbitmap@@HHHHHHHHH@Z
; unit BASE/icon2bs  src_hash 4e77e432ebfa  score 85.6312  seed 1228034883  trial 2
; size 363  text_sha 835401da477ec331
; relocs ["00000047:0014:?IconToBitmap@@YIXPAVicon@@PAVbitmap@@HHHHHHHHH@Z:00000000", "00000084:0014:??2@YAPAXI@Z:00000000", "00000098:0014:??0bitmap@@QAE@FFF@Z:00000000", "000000e5:0014:?IconToBitmap@@YIXPAVicon@@PAVbitmap@@HHHHHHHHH@Z:00000000"]
; hex 83ec108b44243453894c240856578bf25583f820753f8b4424406a008b4c2440508b542440518b442440528b4c2440508b542440518b442440528b4c244050518bd68b4c2438e8000000005d5f5e5b83c410c22400b8200000008b4c244499f77c24448bd8b8010000002bc10fafc383c0206a1ad1f88944241c8bc3c1e00689442420e80000000083c40485c074136a408bc86a406a00e80000000089442410eb08c74424100000000033d28b4424108b68168d3c2a33c0b90800000083c220f3ab81fa000800007ce9506a206a20508b542420508b4424406a018b4c242c506a006a00e8000000008b4c24288b56160fbf46120fafc803ca8b742418c1e606034c24248b6c24188944241403f58b4424108b78168b44244403f78944241885c07e2c8bc18bfe8b6c244485ed7e0e8a1784d2740288104003fb4d75f28b44241c8b54241403f003caff4c241875d48b44241085c0740a6a018b188b4c2414ff135d5f5e5b83c410c22400

   0:	83 ec 10             	sub    esp,0x10
   3:	8b 44 24 34          	mov    eax,DWORD PTR [esp+0x34]
   7:	53                   	push   ebx
   8:	89 4c 24 08          	mov    DWORD PTR [esp+0x8],ecx
   c:	56                   	push   esi
   d:	57                   	push   edi
   e:	8b f2                	mov    esi,edx
  10:	55                   	push   ebp
  11:	83 f8 20             	cmp    eax,0x20
  14:	75 3f                	jne    0x55
  16:	8b 44 24 40          	mov    eax,DWORD PTR [esp+0x40]
  1a:	6a 00                	push   0x0
  1c:	8b 4c 24 40          	mov    ecx,DWORD PTR [esp+0x40]
  20:	50                   	push   eax
  21:	8b 54 24 40          	mov    edx,DWORD PTR [esp+0x40]
  25:	51                   	push   ecx
  26:	8b 44 24 40          	mov    eax,DWORD PTR [esp+0x40]
  2a:	52                   	push   edx
  2b:	8b 4c 24 40          	mov    ecx,DWORD PTR [esp+0x40]
  2f:	50                   	push   eax
  30:	8b 54 24 40          	mov    edx,DWORD PTR [esp+0x40]
  34:	51                   	push   ecx
  35:	8b 44 24 40          	mov    eax,DWORD PTR [esp+0x40]
  39:	52                   	push   edx
  3a:	8b 4c 24 40          	mov    ecx,DWORD PTR [esp+0x40]
  3e:	50                   	push   eax
  3f:	51                   	push   ecx
  40:	8b d6                	mov    edx,esi
  42:	8b 4c 24 38          	mov    ecx,DWORD PTR [esp+0x38]
  46:	e8 00 00 00 00       	call   0x4b
  4b:	5d                   	pop    ebp
  4c:	5f                   	pop    edi
  4d:	5e                   	pop    esi
  4e:	5b                   	pop    ebx
  4f:	83 c4 10             	add    esp,0x10
  52:	c2 24 00             	ret    0x24
  55:	b8 20 00 00 00       	mov    eax,0x20
  5a:	8b 4c 24 44          	mov    ecx,DWORD PTR [esp+0x44]
  5e:	99                   	cdq
  5f:	f7 7c 24 44          	idiv   DWORD PTR [esp+0x44]
  63:	8b d8                	mov    ebx,eax
  65:	b8 01 00 00 00       	mov    eax,0x1
  6a:	2b c1                	sub    eax,ecx
  6c:	0f af c3             	imul   eax,ebx
  6f:	83 c0 20             	add    eax,0x20
  72:	6a 1a                	push   0x1a
  74:	d1 f8                	sar    eax,1
  76:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  7a:	8b c3                	mov    eax,ebx
  7c:	c1 e0 06             	shl    eax,0x6
  7f:	89 44 24 20          	mov    DWORD PTR [esp+0x20],eax
  83:	e8 00 00 00 00       	call   0x88
  88:	83 c4 04             	add    esp,0x4
  8b:	85 c0                	test   eax,eax
  8d:	74 13                	je     0xa2
  8f:	6a 40                	push   0x40
  91:	8b c8                	mov    ecx,eax
  93:	6a 40                	push   0x40
  95:	6a 00                	push   0x0
  97:	e8 00 00 00 00       	call   0x9c
  9c:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  a0:	eb 08                	jmp    0xaa
  a2:	c7 44 24 10 00 00 00 	mov    DWORD PTR [esp+0x10],0x0
  a9:	00 
  aa:	33 d2                	xor    edx,edx
  ac:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  b0:	8b 68 16             	mov    ebp,DWORD PTR [eax+0x16]
  b3:	8d 3c 2a             	lea    edi,[edx+ebp*1]
  b6:	33 c0                	xor    eax,eax
  b8:	b9 08 00 00 00       	mov    ecx,0x8
  bd:	83 c2 20             	add    edx,0x20
  c0:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  c2:	81 fa 00 08 00 00    	cmp    edx,0x800
  c8:	7c e9                	jl     0xb3
  ca:	50                   	push   eax
  cb:	6a 20                	push   0x20
  cd:	6a 20                	push   0x20
  cf:	50                   	push   eax
  d0:	8b 54 24 20          	mov    edx,DWORD PTR [esp+0x20]
  d4:	50                   	push   eax
  d5:	8b 44 24 40          	mov    eax,DWORD PTR [esp+0x40]
  d9:	6a 01                	push   0x1
  db:	8b 4c 24 2c          	mov    ecx,DWORD PTR [esp+0x2c]
  df:	50                   	push   eax
  e0:	6a 00                	push   0x0
  e2:	6a 00                	push   0x0
  e4:	e8 00 00 00 00       	call   0xe9
  e9:	8b 4c 24 28          	mov    ecx,DWORD PTR [esp+0x28]
  ed:	8b 56 16             	mov    edx,DWORD PTR [esi+0x16]
  f0:	0f bf 46 12          	movsx  eax,WORD PTR [esi+0x12]
  f4:	0f af c8             	imul   ecx,eax
  f7:	03 ca                	add    ecx,edx
  f9:	8b 74 24 18          	mov    esi,DWORD PTR [esp+0x18]
  fd:	c1 e6 06             	shl    esi,0x6
 100:	03 4c 24 24          	add    ecx,DWORD PTR [esp+0x24]
 104:	8b 6c 24 18          	mov    ebp,DWORD PTR [esp+0x18]
 108:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
 10c:	03 f5                	add    esi,ebp
 10e:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
 112:	8b 78 16             	mov    edi,DWORD PTR [eax+0x16]
 115:	8b 44 24 44          	mov    eax,DWORD PTR [esp+0x44]
 119:	03 f7                	add    esi,edi
 11b:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
 11f:	85 c0                	test   eax,eax
 121:	7e 2c                	jle    0x14f
 123:	8b c1                	mov    eax,ecx
 125:	8b fe                	mov    edi,esi
 127:	8b 6c 24 44          	mov    ebp,DWORD PTR [esp+0x44]
 12b:	85 ed                	test   ebp,ebp
 12d:	7e 0e                	jle    0x13d
 12f:	8a 17                	mov    dl,BYTE PTR [edi]
 131:	84 d2                	test   dl,dl
 133:	74 02                	je     0x137
 135:	88 10                	mov    BYTE PTR [eax],dl
 137:	40                   	inc    eax
 138:	03 fb                	add    edi,ebx
 13a:	4d                   	dec    ebp
 13b:	75 f2                	jne    0x12f
 13d:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
 141:	8b 54 24 14          	mov    edx,DWORD PTR [esp+0x14]
 145:	03 f0                	add    esi,eax
 147:	03 ca                	add    ecx,edx
 149:	ff 4c 24 18          	dec    DWORD PTR [esp+0x18]
 14d:	75 d4                	jne    0x123
 14f:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
 153:	85 c0                	test   eax,eax
 155:	74 0a                	je     0x161
 157:	6a 01                	push   0x1
 159:	8b 18                	mov    ebx,DWORD PTR [eax]
 15b:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
 15f:	ff 13                	call   DWORD PTR [ebx]
 161:	5d                   	pop    ebp
 162:	5f                   	pop    edi
 163:	5e                   	pop    esi
 164:	5b                   	pop    ebx
 165:	83 c4 10             	add    esp,0x10
 168:	c2 24 00             	ret    0x24
