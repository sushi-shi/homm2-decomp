/*
CatAttack, retail RVA 0x0009311f, size 0x100e.

This file records rejected source experiments. It is evidence, not compiled
source. The prior state census and its 99.9977% durable MAX remain authoritative;
the standalone matrix scorer below uses a different private-symbol comparison
normalization and its lower percentages are ranking evidence only.

Current structural audit
------------------------

The clean candidate and retail function have 173/173 exact basic blocks, 1,027
identical relocation-masked instructions, matching 244-byte frames and saved
registers, and 150 ordered relocations on each side. Raw candidate/retail bytes
differ only at three commutative ownership sites already classified in
docs/matching-matrices/cmbtmgr-catattack-tu-state-0a9f3c.tsv:

  A: (startDirection + frame18) before the direction modulus
  B: projectileX11 += xStep0
  C: the indexed damaged-wall compound assignment

Direct expression matrix
------------------------

The following reviewed axes were crossed with the clean state and 50 forest
states (204/204 candidates completed):

  build/catattack-commutative-axes-20260727.json
  build/catattack-commutative-manifest-20260727.json
  build/match-variants/catattack-commutative-20260727/

The attempted code was:

    (startDirection + frame18) % COMBAT_CATAPULT_DIRECTION_COUNT
    (frame18 + startDirection) % COMBAT_CATAPULT_DIRECTION_COUNT

and:

    projectileX11 += xStep0;
    projectileX11 = xStep0 + projectileX11;

All four source combinations were byte-neutral within every corresponding
state. The standalone scorer's best was 99.978330%, size 4110, relocations
150/150. No source form produced an exact island, so the canonical spellings
remain.

Indexed wall-update structural matrix
-------------------------------------

Three high-level assignment forms were crossed with the clean state and 50
typedef states using seed 0x43415441 (153/153 candidates completed):

  build/catattack-wall-update-axes-20260727.json
  build/catattack-wall-update-manifest-20260727.json
  build/match-variants/catattack-wall-update-20260727/

The attempted code was:

    m_wallStates[index] += IDX(damageLevel13);

    m_wallStates[index] =
        m_wallStates[index] + IDX(damageLevel13);

    m_wallStates[index] =
        IDX(damageLevel13) + m_wallStates[index];

The explicit and compound forms were byte-neutral in every state. The best
standalone score was 99.980610%, size 4110, relocations 150/150, in each of the
three forms. This reproduced a known high state but did not create the missing
exact three-bit combination. No form was retained.

Inline direction-sum diagnostic
-------------------------------

A materially different hypothesis wrapped the direction sum in a small inline
helper, with both formal-parameter orders. Its requested matrix and artifacts
are:

  build/catattack-inline-direction-axes-20260727.json
  build/catattack-inline-direction-manifest-20260727.json
  build/match-variants/catattack-inline-direction-20260727/

The direct arms compiled, but all 102 helper arms failed under MSVC 4.2 at the
generated call/modulus expression. This is a failed diagnostic, not completed
coverage of the inline-helper hypothesis, and it excludes nothing. No helper or
generated source was retained.

Disposition
-----------

The effective CatAttack source is restored byte-for-byte. These complete
matrices do not raise its existing 99.9977% MAX. A future lane should not repeat
commutative spellings or indexed assignment spellings; it needs a different
source structure that can change exactly one of A/B/C without altering the
already exact CFG, frame, size, or relocation stream.

Current-hash completion and corrected inline screen
---------------------------------------------------

A later lane completed the direct-expression matrix against the integrated
source hash with the previously omitted retail-directed float spelling:

    projectileX11 = projectileX11 + xStep0;

It crossed all six direction/float forms with the clean state and 50 forest
states (306/306 candidates completed):

  build/catattack-current-commutative-manifest-20260727.json
  build/match-variants/catattack-current-commutative-20260727/

All three float spellings and both integer operand orders remained byte-neutral
within every corresponding state. The best standalone score was 99.978330%,
size 4110, relocations 150/150.

The malformed earlier inline diagnostic was also corrected and screened as
four clean structural versions: direct, direction helper, projectile-X helper,
and both helpers. Both valid one-helper versions emitted a five-byte `/Ob1`
continuation and grew the function to 4115 bytes; the combined form grew it to
4120 bytes. Scores fell to 99.862030% and 99.748000%, respectively. Because
retail remains 4110 bytes with no corresponding continuation, these helper
orbits contradict retail size and were pruned before state permutations:

  build/catattack-inline-arithmetic-manifest-20260727.json
  build/match-variants/catattack-inline-arithmetic-20260727/
*/
