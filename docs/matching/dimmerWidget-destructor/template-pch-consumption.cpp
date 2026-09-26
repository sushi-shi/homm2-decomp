// VC6 SP5, measured 2026-09-27 on decomp-gold-2.1-buka.
// Follow-up to template-pch-boundary.cpp, which tested /Yc creation only.
// Hypothesis: consuming a real PCH could preserve an earlier template emission
// state and recover the split-owner order in one linked compiler input.
// The semantic nested BaseWidget template and five method bodies are unchanged.
//
// Complete 2 x 2 product, four /Yc producers and four dependent /Yu consumers:
// - PCH boundary: named prefix.h / #pragma hdrstop;
// - whole-class instantiation request: before the PCH boundary / in consumer
//   after the real empty primary-template destructor definition.
// Producer.cpp ends immediately after the boundary and does not define the
// destructor. Consumer.cpp crosses the same boundary, includes SOURCE/KB.h,
// defines the destructor and, in the second family, requests class instantiation.
// Each consumer uses its producer's actual boundary.pch. All eight compile;
// no timeout, truncated product, or missing PCH. Baseline /Od /Ob1 /Gy flags.
//
// PCH sizes: header-prefix 838340; header-consumer 830220;
//            hdrstop-prefix 838348; hdrstop-consumer 830228 bytes.
// Prefix-request producers warn C4661 about the intentionally undefined
// destructor and emit the six prefix methods including the scalar wrapper.
// Consumer-request producers emit no functions. All consumers are warning-free.
//
// Every consumer emits all seven exact method bodies and complete ordered
// relocation sites/types/semantic targets/addends, plus exact header helpers.
// Every consumer has the same order:
//   ctor0,ctor6,Read,Main,Draw,dtor,scalar,$E19,$E18,ctype-id-cleanup.
// Each raw object carries one @comp.id = 0x000b2306. Consuming the PCH does
// not inherit the producer object's favorable scalar-before-dtor tail order;
// it emits the methods again with the same reversed final pair as the parent.
// Linking the producer too would add a compiler input, returning to the
// previously measured split-owner provenance problem rather than removing it.
//
// Disposition: reject these PCH-consumption families; no source/build change.
// No final link is needed for the single-input hypothesis because every complete
// consumer already contradicts retail contribution order. This does not claim
// exhaustive coverage of all possible PCH inputs or source ownership models.
// Artifacts/reproducer:
//   build/link/dimmer-template-pch-consume/{run.py,run.log,results.json}
//   <boundary>-<request>/Producer.cpp,Consumer.cpp,prefix.h,heroWindow.h,
//       resourceGlobals.h,boundary.pch,Producer.obj,Consumer.obj,*.log
// The script reuses the audited role/relocation inspector from
// build/link/dimmer-template-pch/run.py without executing its matrix.

// Producer.cpp:
// #include "prefix.h"
// #pragma hdrstop          // hdrstop family only
//
// Consumer.cpp:
// #include "prefix.h"
// #pragma hdrstop          // hdrstop family only
// #include <SOURCE/KB.h>
// template<class BaseWidget>
// heroWindow::DimmerWidget<BaseWidget>::~DimmerWidget() {}
// template class heroWindow::DimmerWidget<widget>; // consumer-request family only
