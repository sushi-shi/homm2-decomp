// VC6 SP5 source product c79-events-casefold; RVA 0x420e1.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 16 arms, 16.407114s; no compile failure or timeout; source restored.
// build/source-variant-batch/c79-events-casefold/results.json and build/c79-events-casefold-axes.json
// Disposition: apply trial5. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'conversions': '0000', 'candidate': 'baseline'} / 149 / 28d26328075f2b83 / 2222f94c6bf2cedc / 11:11 / 11,0,0,0,0 / True
// 1 / {'conversions': '0001', 'candidate': 'baseline'} / 149 / 28d26328075f2b83 / 2222f94c6bf2cedc / 11:11 / 11,0,0,0,0 / True
// 2 / {'conversions': '0010', 'candidate': 'baseline'} / 144 / 4f9504b43179c6d2 / a709edeff3857c80 / 11:11 / 10,1,0,0,0 / True
// 3 / {'conversions': '0011', 'candidate': 'baseline'} / 144 / 4f9504b43179c6d2 / a709edeff3857c80 / 11:11 / 10,1,0,0,0 / True
// 4 / {'conversions': '0100', 'candidate': 'baseline'} / 149 / 28d26328075f2b83 / 2222f94c6bf2cedc / 11:11 / 11,0,0,0,0 / True
// 5 / {'conversions': '0101', 'candidate': 'baseline'} / 149 / 28d26328075f2b83 / 2222f94c6bf2cedc / 11:11 / 11,0,0,0,0 / True
// 6 / {'conversions': '0110', 'candidate': 'baseline'} / 144 / 4f9504b43179c6d2 / a709edeff3857c80 / 11:11 / 10,1,0,0,0 / True
// 7 / {'conversions': '0111', 'candidate': 'baseline'} / 144 / 4f9504b43179c6d2 / a709edeff3857c80 / 11:11 / 10,1,0,0,0 / True
// 8 / {'conversions': '1000', 'candidate': 'baseline'} / 144 / 01ffcb1da9cbf159 / 6329af92e0c67263 / 11:11 / 10,1,0,0,0 / True
// 9 / {'conversions': '1001', 'candidate': 'baseline'} / 144 / 01ffcb1da9cbf159 / 6329af92e0c67263 / 11:11 / 10,1,0,0,0 / True
// 10 / {'conversions': '1010', 'candidate': 'baseline'} / 139 / 10a5478424c900b1 / 0d4473c1ae980f6c / 11:11 / 10,1,0,0,0 / True
// 11 / {'conversions': '1011', 'candidate': 'baseline'} / 139 / 10a5478424c900b1 / 0d4473c1ae980f6c / 11:11 / 10,1,0,0,0 / True
// 12 / {'conversions': '1100', 'candidate': 'baseline'} / 144 / 01ffcb1da9cbf159 / 6329af92e0c67263 / 11:11 / 10,1,0,0,0 / True
// 13 / {'conversions': '1101', 'candidate': 'baseline'} / 144 / 01ffcb1da9cbf159 / 6329af92e0c67263 / 11:11 / 10,1,0,0,0 / True
// 14 / {'conversions': '1110', 'candidate': 'baseline'} / 139 / 10a5478424c900b1 / 0d4473c1ae980f6c / 11:11 / 10,1,0,0,0 / True
// 15 / {'conversions': '1111', 'candidate': 'baseline'} / 139 / 10a5478424c900b1 / 0d4473c1ae980f6c / 11:11 / 10,1,0,0,0 / True

