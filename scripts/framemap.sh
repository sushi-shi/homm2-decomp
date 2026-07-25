#!/usr/bin/env bash
# framemap.sh UNIT MANGLED — histograms + head stores for both objs
U=$1; S=$2
for o in "build/objdiff/base/$U.obj OURS" "build/delink/$U.c.obj RETAIL"; do
  set -- $o
  echo "== $2"
  llvm-objdump -d -M intel --no-show-raw-insn --disassemble-symbols="$S" $1 2>/dev/null | grep -m1 "sub.*esp"
  llvm-objdump -d -M intel --no-show-raw-insn $1 2>/dev/null | awk -v sym="$S" 'index($0, " <"sym">:"){on=1;next} on && /^[0-9a-f]+ </ && $0 !~ /\$L/{exit} on' > /tmp/fm_$2.txt
  grep -oE "ebp - 0x[0-9a-f]+" /tmp/fm_$2.txt | sort | uniq -c | sort -k3 | awk '{printf "%s:%s ", $4, $1}'
  echo
done
