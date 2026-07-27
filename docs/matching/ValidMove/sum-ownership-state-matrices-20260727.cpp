/*
advManager::ValidMove, retail RVA 0x0000fa15, size 0x4f2.

This file records rejected source experiments. It is evidence, not compiled
source.

Structural audit
----------------

The clean candidate and retail function have 57/57 exact basic blocks, 352
identical relocation-masked instructions, matching 60-byte frames and saved
registers, and 9/9 ordered relocations with no candidate-only target. The raw
1266-byte comparison initially had six differing bytes in two operand-ownership
spans:

  +0x211/+0x214:
      centerY_n + directionY_h

  +0x442/+0x443 and +0x44b/+0x44c:
      m_cursorMapY + m_mapOriginY + 1

The first span is a stack-local accumulator choice. The second is the analogous
member-field accumulator choice in the inlined mapData::GetCell expression.

Flat sum-order matrix
---------------------

Both orders of each sum were crossed with the clean state and 50 forest states
(204/204 candidates completed):

  build/validmove-sum-order-axes-20260727.json
  build/validmove-sum-order-manifest-20260727.json
  build/match-variants/validmove-sum-order-20260727/

The attempted source forms were:

    centerY_n + directionY_h
    directionY_h + centerY_n

and:

    m_cursorMapY + m_mapOriginY + 1
    m_mapOriginY + m_cursorMapY + 1

MSVC canonicalized both source-order choices within every corresponding state.
Forest trials 22, 35, and 38 formed a higher state at 99.994170%, size 1266,
relocations 9/9, and the same exact CFG. The state fixes the first span and
leaves only the four-byte member-field span. Trial 22 was replayed unchanged and
officially raised the hash-scoped MAX from 99.9883% to 99.9942%:

  build/tu-state-noise/validmove-trial22-record-max-20260727/
  build/validmove-trial22-record-max-states-20260727.json

Arithmetic-tree matrix
----------------------

Because a flat swap was byte-neutral, four different arithmetic trees for the
remaining Y coordinate were crossed with clean plus the same 50 forest states
(204/204 candidates completed):

  build/validmove-south-y-tree-axes-20260727.json
  build/validmove-south-y-tree-manifest-20260727.json
  build/match-variants/validmove-south-y-tree-20260727/

The attempted trees were:

    (m_cursorMapY + m_mapOriginY) + 1
    (m_mapOriginY + m_cursorMapY) + 1
    m_mapOriginY + (m_cursorMapY + 1)
    m_cursorMapY + (m_mapOriginY + 1)

All four trees were byte-neutral in every corresponding state. At trial 22 each
remained at 99.994170%, so none recovered the final member-field ownership span.
No source form was retained.

Sibling X-argument ownership
----------------------------

The other argument to the same inlined GetCell call was tested as a structural
ownership lever across clean plus 50 forest states (102/102 candidates
completed):

  build/validmove-south-x-order-axes-20260727.json
  build/validmove-south-x-order-manifest-20260727.json
  build/match-variants/validmove-south-x-order-20260727/

The attempted forms were:

    m_cursorMapX + m_mapOriginX
    m_mapOriginX + m_cursorMapX

The reversed X form is a coherent lower island. At trial 22 it moves the
otherwise 99.994170% object back to 99.988335% while preserving retail size,
57/57 exact blocks, and 9/9 relocations. It did not close the final Y span and
was rejected, but it remains a legitimate structural branch seed rather than a
fuzzy-pruned impossibility.

Disposition
-----------

The reconstructed source is restored byte-for-byte. No generated declarations
or probes remain. The durable result is the unchanged-source 99.9942% MAX and a
four-byte, non-relocation member-accumulator residual. Future work should start
from a materially different inlined GetCell ownership structure; flat order,
parenthesization, and sibling X order are fully covered here.
*/
