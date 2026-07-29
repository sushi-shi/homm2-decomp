; ?GetNumThievesGuilds@game@@QAEHH@Z
; unit SOURCE/GAME  src_hash d80f3564c86e  score 100.0000  seed 1213156658  trial 20
; size 156  text_sha d4a4955fc1d5eff0
; relocs ["00000073:0006:?gpGame@@3PAVgame@@A:00000000"]
; hex 558bec83ec0c535657894df4c745f800000000c745fc00000000e903000000ff45fc8b45088bc8c1e0038d04c02bc1c1e0022bc18b4df40fbe8408e00400003b45fc0f8e450000008b45fc8b4d088bd1c1e1038d0cc92bca2bc28d04888b4df40fbe8408e3040000c1e0028d04808d04808b0d00000000f684086b0b0000020f8403000000ff45f8e992ffffff8b45f8e9000000005f5e5bc9c20400

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 0c             	sub    esp,0xc
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 4d f4             	mov    DWORD PTR [ebp-0xc],ecx
   c:	c7 45 f8 00 00 00 00 	mov    DWORD PTR [ebp-0x8],0x0
  13:	c7 45 fc 00 00 00 00 	mov    DWORD PTR [ebp-0x4],0x0
  1a:	e9 03 00 00 00       	jmp    0x22
  1f:	ff 45 fc             	inc    DWORD PTR [ebp-0x4]
  22:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  25:	8b c8                	mov    ecx,eax
  27:	c1 e0 03             	shl    eax,0x3
  2a:	8d 04 c0             	lea    eax,[eax+eax*8]
  2d:	2b c1                	sub    eax,ecx
  2f:	c1 e0 02             	shl    eax,0x2
  32:	2b c1                	sub    eax,ecx
  34:	8b 4d f4             	mov    ecx,DWORD PTR [ebp-0xc]
  37:	0f be 84 08 e0 04 00 	movsx  eax,BYTE PTR [eax+ecx*1+0x4e0]
  3e:	00
  3f:	3b 45 fc             	cmp    eax,DWORD PTR [ebp-0x4]
  42:	0f 8e 45 00 00 00    	jle    0x8d
  48:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
  4b:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  4e:	8b d1                	mov    edx,ecx
  50:	c1 e1 03             	shl    ecx,0x3
  53:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  56:	2b ca                	sub    ecx,edx
  58:	2b c2                	sub    eax,edx
  5a:	8d 04 88             	lea    eax,[eax+ecx*4]
  5d:	8b 4d f4             	mov    ecx,DWORD PTR [ebp-0xc]
  60:	0f be 84 08 e3 04 00 	movsx  eax,BYTE PTR [eax+ecx*1+0x4e3]
  67:	00
  68:	c1 e0 02             	shl    eax,0x2
  6b:	8d 04 80             	lea    eax,[eax+eax*4]
  6e:	8d 04 80             	lea    eax,[eax+eax*4]
  71:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
  77:	f6 84 08 6b 0b 00 00 	test   BYTE PTR [eax+ecx*1+0xb6b],0x2
  7e:	02
  7f:	0f 84 03 00 00 00    	je     0x88
  85:	ff 45 f8             	inc    DWORD PTR [ebp-0x8]
  88:	e9 92 ff ff ff       	jmp    0x1f
  8d:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
  90:	e9 00 00 00 00       	jmp    0x95
  95:	5f                   	pop    edi
  96:	5e                   	pop    esi
  97:	5b                   	pop    ebx
  98:	c9                   	leave
  99:	c2 04 00             	ret    0x4
