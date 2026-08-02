# member-static-guarded-teardown

**Symptom.** A file-scope static with dynamic init produces the $E trio
(driver 0xf, init 0x61, atexit-register 0x12) plus an UNGUARDED teardown
(0x29): `if (g) { g->unref(); g = 0; }`. Retail's teardown is 0x46 and
guarded by a shared flag byte with one bit per object:

    33c0 a0 <flag> 83e001 85c0 7533     ; if (flag & 1) return
    8a0d <flag> 80c901 880d <flag>      ; flag |= 1
    833d <g> 00 741b ... ff5104 ...     ; if (g) { g->unref(); g = 0; }

(second object identical with `83e002` / `80c902`).

Probe A/B: file statics and function-local statics both fail to reproduce it;
**static class data members** (`struct Bank { static P sA; static P sB; };`
with file-scope definitions) produce the guarded 0x46 teardown byte-for-byte,
bits assigned in definition order.

**Close.** `AudiereMusicState::stream/source` in BASE/soundBackends.h,
definitions at the top of AudiereMusic.cpp. $E14/$E19 init bodies stayed
masked-exact throughout — only the teardown flavor discriminates.
Commit 9182c077.
