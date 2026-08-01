"""homm2.format - `homm2 format` - whitespace normalization for matching source.

  headers  top-level spacing in include/**.h: guard, includes, declarations
  enums    assignment alignment inside each H2_ENUM_* block

Both read a file list, or a single file from stdin as `-`, which is how the nvim
overlay chains them after clang-format on save. `--check` reports without writing.

These shape the MATCHING tree and are byte-neutral by construction: they only move
whitespace, and /Od stack slots hash identifiers, not layout. Deriving the shipped
tree is a different role and lives in homm2/clean/.
"""
