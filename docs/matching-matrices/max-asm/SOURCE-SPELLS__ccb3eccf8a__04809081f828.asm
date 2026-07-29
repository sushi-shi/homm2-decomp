; ?AddBolt@combatManager@@QAEXPAUSBolt@@HHHHHHHHHHHH@Z
; unit SOURCE/SPELLS  src_hash 04809081f828  score 100.0000  seed 31337  trial 53
; size 623  text_sha b417497d6b1b5c6a
; relocs ["000001cb:0014:_abs:00000000", "000001dc:0014:_abs:00000000", "0000020c:0014:_abs:00000000", "0000021e:0014:_abs:00000000", "00000245:0014:_sqrt:00000000", "0000024d:0014:__ftol:00000000", "0000025f:0014:?ResetBoltAngle@combatManager@@QAEXPAUSBolt@@@Z:00000000"]
; hex 558bec83ec18535657894df4837d0c000f8d07000000c7450c00000000817d0c7f0200000f8e07000000c7450c7f020000837d10000f8d07000000c7451000000000817d10ba0100000f8e07000000c74510ba010000837d14000f8d07000000c7451400000000817d147f0200000f8e07000000c745147f020000837d18000f8d07000000c7451800000000817d18ba0100000f8e07000000c74518ba0100008b450c8b4d0889018b45108b4d088941048b45148b4d088941088b45188b4d0889410c8b451c8b4d088941108b45208b4d088941148b45208b4d0889415c8b45248b4d088941608b45288b4d088941188b452c8b4d088941688b45308b4d0889416c8b45348b4d088941208b450c8945f0db45f08b4508d958248b45108945ecdb45ec8b4508d958288b450c8b4d0889412c8b45108b4d088941308b4508c74040000000008b4508c74044000000008b450c8b4d088941548b45108b4d088941588b4508c74070000000008b45388b4d08894174817d282c0100000f840d000000817d282f0100000f8535000000837d0c000f8e0d000000817d0c7f0200000f8c0f0000008b4508c7403401000000e90a0000008b4508c7403400000000e9410000008b45142b450c50e80000000083c4048bd88b45182b451050e80000000083c4043bd80f8e0f0000008b4508c7403401000000e90a0000008b4508c74034000000008b45142b450c50e80000000083c4048945fc8b45182b451050e80000000083c4048945f88b45fc0faf45fc8b4df80faf4df803c18945e8db45e883ec08dd1c24e80000000083c408e8000000008b4d088941648b4508508b4df4e800000000e9000000005f5e5bc9c23400

   0:	55                   	push   ebp
   1:	8b ec                	mov    ebp,esp
   3:	83 ec 18             	sub    esp,0x18
   6:	53                   	push   ebx
   7:	56                   	push   esi
   8:	57                   	push   edi
   9:	89 4d f4             	mov    DWORD PTR [ebp-0xc],ecx
   c:	83 7d 0c 00          	cmp    DWORD PTR [ebp+0xc],0x0
  10:	0f 8d 07 00 00 00    	jge    0x1d
  16:	c7 45 0c 00 00 00 00 	mov    DWORD PTR [ebp+0xc],0x0
  1d:	81 7d 0c 7f 02 00 00 	cmp    DWORD PTR [ebp+0xc],0x27f
  24:	0f 8e 07 00 00 00    	jle    0x31
  2a:	c7 45 0c 7f 02 00 00 	mov    DWORD PTR [ebp+0xc],0x27f
  31:	83 7d 10 00          	cmp    DWORD PTR [ebp+0x10],0x0
  35:	0f 8d 07 00 00 00    	jge    0x42
  3b:	c7 45 10 00 00 00 00 	mov    DWORD PTR [ebp+0x10],0x0
  42:	81 7d 10 ba 01 00 00 	cmp    DWORD PTR [ebp+0x10],0x1ba
  49:	0f 8e 07 00 00 00    	jle    0x56
  4f:	c7 45 10 ba 01 00 00 	mov    DWORD PTR [ebp+0x10],0x1ba
  56:	83 7d 14 00          	cmp    DWORD PTR [ebp+0x14],0x0
  5a:	0f 8d 07 00 00 00    	jge    0x67
  60:	c7 45 14 00 00 00 00 	mov    DWORD PTR [ebp+0x14],0x0
  67:	81 7d 14 7f 02 00 00 	cmp    DWORD PTR [ebp+0x14],0x27f
  6e:	0f 8e 07 00 00 00    	jle    0x7b
  74:	c7 45 14 7f 02 00 00 	mov    DWORD PTR [ebp+0x14],0x27f
  7b:	83 7d 18 00          	cmp    DWORD PTR [ebp+0x18],0x0
  7f:	0f 8d 07 00 00 00    	jge    0x8c
  85:	c7 45 18 00 00 00 00 	mov    DWORD PTR [ebp+0x18],0x0
  8c:	81 7d 18 ba 01 00 00 	cmp    DWORD PTR [ebp+0x18],0x1ba
  93:	0f 8e 07 00 00 00    	jle    0xa0
  99:	c7 45 18 ba 01 00 00 	mov    DWORD PTR [ebp+0x18],0x1ba
  a0:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
  a3:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  a6:	89 01                	mov    DWORD PTR [ecx],eax
  a8:	8b 45 10             	mov    eax,DWORD PTR [ebp+0x10]
  ab:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  ae:	89 41 04             	mov    DWORD PTR [ecx+0x4],eax
  b1:	8b 45 14             	mov    eax,DWORD PTR [ebp+0x14]
  b4:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  b7:	89 41 08             	mov    DWORD PTR [ecx+0x8],eax
  ba:	8b 45 18             	mov    eax,DWORD PTR [ebp+0x18]
  bd:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  c0:	89 41 0c             	mov    DWORD PTR [ecx+0xc],eax
  c3:	8b 45 1c             	mov    eax,DWORD PTR [ebp+0x1c]
  c6:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  c9:	89 41 10             	mov    DWORD PTR [ecx+0x10],eax
  cc:	8b 45 20             	mov    eax,DWORD PTR [ebp+0x20]
  cf:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  d2:	89 41 14             	mov    DWORD PTR [ecx+0x14],eax
  d5:	8b 45 20             	mov    eax,DWORD PTR [ebp+0x20]
  d8:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  db:	89 41 5c             	mov    DWORD PTR [ecx+0x5c],eax
  de:	8b 45 24             	mov    eax,DWORD PTR [ebp+0x24]
  e1:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  e4:	89 41 60             	mov    DWORD PTR [ecx+0x60],eax
  e7:	8b 45 28             	mov    eax,DWORD PTR [ebp+0x28]
  ea:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  ed:	89 41 18             	mov    DWORD PTR [ecx+0x18],eax
  f0:	8b 45 2c             	mov    eax,DWORD PTR [ebp+0x2c]
  f3:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  f6:	89 41 68             	mov    DWORD PTR [ecx+0x68],eax
  f9:	8b 45 30             	mov    eax,DWORD PTR [ebp+0x30]
  fc:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  ff:	89 41 6c             	mov    DWORD PTR [ecx+0x6c],eax
 102:	8b 45 34             	mov    eax,DWORD PTR [ebp+0x34]
 105:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
 108:	89 41 20             	mov    DWORD PTR [ecx+0x20],eax
 10b:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 10e:	89 45 f0             	mov    DWORD PTR [ebp-0x10],eax
 111:	db 45 f0             	fild   DWORD PTR [ebp-0x10]
 114:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 117:	d9 58 24             	fstp   DWORD PTR [eax+0x24]
 11a:	8b 45 10             	mov    eax,DWORD PTR [ebp+0x10]
 11d:	89 45 ec             	mov    DWORD PTR [ebp-0x14],eax
 120:	db 45 ec             	fild   DWORD PTR [ebp-0x14]
 123:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 126:	d9 58 28             	fstp   DWORD PTR [eax+0x28]
 129:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 12c:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
 12f:	89 41 2c             	mov    DWORD PTR [ecx+0x2c],eax
 132:	8b 45 10             	mov    eax,DWORD PTR [ebp+0x10]
 135:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
 138:	89 41 30             	mov    DWORD PTR [ecx+0x30],eax
 13b:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 13e:	c7 40 40 00 00 00 00 	mov    DWORD PTR [eax+0x40],0x0
 145:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 148:	c7 40 44 00 00 00 00 	mov    DWORD PTR [eax+0x44],0x0
 14f:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 152:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
 155:	89 41 54             	mov    DWORD PTR [ecx+0x54],eax
 158:	8b 45 10             	mov    eax,DWORD PTR [ebp+0x10]
 15b:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
 15e:	89 41 58             	mov    DWORD PTR [ecx+0x58],eax
 161:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 164:	c7 40 70 00 00 00 00 	mov    DWORD PTR [eax+0x70],0x0
 16b:	8b 45 38             	mov    eax,DWORD PTR [ebp+0x38]
 16e:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
 171:	89 41 74             	mov    DWORD PTR [ecx+0x74],eax
 174:	81 7d 28 2c 01 00 00 	cmp    DWORD PTR [ebp+0x28],0x12c
 17b:	0f 84 0d 00 00 00    	je     0x18e
 181:	81 7d 28 2f 01 00 00 	cmp    DWORD PTR [ebp+0x28],0x12f
 188:	0f 85 35 00 00 00    	jne    0x1c3
 18e:	83 7d 0c 00          	cmp    DWORD PTR [ebp+0xc],0x0
 192:	0f 8e 0d 00 00 00    	jle    0x1a5
 198:	81 7d 0c 7f 02 00 00 	cmp    DWORD PTR [ebp+0xc],0x27f
 19f:	0f 8c 0f 00 00 00    	jl     0x1b4
 1a5:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 1a8:	c7 40 34 01 00 00 00 	mov    DWORD PTR [eax+0x34],0x1
 1af:	e9 0a 00 00 00       	jmp    0x1be
 1b4:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 1b7:	c7 40 34 00 00 00 00 	mov    DWORD PTR [eax+0x34],0x0
 1be:	e9 41 00 00 00       	jmp    0x204
 1c3:	8b 45 14             	mov    eax,DWORD PTR [ebp+0x14]
 1c6:	2b 45 0c             	sub    eax,DWORD PTR [ebp+0xc]
 1c9:	50                   	push   eax
 1ca:	e8 00 00 00 00       	call   0x1cf
 1cf:	83 c4 04             	add    esp,0x4
 1d2:	8b d8                	mov    ebx,eax
 1d4:	8b 45 18             	mov    eax,DWORD PTR [ebp+0x18]
 1d7:	2b 45 10             	sub    eax,DWORD PTR [ebp+0x10]
 1da:	50                   	push   eax
 1db:	e8 00 00 00 00       	call   0x1e0
 1e0:	83 c4 04             	add    esp,0x4
 1e3:	3b d8                	cmp    ebx,eax
 1e5:	0f 8e 0f 00 00 00    	jle    0x1fa
 1eb:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 1ee:	c7 40 34 01 00 00 00 	mov    DWORD PTR [eax+0x34],0x1
 1f5:	e9 0a 00 00 00       	jmp    0x204
 1fa:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 1fd:	c7 40 34 00 00 00 00 	mov    DWORD PTR [eax+0x34],0x0
 204:	8b 45 14             	mov    eax,DWORD PTR [ebp+0x14]
 207:	2b 45 0c             	sub    eax,DWORD PTR [ebp+0xc]
 20a:	50                   	push   eax
 20b:	e8 00 00 00 00       	call   0x210
 210:	83 c4 04             	add    esp,0x4
 213:	89 45 fc             	mov    DWORD PTR [ebp-0x4],eax
 216:	8b 45 18             	mov    eax,DWORD PTR [ebp+0x18]
 219:	2b 45 10             	sub    eax,DWORD PTR [ebp+0x10]
 21c:	50                   	push   eax
 21d:	e8 00 00 00 00       	call   0x222
 222:	83 c4 04             	add    esp,0x4
 225:	89 45 f8             	mov    DWORD PTR [ebp-0x8],eax
 228:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
 22b:	0f af 45 fc          	imul   eax,DWORD PTR [ebp-0x4]
 22f:	8b 4d f8             	mov    ecx,DWORD PTR [ebp-0x8]
 232:	0f af 4d f8          	imul   ecx,DWORD PTR [ebp-0x8]
 236:	03 c1                	add    eax,ecx
 238:	89 45 e8             	mov    DWORD PTR [ebp-0x18],eax
 23b:	db 45 e8             	fild   DWORD PTR [ebp-0x18]
 23e:	83 ec 08             	sub    esp,0x8
 241:	dd 1c 24             	fstp   QWORD PTR [esp]
 244:	e8 00 00 00 00       	call   0x249
 249:	83 c4 08             	add    esp,0x8
 24c:	e8 00 00 00 00       	call   0x251
 251:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
 254:	89 41 64             	mov    DWORD PTR [ecx+0x64],eax
 257:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 25a:	50                   	push   eax
 25b:	8b 4d f4             	mov    ecx,DWORD PTR [ebp-0xc]
 25e:	e8 00 00 00 00       	call   0x263
 263:	e9 00 00 00 00       	jmp    0x268
 268:	5f                   	pop    edi
 269:	5e                   	pop    esi
 26a:	5b                   	pop    ebx
 26b:	c9                   	leave
 26c:	c2 34 00             	ret    0x34
