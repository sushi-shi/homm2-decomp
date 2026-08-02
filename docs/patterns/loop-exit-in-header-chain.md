# loop-exit-in-header-chain

**Symptom.** Our `if (cond) break;` inside a scan loop lowers as an inverted
jcc around a `jmp <exit>` stub; retail shows a single DIRECT conditional jump
to the exit:

    ours:   39 55 f4  cmp [ebp-0xc], edx      retail:  39 55 f4  cmp [ebp-0xc], edx
            7f 05     jg  past                         0f8e <exit> jle <exit>
            e9 <exit> jmp exit

**Close.** Move the break conditions into the loop header as one chain; the
whole backward hyphenation scan of font::ExtractLine is

    while (width >= maxWidth
           || (pos > wordStart + 1
               && !IsVowel(text[pos])
               && !((!IsVowel(text[pos+1]) && text[pos] == text[pos+1])
                    || text[pos] == '-'))) { width -= ...; pos--; }

Each condition then emits one direct jcc to its own exit-dispatch target.
Related: the od-goto-stub family — `if (cond) break` is jcc+jmp, header
conditions are direct jcc. Commit e4852e58.
