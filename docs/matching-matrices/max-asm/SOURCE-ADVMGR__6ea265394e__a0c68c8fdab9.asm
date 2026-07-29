; ?IsCrystalBallInEffect@advManager@@QAEHHHH@Z
; unit SOURCE/ADVMGR  src_hash a0c68c8fdab9  score 100.0000  seed 8181  trial 1
; size 215  text_sha ff714bc46b9a9076
; relocs ["0000001c:0006:?gpCurPlayer@@3PAVplayerData@@A:00000000", "00000032:0006:?gpCurPlayer@@3PAVplayerData@@A:00000000", "00000048:0006:?gpGame@@3PAVgame@@A:00000000", "0000005a:0014:?HasArtifact@hero@@QAEHH@Z:00000000", "0000009f:0014:_sqrt:00000000", "000000a7:0014:__ftol:00000000"]
; hex 558bec83ec14535657894df0c745f800000000e903000000ff45f8a1000000000fbe40013b45f80f8e9c0000008b45f88b0d000000000fbe4408048d04808d04808d048003c003050000000005c42700008945fc6a5b8b4dfce80000000085c00f845e0000008b45fc8b401d2b450c8b4dfc8b491d2b4d0c0fafc18b4dfc8b49192b4d088b55fc8b52192b55080fafca03c18945ecdb45ec83ec08dd1c24e80000000083c408e8000000008945f48b45103945f40f8f0a000000b801000000e90c000000e94fffffff33c0e9000000005f5e5bc9c20c00

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 14             	sub    esp,0x14
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 4d f0             	mov    DWORD PTR [ebp-0x10],ecx
   c:	c7 45 f8 00 00 00 00 	mov    DWORD PTR [ebp-0x8],0x0
  13:	e9 03 00 00 00       	jmp    0x1b
  18:	ff 45 f8             	inc    DWORD PTR [ebp-0x8]
  1b:	a1 00 00 00 00       	mov    eax,ds:0x0
  20:	0f be 40 01          	movsx  eax,BYTE PTR [eax+0x1]
  24:	3b 45 f8             	cmp    eax,DWORD PTR [ebp-0x8]
  27:	0f 8e 9c 00 00 00    	jle    0xc9
  2d:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
  30:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
  36:	0f be 44 08 04       	movsx  eax,BYTE PTR [eax+ecx*1+0x4]
  3b:	8d 04 80             	lea    eax,[eax+eax*4]
  3e:	8d 04 80             	lea    eax,[eax+eax*4]
  41:	8d 04 80             	lea    eax,[eax+eax*4]
  44:	03 c0                	add    eax,eax
  46:	03 05 00 00 00 00    	add    eax,DWORD PTR ds:0x0
  4c:	05 c4 27 00 00       	add    eax,0x27c4
  51:	89 45 fc             	mov    DWORD PTR [ebp-0x4],eax
  54:	6a 5b                	push   0x5b
  56:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
  59:	e8 00 00 00 00       	call   0x5e
  5e:	85 c0                	test   eax,eax
  60:	0f 84 5e 00 00 00    	je     0xc4
  66:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
  69:	8b 40 1d             	mov    eax,DWORD PTR [eax+0x1d]
  6c:	2b 45 0c             	sub    eax,DWORD PTR [ebp+0xc]
  6f:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
  72:	8b 49 1d             	mov    ecx,DWORD PTR [ecx+0x1d]
  75:	2b 4d 0c             	sub    ecx,DWORD PTR [ebp+0xc]
  78:	0f af c1             	imul   eax,ecx
  7b:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
  7e:	8b 49 19             	mov    ecx,DWORD PTR [ecx+0x19]
  81:	2b 4d 08             	sub    ecx,DWORD PTR [ebp+0x8]
  84:	8b 55 fc             	mov    edx,DWORD PTR [ebp-0x4]
  87:	8b 52 19             	mov    edx,DWORD PTR [edx+0x19]
  8a:	2b 55 08             	sub    edx,DWORD PTR [ebp+0x8]
  8d:	0f af ca             	imul   ecx,edx
  90:	03 c1                	add    eax,ecx
  92:	89 45 ec             	mov    DWORD PTR [ebp-0x14],eax
  95:	db 45 ec             	fild   DWORD PTR [ebp-0x14]
  98:	83 ec 08             	sub    esp,0x8
  9b:	dd 1c 24             	fstp   QWORD PTR [esp]
  9e:	e8 00 00 00 00       	call   0xa3
  a3:	83 c4 08             	add    esp,0x8
  a6:	e8 00 00 00 00       	call   0xab
  ab:	89 45 f4             	mov    DWORD PTR [ebp-0xc],eax
  ae:	8b 45 10             	mov    eax,DWORD PTR [ebp+0x10]
  b1:	39 45 f4             	cmp    DWORD PTR [ebp-0xc],eax
  b4:	0f 8f 0a 00 00 00    	jg     0xc4
  ba:	b8 01 00 00 00       	mov    eax,0x1
  bf:	e9 0c 00 00 00       	jmp    0xd0
  c4:	e9 4f ff ff ff       	jmp    0x18
  c9:	33 c0                	xor    eax,eax
  cb:	e9 00 00 00 00       	jmp    0xd0
  d0:	5f                   	pop    edi
  d1:	5e                   	pop    esi
  d2:	5b                   	pop    ebx
  d3:	c9                   	leave
  d4:	c2 0c 00             	ret    0xc
