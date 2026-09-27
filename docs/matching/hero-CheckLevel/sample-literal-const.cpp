// Reviewed exact-span source product, RVA0x620c8, src/SOURCE/HERO.cpp.
// Baseline:
//     samp = LoadPlaySample(const_cast<char*>("nwherolv.82m"));
// Retained:
//     samp = LoadPlaySample("nwherolv.82m");
// Both clean-state arms compile:1699 bytes,67 blocks,56 relocs,1.81s total.
// Manifest: build/c27-manifest.json; complete results: build/c27-batch/results.json.
// Both runner exact flags reject private relocation identity; candidate reports
// four sibling private-string counter changes. Independent C27 native census
// resolves all44 functions/allocated sections; complete retail checker passes
// every byte,site,target for CheckLevel. See docs/reconstruction/C27.md.
