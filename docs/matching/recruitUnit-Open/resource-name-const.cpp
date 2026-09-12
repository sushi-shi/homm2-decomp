// C14: complete 2 x 2 structural/type-boundary matrix, 4/4 in 2.83s.
// build/c14-{axes,manifest}.json; build/c14-batch/results.json.
// Each site independently used const_cast<char*>(condition ? a : b) or
// condition ? a : b. All arms: Open 619 bytes, 19 blocks, 99.954025% fuzzy.
// Runner baseline has the same compiler-EH relocation limitations. Independent
// native before/after proof: all 11 functions and all allocated sections exact,
// including EH metadata. Retain both direct expressions. See C14.md for the
// explicit distinction between native equivalence and partial retail proof.

// recruitUnit::Open, existing heroWindow(H2_CONST char*) interface:
m_window = new heroWindow(
    WINDOW_X,
    WINDOW_Y,
    m_resourceType == RECRUIT_NO_RESOURCE ? "recruit0.bin" : "recruit1.bin"
);

// QuickViewRecruit:
recruitWindow = new heroWindow(
    QUICK_WINDOW_X,
    QUICK_WINDOW_Y,
    resourceType == RECRUIT_NO_RESOURCE ? "recruiq0.bin" : "recruiq1.bin"
);
