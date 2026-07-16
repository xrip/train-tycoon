# Release checklist (RELEASE-01)

The plan's final proof is a mix of automated gates and one human step. Everything
that can be automated is scripted and currently green; only the human visual
proof remains.

## Automated — all green (2026-07-16)

| Check | How | Status |
|---|---|---|
| Debug build + full tests | `cmake --build --preset build-debug` then `ctest --preset test-debug-full` | pass |
| Release build + full tests | `cmake --build --preset build-release` then `ctest --preset test-release-full` | pass |
| AddressSanitizer full tests | build with `/fsanitize=address /EHsc`, run full ctest | pass, 0 errors |
| All scenarios | `full: all scenario maps have fixed digests` covers all 13 scenarios | pass |
| Long simulation | `generate_scenario` drives up to 200,000 `game_update` steps per scenario | pass |
| Rendered frames | `full: scenario zero input and frames have fixed digests` (pixel digests) | pass |
| Save round trip | `fast: save buffer bytes and round trip` | pass |
| MIDI device (headless) | `TRAIN_TYCOON_MIDI_TEST_MS=2000 ./build/Release/train_tycoon.exe` -> exit 0 | pass |

## Human step — required, not automatable

Launch the game and play a full session, confirming it looks and behaves like the
original and that MIDI music is audible:

```
./build/Release/train_tycoon.exe
```

Confirm: menus/dialogs render, a scenario plays start to finish, building track
and buying trains works, the economy/report graphs update, saving and loading a
game works, and music plays through your speakers. This visual/audible proof is
the only remaining sub-check; the plan designates it a manual step
("UI and final exits: manual play check"; "Real MIDI device play is a separate
device check").
