/*
Functions:
  DDSetFullScreenStatus  PoL 0x000366b0 / Buka 0x000b0e4d
  game::SetupHotSeatGame PoL 0x00011200 / Buka 0x00092817

Retained mixed source
---------------------
DDSetFullScreenStatus:
  Port windowHeight0 -> windowHeight.
  Keep PoL result0 instead of Buka hres.

SetupHotSeatGame:
  Port Buka declaration/type order and two names:
    i32 i;
    char name[DEFAULT_PLAYER_NAME_CAPACITY];
  Keep PoL window instead of Buka dialogWindow.

DD complete identifier product
------------------------------
Manifest: build/dd-set-full-screen-buka-names-manifest.json
Results: build/source-variant-batch/dd-set-full-screen-buka-names/results.csv
Coverage: 16/16 clean combinations of windowHeight0/result0 with
windowHeight/hres and conservative controls.
  PoL names:                    100.000000%
  windowHeight + result0:       100.000000%
  windowHeight + hres:           99.911330%
All are 773 bytes, 25/25 blocks, and 67/67 relocs.

Setup complete structural/name products
---------------------------------------
Structure manifest: build/setup-hot-seat-declaration-order-manifest.json
Current-order names: build/setup-hot-seat-current-order-buka-names-manifest.json
Buka-order names: build/setup-hot-seat-buka-order-names-manifest.json
Results live in like-named build/source-variant-batch directories.
Coverage: 2/2 structural arms and 125/125 names in each arm (250 name
states total); no TU-state probes.

Clean permuter orbit:
  current order + PoL names:       99.893990%
  Buka order + PoL names:          99.855125%
  Buka order + Buka i/name,
    but PoL window:                99.893990%
  Buka order + all Buka names:     99.844520%

The retained mixed normal build is exact at 568 bytes, 30/30 blocks, and
ordered 39/39 relocations. The permuter's shared sub-100 baseline is a
legitimate SETUP TU-state orbit; paired results identify the coupled
declaration/name change and the rejected dialogWindow name.

Disposition: matching-decomp ports for windowHeight and the coupled i/name
declaration; compiler/version-specific hres/dialogWindow names. No gameplay
or shipped-byte change.
*/
