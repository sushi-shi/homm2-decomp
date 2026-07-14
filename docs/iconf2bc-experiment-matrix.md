# BASE/iconf2bc experiment matrix

Current integrated state: checkpoint `000768f`, source SHA-256
`bcf4f468adf3b75d8226120b3eb00a72ba98833d8aea434625f13dba3e9b5cad`.
Retail RVA/size `0xd9790` / `0x54d`; canonical candidate 85.166245%, function end `0x540`,
frame `sub esp,8`, 84 candidate vs 83 retail relocations. Decoder entry is at `+0xe9` versus
retail `+0xec`. Relocation occurrence counts agree except for one excess setup `gFCY` load;
the previous clipped-fill and clipped-dim excess loads are gone. No base-only relocation target.

The new-experiment table began at checkpoint `838105c` / source checkpoint `7386907`, SHA-256
`648ecb4b963c5b97aea5908738d26509ad680853a3041817321b61aa955070f9`, score 83.4333%,
function end `0x550`. Every row records whether that state was retained, reverted, or
byte-identical.

## Reconstructed prior pass matrix

These were all based on the complete reconstructed decoder and were reverted unless marked retained.
No permutation tool was used.

| Shape / axis | Match | Size | Relocs | Result |
|---|---:|---:|---:|---|
| older incomplete body / invalid wall | 71.103% | not retained | 83 | superseded |
| corrected Iconf2b body before int color indices | 77.60453% | 0x518-ish | 83 | reverted |
| hybrid direct-src/entry setup before int indices | 77.9597% | 0x520 | 83 | superseded |
| clipped tail local cursor, byte index | 78.04534% | 0x526 | 82 | superseded |
| direct clipped lookup without index local | 77.166245% | 0x522 | 83 | reverted |
| signed/int color indices in both literal loops | 82.94206% | 0x548 | 85 | retained basis |
| assignment-expression currentY predicates | 82.95466% | 0x549 | 85 | retained basis |
| explicit setup entryX local | 82.36776% | not retained | 85 | reverted |
| short width local | 82.40554% | not retained | 85 | reverted |
| sibling data/entries/entry/src/entryY setup | 82.01008% | 0x544 | 85 | reverted |
| split `int w; w = entry->w` | 82.36776% | not retained | 85 | reverted |
| color-table register/alias locals | 77.78% range | not retained | table unpinned | reverted |
| mutate parameter x as setup cursor | 77.39799% | not retained | not retained | reverted |
| explicit frame byte-offset local | 77.78338% | not retained | not retained | reverted |
| pointer-form table lookup | byte-identical to then-best | same | same | reverted |
| direct global dim palette instead of local alias | 82.94206% | 0x548 | 85 | reverted |
| dim count/store/declaration statement orders | byte-identical | same | same | reverted |
| width identifier and X commutative-order variants | byte-identical | same | same | reverted |
| reverse vertical-clause order in setup/fill/dim | 82.34% | not retained | 85 | reverted |
| sibling direct-gFCY predicates | 82.94% | not retained | 85 | reverted |
| nested vertical tests / initialized locals | 82.37% | not retained | 85 | reverted |
| `register` vertical locals | ignored by VC4.2 | same | 85 | reverted |
| reuse function-wide Y for vertical snapshots | 82.54% | not retained | 86 after clipR fix | reverted |
| direct clip-right snapshot reused as count | 82.95% basis | 0x549 | 85 | superseded |
| distinct right-edge cursor plus global count expression | 83.07% | 0x550 | 86 | retained |
| clipped mapped-byte/cursor lifetime reorder | 83.4333% | 0x550 | 86 | retained canonical |
| split dim pixel postincrement / dim statement order | byte-identical | 0x550 | 86 | retained readable form |

Historical pre-reconstruction searches listed in the repository ledger (local/global/mutable setup,
count signedness, memset forms, pointer-update orders, palette/destination global loads, and 136 AST
variants) are excluded from all future work at this compiler/header state.

## New experiments after checkpoint 838105c

Append every source-hash-distinct build here with SHA-256, score, end/frame, relocation count, and
retained/reverted/byte-identical disposition.

