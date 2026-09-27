// C49, parent d216dd01: all3/3 arms complete in 3.602s, source restored.
// Original: nValue = (i32)((float)nAttack * chanceA);
// Explicit: nValue = static_cast<i32>(static_cast<float>(nAttack) * chanceA);
// Retained: nValue = static_cast<i32>(nAttack * chanceA);
// build/c49-likelihood-{axes,manifest}.json and -batch/results.json.
// Every arm:84 bytes,1 relocation, strict exact; text2aa146732cba2117,
// normalized relocations46766e90b39c5837. Final independent retail proof and
// all89 PHILAI emitted functions/allocated sections/native relocs unchanged.