// AXIS conversions
// BEFORE (complete exact span):
// | VA(0x004420e1, 0x95)
// | i8 StrEqNoCase(H2_CONST char* firstString, H2_CONST char* sndString) {
// |     H2_CONST char* firstPtr = firstString;
// |     H2_CONST char* secondPtr = sndString;
// |     i32 chCount = 0;
// |     char fstUpper;
// |     char sndUpper;
// |
// |     while (1) {
// |         chCount++;
// |         if (chCount == SITE_STRING_LIMIT)
// |             return 1;
// |         fstUpper = static_cast<char>(toupper(static_cast<i32>(*firstPtr)));
// |         sndUpper = static_cast<char>(toupper(static_cast<i32>(*secondPtr)));
// |         if (fstUpper == sndUpper) {
// |             if (fstUpper == 0)
// |                 return 1;
// |             firstPtr++;
// |             secondPtr++;
// |         } else {
// |             return 0;
// |         }
// |     }
// | }
// |
// OPTION 0000
// OPTION 0001
// | --- before
// | +++ 0001
// | @@ -11,7 +11,7 @@
// |          if (chCount == SITE_STRING_LIMIT)
// |              return 1;
// |          fstUpper = static_cast<char>(toupper(static_cast<i32>(*firstPtr)));
// | -        sndUpper = static_cast<char>(toupper(static_cast<i32>(*secondPtr)));
// | +        sndUpper = toupper(static_cast<i32>(*secondPtr));
// |          if (fstUpper == sndUpper) {
// |              if (fstUpper == 0)
// |                  return 1;
// OPTION 0010
// | --- before
// | +++ 0010
// | @@ -11,7 +11,7 @@
// |          if (chCount == SITE_STRING_LIMIT)
// |              return 1;
// |          fstUpper = static_cast<char>(toupper(static_cast<i32>(*firstPtr)));
// | -        sndUpper = static_cast<char>(toupper(static_cast<i32>(*secondPtr)));
// | +        sndUpper = static_cast<char>(toupper(*secondPtr));
// |          if (fstUpper == sndUpper) {
// |              if (fstUpper == 0)
// |                  return 1;
// OPTION 0011
// | --- before
// | +++ 0011
// | @@ -11,7 +11,7 @@
// |          if (chCount == SITE_STRING_LIMIT)
// |              return 1;
// |          fstUpper = static_cast<char>(toupper(static_cast<i32>(*firstPtr)));
// | -        sndUpper = static_cast<char>(toupper(static_cast<i32>(*secondPtr)));
// | +        sndUpper = toupper(*secondPtr);
// |          if (fstUpper == sndUpper) {
// |              if (fstUpper == 0)
// |                  return 1;
// OPTION 0100
// | --- before
// | +++ 0100
// | @@ -10,7 +10,7 @@
// |          chCount++;
// |          if (chCount == SITE_STRING_LIMIT)
// |              return 1;
// | -        fstUpper = static_cast<char>(toupper(static_cast<i32>(*firstPtr)));
// | +        fstUpper = toupper(static_cast<i32>(*firstPtr));
// |          sndUpper = static_cast<char>(toupper(static_cast<i32>(*secondPtr)));
// |          if (fstUpper == sndUpper) {
// |              if (fstUpper == 0)
// OPTION 0101
// | --- before
// | +++ 0101
// | @@ -10,8 +10,8 @@
// |          chCount++;
// |          if (chCount == SITE_STRING_LIMIT)
// |              return 1;
// | -        fstUpper = static_cast<char>(toupper(static_cast<i32>(*firstPtr)));
// | -        sndUpper = static_cast<char>(toupper(static_cast<i32>(*secondPtr)));
// | +        fstUpper = toupper(static_cast<i32>(*firstPtr));
// | +        sndUpper = toupper(static_cast<i32>(*secondPtr));
// |          if (fstUpper == sndUpper) {
// |              if (fstUpper == 0)
// |                  return 1;
// OPTION 0110
// | --- before
// | +++ 0110
// | @@ -10,8 +10,8 @@
// |          chCount++;
// |          if (chCount == SITE_STRING_LIMIT)
// |              return 1;
// | -        fstUpper = static_cast<char>(toupper(static_cast<i32>(*firstPtr)));
// | -        sndUpper = static_cast<char>(toupper(static_cast<i32>(*secondPtr)));
// | +        fstUpper = toupper(static_cast<i32>(*firstPtr));
// | +        sndUpper = static_cast<char>(toupper(*secondPtr));
// |          if (fstUpper == sndUpper) {
// |              if (fstUpper == 0)
// |                  return 1;
// OPTION 0111
// | --- before
// | +++ 0111
// | @@ -10,8 +10,8 @@
// |          chCount++;
// |          if (chCount == SITE_STRING_LIMIT)
// |              return 1;
// | -        fstUpper = static_cast<char>(toupper(static_cast<i32>(*firstPtr)));
// | -        sndUpper = static_cast<char>(toupper(static_cast<i32>(*secondPtr)));
// | +        fstUpper = toupper(static_cast<i32>(*firstPtr));
// | +        sndUpper = toupper(*secondPtr);
// |          if (fstUpper == sndUpper) {
// |              if (fstUpper == 0)
// |                  return 1;
// OPTION 1000
// | --- before
// | +++ 1000
// | @@ -10,7 +10,7 @@
// |          chCount++;
// |          if (chCount == SITE_STRING_LIMIT)
// |              return 1;
// | -        fstUpper = static_cast<char>(toupper(static_cast<i32>(*firstPtr)));
// | +        fstUpper = static_cast<char>(toupper(*firstPtr));
// |          sndUpper = static_cast<char>(toupper(static_cast<i32>(*secondPtr)));
// |          if (fstUpper == sndUpper) {
// |              if (fstUpper == 0)
// OPTION 1001
// | --- before
// | +++ 1001
// | @@ -10,8 +10,8 @@
// |          chCount++;
// |          if (chCount == SITE_STRING_LIMIT)
// |              return 1;
// | -        fstUpper = static_cast<char>(toupper(static_cast<i32>(*firstPtr)));
// | -        sndUpper = static_cast<char>(toupper(static_cast<i32>(*secondPtr)));
// | +        fstUpper = static_cast<char>(toupper(*firstPtr));
// | +        sndUpper = toupper(static_cast<i32>(*secondPtr));
// |          if (fstUpper == sndUpper) {
// |              if (fstUpper == 0)
// |                  return 1;
// OPTION 1010
// | --- before
// | +++ 1010
// | @@ -10,8 +10,8 @@
// |          chCount++;
// |          if (chCount == SITE_STRING_LIMIT)
// |              return 1;
// | -        fstUpper = static_cast<char>(toupper(static_cast<i32>(*firstPtr)));
// | -        sndUpper = static_cast<char>(toupper(static_cast<i32>(*secondPtr)));
// | +        fstUpper = static_cast<char>(toupper(*firstPtr));
// | +        sndUpper = static_cast<char>(toupper(*secondPtr));
// |          if (fstUpper == sndUpper) {
// |              if (fstUpper == 0)
// |                  return 1;
// OPTION 1011
// | --- before
// | +++ 1011
// | @@ -10,8 +10,8 @@
// |          chCount++;
// |          if (chCount == SITE_STRING_LIMIT)
// |              return 1;
// | -        fstUpper = static_cast<char>(toupper(static_cast<i32>(*firstPtr)));
// | -        sndUpper = static_cast<char>(toupper(static_cast<i32>(*secondPtr)));
// | +        fstUpper = static_cast<char>(toupper(*firstPtr));
// | +        sndUpper = toupper(*secondPtr);
// |          if (fstUpper == sndUpper) {
// |              if (fstUpper == 0)
// |                  return 1;
// OPTION 1100
// | --- before
// | +++ 1100
// | @@ -10,7 +10,7 @@
// |          chCount++;
// |          if (chCount == SITE_STRING_LIMIT)
// |              return 1;
// | -        fstUpper = static_cast<char>(toupper(static_cast<i32>(*firstPtr)));
// | +        fstUpper = toupper(*firstPtr);
// |          sndUpper = static_cast<char>(toupper(static_cast<i32>(*secondPtr)));
// |          if (fstUpper == sndUpper) {
// |              if (fstUpper == 0)
// OPTION 1101
// | --- before
// | +++ 1101
// | @@ -10,8 +10,8 @@
// |          chCount++;
// |          if (chCount == SITE_STRING_LIMIT)
// |              return 1;
// | -        fstUpper = static_cast<char>(toupper(static_cast<i32>(*firstPtr)));
// | -        sndUpper = static_cast<char>(toupper(static_cast<i32>(*secondPtr)));
// | +        fstUpper = toupper(*firstPtr);
// | +        sndUpper = toupper(static_cast<i32>(*secondPtr));
// |          if (fstUpper == sndUpper) {
// |              if (fstUpper == 0)
// |                  return 1;
// OPTION 1110
// | --- before
// | +++ 1110
// | @@ -10,8 +10,8 @@
// |          chCount++;
// |          if (chCount == SITE_STRING_LIMIT)
// |              return 1;
// | -        fstUpper = static_cast<char>(toupper(static_cast<i32>(*firstPtr)));
// | -        sndUpper = static_cast<char>(toupper(static_cast<i32>(*secondPtr)));
// | +        fstUpper = toupper(*firstPtr);
// | +        sndUpper = static_cast<char>(toupper(*secondPtr));
// |          if (fstUpper == sndUpper) {
// |              if (fstUpper == 0)
// |                  return 1;
// OPTION 1111
// | --- before
// | +++ 1111
// | @@ -10,8 +10,8 @@
// |          chCount++;
// |          if (chCount == SITE_STRING_LIMIT)
// |              return 1;
// | -        fstUpper = static_cast<char>(toupper(static_cast<i32>(*firstPtr)));
// | -        sndUpper = static_cast<char>(toupper(static_cast<i32>(*secondPtr)));
// | +        fstUpper = toupper(*firstPtr);
// | +        sndUpper = toupper(*secondPtr);
// |          if (fstUpper == sndUpper) {
// |              if (fstUpper == 0)
// |                  return 1;
