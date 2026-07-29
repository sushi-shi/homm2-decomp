; ?ResetCyclingCreatures@combatManager@@QAEXXZ
; unit SOURCE/COMMAND  src_hash bcc2bb1d844a  score 100.0000  seed 1129923404  trial 1
; size 567  text_sha ab67ca7f8c7b3a0f
; relocs ["00000047:0006:?gpCombatManager@@3PAVcombatManager@@A:00000000", "00000085:0006:?gpCombatManager@@3PAVcombatManager@@A:00000000", "000000cf:0006:?gpCombatManager@@3PAVcombatManager@@A:00000000", "00000103:0006:?gpCombatManager@@3PAVcombatManager@@A:00000000", "00000108:0014:?DrawFrame@combatManager@@QAEXHHHHHHH@Z:00000000", "00000139:0006:?gpCombatManager@@3PAVcombatManager@@A:00000000", "00000177:0006:?gpCombatManager@@3PAVcombatManager@@A:00000000", "000001bd:0006:?gpCombatManager@@3PAVcombatManager@@A:00000000", "000001e1:0014:?KBTickCount@@YIJXZ:00000000", "000001f9:0014:?KBTickCount@@YIJXZ:00000000", "00000207:0014:?KBTickCount@@YIJXZ:00000000", "00000224:0006:?gpCombatManager@@3PAVcombatManager@@A:00000000", "00000229:0014:?DrawFrame@combatManager@@QAEXHHHHHHH@Z:00000000"]
; hex 558bec83ec18535657894de8c745f400000000c745fc00000000c745f800000000e903000000ff45f8837df8020f8db1000000c745f000000000e903000000ff45f08b45f88b0d000000008b55f0399481473500000f8e840000008b45f08bc8c1e0038d04c08d04c18b4df88bd1c1e1062bca8d0ccac1e1032bca8d0c4903c98d0441030500000000054f3500008945f48b45f4f680d0000000100f85390000008b45f483787e080f8c2c0000008b45f483787e0c0f8f1f000000ff45fc8b45f08b4df88d0c89c1e1048d04818b0d00000000ff84087ff30000e960ffffffe942ffffff837dfc000f8505000000e93f0100006a016a016a4b6a016a016a016a008b0d00000000e800000000c745f800000000e903000000ff45f8837df8020f8dd3000000c745f000000000e903000000ff45f08b45f88b0d000000008b55f0399481473500000f8ea60000008b45f08bc8c1e0038d04c08d04c18b4df88bd1c1e1062bca8d0ccac1e1032bca8d0c4903c98d0441030500000000054f3500008945f48b45f4f680d0000000100f855b0000008b45f08bc8c1e0038d04c08d04c18b4df88bd1c1e1062bca8d0ccac1e1032bca8d0c4903c98d0441030500000000054f3500008945f48b45f4c7407e070000008b45f4c7808200000000000000e8000000008b4df48981ee000000e93effffffe920ffffffe8000000008b4de88981bd330000e8000000008b4de88981c13300006a016a016a4b6a006a006a016a018b0d00000000e800000000e9000000005f5e5bc9c3

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 18             	sub    esp,0x18
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 4d e8             	mov    DWORD PTR [ebp-0x18],ecx
   c:	c7 45 f4 00 00 00 00 	mov    DWORD PTR [ebp-0xc],0x0
  13:	c7 45 fc 00 00 00 00 	mov    DWORD PTR [ebp-0x4],0x0
  1a:	c7 45 f8 00 00 00 00 	mov    DWORD PTR [ebp-0x8],0x0
  21:	e9 03 00 00 00       	jmp    0x29
  26:	ff 45 f8             	inc    DWORD PTR [ebp-0x8]
  29:	83 7d f8 02          	cmp    DWORD PTR [ebp-0x8],0x2
  2d:	0f 8d b1 00 00 00    	jge    0xe4
  33:	c7 45 f0 00 00 00 00 	mov    DWORD PTR [ebp-0x10],0x0
  3a:	e9 03 00 00 00       	jmp    0x42
  3f:	ff 45 f0             	inc    DWORD PTR [ebp-0x10]
  42:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
  45:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
  4b:	8b 55 f0             	mov    edx,DWORD PTR [ebp-0x10]
  4e:	39 94 81 47 35 00 00 	cmp    DWORD PTR [ecx+eax*4+0x3547],edx
  55:	0f 8e 84 00 00 00    	jle    0xdf
  5b:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  5e:	8b c8                	mov    ecx,eax
  60:	c1 e0 03             	shl    eax,0x3
  63:	8d 04 c0             	lea    eax,[eax+eax*8]
  66:	8d 04 c1             	lea    eax,[ecx+eax*8]
  69:	8b 4d f8             	mov    ecx,DWORD PTR [ebp-0x8]
  6c:	8b d1                	mov    edx,ecx
  6e:	c1 e1 06             	shl    ecx,0x6
  71:	2b ca                	sub    ecx,edx
  73:	8d 0c ca             	lea    ecx,[edx+ecx*8]
  76:	c1 e1 03             	shl    ecx,0x3
  79:	2b ca                	sub    ecx,edx
  7b:	8d 0c 49             	lea    ecx,[ecx+ecx*2]
  7e:	03 c9                	add    ecx,ecx
  80:	8d 04 41             	lea    eax,[ecx+eax*2]
  83:	03 05 00 00 00 00    	add    eax,DWORD PTR ds:0x0
  89:	05 4f 35 00 00       	add    eax,0x354f
  8e:	89 45 f4             	mov    DWORD PTR [ebp-0xc],eax
  91:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
  94:	f6 80 d0 00 00 00 10 	test   BYTE PTR [eax+0xd0],0x10
  9b:	0f 85 39 00 00 00    	jne    0xda
  a1:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
  a4:	83 78 7e 08          	cmp    DWORD PTR [eax+0x7e],0x8
  a8:	0f 8c 2c 00 00 00    	jl     0xda
  ae:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
  b1:	83 78 7e 0c          	cmp    DWORD PTR [eax+0x7e],0xc
  b5:	0f 8f 1f 00 00 00    	jg     0xda
  bb:	ff 45 fc             	inc    DWORD PTR [ebp-0x4]
  be:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  c1:	8b 4d f8             	mov    ecx,DWORD PTR [ebp-0x8]
  c4:	8d 0c 89             	lea    ecx,[ecx+ecx*4]
  c7:	c1 e1 04             	shl    ecx,0x4
  ca:	8d 04 81             	lea    eax,[ecx+eax*4]
  cd:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
  d3:	ff 84 08 7f f3 00 00 	inc    DWORD PTR [eax+ecx*1+0xf37f]
  da:	e9 60 ff ff ff       	jmp    0x3f
  df:	e9 42 ff ff ff       	jmp    0x26
  e4:	83 7d fc 00          	cmp    DWORD PTR [ebp-0x4],0x0
  e8:	0f 85 05 00 00 00    	jne    0xf3
  ee:	e9 3f 01 00 00       	jmp    0x232
  f3:	6a 01                	push   0x1
  f5:	6a 01                	push   0x1
  f7:	6a 4b                	push   0x4b
  f9:	6a 01                	push   0x1
  fb:	6a 01                	push   0x1
  fd:	6a 01                	push   0x1
  ff:	6a 00                	push   0x0
 101:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
 107:	e8 00 00 00 00       	call   0x10c
 10c:	c7 45 f8 00 00 00 00 	mov    DWORD PTR [ebp-0x8],0x0
 113:	e9 03 00 00 00       	jmp    0x11b
 118:	ff 45 f8             	inc    DWORD PTR [ebp-0x8]
 11b:	83 7d f8 02          	cmp    DWORD PTR [ebp-0x8],0x2
 11f:	0f 8d d3 00 00 00    	jge    0x1f8
 125:	c7 45 f0 00 00 00 00 	mov    DWORD PTR [ebp-0x10],0x0
 12c:	e9 03 00 00 00       	jmp    0x134
 131:	ff 45 f0             	inc    DWORD PTR [ebp-0x10]
 134:	8b 45 f8             	mov    eax,DWORD PTR [ebp-0x8]
 137:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
 13d:	8b 55 f0             	mov    edx,DWORD PTR [ebp-0x10]
 140:	39 94 81 47 35 00 00 	cmp    DWORD PTR [ecx+eax*4+0x3547],edx
 147:	0f 8e a6 00 00 00    	jle    0x1f3
 14d:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 150:	8b c8                	mov    ecx,eax
 152:	c1 e0 03             	shl    eax,0x3
 155:	8d 04 c0             	lea    eax,[eax+eax*8]
 158:	8d 04 c1             	lea    eax,[ecx+eax*8]
 15b:	8b 4d f8             	mov    ecx,DWORD PTR [ebp-0x8]
 15e:	8b d1                	mov    edx,ecx
 160:	c1 e1 06             	shl    ecx,0x6
 163:	2b ca                	sub    ecx,edx
 165:	8d 0c ca             	lea    ecx,[edx+ecx*8]
 168:	c1 e1 03             	shl    ecx,0x3
 16b:	2b ca                	sub    ecx,edx
 16d:	8d 0c 49             	lea    ecx,[ecx+ecx*2]
 170:	03 c9                	add    ecx,ecx
 172:	8d 04 41             	lea    eax,[ecx+eax*2]
 175:	03 05 00 00 00 00    	add    eax,DWORD PTR ds:0x0
 17b:	05 4f 35 00 00       	add    eax,0x354f
 180:	89 45 f4             	mov    DWORD PTR [ebp-0xc],eax
 183:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
 186:	f6 80 d0 00 00 00 10 	test   BYTE PTR [eax+0xd0],0x10
 18d:	0f 85 5b 00 00 00    	jne    0x1ee
 193:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
 196:	8b c8                	mov    ecx,eax
 198:	c1 e0 03             	shl    eax,0x3
 19b:	8d 04 c0             	lea    eax,[eax+eax*8]
 19e:	8d 04 c1             	lea    eax,[ecx+eax*8]
 1a1:	8b 4d f8             	mov    ecx,DWORD PTR [ebp-0x8]
 1a4:	8b d1                	mov    edx,ecx
 1a6:	c1 e1 06             	shl    ecx,0x6
 1a9:	2b ca                	sub    ecx,edx
 1ab:	8d 0c ca             	lea    ecx,[edx+ecx*8]
 1ae:	c1 e1 03             	shl    ecx,0x3
 1b1:	2b ca                	sub    ecx,edx
 1b3:	8d 0c 49             	lea    ecx,[ecx+ecx*2]
 1b6:	03 c9                	add    ecx,ecx
 1b8:	8d 04 41             	lea    eax,[ecx+eax*2]
 1bb:	03 05 00 00 00 00    	add    eax,DWORD PTR ds:0x0
 1c1:	05 4f 35 00 00       	add    eax,0x354f
 1c6:	89 45 f4             	mov    DWORD PTR [ebp-0xc],eax
 1c9:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
 1cc:	c7 40 7e 07 00 00 00 	mov    DWORD PTR [eax+0x7e],0x7
 1d3:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
 1d6:	c7 80 82 00 00 00 00 	mov    DWORD PTR [eax+0x82],0x0
 1dd:	00 00 00 
 1e0:	e8 00 00 00 00       	call   0x1e5
 1e5:	8b 4d f4             	mov    ecx,DWORD PTR [ebp-0xc]
 1e8:	89 81 ee 00 00 00    	mov    DWORD PTR [ecx+0xee],eax
 1ee:	e9 3e ff ff ff       	jmp    0x131
 1f3:	e9 20 ff ff ff       	jmp    0x118
 1f8:	e8 00 00 00 00       	call   0x1fd
 1fd:	8b 4d e8             	mov    ecx,DWORD PTR [ebp-0x18]
 200:	89 81 bd 33 00 00    	mov    DWORD PTR [ecx+0x33bd],eax
 206:	e8 00 00 00 00       	call   0x20b
 20b:	8b 4d e8             	mov    ecx,DWORD PTR [ebp-0x18]
 20e:	89 81 c1 33 00 00    	mov    DWORD PTR [ecx+0x33c1],eax
 214:	6a 01                	push   0x1
 216:	6a 01                	push   0x1
 218:	6a 4b                	push   0x4b
 21a:	6a 00                	push   0x0
 21c:	6a 00                	push   0x0
 21e:	6a 01                	push   0x1
 220:	6a 01                	push   0x1
 222:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
 228:	e8 00 00 00 00       	call   0x22d
 22d:	e9 00 00 00 00       	jmp    0x232
 232:	5f                   	pop    edi
 233:	5e                   	pop    esi
 234:	5b                   	pop    ebx
 235:	c9                   	leave
 236:	c3                   	ret
