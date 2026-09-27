// VC6 SP5 source product c79-events-receive-text; RVA 0x49582.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 4 arms, 4.914000s; no compile failure or timeout; source restored.
// build/source-variant-batch/c79-events-receive-text/results.json and build/c79-events-receive-text-axes.json
// Disposition: apply trial3. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'0': 'keep', '1': 'keep', 'candidate': 'baseline'} / 990 / 44bed41da6c7feb6 / 5fdc30127eca6140 / 33:33 / 33,0,0,0,0 / True
// 1 / {'0': 'keep', '1': 'recover', 'candidate': 'baseline'} / 990 / 44bed41da6c7feb6 / 5fdc30127eca6140 / 33:33 / 33,0,0,0,0 / True
// 2 / {'0': 'recover', '1': 'keep', 'candidate': 'baseline'} / 990 / 44bed41da6c7feb6 / 5fdc30127eca6140 / 33:33 / 33,0,0,0,0 / True
// 3 / {'0': 'recover', '1': 'recover', 'candidate': 'baseline'} / 990 / 44bed41da6c7feb6 / 5fdc30127eca6140 / 33:33 / 33,0,0,0,0 / True

// AXIS 0
// BEFORE (complete exact span):
// | const_cast<char*>("\xce\xf8\xe8\xe1\xea\xe0 \xef\xee\xeb\xf3\xf7\xe5\xed\xe8\xff \xe8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xe8. \xcf\xf0\xee\xe4\xee\xeb\xe6\xe0\xf2\xfc?")
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +1 @@
// | -const_cast<char*>("\xce\xf8\xe8\xe1\xea\xe0 \xef\xee\xeb\xf3\xf7\xe5\xed\xe8\xff \xe8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xe8. \xcf\xf0\xee\xe4\xee\xeb\xe6\xe0\xf2\xfc?")
// | +"\xce\xf8\xe8\xe1\xea\xe0 \xef\xee\xeb\xf3\xf7\xe5\xed\xe8\xff \xe8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xe8. \xcf\xf0\xee\xe4\xee\xeb\xe6\xe0\xf2\xfc?"

// AXIS 1
// BEFORE (complete exact span):
// | const_cast<char*>("\xc8\xe3\xf0\xe0 \xef\xf0\xe5\xea\xf0\xe0\xf9\xe5\xed\xe0.")
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +1 @@
// | -const_cast<char*>("\xc8\xe3\xf0\xe0 \xef\xf0\xe5\xea\xf0\xe0\xf9\xe5\xed\xe0.")
// | +"\xc8\xe3\xf0\xe0 \xef\xf0\xe5\xea\xf0\xe0\xf9\xe5\xed\xe0."
