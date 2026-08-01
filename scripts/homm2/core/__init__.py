"""homm2.core - the shared engine library every role package imports.

Nothing here is a command. These are the primitives that must have exactly one
definition in the tree, because a second copy is a silent divergence rather than
a build error:

  paths     the repository-root resolver (see its docstring for why)
  od_slots  the MSVC 4.2 /Od local stack-slot model
"""
