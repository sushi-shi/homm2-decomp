"""Explicit Clang language modes used by reconstruction tooling."""

from enum import Enum


class ClangMode(Enum):
    # Closest Clang language mode to the pre-standard retail MSVC compiler.
    RETAIL_ANALYSIS = "c++98"

    # Exercise the modern strongly typed enum model.
    STRICT = "c++20"

    @property
    def driver_flag(self) -> str:
        return f"-std={self.value}"

    @property
    def clang_cl_flag(self) -> str:
        return f"/std:{self.value}"
