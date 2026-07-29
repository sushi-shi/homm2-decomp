; ?NORMALISELO@@YIHH@Z
; unit BASE/Bzip  src_hash c68595a63256  score 100.0000  seed 31337  trial 1
; size 49  text_sha c2f74566b1b29b1d
; relocs ["00000023:0006:?lastPP@@3HA:00000000"]
; hex 558bec83ec04535657894dfc837dfc000f8c080000008b45fce9090000008b45fc030500000000e9000000005f5e5bc9c3

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 04             	sub    esp,0x4
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 4d fc             	mov    DWORD PTR [ebp-0x4],ecx
   c:	83 7d fc 00          	cmp    DWORD PTR [ebp-0x4],0x0
  10:	0f 8c 08 00 00 00    	jl     0x1e
  16:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
  19:	e9 09 00 00 00       	jmp    0x27
  1e:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
  21:	03 05 00 00 00 00    	add    eax,DWORD PTR ds:0x0
  27:	e9 00 00 00 00       	jmp    0x2c
  2c:	5f                   	pop    edi
  2d:	5e                   	pop    esi
  2e:	5b                   	pop    ebx
  2f:	c9                   	leave
  30:	c3                   	ret
