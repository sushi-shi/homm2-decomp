; ?FindToken@@YIPADPADD@Z
; unit BASE/Misc  src_hash ca404ff868df  score 99.6552  seed 1213156658  trial 2
; size 49  text_sha 85c89094b21cf166
; relocs []
; hex 532bc056578bf98bf1b9fffffffff2aef7d14933c085c97e0c8a1c303ada740b403bc87ff433c05f5e5bc303c65f5e5bc3

   0:	53                   	push   ebx
   1:	2b c0                	sub    eax,eax
   3:	56                   	push   esi
   4:	57                   	push   edi
   5:	8b f9                	mov    edi,ecx
   7:	8b f1                	mov    esi,ecx
   9:	b9 ff ff ff ff       	mov    ecx,0xffffffff
   e:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  10:	f7 d1                	not    ecx
  12:	49                   	dec    ecx
  13:	33 c0                	xor    eax,eax
  15:	85 c9                	test   ecx,ecx
  17:	7e 0c                	jle    0x25
  19:	8a 1c 30             	mov    bl,BYTE PTR [eax+esi*1]
  1c:	3a da                	cmp    bl,dl
  1e:	74 0b                	je     0x2b
  20:	40                   	inc    eax
  21:	3b c8                	cmp    ecx,eax
  23:	7f f4                	jg     0x19
  25:	33 c0                	xor    eax,eax
  27:	5f                   	pop    edi
  28:	5e                   	pop    esi
  29:	5b                   	pop    ebx
  2a:	c3                   	ret
  2b:	03 c6                	add    eax,esi
  2d:	5f                   	pop    edi
  2e:	5e                   	pop    esi
  2f:	5b                   	pop    ebx
  30:	c3                   	ret
