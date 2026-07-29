; ?SetRandomHeroArmies@game@@QAEXHH@Z
; unit SOURCE/GAME  src_hash 793c0a86e88a  score 100.0000  seed 31337  trial 76
; size 888  text_sha 6aa6884c0a5b3220
; relocs ["0000019d:0014:?Random@@YIHHH@Z:00000000", "000001dc:0014:?Random@@YIHHH@Z:00000000", "0000034d:0014:?Random@@YIHHH@Z:00000000"]
; hex 558bec81ec90000000535657898d70ffffff8b45088d04808d04808d048003c0038570ffffff05292800008945f8c745f40000000066c74588000066c7458a1e0066c7458c320066c7458e010066c74590030066c74592050066c74594030066c74596020066c74598040066c7459a0b0066c7459c0f0066c7459e190066c745a00c0066c745a2030066c745a4050066c745a60e0066c745a8020066c745aa030066c745ac140066c745ae0a0066c745b0140066c745b2150066c745b4020066c745b6040066c745b8170066c745ba010066c745bc020066c745be1d0066c745c0060066c745c20a0066c745c41e0066c745c6020066c745c8040066c745ca1f0066c745cc010066c745ce020066c745d0260066c745d2060066c745d40a0066c745d6270066c745d8020066c745da040066c745dc280066c745de010066c745e0020066c745e22f0066c745e4060066c745e60a0066c745e8300066c745ea020066c745ec040066c745ee320066c745f0010066c745f20200c78578ffffff01000000837d0c01bb0000000083d3ff83e31e83c332ba6300000033c9e8000000003bd80f8e0f000000c7857cffffff01000000e90a000000c7857cffffff00000000837d0c01bb0000000083d3ff83e32883c319ba6300000033c9e8000000003bd80f8e0c000000c7458001000000e907000000c7458000000000837d80000f850a000000c7857cffffff01000000c7458400000000e903000000ff4584837d84050f8d1c0000008b45848b4df8c60408ff8b45848b4df866c7444105ffffe9d7ffffffc7458400000000e903000000ff4584837d84020f8d0f0100008b458483bc8578ffffff000f84f90000008b45088d04808d04808d04808b8d70ffffff33d28a9441db2700008d0455000000008d04c08b4d848d0c498d04488a4405888b4df48b55f88804118b45088d04808d04808d04808b8d70ffffff33d28a9441db2700008d0455000000008d04c08b4d848d0c498d04480fbf44058a8d048003c0898574ffffff8b45088d04808d04808d04808b8d70ffffff33d28a9441db2700008d0455000000008d04c08b4d848d0c498d04480fbf44058c8d048003c083c0098945fc837d0c000f84140000008b8574ffffff0345fc992bc2d1f8898574ffffff8b55fc8b8d74ffffffe800000000b90a00000099f7f98b4df48b55f86689444a05ff45f4e9e4feffffe9000000005f5e5bc9c20800

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	81 ec 90 00 00 00    	sub    esp,0x90
   9:	53                   	push   ebx
   a:	56                   	push   esi
   b:	57                   	push   edi
   c:	89 8d 70 ff ff ff    	mov    DWORD PTR [ebp-0x90],ecx
  12:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  15:	8d 04 80             	lea    eax,[eax+eax*4]
  18:	8d 04 80             	lea    eax,[eax+eax*4]
  1b:	8d 04 80             	lea    eax,[eax+eax*4]
  1e:	03 c0                	add    eax,eax
  20:	03 85 70 ff ff ff    	add    eax,DWORD PTR [ebp-0x90]
  26:	05 29 28 00 00       	add    eax,0x2829
  2b:	89 45 f8             	mov    DWORD PTR [ebp-0x8],eax
  2e:	c7 45 f4 00 00 00 00 	mov    DWORD PTR [ebp-0xc],0x0
  35:	66 c7 45 88 00 00    	mov    WORD PTR [ebp-0x78],0x0
  3b:	66 c7 45 8a 1e 00    	mov    WORD PTR [ebp-0x76],0x1e
  41:	66 c7 45 8c 32 00    	mov    WORD PTR [ebp-0x74],0x32
  47:	66 c7 45 8e 01 00    	mov    WORD PTR [ebp-0x72],0x1
  4d:	66 c7 45 90 03 00    	mov    WORD PTR [ebp-0x70],0x3
  53:	66 c7 45 92 05 00    	mov    WORD PTR [ebp-0x6e],0x5
  59:	66 c7 45 94 03 00    	mov    WORD PTR [ebp-0x6c],0x3
  5f:	66 c7 45 96 02 00    	mov    WORD PTR [ebp-0x6a],0x2
  65:	66 c7 45 98 04 00    	mov    WORD PTR [ebp-0x68],0x4
  6b:	66 c7 45 9a 0b 00    	mov    WORD PTR [ebp-0x66],0xb
  71:	66 c7 45 9c 0f 00    	mov    WORD PTR [ebp-0x64],0xf
  77:	66 c7 45 9e 19 00    	mov    WORD PTR [ebp-0x62],0x19
  7d:	66 c7 45 a0 0c 00    	mov    WORD PTR [ebp-0x60],0xc
  83:	66 c7 45 a2 03 00    	mov    WORD PTR [ebp-0x5e],0x3
  89:	66 c7 45 a4 05 00    	mov    WORD PTR [ebp-0x5c],0x5
  8f:	66 c7 45 a6 0e 00    	mov    WORD PTR [ebp-0x5a],0xe
  95:	66 c7 45 a8 02 00    	mov    WORD PTR [ebp-0x58],0x2
  9b:	66 c7 45 aa 03 00    	mov    WORD PTR [ebp-0x56],0x3
  a1:	66 c7 45 ac 14 00    	mov    WORD PTR [ebp-0x54],0x14
  a7:	66 c7 45 ae 0a 00    	mov    WORD PTR [ebp-0x52],0xa
  ad:	66 c7 45 b0 14 00    	mov    WORD PTR [ebp-0x50],0x14
  b3:	66 c7 45 b2 15 00    	mov    WORD PTR [ebp-0x4e],0x15
  b9:	66 c7 45 b4 02 00    	mov    WORD PTR [ebp-0x4c],0x2
  bf:	66 c7 45 b6 04 00    	mov    WORD PTR [ebp-0x4a],0x4
  c5:	66 c7 45 b8 17 00    	mov    WORD PTR [ebp-0x48],0x17
  cb:	66 c7 45 ba 01 00    	mov    WORD PTR [ebp-0x46],0x1
  d1:	66 c7 45 bc 02 00    	mov    WORD PTR [ebp-0x44],0x2
  d7:	66 c7 45 be 1d 00    	mov    WORD PTR [ebp-0x42],0x1d
  dd:	66 c7 45 c0 06 00    	mov    WORD PTR [ebp-0x40],0x6
  e3:	66 c7 45 c2 0a 00    	mov    WORD PTR [ebp-0x3e],0xa
  e9:	66 c7 45 c4 1e 00    	mov    WORD PTR [ebp-0x3c],0x1e
  ef:	66 c7 45 c6 02 00    	mov    WORD PTR [ebp-0x3a],0x2
  f5:	66 c7 45 c8 04 00    	mov    WORD PTR [ebp-0x38],0x4
  fb:	66 c7 45 ca 1f 00    	mov    WORD PTR [ebp-0x36],0x1f
 101:	66 c7 45 cc 01 00    	mov    WORD PTR [ebp-0x34],0x1
 107:	66 c7 45 ce 02 00    	mov    WORD PTR [ebp-0x32],0x2
 10d:	66 c7 45 d0 26 00    	mov    WORD PTR [ebp-0x30],0x26
 113:	66 c7 45 d2 06 00    	mov    WORD PTR [ebp-0x2e],0x6
 119:	66 c7 45 d4 0a 00    	mov    WORD PTR [ebp-0x2c],0xa
 11f:	66 c7 45 d6 27 00    	mov    WORD PTR [ebp-0x2a],0x27
 125:	66 c7 45 d8 02 00    	mov    WORD PTR [ebp-0x28],0x2
 12b:	66 c7 45 da 04 00    	mov    WORD PTR [ebp-0x26],0x4
 131:	66 c7 45 dc 28 00    	mov    WORD PTR [ebp-0x24],0x28
 137:	66 c7 45 de 01 00    	mov    WORD PTR [ebp-0x22],0x1
 13d:	66 c7 45 e0 02 00    	mov    WORD PTR [ebp-0x20],0x2
 143:	66 c7 45 e2 2f 00    	mov    WORD PTR [ebp-0x1e],0x2f
 149:	66 c7 45 e4 06 00    	mov    WORD PTR [ebp-0x1c],0x6
 14f:	66 c7 45 e6 0a 00    	mov    WORD PTR [ebp-0x1a],0xa
 155:	66 c7 45 e8 30 00    	mov    WORD PTR [ebp-0x18],0x30
 15b:	66 c7 45 ea 02 00    	mov    WORD PTR [ebp-0x16],0x2
 161:	66 c7 45 ec 04 00    	mov    WORD PTR [ebp-0x14],0x4
 167:	66 c7 45 ee 32 00    	mov    WORD PTR [ebp-0x12],0x32
 16d:	66 c7 45 f0 01 00    	mov    WORD PTR [ebp-0x10],0x1
 173:	66 c7 45 f2 02 00    	mov    WORD PTR [ebp-0xe],0x2
 179:	c7 85 78 ff ff ff 01 	mov    DWORD PTR [ebp-0x88],0x1
 180:	00 00 00 
 183:	83 7d 0c 01          	cmp    DWORD PTR [ebp+0xc],0x1
 187:	bb 00 00 00 00       	mov    ebx,0x0
 18c:	83 d3 ff             	adc    ebx,0xffffffff
 18f:	83 e3 1e             	and    ebx,0x1e
 192:	83 c3 32             	add    ebx,0x32
 195:	ba 63 00 00 00       	mov    edx,0x63
 19a:	33 c9                	xor    ecx,ecx
 19c:	e8 00 00 00 00       	call   0x1a1
 1a1:	3b d8                	cmp    ebx,eax
 1a3:	0f 8e 0f 00 00 00    	jle    0x1b8
 1a9:	c7 85 7c ff ff ff 01 	mov    DWORD PTR [ebp-0x84],0x1
 1b0:	00 00 00 
 1b3:	e9 0a 00 00 00       	jmp    0x1c2
 1b8:	c7 85 7c ff ff ff 00 	mov    DWORD PTR [ebp-0x84],0x0
 1bf:	00 00 00 
 1c2:	83 7d 0c 01          	cmp    DWORD PTR [ebp+0xc],0x1
 1c6:	bb 00 00 00 00       	mov    ebx,0x0
 1cb:	83 d3 ff             	adc    ebx,0xffffffff
 1ce:	83 e3 28             	and    ebx,0x28
 1d1:	83 c3 19             	add    ebx,0x19
 1d4:	ba 63 00 00 00       	mov    edx,0x63
 1d9:	33 c9                	xor    ecx,ecx
 1db:	e8 00 00 00 00       	call   0x1e0
 1e0:	3b d8                	cmp    ebx,eax
 1e2:	0f 8e 0c 00 00 00    	jle    0x1f4
 1e8:	c7 45 80 01 00 00 00 	mov    DWORD PTR [ebp-0x80],0x1
 1ef:	e9 07 00 00 00       	jmp    0x1fb
 1f4:	c7 45 80 00 00 00 00 	mov    DWORD PTR [ebp-0x80],0x0
 1fb:	83 7d 80 00          	cmp    DWORD PTR [ebp-0x80],0x0
 1ff:	0f 85 0a 00 00 00    	jne    0x20f
 205:	c7 85 7c ff ff ff 01 	mov    DWORD PTR [ebp-0x84],0x1
 20c:	00 00 00 
 20f:	c7 45 84 00 00 00 00 	mov    DWORD PTR [ebp-0x7c],0x0
 216:	e9 03 00 00 00       	jmp    0x21e
 21b:	ff 45 84             	inc    DWORD PTR [ebp-0x7c]
 21e:	83 7d 84 05          	cmp    DWORD PTR [ebp-0x7c],0x5
 222:	0f 8d 1c 00 00 00    	jge    0x244
 228:	8b 45 84             	mov    eax,DWORD PTR [ebp-0x7c]
 22b:	8b 4d f8             	mov    ecx,DWORD PTR [ebp-0x8]
 22e:	c6 04 08 ff          	mov    BYTE PTR [eax+ecx*1],0xff
 232:	8b 45 84             	mov    eax,DWORD PTR [ebp-0x7c]
 235:	8b 4d f8             	mov    ecx,DWORD PTR [ebp-0x8]
 238:	66 c7 44 41 05 ff ff 	mov    WORD PTR [ecx+eax*2+0x5],0xffff
 23f:	e9 d7 ff ff ff       	jmp    0x21b
 244:	c7 45 84 00 00 00 00 	mov    DWORD PTR [ebp-0x7c],0x0
 24b:	e9 03 00 00 00       	jmp    0x253
 250:	ff 45 84             	inc    DWORD PTR [ebp-0x7c]
 253:	83 7d 84 02          	cmp    DWORD PTR [ebp-0x7c],0x2
 257:	0f 8d 0f 01 00 00    	jge    0x36c
 25d:	8b 45 84             	mov    eax,DWORD PTR [ebp-0x7c]
 260:	83 bc 85 78 ff ff ff 	cmp    DWORD PTR [ebp+eax*4-0x88],0x0
 267:	00 
 268:	0f 84 f9 00 00 00    	je     0x367
 26e:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 271:	8d 04 80             	lea    eax,[eax+eax*4]
 274:	8d 04 80             	lea    eax,[eax+eax*4]
 277:	8d 04 80             	lea    eax,[eax+eax*4]
 27a:	8b 8d 70 ff ff ff    	mov    ecx,DWORD PTR [ebp-0x90]
 280:	33 d2                	xor    edx,edx
 282:	8a 94 41 db 27 00 00 	mov    dl,BYTE PTR [ecx+eax*2+0x27db]
 289:	8d 04 55 00 00 00 00 	lea    eax,[edx*2+0x0]
 290:	8d 04 c0             	lea    eax,[eax+eax*8]
 293:	8b 4d 84             	mov    ecx,DWORD PTR [ebp-0x7c]
 296:	8d 0c 49             	lea    ecx,[ecx+ecx*2]
 299:	8d 04 48             	lea    eax,[eax+ecx*2]
 29c:	8a 44 05 88          	mov    al,BYTE PTR [ebp+eax*1-0x78]
 2a0:	8b 4d f4             	mov    ecx,DWORD PTR [ebp-0xc]
 2a3:	8b 55 f8             	mov    edx,DWORD PTR [ebp-0x8]
 2a6:	88 04 11             	mov    BYTE PTR [ecx+edx*1],al
 2a9:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 2ac:	8d 04 80             	lea    eax,[eax+eax*4]
 2af:	8d 04 80             	lea    eax,[eax+eax*4]
 2b2:	8d 04 80             	lea    eax,[eax+eax*4]
 2b5:	8b 8d 70 ff ff ff    	mov    ecx,DWORD PTR [ebp-0x90]
 2bb:	33 d2                	xor    edx,edx
 2bd:	8a 94 41 db 27 00 00 	mov    dl,BYTE PTR [ecx+eax*2+0x27db]
 2c4:	8d 04 55 00 00 00 00 	lea    eax,[edx*2+0x0]
 2cb:	8d 04 c0             	lea    eax,[eax+eax*8]
 2ce:	8b 4d 84             	mov    ecx,DWORD PTR [ebp-0x7c]
 2d1:	8d 0c 49             	lea    ecx,[ecx+ecx*2]
 2d4:	8d 04 48             	lea    eax,[eax+ecx*2]
 2d7:	0f bf 44 05 8a       	movsx  eax,WORD PTR [ebp+eax*1-0x76]
 2dc:	8d 04 80             	lea    eax,[eax+eax*4]
 2df:	03 c0                	add    eax,eax
 2e1:	89 85 74 ff ff ff    	mov    DWORD PTR [ebp-0x8c],eax
 2e7:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 2ea:	8d 04 80             	lea    eax,[eax+eax*4]
 2ed:	8d 04 80             	lea    eax,[eax+eax*4]
 2f0:	8d 04 80             	lea    eax,[eax+eax*4]
 2f3:	8b 8d 70 ff ff ff    	mov    ecx,DWORD PTR [ebp-0x90]
 2f9:	33 d2                	xor    edx,edx
 2fb:	8a 94 41 db 27 00 00 	mov    dl,BYTE PTR [ecx+eax*2+0x27db]
 302:	8d 04 55 00 00 00 00 	lea    eax,[edx*2+0x0]
 309:	8d 04 c0             	lea    eax,[eax+eax*8]
 30c:	8b 4d 84             	mov    ecx,DWORD PTR [ebp-0x7c]
 30f:	8d 0c 49             	lea    ecx,[ecx+ecx*2]
 312:	8d 04 48             	lea    eax,[eax+ecx*2]
 315:	0f bf 44 05 8c       	movsx  eax,WORD PTR [ebp+eax*1-0x74]
 31a:	8d 04 80             	lea    eax,[eax+eax*4]
 31d:	03 c0                	add    eax,eax
 31f:	83 c0 09             	add    eax,0x9
 322:	89 45 fc             	mov    DWORD PTR [ebp-0x4],eax
 325:	83 7d 0c 00          	cmp    DWORD PTR [ebp+0xc],0x0
 329:	0f 84 14 00 00 00    	je     0x343
 32f:	8b 85 74 ff ff ff    	mov    eax,DWORD PTR [ebp-0x8c]
 335:	03 45 fc             	add    eax,DWORD PTR [ebp-0x4]
 338:	99                   	cdq
 339:	2b c2                	sub    eax,edx
 33b:	d1 f8                	sar    eax,1
 33d:	89 85 74 ff ff ff    	mov    DWORD PTR [ebp-0x8c],eax
 343:	8b 55 fc             	mov    edx,DWORD PTR [ebp-0x4]
 346:	8b 8d 74 ff ff ff    	mov    ecx,DWORD PTR [ebp-0x8c]
 34c:	e8 00 00 00 00       	call   0x351
 351:	b9 0a 00 00 00       	mov    ecx,0xa
 356:	99                   	cdq
 357:	f7 f9                	idiv   ecx
 359:	8b 4d f4             	mov    ecx,DWORD PTR [ebp-0xc]
 35c:	8b 55 f8             	mov    edx,DWORD PTR [ebp-0x8]
 35f:	66 89 44 4a 05       	mov    WORD PTR [edx+ecx*2+0x5],ax
 364:	ff 45 f4             	inc    DWORD PTR [ebp-0xc]
 367:	e9 e4 fe ff ff       	jmp    0x250
 36c:	e9 00 00 00 00       	jmp    0x371
 371:	5f                   	pop    edi
 372:	5e                   	pop    esi
 373:	5b                   	pop    ebx
 374:	c9                   	leave
 375:	c2 08 00             	ret    0x8
