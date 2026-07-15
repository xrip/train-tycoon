# Refactor decisions

## D-001: Language and style

- End state: C++20.
- Style: C with Classes.
- Classes are for ownership, state, and simple rules.
- Plain data uses structs; simple algorithms use free functions.
- Standard-library templates are welcome when they make code clearer.
- Clear namespace aliases may be used in `.cpp` files. Public headers keep
  full `std::` names, and `using namespace std` is not allowed.
- One small `Grid<T>` is allowed.
- No template design systems, CRTP, concepts, policy classes, deep class trees,
  event systems, or generic frameworks.

## D-002: Compatibility

Keep game behavior and the current `.rms` byte form. The model memory layout
may change, but `SaveCodec` must write the old field and array order.

## D-003: Scope

Refactor all first-party code under `win32/`. Keep MiniFB and `stb_image` as
vendor code. New sound effects, networking, new rules, and portability are out
of scope.

## D-004: Tests

Use vendored doctest 2.5.3, clear rule checks, and fixed output digests. The
build must not fetch test code from the network.

## D-005: Names

Do not keep a recoverable obfuscated name in changed native code. Read native
behavior and call sites first. Use the decompile only when native evidence is
not enough. Record uncertain evidence here before a rename.

### Open name set

- Methods: `gc_b`, `gc_k`, `gc_m`, `gc_u`.
- Weak method groups: `dayResetP`, `dayResetAg`, `dayResetAd`,
  `everyTwoTickZ`, and all `*Inlined` names.
- Short state fields: `aJ`, `ar`, `at`, `bT`, `bx`, `ca`, `cB`, `ce`, `cH`,
  `D`, `dc`, `dH`, `dI`, `dk`, `dp`, `eD`, `ee`, `fR`, `gw`, `hx`, and `j`.

### Recovered in RT-01

- `gc_a` -> `gc_appendTileDrawOffset`: its only caller adds one x/y pair to
  the isometric tile draw-order buffers.
- `gc_writeRlePairInlined` -> `gc_writeRleRun`: it writes one literal or
  repeated run to the save RLE output.

### Recovered in RT-02

- `gc_b` -> `gc_loadPackedSpriteData`: it reads the packed sprite offset and
  byte tables.
- `gc_loadSpriteStream` -> `gc_openSpriteStream`: it opens the stream used to
  rebuild sprite PNG data.
- `spriteStream` -> `sprite_stream`: it is the owned sprite input stream.

## D-006: Plain native resources

The native project keeps plain resource data. Old files `1` to `5` were changed
once with their 1-to-127 XOR key and given names based on their use. Run-time
XOR code and numeric resource paths are not part of the native design.
