# Library members link in first-reference order

Measured with the pinned VC6 SP5 `LINK.EXE` under Wine, 2026-10-07, on
reduced objects and on the `EDT2PL.exe` link.

## Mechanism

LINK searches one library at a time. Within a library it walks the list of
undefined externals in the order they entered the symbol table (each object
in link order, each object's externals in symbol-table order, which for VC6
is the order of first use in the source), and pulls the member defining
each. A pulled member's own new externals are appended to the end of the
list. The order of members inside the archive does not matter: four
archives holding the same members in four orders all linked `a c b d` for an
object referencing `fa`, `fc` where `a` needs `b` and `c` needs `d`.

Consequences:

- A function defined out of retail order moves its first external
  references, and with them the members they pull. EDITMGR defined
  `DrawMap`/`DrawView` after `DrawCell`; restoring their place pulled bmap2
  ahead of the scale-downs, as in retail.
- An old name and its underscore spelling are different externals:
  `strnicmp` resolves through OLDNAMES (an alias member searched first, whose
  `__strnicmp` joins the list when OLDNAMES is scanned), `_strnicmp`
  directly. One direct spelling pulled `strnicmp.obj` ahead of the OLDNAMES
  group.
- A member referenced early but placed late in retail lives in a later
  library. In both images `MiscRuntime` follows `Misc` although `GetIconEntry`
  is referenced before any `Misc` symbol; it opens the next BASE archive.
- OLDNAMES members carry an empty `.text` (default 16-byte alignment): with
  OLDNAMES searched before the import libraries, that fill precedes the first
  import thunk, as retail's 0xcc padding shows.

## Use

Simulate the list (explicit objects' externals in symbol-table order, FIFO
over each library) before reordering archives; the member that lands early
names the reference that entered the list too soon.
