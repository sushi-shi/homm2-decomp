; ?FindIndex@@YIHPAUindexArray@@HHH@Z
; unit BASE/Misc  src_hash c6baaffbe8c9  score 100.0000  seed 1213156658  trial 3
; size 149  text_sha 51f4e44f4ab1b94f
; relocs ["00000011:0006:_giFindMid$Sdata_bss_1c9d0fea0c2c9ee6bdcaa80b92f7bd71ffdd5dbda26ca1adc7a47f4470d601c7_0:00000000", "00000020:0006:_giFindMid$Sdata_bss_1c9d0fea0c2c9ee6bdcaa80b92f7bd71ffdd5dbda26ca1adc7a47f4470d601c7_0:00000000", "00000034:0006:_giFindMid$Sdata_bss_1c9d0fea0c2c9ee6bdcaa80b92f7bd71ffdd5dbda26ca1adc7a47f4470d601c7_0:00000000", "00000040:0006:_giFindMid$Sdata_bss_1c9d0fea0c2c9ee6bdcaa80b92f7bd71ffdd5dbda26ca1adc7a47f4470d601c7_0:00000000"]
; hex 53568b74240c578b7c24148d0432d1f8a3000000008bc62bc283f8017e3e8b1d0000000033c0668b04993bc77e048bf3eb088b15000000007d158d0432d1f8a3000000008bc62bc283f8017fd1eb0d33c05f668b4491025e5bc2080033c0668b04913bc7750d33c05f668b4491025e5bc2080033c0668b04b13bc7750d33c05f668b44b1025e5bc20800b8ffff00005f5e5bc20800

   0:	53                   	push   ebx
   1:	56                   	push   esi
   2:	8b 74 24 0c          	mov    esi,DWORD PTR [esp+0xc]
   6:	57                   	push   edi
   7:	8b 7c 24 14          	mov    edi,DWORD PTR [esp+0x14]
   b:	8d 04 32             	lea    eax,[edx+esi*1]
   e:	d1 f8                	sar    eax,1
  10:	a3 00 00 00 00       	mov    ds:0x0,eax
  15:	8b c6                	mov    eax,esi
  17:	2b c2                	sub    eax,edx
  19:	83 f8 01             	cmp    eax,0x1
  1c:	7e 3e                	jle    0x5c
  1e:	8b 1d 00 00 00 00    	mov    ebx,DWORD PTR ds:0x0
  24:	33 c0                	xor    eax,eax
  26:	66 8b 04 99          	mov    ax,WORD PTR [ecx+ebx*4]
  2a:	3b c7                	cmp    eax,edi
  2c:	7e 04                	jle    0x32
  2e:	8b f3                	mov    esi,ebx
  30:	eb 08                	jmp    0x3a
  32:	8b 15 00 00 00 00    	mov    edx,DWORD PTR ds:0x0
  38:	7d 15                	jge    0x4f
  3a:	8d 04 32             	lea    eax,[edx+esi*1]
  3d:	d1 f8                	sar    eax,1
  3f:	a3 00 00 00 00       	mov    ds:0x0,eax
  44:	8b c6                	mov    eax,esi
  46:	2b c2                	sub    eax,edx
  48:	83 f8 01             	cmp    eax,0x1
  4b:	7f d1                	jg     0x1e
  4d:	eb 0d                	jmp    0x5c
  4f:	33 c0                	xor    eax,eax
  51:	5f                   	pop    edi
  52:	66 8b 44 91 02       	mov    ax,WORD PTR [ecx+edx*4+0x2]
  57:	5e                   	pop    esi
  58:	5b                   	pop    ebx
  59:	c2 08 00             	ret    0x8
  5c:	33 c0                	xor    eax,eax
  5e:	66 8b 04 91          	mov    ax,WORD PTR [ecx+edx*4]
  62:	3b c7                	cmp    eax,edi
  64:	75 0d                	jne    0x73
  66:	33 c0                	xor    eax,eax
  68:	5f                   	pop    edi
  69:	66 8b 44 91 02       	mov    ax,WORD PTR [ecx+edx*4+0x2]
  6e:	5e                   	pop    esi
  6f:	5b                   	pop    ebx
  70:	c2 08 00             	ret    0x8
  73:	33 c0                	xor    eax,eax
  75:	66 8b 04 b1          	mov    ax,WORD PTR [ecx+esi*4]
  79:	3b c7                	cmp    eax,edi
  7b:	75 0d                	jne    0x8a
  7d:	33 c0                	xor    eax,eax
  7f:	5f                   	pop    edi
  80:	66 8b 44 b1 02       	mov    ax,WORD PTR [ecx+esi*4+0x2]
  85:	5e                   	pop    esi
  86:	5b                   	pop    ebx
  87:	c2 08 00             	ret    0x8
  8a:	b8 ff ff 00 00       	mov    eax,0xffff
  8f:	5f                   	pop    edi
  90:	5e                   	pop    esi
  91:	5b                   	pop    ebx
  92:	c2 08 00             	ret    0x8
