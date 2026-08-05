/*
textEntryWidget::Read (RVA 0x000d8920, retail size 620)
Gold/Buka 2.1 field-publication matrix, 2026-08-05.

The PoL reconstruction already had the retail ten-block CFG, size, and ordered
52/52 relocations.  Its only remaining divergence was a two-instruction
schedule in the default rectangle branch, for a score of 98.675500%.

The exact Gold/Buka source supplied three independently testable facts:

  - mask the serialized alignment word,
  - publish m_maxLines and m_preserveTextOnFocus directly rather than through
    an invented preserveText temporary,
  - read the serialized m_kind field before overriding it.

The complete 2 x 2 x 2 x 51 matrix finished 408/408 builds.  Every arm with
direct default-field publication was exact, including the clean state; the
alignment mask and kind publication were byte-neutral in this PoL compiler
orbit.  Every temporary-owned arm retained the old residual.  The accepted
source is therefore the minimal direct structure: clean 100.000000%, size 620,
exact 10/10 CFG, and ordered 52/52 relocations.

Artifacts:
  build/textentry-read-cross-version-axes.json
  build/textentry-read-cross-version-manifest.json
  build/source-variant-batch/textentry-read-cross-version/results.json

Disposition: retain only the direct field publication.  Do not add compiler-
erased donor spellings or generated state declarations.
*/

#if 0
m_rectH = m_height;
m_maxLines = 1;
if (type == TEXT_ENTRY_READ_MULTILINE)
    m_preserveTextOnFocus = 1;
else
    m_preserveTextOnFocus = 0;
#endif