| Source SHA-256 | Shape | Match | End/frame | Relocs | Result |
|---|---|---:|---|---:|---|
| `d0344e9f2f59b64a3b19f4282ded7e5b801d3cb1de7f1e52a8d2d6917f7ea2d9` | `const int w` | 83.4333% | `0x550`, `sub esp,8` | 86/83 | byte-identical; reverted |
| `d92b9fb6e5ef9cc6ba11c189ffddbb80f311af3e113dcb9a74b058684c962ffc` | setup `const int&` Y alias | 83.4333% | `0x550`, `sub esp,8` | 86/83 | byte-identical; reverted |
| `94b427a21f2936946ee27d3d385f3dff47ae2784803cfeba22288155eafa90a1` | explicit setup failure gotos | 81.91% | `0x554`, `sub esp,8`; decoder `+0xee` | 86/83 | structural regression; reverted |
| `9eb9fc3ff5b6d9c19606543f82a9d1b79ee110f7892d99d2a5a3f87d3b96475d` | rename invented Y scratch to `gFCYPos` | 83.4333% | `0x550`, `sub esp,8` | 86/83 | byte-identical; reverted |
| `9d338b3d48d074e48110e381f8177374a9bfaf648389a4eed105047d632e6ba1` | affected local identifiers `currentY` -> `rowY` | 83.4333% | `0x550`, `sub esp,8` | 86/83 | byte-identical; reverted |
| `56aecffb0815fa8496e5f15b8a5d1f73116ead23a15557c5eafa67da23b91681` | fill/dim De Morgan reject predicates | 83.4333% | `0x550`, `sub esp,8` | 86/83 | byte-identical; reverted |
| `5df8107203e6397a511030050b5783772f0aec94d5fff833754390ba6586e57f` | affected Y snapshots as signed `long` | 83.4333% | `0x550`, `sub esp,8` | 86/83 | byte-identical; reverted |
| `db0a58280680ec9a9d007152d6f16a347bbf82f2b3920022304a3a7da2529843` | reuse dead parameter `y` for three snapshots | 82.29% | `0x54b`, `sub esp,8`; decoder `+0xe8` | 86/83 | register/offset regression; reverted |
| `dfe8868c81840daddc3fef7a44c82e82d6f39968b8ebb5178322738fe154ac32` | reuse decoded `flags` for fill/dim Y snapshots | 82.82% | `0x54f`, `sub esp,8`; decoder `+0xec` | 86/83 | allocation regression; reverted |
| `e670fa455e121a198121eb2507e4ab83ec0b2eedf120b533a20d2adaa164928c` | dim reuses `left` for Y then horizontal bound | 82.72% | `0x54f`, `sub esp,8`; decoder `+0xec` | 86/83 | allocation regression; reverted |
| `84483e0315f710a9a6e11c8d2f175e2a4cd3e6e1c58270402f33d952b70149cb` | grouped leading-comma Y snapshots in all three predicates | 75.75% | `0x570`, `sub esp,12`; decoder `+0x101` | 86/83 | introduced spill/frame regression; reverted |
| `26d3c4ecb467df6a98759c03901bcadfc4a38bfaa49ef8588675ac84546dd937` | leading-comma Y snapshot in fill only | 80.69% | `0x551`, `sub esp,12`; decoder `+0xec` | 86/83 | introduced spill/frame regression; reverted |
| `7a0b7cc84ba1bffef9c338dcc70e3ea50ab1a64e6e0dce80da5d79cf9daa164a` | `gFCY | 0` identity at three snapshots | 83.4333% | `0x550`, `sub esp,8` | 86/83 | byte-identical; reverted |
| `4742dd3baab86d7c97e31ebb792892508f98c2702e75dc5024eb99326fc7d478` | unsigned dim-loop snapshot with signed test | 83.35% | `0x550`, `sub esp,8` | 86/83 | slight instruction regression; reverted |
| `ba516013e108e8a5af6d8fa57e90940cd310dda05437b23f695761edb93a75f5` | local include order `Misc` before `X_GLOBAL` | 83.4333% | `0x550`, `sub esp,8` | 86/83 | byte-identical; reverted |
| `60b4bbc58ee358d7fa04e3cdaf3398b66d550841d4a1073fc539e3d98d9b231e` | local include order `Misc` before class definitions | 83.4333% | `0x550`, `sub esp,8` | 86/83 | byte-identical; reverted |
| `a093c5755eab0c4caa5ead7c3542479b5d20dd9cf9fbfc298a0e3dfa281da2ee` | fill Y snapshot redundantly republished after predicate | 81.68% | `0x562`, `sub esp,12`; decoder `+0xec` | 87/83 | store retained and spilled; reverted |
| `b2b740205d2809a54d39a1816eb5d91a00076c54479bb2240b29efaae62e2a97` | Y scratch declared signed `long` | 83.4333% | `0x550`, `sub esp,8` | 86/83 | byte-identical; reverted |
| source canonical / header `eb8d49b78427e18e782905255ba9bde88d8e5a8091f2e34b9d63db0eb9a4c335` | serialized `srcOffset` as unsigned int | 83.4333% | `0x550`, `sub esp,8` | 86/83 | iconf2bc byte-identical; shared edit reverted, not retained |
| `7e08006a7d449cb9df11733d4b3be5fde3d351f97df61e43fa97ffa154f73404` | early direct `IconEntry.h` declaration | 77.19% | `0x550`, `sub esp,8`; decoder `+0xec` | 86/83 | broad allocation regression; reverted |
| `5248f59303e9aacd46a368b1bb66f6909a7c1ab940ba65c0fa3782091244ff89` | staged right-clipped skip arithmetic | 83.4333% | `0x550`, `sub esp,8` | 86/83 | byte-identical; reverted |
| `aaa329a462e50e0dc9a3ad9cee544f86e305bce46984252ee7ec4b95109e76e6` | block-local skip plus common goto publication | 83.4333% | `0x550`, `sub esp,8` | 86/83 | byte-identical; reverted |
| `2912fb38754471dc9505b68d15a350b3efb789de5672291b043bc5acea39680e` | function-wide skip plus common goto publication | 83.4333% | `0x550`, `sub esp,8` | 86/83 | byte-identical; reverted |
| source `0bc854c03b163010d0a5d0e85aee312df44060c4aac98438dd393ee4ceb62638`, icon header `ce5e720af2f2113c1f629127109b8eeee3d8045aaadb3cd837c35bd264385041` | `icon::m_data` as unsigned byte pointer, direct use | 83.4333% | `0x550`, `sub esp,8` | 86/83 | target byte-identical; shared type also broke four char-pointer users, reverted |
| `f06a78f1eb44777e6537283c2f61a1c29dae94c471a6353e875efea9a1ac70af` | const local/static entry pointers | 83.4333% | `0x550`, `sub esp,8` | 86/83 | byte-identical; reverted |
| `328e2202cc4209c77e75f6f973d5ae489c13b95fe91e23bea85f2aebe1642afa` | skip declared before count in common publication shape | 83.4333% | `0x550`, `sub esp,8` | 86/83 | byte-identical; superseded |
| `57c9e8b95a525fba956867309b394c94852ee88e57da27e1064496e5b9d227ac` | branch-selected destination cursor + common skip publication | 83.46% | `0x54f`, `sub esp,8`; decoder `+0xec` | 86/83 | improved and retained |
| `f16b1c581880ee9db6de8eca4f018af5110f7adf3fd753c4ef95927ad1c3b040` | commute right-edge destination addition | 83.46% | `0x54f`, `sub esp,8` | 86/83 | byte-identical to retained; reverted |
| `91d9f514c38421eee55f92fc70f8a722f60759b73232f3b9a5d69f152c803418` | chained selected/global destination assignment | 83.46% | `0x54f`, `sub esp,8` | 86/83 | byte-identical to retained; reverted |
| `4ca5cf10976a8425500f5022ca7e7d127e0745c298d2db8f9dc69d5373383e9f` | destination locals renamed `dst`/`copyDst` | 83.46% | `0x54f`, `sub esp,8` | 86/83 | byte-identical to retained; reverted |
| `53263f95b8a0d3e851d9955129384273e6882f9662b0ab362b9c3d957c56b3d7` | retained destination plus literal count snapshot | 83.458435% | `0x54f`, `sub esp,8` | 86/83 | count copy optimized away; retained source evidence |
| `98ba8916cb186d89ddcded1d270e23cdf72430d2f063a25670a9b8b66021084d` | initialized Y snapshots on improved state | 83.458435% | `0x54f`, `sub esp,8` | 86/83 | byte-identical to improved state; retained cleaner form |
| `b502d033aef2e99e590c272a5cb46602ace6c7c582b7464df01b4f13a4171d4c` | staged right-skip arithmetic plus common publication | 83.458435% | `0x54f`, `sub esp,8` | 86/83 | byte-identical; reverted |
| `a125d93a04d126d03a3634cc2672e10287ce933cdb2bb4af38bad52b6534f4a4` | split clipped source fetch after destination snapshot | 83.458435% | `0x54f`, `sub esp,8` | 86/83 | byte-identical; reverted |
| `bcf4f468adf3b75d8226120b3eb00a72ba98833d8aea434625f13dba3e9b5cad` | narrow `IconEntry.h` plus remove redundant owner header | 85.166245% | `0x540`, `sub esp,8`; decoder `+0xe9` | 84/83 | removes fill/dim excess `gFCY` loads; retained active checkpoint, not a wall |
| `697ed6e1b172cdf99421c052ca2d90172e67bbafcbe6e988e75259fea8bb3ffe` | narrow `IconEntry.h` while retaining owner header | 77.77078% | `0x545`, `sub esp,8`; decoder `+0xea` | 84/83; 9 `gFCY` | broad allocation regression; reverted |
| `35e4408a6b2d1a69459451e7cdc9096460c4d3b89233c1f0fdc816da6da48c58` | remove redundant owner header while retaining broad `Misc.h` | 78.22418% | `0x54a`, `sub esp,8`; decoder `+0xea` | 84/83; 9 `gFCY` | broad allocation regression; reverted |
| `5c2ba8f663e24756bfdff86b78a483288050594204c637595fff2bade54eccf4` | setup Y snapshot assigned in the vertical predicate on narrow-header state | 85.166245% | `0x540`, `sub esp,8`; decoder `+0xe9` | 84/83; 9 `gFCY` | byte-identical to narrow-header state; reverted |
| `59e4f5ad980e774cd2cb807314b0ceb876a4d6b654bd7e427cfb5d3b5ce13dcc` | commute first setup vertical failure comparison | 85.166245% | `0x540`, `sub esp,8`; decoder `+0xe9` | 84/83; 9 `gFCY` | byte-identical to narrow-header state; reverted |
| `79363de918b72c755599d6d6243e68490ab582b90aa00ec1f2bebf2c10376b2c` | suppress direct `IconEntry` `SIZE` assertion | 80.13% | `0x54a`, `sub esp,8`; decoder `+0xea` | 84/83; 9 `gFCY` | cumulative allocation regression; reverted |
| `8e19b127a25bb474d8d83700801adabc6055e75b4e59f237a3c28276600b2f50` | setup width local renamed semantically | 85.166245% | `0x540`, `sub esp,8`; decoder `+0xe9` | 84/83; 9 `gFCY` | byte-identical to narrow-header state; reverted |
| `d18bb112c68e68d111024756da512d516fdd1d5139f9ddef36e05847c639914d` | spell frame-source advance as indexed address | 85.166245% | `0x540`, `sub esp,8`; decoder `+0xe9` | 84/83; 9 `gFCY` | byte-identical to narrow-header state; reverted |
| `60f84d3ce650a829ac27589809c565bdd6b41f78843ce5a19fadda070ef85ed6` | volatile setup width to force retail-like spill | 84.74307% | `0x553`, `sub esp,12`; decoder `+0xfe` | 84/83; 9 `gFCY` | extra lifetime/frame and broad allocation regression; reverted |
| `0074b3450ba5602ea6871aaeb2913847843fa49b369ae1cb4902cf2e4a99fb6e` | bound setup width/Y snapshots to a lexical block | 85.166245% | `0x540`, `sub esp,8`; decoder `+0xe9` | 84/83; 9 `gFCY` | byte-identical to narrow-header state; reverted |
