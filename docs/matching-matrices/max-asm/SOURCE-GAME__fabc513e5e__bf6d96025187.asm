; ?InitRandomArtifacts@game@@QAEXXZ
; unit SOURCE/GAME  src_hash bf6d96025187  score 100.0000  seed 1397509972  trial 22
; size 198  text_sha 4803a4f121d5ccbd
; relocs ["0000001a:0014:_memset:00000000", "00000035:0006:?MAP_WIDTH@@3HA:00000000", "0000004f:0006:?MAP_HEIGHT@@3HA:00000000"]
; hex 558bec83ec14535657894dec6a676a008b45ec053661000050e80000000083c40cc745fc00000000e903000000ff45fc8b45fc3905000000000f8e7d000000c745f400000000e903000000ff45f4a1000000003945f40f8d5b000000e9000000008b45ec8b80460b00000faf45f48d0440c1e0028b4dec03813e0b00008b4dfc8d0c498d04888945f08b45f033c98a480981f9a90000000f85150000008b45f033c98a4803d1f98b45ecc684013661000001e994ffffffe971ffffffe9000000005f5e5bc9c3

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 14             	sub    esp,0x14
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 4d ec             	mov    DWORD PTR [ebp-0x14],ecx
   c:	6a 67                	push   0x67
   e:	6a 00                	push   0x0
  10:	8b 45 ec             	mov    eax,DWORD PTR [ebp-0x14]
  13:	05 36 61 00 00       	add    eax,0x6136
  18:	50                   	push   eax
  19:	e8 00 00 00 00       	call   0x1e
  1e:	83 c4 0c             	add    esp,0xc
  21:	c7 45 fc 00 00 00 00 	mov    DWORD PTR [ebp-0x4],0x0
  28:	e9 03 00 00 00       	jmp    0x30
  2d:	ff 45 fc             	inc    DWORD PTR [ebp-0x4]
  30:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
  33:	39 05 00 00 00 00    	cmp    DWORD PTR ds:0x0,eax
  39:	0f 8e 7d 00 00 00    	jle    0xbc
  3f:	c7 45 f4 00 00 00 00 	mov    DWORD PTR [ebp-0xc],0x0
  46:	e9 03 00 00 00       	jmp    0x4e
  4b:	ff 45 f4             	inc    DWORD PTR [ebp-0xc]
  4e:	a1 00 00 00 00       	mov    eax,ds:0x0
  53:	39 45 f4             	cmp    DWORD PTR [ebp-0xc],eax
  56:	0f 8d 5b 00 00 00    	jge    0xb7
  5c:	e9 00 00 00 00       	jmp    0x61
  61:	8b 45 ec             	mov    eax,DWORD PTR [ebp-0x14]
  64:	8b 80 46 0b 00 00    	mov    eax,DWORD PTR [eax+0xb46]
  6a:	0f af 45 f4          	imul   eax,DWORD PTR [ebp-0xc]
  6e:	8d 04 40             	lea    eax,[eax+eax*2]
  71:	c1 e0 02             	shl    eax,0x2
  74:	8b 4d ec             	mov    ecx,DWORD PTR [ebp-0x14]
  77:	03 81 3e 0b 00 00    	add    eax,DWORD PTR [ecx+0xb3e]
  7d:	8b 4d fc             	mov    ecx,DWORD PTR [ebp-0x4]
  80:	8d 0c 49             	lea    ecx,[ecx+ecx*2]
  83:	8d 04 88             	lea    eax,[eax+ecx*4]
  86:	89 45 f0             	mov    DWORD PTR [ebp-0x10],eax
  89:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  8c:	33 c9                	xor    ecx,ecx
  8e:	8a 48 09             	mov    cl,BYTE PTR [eax+0x9]
  91:	81 f9 a9 00 00 00    	cmp    ecx,0xa9
  97:	0f 85 15 00 00 00    	jne    0xb2
  9d:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  a0:	33 c9                	xor    ecx,ecx
  a2:	8a 48 03             	mov    cl,BYTE PTR [eax+0x3]
  a5:	d1 f9                	sar    ecx,1
  a7:	8b 45 ec             	mov    eax,DWORD PTR [ebp-0x14]
  aa:	c6 84 01 36 61 00 00 	mov    BYTE PTR [ecx+eax*1+0x6136],0x1
  b1:	01 
  b2:	e9 94 ff ff ff       	jmp    0x4b
  b7:	e9 71 ff ff ff       	jmp    0x2d
  bc:	e9 00 00 00 00       	jmp    0xc1
  c1:	5f                   	pop    edi
  c2:	5e                   	pop    esi
  c3:	5b                   	pop    ebx
  c4:	c9                   	leave
  c5:	c3                   	ret
