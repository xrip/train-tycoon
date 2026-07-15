# Known issues and fixed port defects

Open items need their own rule test and behavior decision.

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

## MAP-RNG-01: map generation depends on C argument order

`gc_loadState25Setup` has two calls with several `rint(gc)` expressions in one
C argument list. C does not set argument evaluation order, so MSVC Debug and
Release generate different maps from the same seed. The Java source at
`GameCanvas.java:1589` and `GameCanvas.java:1592` evaluates these arguments
left to right.

Fixed during `BASE-05`: the five raise values and four lower values are read
into clear local variables in Java order before each call. The fixed-seed
Debug and Release digests prove the same call order in both builds.

## PORT-READ-01: multi-byte reads depended on C expression order

Resource and save readers made several state-changing byte reads in one C
expression. Debug and Release could read the same bytes in a different order.
The Java source reads left to right.

Fixed during `BASE-05`: resource readers now read each byte in a separate
statement, and save readers build unsigned bit patterns one byte at a time.
Debug and Release now give the same scenario, save, and frame values.
