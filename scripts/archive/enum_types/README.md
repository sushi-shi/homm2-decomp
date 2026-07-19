# Archived enum-type census

This package preserves the one-off integer-domain census, strict Clang probe,
and symbol audit used during the enum migration. It is not an active `homm2`
command or test package.

The source parser expects direct enum declarations and a provisional
`config/integer_domains.toml` classification manifest. The reconstructed code
now uses dual-mode `H2_ENUM_*` declarations, keeps TU-only domains in `.cpp`
files, and is reviewed by `homm2 constants`, so those assumptions no longer
hold. Update the parser and supply an explicit experiment manifest before
trying to reproduce this archived workflow.
