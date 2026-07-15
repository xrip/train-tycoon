# Known issues outside refactor behavior

Do not fix these in a refactor commit.

## K-001: Game lifetime does not release owned state

Tests and `main` free only the outer `GameCanvas`. More than 100 pointer fields
own data. The C++ ownership stage will give them normal lifetime without a game
rule change.

## K-002: Possible zero division

`gc_computeMenuRowDisplayValue` has a branch that can divide by `money` when
`money` is zero. It needs a separate rule test and behavior decision.

## K-003: Unused or unclear fields

Several short fields are only set during first-value work and have no native
read. Do not remove them before the naming and dead-code proof item.

## K-004: High-score network is not present

High-score sync is a known no-op. Networking is not part of this refactor.

## K-005: No original sound effects

The old game has MIDI music but no separate sound-effect files or calls. Sound
effects are not part of this refactor.
