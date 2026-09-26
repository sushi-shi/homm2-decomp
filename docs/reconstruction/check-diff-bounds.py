"""Source-derived diff output bound and header-format checks, not game runtime tests.

The separate complete retail checks pin WriteDiffHeaderInfo/GetSkipCopyLen/CreateDiffFile.
This model intentionally retains the old-file terminal length, including its known input
overread/truncation consequences; it proves destination capacity, not round-trip correctness.
"""
import itertools,re
from pathlib import Path

source=Path('src/SOURCE/GAME.cpp').read_text()
constants={
 'DIFF_WORD_SHIFT':16,'DIFF_WORD_HEADER_SIZE':3,'DIFF_BYTE_SHIFT':8,
 'DIFF_BYTE_HEADER_SIZE':2,'DIFF_BUFFER_EXTRA':5000,'DIFF_MAX_SHORT_MATCH':3,
 'COMMAND_SHIFT':7,'LEN_WORD_FLAG':0x40,'LEN_BYTE_FLAG':0x20,
 'LEN_WORD_HIGH_MASK':0x3f,'LEN_HIGH_MASK':0x2f0000,'LEN_LOW_MASK':0xffff,
 'LEN_SHORT_MASK':0x1f,'LEN_MID_MASK':0x1f00,'LEN_BYTE_MASK':0xff,
 'LEN_BYTE_MAX':0x1f,'LEN_WORD_MAX':0x1fff,'JOIN_HEADER_SIZE':2,
}
for name,value in constants.items():
    m=re.search(r'\b'+name+r'\s*=\s*(0x[0-9a-f]+|[0-9]+)\b',source)
    assert m and int(m.group(1),0)==value,(name,value)
for fragment in [
 'length = oldSize - position;',
 'memcpy(diffOut + diffTotal, fullData + position, length);',
 'if (matchLen <= DIFF_MAX_SHORT_MATCH)',
 'WriteDiffHeaderInfo(0, matchLen, diffOut, &diffTotal);',
 'H2_ALLOC((oldSize > joinSize ? oldSize : joinSize) + DIFF_BUFFER_EXTRA)',
]:assert fragment in source,fragment

def width(length):
    assert length>=0
    return 3 if length>8191 else (2 if length>31 else 1)

def encoded_header(command,length):
    assert command in (0,1) and length>=0
    flags=command<<7
    if length>8191:
        flags|=0x40|(length&0x2f0000)>>16
        return bytes([flags,length&255,(length>>8)&255])
    if length>31:
        flags|=0x20|(length&0x1f00)>>8
        return bytes([flags,length&255])
    return bytes([flags|length])

def decoded_header(header):
    flag=header[0]
    if flag&0x40:return ((flag&0x3f)<<16)|header[1]|header[2]<<8
    if flag&0x20:return ((flag&0x1f)<<8)|header[1]
    return flag&0x1f

def emitted_size(old,new):
    position=length=0
    output=2
    while True:
        if position+length>=len(old) or position+length>=len(new):
            length=len(old)-position
            output+=width(length)+length
            # Deliberately count the inherited terminal copy without reading beyond new.
            return output
        if old[position+length]==new[position+length]:
            match=1
            while (position+length+match<len(old) and position+length+match<len(new)
                   and old[position+length+match]==new[position+length+match]):
                match+=1
            if match<=3:
                length+=match
            else:
                if length:
                    output+=width(length)+length
                    position+=length
                    length=0
                assert match-width(match)>=3
                output+=width(match)
                position+=match
        else:
            while (position+length<len(old) and position+length<len(new)
                   and old[position+length]!=new[position+length]):
                length+=1

sequences=[bytes(bits) for size in range(9) for bits in itertools.product((0,1),repeat=size)]
count=0
for old in sequences:
    for new in sequences:
        total=emitted_size(old,new)
        assert total<=len(old)+5<=max(len(old),len(new))+5000
        count+=1
print('PASS',count,'binary old/new pairs through length8: diff output <= oldSize+5')
for length in [0,1,3,4,31,32,8191,8192,65535,65536,700000]:
    a,b=bytes(length),b'\x01'*length
    assert emitted_size(a,b)==2+width(length)+length
    assert emitted_size(a,a)<=length+5
    assert width(length)==len(encoded_header(0,length))
print('PASS byte/word thresholds and tight oldSize+5 example at length8192')
for length in range(700001):
    for command in (0,1):
        header=encoded_header(command,length)
        assert decoded_header(header)==length and header[0]>>7==command
print('PASS all lengths0..700000 and both commands round-trip through the modeled header')
assert decoded_header(encoded_header(0,0x100000))==0
print('RETAINED FORMAT LIMIT: length0x100000 loses bit20 and decodes as0; no mask correction applied')
print('CONTRACT: nonnegative sizes, no signed arithmetic overflow, successful storage; output proof does not establish readable terminal input or correct join content')
