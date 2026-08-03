# goto-label-jump-stub

**Symptom.** A loop exit that our `break;` lowers as ONE direct jump reaches
retail's join through a two-hop stub parked after the last statement, with a
short jump hopping over the stub into the epilogue:

    ours (break;)                        retail
    ------------------------------------ ---------------------------------------
    ; loop body exit                     ; loop body exit
    e9 48 01 00 00  jmp <join>           e9 59 02 00 00  jmp <stub>
    ...                                  ...
    ; last statement, then epilogue      ; last statement, then
    5b              pop  ebx             eb 05           jmp <epilogue>
    8b e5           mov  esp, ebp        e9 e7 fe ff ff  jmp <join>   ; <stub>
    5d              pop  ebp             5b              pop  ebx
    c2 0c 00        ret  0xc             8b e5           mov  esp, ebp
                                         5d              pop  ebp
                                         c2 0c 00        ret  0xc

(CreateDiffFile 0x5f47a. `<join>` is the first statement after the
`if (...) {...} else { ... while (1) {...} }`, i.e. +0x433.)

**Cause.** VC6 /Od gives a source `goto` its own label id. When the label
happens to sit exactly where an existing block already starts, the two labels
are not coalesced: the goto's label is emitted at the END of the function as a
`jmp` to the real one, and the fall-through into the epilogue jumps over it. A
`break` reuses the loop's own exit label and jumps direct.

**Close.** Spell the loop exit as a goto to a label placed on the join:

                    length = 0;
                    goto Finish;
                }
                ...
        }
    Finish:
        sprintf(gText, "%s%s", ".\\DATA\\", diffName);

Measured: `break` 1356/1359 bytes, `goto Finish` 1363 = retail size, EXACT.
Wrapping the tail in `for(;;)`, `while(1)`, `do{}while(0)` or appending a dead
`goto` after `return;` all produce byte-identical output to the plain `break`
(VC6 drops the unreachable back edge), so the stub can only come from a live
`goto`.

Related: [if-else-two-jmp-backedge](if-else-two-jmp-backedge.md) is the same
jmp-to-jmp shape produced by if/else nesting rather than by a label id.
