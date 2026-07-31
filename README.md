# Heroes of Might and Magic II: The Price of Loyalty

This project aims to completely recover and modernize the source code for Heroes of Might and Magic II PoL.

This project does **not** contain resources from the original game.
You would have to get them separately to properly play the game.


## DISCLAIMER

The decompilation is mainly done by GPT-5.6 Sol and Claude Fable 5.0.
Using models for decompilation is actually the best use case for LLMs.
That is because when you get 100% matching function from a model, you don't need to trust its output and ASM speaks for itself.

Still, porting was done with GPT-5.6 Sol, and that code I would trust way less and it does require a proper review + cleaning.
Once Windows/Linux/WASM ports are verified/reimplemented the sentence above will be removed.


## Repository branch structure

```
decomp-pol-2.0
    |
    +------------------+
    |                  |
source-pol-2.0     classic-pol-2.0
    |
master-pol-2.0 (you are here)
    |
ironfist-pol-2.0

decomp-gold-2.1-buka
```

1. decomp-pol-2.0       - main decompilation effort is done here. Changes there trickle down into other branches.
2. source-pol-2.0       - automatically generated branch with all decompilation logic removed.
3. classic-pol-2.0      - --//--. This branch also preserves the original game mangling.
4. master-pol-2.0       - cross-platform version of the engine. It has support for WASM, Linux and Windows targets.
5. ironfist-pol-2.0     - Project Ironfist shellcode code applied directly onto the original source code.
6. decomp-gold-2.1-buka - secondary decompilation effort, it can be used for cross-references between two versions.


## Matching

The current matching % can be read in README.md in `decomp-pol-2.0` branch. It should be somewhere at >99%.


### On how this percentage is calculated

MSVC compiler, the game was compiled with, is extremely unstable: Even a single `typedef` above the matching
function can change register allocation in a function that follows it. Essentially anything that is not a C-preprocessor
definition will cause churn in the whole TU. This makes total matching unfeasible, since changes in one function will break
following functions. To combat that we calculate a maximum matching percentage the function has ever reached with that definition
(this is tracked by hashing the function body). Whenever function seems to hit a wall in terms of getting higher matching percentage
the script is used to generate a lot of noise above the function, if the function ever hits 100%, we record that and remove the noise,
after that the function is treated as done.

For example:

| Module   | Units |    Functions exact | Functions exact-max | Fuzzy | Fuzzy-max |
| :------- | ----: | -----------------: | ------------------: | ----: | --------: |
| `EDITOR` |     1 |     9 / 11 (81.8%) |    11 / 11 (100.0%) | 99.0% |    100.0% |

You can see that the `EDITOR` module has reached 100% at some point. However, changes in headers it imports (caused by matching other
modules) have regressed two functions to no longer match. This is not something that can be reliably controlled and it can flip again
at any moment the headers are touched, hence we declare `Functions exact-max` to be 100% and do not try to match this module further.


### Data and executable matching

Note that we only match functions by themselves, we do not match the finally linked executable. This is possible to do, but it requires
figuring out how linker allocates `.bss`, `.rdata` and `.data` segments, which will most likely also be unstable.

Also we do not match `.rsrc` at all, as it doesn't seem to be required for anything.


## Build

The main way to build the game is by using `nix`:
```sh
nix build .#homm2-linux
nix build .#homm2-windows
nix build .#homm2-web
```

### Building without Nix

Requirements:

1. CMake 3.20+, Ninja, pkg-config
2. A C++20 compiler with 32-bit support
3. 32-bit SDL3 and FFmpeg libraries (`libavcodec`, `libavformat`, `libavutil`, `libswresample`)

```sh
cmake -S . -B build -G Ninja
cmake --build build
HOMM2_DATA=/path/to/heroes2 ./build/homm2
```

## Installation

### Game data

To run the engine you need an installed copy of the game.
`HOMM2_DATA` should point to the game directory which holds `DATA/HEROES2.AGG` (case-insensitive).

```sh
export HOMM2_DATA=/path/to/heroes2
```

If `HOMM2_DATA` is not present, it will be looked up (in the order of priority):
* executable directory
* current directory
* `$XDG_DATA_HOME/homm2`
* `$XDG_DATA_HOME/homm2/data`
* `~/games/homm2`


On Linux and WASM `HOMM2_DATA` is supported to be read-only.
Game preferences, saves, high scores and network excahange files will be stored in the user data directory
For example: `~/.local/share/homm2/homm2`.

On Windows all user data is stored in its original location.


### Linux

Once `HOMM2_DATA` is set simply run `nix run`.


### Windows

Copy the executable to the writable game directory and run `HEROES2W.EXE`.

#### Wine

The first run will set up Wine prefix at `<game dir>/.wineprefix` to run the game.

```sh
nix build .#homm2-windows
cp result/bin/HEROES2W.EXE /path/to/heroes2/
cp result/run-game.sh /path/to/heroes2/
/path/to/heroes2/run-game.sh
```


### Web

```sh
HOMM2_DATA=/path/to/heroes2 nix run .#web
```

The launcher packages the installed data, serves the bundle on port 8080
(`HOMM2_WEB_PORT`) and caches it under `~/.cache/homm2-web` (`HOMM2_WEB_OUTPUT`).
This target cross-compiles SDL3 and a minimal FFmpeg, so the first build takes a while.
