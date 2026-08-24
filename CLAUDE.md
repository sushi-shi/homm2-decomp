# HoMM2 Port Guide

Two environment variables exist for checking the game without a screen:

- `HOMM2_SCREENSHOT=<prefix>` writes presented frames as `<prefix>.<frame>.ppm`
  (`HOMM2_SCREENSHOT_EVERY=<n>` for every nth frame, otherwise just the first).
- `HOMM2_INPUT_REPLAY=<file>` replays input, one `<milliseconds> <action>` per line:
  `move`/`left-down`/`left-up`/`right-down`/`right-up` take `<x> <y>`,
  `key-down`/`key-up` take an SDL key name, and `text` takes committed UTF-8
  text (for example `1200 text Привет`).
