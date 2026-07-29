; ?MainLoop@executive@@QAEXXZ
; unit BASE/EXEC  src_hash 1121c22f25a2  score 100.0000  seed 1163412803  trial 3
; size 250  text_sha e91d1cf665a2c578
; relocs ["00000017:0006:?gpInputManager@@3PAVinputManager@@A:00000000", "0000001c:0014:?Flush@inputManager@@QAEXXZ:00000000", "00000021:0014:?Process1WindowsMessage@@YIXXZ:00000000", "0000002b:0006:?gpInputManager@@3PAVinputManager@@A:00000000", "00000031:0014:?GetEvent@inputManager@@QAE?AUtag_message@@XZ:00000000", "00000078:0006:?gpWindowManager@@3PAVheroWindowManager@@A:00000000", "000000bd:0014:?RemoveManager@executive@@QAEXPAVbaseManager@@@Z:00000000"]
; hex 83ec3853568b0157558bd933ed3bc50f84dd0000008b0d00000000e800000000e8000000008d44242c8b0d0000000050e8000000008d7c24108bf0b907000000f3a5be010000008b0389430885c00f849e0000008bfe85f60f848c00000085ed0f858c0000008b4b0839793275648b44241083f8047509a1000000003bc174528d442410508b01ff50083bc7740783f8027406eb3d33f6eb398b442410f6c44074308b4424143bc7742783f802740783f8047416eb1c8b43088bcb50e800000000c7430800000000eb088b44242889430c458b430885c074118b40048943088b430885c00f856cffffff85ed0f842effffff5d5f5e5b83c438c3

   0:	83 ec 38             	sub    esp,0x38
   3:	53                   	push   ebx
   4:	56                   	push   esi
   5:	8b 01                	mov    eax,DWORD PTR [ecx]
   7:	57                   	push   edi
   8:	55                   	push   ebp
   9:	8b d9                	mov    ebx,ecx
   b:	33 ed                	xor    ebp,ebp
   d:	3b c5                	cmp    eax,ebp
   f:	0f 84 dd 00 00 00    	je     0xf2
  15:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
  1b:	e8 00 00 00 00       	call   0x20
  20:	e8 00 00 00 00       	call   0x25
  25:	8d 44 24 2c          	lea    eax,[esp+0x2c]
  29:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
  2f:	50                   	push   eax
  30:	e8 00 00 00 00       	call   0x35
  35:	8d 7c 24 10          	lea    edi,[esp+0x10]
  39:	8b f0                	mov    esi,eax
  3b:	b9 07 00 00 00       	mov    ecx,0x7
  40:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  42:	be 01 00 00 00       	mov    esi,0x1
  47:	8b 03                	mov    eax,DWORD PTR [ebx]
  49:	89 43 08             	mov    DWORD PTR [ebx+0x8],eax
  4c:	85 c0                	test   eax,eax
  4e:	0f 84 9e 00 00 00    	je     0xf2
  54:	8b fe                	mov    edi,esi
  56:	85 f6                	test   esi,esi
  58:	0f 84 8c 00 00 00    	je     0xea
  5e:	85 ed                	test   ebp,ebp
  60:	0f 85 8c 00 00 00    	jne    0xf2
  66:	8b 4b 08             	mov    ecx,DWORD PTR [ebx+0x8]
  69:	39 79 32             	cmp    DWORD PTR [ecx+0x32],edi
  6c:	75 64                	jne    0xd2
  6e:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  72:	83 f8 04             	cmp    eax,0x4
  75:	75 09                	jne    0x80
  77:	a1 00 00 00 00       	mov    eax,ds:0x0
  7c:	3b c1                	cmp    eax,ecx
  7e:	74 52                	je     0xd2
  80:	8d 44 24 10          	lea    eax,[esp+0x10]
  84:	50                   	push   eax
  85:	8b 01                	mov    eax,DWORD PTR [ecx]
  87:	ff 50 08             	call   DWORD PTR [eax+0x8]
  8a:	3b c7                	cmp    eax,edi
  8c:	74 07                	je     0x95
  8e:	83 f8 02             	cmp    eax,0x2
  91:	74 06                	je     0x99
  93:	eb 3d                	jmp    0xd2
  95:	33 f6                	xor    esi,esi
  97:	eb 39                	jmp    0xd2
  99:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  9d:	f6 c4 40             	test   ah,0x40
  a0:	74 30                	je     0xd2
  a2:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  a6:	3b c7                	cmp    eax,edi
  a8:	74 27                	je     0xd1
  aa:	83 f8 02             	cmp    eax,0x2
  ad:	74 07                	je     0xb6
  af:	83 f8 04             	cmp    eax,0x4
  b2:	74 16                	je     0xca
  b4:	eb 1c                	jmp    0xd2
  b6:	8b 43 08             	mov    eax,DWORD PTR [ebx+0x8]
  b9:	8b cb                	mov    ecx,ebx
  bb:	50                   	push   eax
  bc:	e8 00 00 00 00       	call   0xc1
  c1:	c7 43 08 00 00 00 00 	mov    DWORD PTR [ebx+0x8],0x0
  c8:	eb 08                	jmp    0xd2
  ca:	8b 44 24 28          	mov    eax,DWORD PTR [esp+0x28]
  ce:	89 43 0c             	mov    DWORD PTR [ebx+0xc],eax
  d1:	45                   	inc    ebp
  d2:	8b 43 08             	mov    eax,DWORD PTR [ebx+0x8]
  d5:	85 c0                	test   eax,eax
  d7:	74 11                	je     0xea
  d9:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
  dc:	89 43 08             	mov    DWORD PTR [ebx+0x8],eax
  df:	8b 43 08             	mov    eax,DWORD PTR [ebx+0x8]
  e2:	85 c0                	test   eax,eax
  e4:	0f 85 6c ff ff ff    	jne    0x56
  ea:	85 ed                	test   ebp,ebp
  ec:	0f 84 2e ff ff ff    	je     0x20
  f2:	5d                   	pop    ebp
  f3:	5f                   	pop    edi
  f4:	5e                   	pop    esi
  f5:	5b                   	pop    ebx
  f6:	83 c4 38             	add    esp,0x38
  f9:	c3                   	ret
