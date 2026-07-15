# Native C++ style

## Design limit

This is C++20 in a C-with-Classes style.

- Use a class when it owns a resource, keeps state valid, or gives one clear
  service.
- Use a struct for plain data.
- Use a free function for a small operation that has no owned state.
- Prefer direct code over a framework or a general design for one use.
- Do not add template metaprogramming, CRTP, policy systems, deep class trees,
  event systems, or generic frameworks.

Standard-library types are normal code. Use `std::array` for fixed storage,
`std::vector` for dynamic owned storage, `std::span` for a view, `std::string`
for text, `std::filesystem::path` for paths, and `std::unique_ptr` for one
heap owner. Use `std::shared_ptr` only when ownership is truly shared.

## Names

- Types use `PascalCase`.
- Functions and variables use `snake_case` in new parts.
- Constants use `kPascalCase`.
- A name says its game or system purpose, not its old Java shape.
- When touched code has a recoverable obfuscated name, recover the name as
  part of that work. Ask and record the evidence when meaning is uncertain.
- Do not make a broad name-only pass over code outside the current item.

## Namespaces and headers

- Do not use `using namespace std`.
- A `.cpp` file may use a clear namespace alias such as
  `namespace filesystem = std::filesystem`.
- Public headers keep full `std::` names.
- Headers are self-contained and have an include guard.
- Include only what the file uses. Keep vendor headers out of public game
  headers.

## Ownership and interfaces

- One object has one clear owner.
- Use a reference for a required non-owning object and a pointer for an
  optional non-owning object.
- Use `std::span` for a non-owning sequence with a known size.
- Do not add a raw owning pointer. Existing raw ownership is removed only by
  its planned ownership item.
- Keep file, image, record-store, and WinMM handles in small RAII classes.

## Source form

- Use four spaces and no tabs.
- Put an opening brace on the same line as its statement.
- Keep one operation per line when order or side effects matter.
- Do not put two random reads, stream reads, or save reads in one expression.
- Prefer a clear loop over a complex ranges chain.
- Comments say why a rule exists; names and code say what happens.

## Change gate

- Keep every normal commit buildable.
- Run `test-debug-fast` for a normal commit and both full presets at item exit.
- A refactor must not change a fixed digest.
- A game fix needs a rule test and decompile evidence when native behavior is
  not enough.
